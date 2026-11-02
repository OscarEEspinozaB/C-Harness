/* ch_verify.c: the C-Harness verifier.
 *
 * Rules:
 *   CONST-1   a `T *` parameter the function never writes must be `const T *`.
 *   ALIAS-1   a variable is not passed twice as output `&x` in one call.
 *   STATE-1   the direct address of a `static` does not leave its function with write access.
 *   NAME-1    a local variable is not named like a function of the same file.
 *
 * CONST-1 does not model what writing is. It inserts `const` into the parameter
 * and its redeclarations, parses the file again and lets C decide: if it still
 * compiles, the `const` was possible.
 *
 * Usage: ch_verify file.c [compiler options]
 * Exit status: 0 no findings, 1 findings, 2 could not analyze.
 */
#include <clang-c/Index.h>

enum { CH_NO_FINDINGS = 0, CH_FINDINGS = 1, CH_NOT_ANALYZED = 2 };

struct candidate {
  CXCursor function;
  unsigned index;
};

struct candidates {
  struct candidate *data;
  size_t count;
  size_t capacity;
  int out_of_memory;
};

struct file_edit {
  CXFile file;
  unsigned *offsets;
  size_t count;
  size_t capacity;
};

struct file_variant {
  char *path;
  char *contents;
  size_t length;
};

struct redeclarations {
  CXCursor canonical;
  unsigned index;
  struct file_edit *files;
  size_t file_count;
  size_t file_capacity;
  int skip;
};

struct analysis {
  CXIndex index;
  const char *path;
  const char *const *options;
  int option_count;
};

static char *read_file(const char *path, size_t *length) {
  FILE *file = fopen(path, "rb");
  if (!file) return NULL;
  char *text = NULL;
  long size = -1;
  if (fseek(file, 0, SEEK_END) == 0) size = ftell(file);
  if (size >= 0 && fseek(file, 0, SEEK_SET) == 0) {
    size_t expected = (size_t)size;
    text = malloc(expected + 1);
    if (text && fread(text, 1, expected, file) == expected) {
      text[expected] = '\0';
      *length = expected;
    } else {
      free(text);
      text = NULL;
    }
  }
  fclose(file);
  return text;
}

/* libclang may return NULL for a CXString: every read goes through here. */
static const char *string_text(CXString string) {
  const char *text = clang_getCString(string);
  return text ? text : "";
}

static unsigned count_errors(CXTranslationUnit unit) {
  unsigned errors = 0;
  unsigned total = clang_getNumDiagnostics(unit);
  for (unsigned i = 0; i < total; i++) {
    CXDiagnostic diag = clang_getDiagnostic(unit, i);
    if (clang_getDiagnosticSeverity(diag) >= CXDiagnostic_Error) errors++;
    clang_disposeDiagnostic(diag);
  }
  return errors;
}

/* Errors other than the three this verifier raises itself: the file does not compile,
 * and a missing declaration would hide writes from CONST-1. */
static unsigned count_hard_errors(CXTranslationUnit unit) {
  unsigned errors = 0;
  unsigned total = clang_getNumDiagnostics(unit);
  for (unsigned i = 0; i < total; i++) {
    CXDiagnostic diag = clang_getDiagnostic(unit, i);
    if (clang_getDiagnosticSeverity(diag) >= CXDiagnostic_Error) {
      CXString option = clang_getDiagnosticOption(diag, NULL);
      const char *name = string_text(option);
      if (!strstr(name, "cast-qual") && !strstr(name, "incompatible-pointer-types-discards-qualifiers") &&
          !strstr(name, "incompatible-function-pointer-types"))
        errors++;
      clang_disposeString(option);
    }
    clang_disposeDiagnostic(diag);
  }
  return errors;
}

static void show_errors(CXTranslationUnit unit) {
  unsigned total = clang_getNumDiagnostics(unit);
  for (unsigned i = 0; i < total; i++) {
    CXDiagnostic diag = clang_getDiagnostic(unit, i);
    if (clang_getDiagnosticSeverity(diag) >= CXDiagnostic_Error) {
      CXString text = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
      fprintf(stderr, "%s\n", string_text(text));
      clang_disposeString(text);
    }
    clang_disposeDiagnostic(diag);
  }
}

static int is_analyzable_parameter(CXCursor parameter) {
  CXType type = clang_getCursorType(parameter);
  if (type.kind != CXType_Pointer) return 0;
  CXType pointee = clang_getPointeeType(type);
  if (clang_isConstQualifiedType(pointee)) return 0;
  enum CXTypeKind canonical_kind = clang_getCanonicalType(pointee).kind;
  return canonical_kind != CXType_Pointer && canonical_kind != CXType_FunctionProto && canonical_kind != CXType_FunctionNoProto;
}

static void add_candidate(struct candidates *list, CXCursor function, unsigned index) {
  if (list->count == list->capacity) {
    size_t new_size = list->capacity ? list->capacity * 2 : 16;
    struct candidate *data = realloc(list->data, new_size * sizeof *data);
    if (!data) {
      list->out_of_memory = 1;
      return;
    }
    list->data = data;
    list->capacity = new_size;
  }
  list->data[list->count].function = function;
  list->data[list->count].index = index;
  list->count++;
}

static enum CXChildVisitResult collect_candidates(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct candidates *list = data;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_isCursorDefinition(cursor)) return CXChildVisit_Continue;
  if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor))) return CXChildVisit_Continue;
  int total = clang_Cursor_getNumArguments(cursor);
  for (int i = 0; i < total; i++) {
    if (is_analyzable_parameter(clang_Cursor_getArgument(cursor, (unsigned)i)))
      add_candidate(list, cursor, (unsigned)i);
  }
  return CXChildVisit_Continue;
}

static struct file_edit *edit_for_file(struct redeclarations *r, CXFile file) {
  for (size_t i = 0; i < r->file_count; i++) {
    if (clang_File_isEqual(r->files[i].file, file)) return &r->files[i];
  }
  if (r->file_count == r->file_capacity) {
    size_t new_size = r->file_capacity ? r->file_capacity * 2 : 2;
    struct file_edit *files = realloc(r->files, new_size * sizeof *files);
    if (!files) return NULL;
    r->files = files;
    r->file_capacity = new_size;
  }
  struct file_edit *edit = &r->files[r->file_count++];
  edit->file = file;
  edit->offsets = NULL;
  edit->count = 0;
  edit->capacity = 0;
  return edit;
}

static void add_offset(struct redeclarations *r, CXFile file, unsigned offset) {
  struct file_edit *edit = edit_for_file(r, file);
  if (!edit) {
    r->skip = 1;
    return;
  }
  if (edit->count == edit->capacity) {
    size_t new_size = edit->capacity ? edit->capacity * 2 : 4;
    unsigned *data = realloc(edit->offsets, new_size * sizeof *data);
    if (!data) {
      r->skip = 1;
      return;
    }
    edit->offsets = data;
    edit->capacity = new_size;
  }
  edit->offsets[edit->count++] = offset;
}

static enum CXChildVisitResult collect_redeclarations(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct redeclarations *r = data;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_equalCursors(clang_getCanonicalCursor(cursor), r->canonical)) return CXChildVisit_Continue;
  if (clang_Cursor_getNumArguments(cursor) <= (int)r->index) {
    r->skip = 1;
    return CXChildVisit_Continue;
  }
  CXCursor parameter = clang_Cursor_getArgument(cursor, r->index);
  CXSourceLocation start = clang_getRangeStart(clang_getCursorExtent(parameter));
  CXFile file = NULL;
  unsigned file_offset = 0;
  unsigned spelling_offset = 0;
  clang_getFileLocation(start, &file, NULL, NULL, &file_offset);
  clang_getSpellingLocation(start, NULL, NULL, NULL, &spelling_offset);
  /* System headers cannot change, and macro expansions have no single place to edit. */
  if (!file || clang_Location_isInSystemHeader(start) || file_offset != spelling_offset)
    r->skip = 1;
  else
    add_offset(r, file, file_offset);
  return CXChildVisit_Continue;
}

static char *insert_const(const char *text, size_t length, const struct file_edit *edit, size_t *new_size) {
  static const char marker[] = "const ";
  size_t marker_size = sizeof marker - 1;
  char *output = malloc(length + edit->count * marker_size + 1);
  if (!output) return NULL;
  size_t consumed = 0;
  size_t written = 0;
  for (size_t i = 0; i < edit->count; i++) {
    size_t end = edit->offsets[i];
    memcpy(output + written, text + consumed, end - consumed);
    written += end - consumed;
    consumed = end;
    memcpy(output + written, marker, marker_size);
    written += marker_size;
  }
  memcpy(output + written, text + consumed, length - consumed);
  written += length - consumed;
  output[written] = '\0';
  *new_size = written;
  return output;
}

static CXTranslationUnit parse(const struct analysis *a, struct CXUnsavedFile *files, unsigned file_count) {
  return clang_parseTranslationUnit(a->index, a->path, a->options, a->option_count, files, file_count,
                                    CXTranslationUnit_None);
}

static void report_const(const char *path, const struct candidate *c) {
  CXCursor parameter = clang_Cursor_getArgument(c->function, c->index);
  unsigned line = 0;
  clang_getFileLocation(clang_getCursorLocation(parameter), NULL, &line, NULL, NULL);
  CXString name = clang_getCursorSpelling(parameter);
  CXString function = clang_getCursorSpelling(c->function);
  CXString pointee = clang_getTypeSpelling(clang_getPointeeType(clang_getCursorType(parameter)));
  printf("%s:%u: CONST-1: parameter '%s' of '%s' is never written: declare it 'const %s *'\n", path, line,
         string_text(name), string_text(function), string_text(pointee));
  clang_disposeString(pointee);
  clang_disposeString(function);
  clang_disposeString(name);
}

/* Applies the edit of one file. Returns 0 on failure. */
static int prepare_variant(const struct file_edit *edit, struct file_variant *variant) {
  CXString name = clang_getFileName(edit->file);
  const char *file_name = string_text(name);
  size_t name_size = strlen(file_name) + 1;
  size_t length = 0;
  int done = 0;
  char *text = read_file(file_name, &length);
  if (text) {
    size_t new_size = 0;
    variant->path = malloc(name_size);
    variant->contents = insert_const(text, length, edit, &new_size);
    free(text);
    if (variant->path && variant->contents) {
      memcpy(variant->path, file_name, name_size);
      variant->length = new_size;
      done = 1;
    }
  }
  clang_disposeString(name);
  return done;
}

/* Returns 1 if the edits add no errors to the baseline, 0 if they do, -1 on failure. */
static int edits_add_no_errors(const struct analysis *a, const struct redeclarations *r, unsigned baseline_errors) {
  size_t file_count = r->file_count;
  struct file_variant *variants = calloc(file_count, sizeof *variants);
  struct CXUnsavedFile *unsaved = calloc(file_count, sizeof *unsaved);
  int answer = -1;
  if (variants && unsaved) {
    size_t ready = 0;
    while (ready < file_count && prepare_variant(&r->files[ready], &variants[ready])) {
      unsaved[ready].Filename = variants[ready].path;
      unsaved[ready].Contents = variants[ready].contents;
      unsaved[ready].Length = (unsigned long)variants[ready].length;
      ready++;
    }
    if (ready == file_count) {
      CXTranslationUnit unit = parse(a, unsaved, (unsigned)ready);
      if (unit) {
        answer = count_errors(unit) <= baseline_errors;
        clang_disposeTranslationUnit(unit);
      }
    }
    for (size_t i = 0; i < file_count; i++) {
      free(variants[i].path);
      free(variants[i].contents);
    }
  }
  free(unsaved);
  free(variants);
  return answer;
}

/* Returns 1 if `const` is possible, 0 if not, -1 if it could not be decided. */
static int accepts_const(const struct analysis *a, const struct candidate *c, CXTranslationUnit base,
                         unsigned baseline_errors) {
  struct redeclarations r = { .canonical = clang_getCanonicalCursor(c->function), .index = c->index };
  clang_visitChildren(clang_getTranslationUnitCursor(base), collect_redeclarations, &r);
  int answer = -1;
  if (!r.skip && r.file_count > 0) answer = edits_add_no_errors(a, &r, baseline_errors);
  for (size_t i = 0; i < r.file_count; i++) free(r.files[i].offsets);
  free(r.files);
  return answer;
}

struct review {
  const char *path;
  CXCursor function;
  int findings;
};

struct function_search {
  const char *name;
  int found;
};

struct children {
  CXCursor cursors[2];
  unsigned count;
};

static void report(struct review *r, CXCursor site, const char *rule, const char *name, const char *problem) {
  unsigned line = 0;
  clang_getFileLocation(clang_getCursorLocation(site), NULL, &line, NULL, NULL);
  printf("%s:%u: %s: '%s' %s\n", r->path, line, rule, name, problem);
  r->findings++;
}

static enum CXChildVisitResult take_children(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct children *h = data;
  h->cursors[h->count++] = cursor;
  return h->count == 2 ? CXChildVisit_Break : CXChildVisit_Continue;
}

static struct children read_children(CXCursor cursor) {
  struct children h = { { clang_getNullCursor(), clang_getNullCursor() }, 0 };
  clang_visitChildren(cursor, take_children, &h);
  return h;
}

/* Strips parentheses and implicit conversions. */
static CXCursor unwrap(CXCursor cursor) {
  enum CXCursorKind kind = clang_getCursorKind(cursor);
  while (kind == CXCursor_UnexposedExpr || kind == CXCursor_ParenExpr) {
    struct children h = read_children(cursor);
    if (h.count == 0) break;
    cursor = h.cursors[0];
    kind = clang_getCursorKind(cursor);
  }
  return cursor;
}

/* For `&x` returns the declaration of x; otherwise, the null cursor. */
static CXCursor addressed_variable(CXCursor expression) {
  CXCursor e = unwrap(expression);
  if (clang_getCursorKind(e) != CXCursor_UnaryOperator) return clang_getNullCursor();
  if (clang_getCursorUnaryOperatorKind(e) != CXUnaryOperator_AddrOf) return clang_getNullCursor();
  struct children h = read_children(e);
  if (h.count == 0) return clang_getNullCursor();
  CXCursor operand = unwrap(h.cursors[0]);
  if (clang_getCursorKind(operand) != CXCursor_DeclRefExpr) return clang_getNullCursor();
  CXCursor declared = clang_getCursorReferenced(operand);
  return clang_getCursorKind(declared) == CXCursor_VarDecl ? declared : clang_getNullCursor();
}

static int is_writable_state(CXCursor variable) {
  if (clang_Cursor_isNull(variable)) return 0;
  if (clang_Cursor_getStorageClass(variable) != CX_SC_Static) return 0;
  CXType type = clang_getCursorType(variable);
  if (type.kind == CXType_ConstantArray) type = clang_getArrayElementType(type);
  return !clang_isConstQualifiedType(type);
}

/* Returns the writable `static` variable whose address the expression yields. */
static CXCursor state_in_expression(CXCursor expression) {
  CXCursor direct = addressed_variable(expression);
  if (is_writable_state(direct)) return direct;
  CXCursor e = unwrap(expression);
  if (clang_getCursorKind(e) == CXCursor_DeclRefExpr) {
    CXCursor declared = clang_getCursorReferenced(e);
    if (clang_getCursorKind(declared) == CXCursor_VarDecl && is_writable_state(declared) &&
        clang_getCursorType(declared).kind == CXType_ConstantArray)
      return declared;
  }
  return clang_getNullCursor();
}

static void report_state(struct review *r, CXCursor site, CXCursor variable) {
  CXString name = clang_getCursorSpelling(variable);
  report(r, site, "STATE-1", string_text(name), "is function or module state and its address leaves with write access");
  clang_disposeString(name);
}

static void check_return(struct review *r, CXCursor return_stmt) {
  struct children h = read_children(return_stmt);
  if (h.count == 0) return;
  CXType returned = clang_getCursorResultType(r->function);
  if (returned.kind == CXType_Pointer && clang_isConstQualifiedType(clang_getPointeeType(returned))) return;
  CXCursor variable = state_in_expression(h.cursors[0]);
  if (!clang_Cursor_isNull(variable)) report_state(r, return_stmt, variable);
}

static void check_assignment(struct review *r, CXCursor operation) {
  if (clang_getCursorBinaryOperatorKind(operation) != CXBinaryOperator_Assign) return;
  struct children h = read_children(operation);
  if (h.count < 2) return;
  /* Storing it in a local variable does not take it out of the function. */
  if (clang_getCursorKind(unwrap(h.cursors[0])) == CXCursor_DeclRefExpr) return;
  /* A pointer to const gives no write access. */
  CXType target = clang_getCursorType(h.cursors[0]);
  if (target.kind == CXType_Pointer && clang_isConstQualifiedType(clang_getPointeeType(target))) return;
  CXCursor variable = state_in_expression(h.cursors[1]);
  if (!clang_Cursor_isNull(variable)) report_state(r, operation, variable);
}

static int is_output_parameter(CXType function, unsigned index) {
  if ((int)index >= clang_getNumArgTypes(function)) return 0;
  CXType type = clang_getArgType(function, index);
  return type.kind == CXType_Pointer && !clang_isConstQualifiedType(clang_getPointeeType(type));
}

static void check_call(struct review *r, CXCursor call) {
  CXCursor callee = clang_getCursorReferenced(call);
  if (clang_getCursorKind(callee) != CXCursor_FunctionDecl) return;
  CXType function = clang_getCursorType(callee);
  int total = clang_Cursor_getNumArguments(call);
  for (int i = 0; i < total; i++) {
    if (!is_output_parameter(function, (unsigned)i)) continue;
    CXCursor first = addressed_variable(clang_Cursor_getArgument(call, (unsigned)i));
    if (clang_Cursor_isNull(first)) continue;
    for (int j = i + 1; j < total; j++) {
      if (!is_output_parameter(function, (unsigned)j)) continue;
      CXCursor second = addressed_variable(clang_Cursor_getArgument(call, (unsigned)j));
      if (!clang_Cursor_isNull(second) && clang_equalCursors(first, second)) {
        CXString name = clang_getCursorSpelling(first);
        report(r, call, "ALIAS-1", string_text(name), "is passed twice as output in the same call");
        clang_disposeString(name);
      }
    }
  }
}

static enum CXChildVisitResult find_function(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct function_search *b = data;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor))) return CXChildVisit_Continue;
  CXString name = clang_getCursorSpelling(cursor);
  b->found = strcmp(string_text(name), b->name) == 0;
  clang_disposeString(name);
  return b->found ? CXChildVisit_Break : CXChildVisit_Continue;
}

static void check_name(struct review *r, CXCursor declaration) {
  CXString name = clang_getCursorSpelling(declaration);
  struct function_search b = { string_text(name), 0 };
  if (b.name[0] != '\0') {
    CXCursor root = clang_getTranslationUnitCursor(clang_Cursor_getTranslationUnit(declaration));
    clang_visitChildren(root, find_function, &b);
    if (b.found) report(r, declaration, "NAME-1", b.name, "already names a function of this file");
  }
  clang_disposeString(name);
}

static enum CXChildVisitResult check_node(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct review *r = data;
  enum CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind == CXCursor_VarDecl || kind == CXCursor_ParmDecl) check_name(r, cursor);
  else if (kind == CXCursor_CallExpr) check_call(r, cursor);
  else if (kind == CXCursor_ReturnStmt) check_return(r, cursor);
  else if (kind == CXCursor_BinaryOperator) check_assignment(r, cursor);
  return CXChildVisit_Recurse;
}

static enum CXChildVisitResult check_function(CXCursor cursor, CXCursor parent, CXClientData data) {
  (void)parent;
  struct review *r = data;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_isCursorDefinition(cursor)) return CXChildVisit_Continue;
  if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor))) return CXChildVisit_Continue;
  r->function = cursor;
  clang_visitChildren(cursor, check_node, r);
  return CXChildVisit_Continue;
}

static int verify(const struct analysis *a) {
  CXTranslationUnit base = parse(a, NULL, 0);
  if (!base) {
    fprintf(stderr, "%s: libclang could not parse the file\n", a->path);
    return CH_NOT_ANALYZED;
  }
  int result = CH_NO_FINDINGS;
  unsigned baseline_errors = count_errors(base);
  if (count_hard_errors(base) > 0) {
    show_errors(base);
    result = CH_NOT_ANALYZED;
  } else {
    struct candidates list = { 0 };
    clang_visitChildren(clang_getTranslationUnitCursor(base), collect_candidates, &list);
    if (list.out_of_memory) result = CH_NOT_ANALYZED;
    for (size_t i = 0; result != CH_NOT_ANALYZED && i < list.count; i++) {
      int answer = accepts_const(a, &list.data[i], base, baseline_errors);
      if (answer > 0) {
        report_const(a->path, &list.data[i]);
        result = CH_FINDINGS;
      } else if (answer < 0) {
        fprintf(stderr, "%s: note: a parameter was not analyzed\n", a->path);
      }
    }
    free(list.data);
    struct review review = { .path = a->path };
    clang_visitChildren(clang_getTranslationUnitCursor(base), check_function, &review);
    if (review.findings > 0 && result == CH_NO_FINDINGS) result = CH_FINDINGS;
  }
  clang_disposeTranslationUnit(base);
  return result;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s file.c [compiler options]\n", argv[0]);
    return CH_NOT_ANALYZED;
  }
  /* Discarding `const`, implicitly or with a cast, is a possible write: it counts as an error. */
  size_t option_total = (size_t)argc - 2 + 3;
  const char **options = malloc(option_total * sizeof *options);
  int result = CH_NOT_ANALYZED;
  if (options) {
    options[0] = "-Werror=incompatible-pointer-types-discards-qualifiers";
    options[1] = "-Werror=incompatible-function-pointer-types";
    options[2] = "-Werror=cast-qual";
    for (int i = 2; i < argc; i++) options[i + 1] = argv[i];
    struct analysis a = { clang_createIndex(0, 0), argv[1], options, (int)option_total };
    result = verify(&a);
    clang_disposeIndex(a.index);
    free(options);
  }
  return result;
}
