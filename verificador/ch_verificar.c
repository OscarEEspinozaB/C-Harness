/* ch_verificar.c: verificador de C-Harness.
 *
 * Regla CONST-1: un parámetro `T *` que la función no escribe debe ser `const T *`.
 *
 * El verificador no modela qué es escribir. Inserta `const` en el parámetro y
 * en sus redeclaraciones, reanaliza el archivo y deja que C decida: si sigue
 * compilando, el `const` era posible.
 *
 * Uso: ch_verificar archivo.c [opciones de compilación]
 * Salida: 0 sin hallazgos, 1 con hallazgos, 2 si no se pudo analizar.
 */
#include <clang-c/Index.h>

enum { CH_SIN_HALLAZGOS = 0, CH_CON_HALLAZGOS = 1, CH_SIN_ANALISIS = 2 };

struct candidato {
  CXCursor funcion;
  unsigned indice;
};

struct candidatos {
  struct candidato *datos;
  size_t cuantos;
  size_t capacidad;
  int sin_memoria;
};

struct redeclaraciones {
  CXCursor canonica;
  unsigned indice;
  unsigned *desplazamientos;
  size_t cuantos;
  size_t capacidad;
  int omitir;
};

struct analisis {
  CXIndex indice;
  const char *ruta;
  const char *const *opciones;
  int cuantas_opciones;
};

static char *leer_archivo(const char *ruta, size_t *longitud) {
  FILE *archivo = fopen(ruta, "rb");
  if (!archivo) return NULL;
  char *texto = NULL;
  long tam = -1;
  if (fseek(archivo, 0, SEEK_END) == 0) tam = ftell(archivo);
  if (tam >= 0 && fseek(archivo, 0, SEEK_SET) == 0) {
    size_t esperado = (size_t)tam;
    texto = malloc(esperado + 1);
    if (texto && fread(texto, 1, esperado, archivo) == esperado) {
      texto[esperado] = '\0';
      *longitud = esperado;
    } else {
      free(texto);
      texto = NULL;
    }
  }
  fclose(archivo);
  return texto;
}

static unsigned contar_errores(CXTranslationUnit unidad) {
  unsigned errores = 0;
  unsigned total = clang_getNumDiagnostics(unidad);
  for (unsigned i = 0; i < total; i++) {
    CXDiagnostic diagnostico = clang_getDiagnostic(unidad, i);
    if (clang_getDiagnosticSeverity(diagnostico) >= CXDiagnostic_Error) errores++;
    clang_disposeDiagnostic(diagnostico);
  }
  return errores;
}

static void mostrar_errores(CXTranslationUnit unidad) {
  unsigned total = clang_getNumDiagnostics(unidad);
  for (unsigned i = 0; i < total; i++) {
    CXDiagnostic diagnostico = clang_getDiagnostic(unidad, i);
    if (clang_getDiagnosticSeverity(diagnostico) >= CXDiagnostic_Error) {
      CXString texto = clang_formatDiagnostic(diagnostico, clang_defaultDiagnosticDisplayOptions());
      fprintf(stderr, "%s\n", clang_getCString(texto));
      clang_disposeString(texto);
    }
    clang_disposeDiagnostic(diagnostico);
  }
}

static int es_parametro_analizable(CXCursor parametro) {
  CXType tipo = clang_getCursorType(parametro);
  if (tipo.kind != CXType_Pointer) return 0;
  CXType apuntado = clang_getPointeeType(tipo);
  if (clang_isConstQualifiedType(apuntado)) return 0;
  enum CXTypeKind real = clang_getCanonicalType(apuntado).kind;
  return real != CXType_Pointer && real != CXType_FunctionProto && real != CXType_FunctionNoProto;
}

static void agregar_candidato(struct candidatos *lista, CXCursor funcion, unsigned indice) {
  if (lista->cuantos == lista->capacidad) {
    size_t nueva = lista->capacidad ? lista->capacidad * 2 : 16;
    struct candidato *datos = realloc(lista->datos, nueva * sizeof *datos);
    if (!datos) {
      lista->sin_memoria = 1;
      return;
    }
    lista->datos = datos;
    lista->capacidad = nueva;
  }
  lista->datos[lista->cuantos].funcion = funcion;
  lista->datos[lista->cuantos].indice = indice;
  lista->cuantos++;
}

static enum CXChildVisitResult recoger_candidatos(CXCursor cursor, CXCursor padre, CXClientData datos) {
  (void)padre;
  struct candidatos *lista = datos;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_isCursorDefinition(cursor)) return CXChildVisit_Continue;
  if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor))) return CXChildVisit_Continue;
  int total = clang_Cursor_getNumArguments(cursor);
  for (int i = 0; i < total; i++) {
    if (es_parametro_analizable(clang_Cursor_getArgument(cursor, (unsigned)i)))
      agregar_candidato(lista, cursor, (unsigned)i);
  }
  return CXChildVisit_Continue;
}

static void agregar_desplazamiento(struct redeclaraciones *r, unsigned desplazamiento) {
  if (r->cuantos == r->capacidad) {
    size_t nueva = r->capacidad ? r->capacidad * 2 : 4;
    unsigned *datos = realloc(r->desplazamientos, nueva * sizeof *datos);
    if (!datos) {
      r->omitir = 1;
      return;
    }
    r->desplazamientos = datos;
    r->capacidad = nueva;
  }
  r->desplazamientos[r->cuantos++] = desplazamiento;
}

static enum CXChildVisitResult recoger_redeclaraciones(CXCursor cursor, CXCursor padre, CXClientData datos) {
  (void)padre;
  struct redeclaraciones *r = datos;
  if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return CXChildVisit_Continue;
  if (!clang_equalCursors(clang_getCanonicalCursor(cursor), r->canonica)) return CXChildVisit_Continue;
  if (clang_Cursor_getNumArguments(cursor) <= (int)r->indice) {
    r->omitir = 1;
    return CXChildVisit_Continue;
  }
  CXCursor parametro = clang_Cursor_getArgument(cursor, r->indice);
  CXSourceLocation inicio = clang_getRangeStart(clang_getCursorExtent(parametro));
  unsigned en_archivo = 0;
  unsigned escrito = 0;
  clang_getFileLocation(inicio, NULL, NULL, NULL, &en_archivo);
  clang_getSpellingLocation(inicio, NULL, NULL, NULL, &escrito);
  /* Declaraciones en cabeceras o generadas por macros: aún no se reescriben. */
  if (!clang_Location_isFromMainFile(inicio) || en_archivo != escrito)
    r->omitir = 1;
  else
    agregar_desplazamiento(r, en_archivo);
  return CXChildVisit_Continue;
}

static char *insertar_const(const char *texto, size_t longitud, const struct redeclaraciones *r, size_t *nueva) {
  static const char marca[] = "const ";
  size_t tam_marca = sizeof marca - 1;
  char *salida = malloc(longitud + r->cuantos * tam_marca + 1);
  if (!salida) return NULL;
  size_t leido = 0;
  size_t escrito = 0;
  for (size_t i = 0; i < r->cuantos; i++) {
    size_t hasta = r->desplazamientos[i];
    memcpy(salida + escrito, texto + leido, hasta - leido);
    escrito += hasta - leido;
    leido = hasta;
    memcpy(salida + escrito, marca, tam_marca);
    escrito += tam_marca;
  }
  memcpy(salida + escrito, texto + leido, longitud - leido);
  escrito += longitud - leido;
  salida[escrito] = '\0';
  *nueva = escrito;
  return salida;
}

static CXTranslationUnit analizar(const struct analisis *a, const char *texto, size_t longitud) {
  struct CXUnsavedFile variante = { a->ruta, texto, (unsigned long)longitud };
  return clang_parseTranslationUnit(a->indice, a->ruta, a->opciones, a->cuantas_opciones, &variante,
                                    texto ? 1U : 0U, CXTranslationUnit_None);
}

static void informar(const char *ruta, const struct candidato *c) {
  CXCursor parametro = clang_Cursor_getArgument(c->funcion, c->indice);
  unsigned linea = 0;
  clang_getFileLocation(clang_getCursorLocation(parametro), NULL, &linea, NULL, NULL);
  CXString nombre = clang_getCursorSpelling(parametro);
  CXString funcion = clang_getCursorSpelling(c->funcion);
  CXString apuntado = clang_getTypeSpelling(clang_getPointeeType(clang_getCursorType(parametro)));
  printf("%s:%u: CONST-1: el parámetro '%s' de '%s' no se escribe: decláralo 'const %s *'\n", ruta, linea,
         clang_getCString(nombre), clang_getCString(funcion), clang_getCString(apuntado));
  clang_disposeString(apuntado);
  clang_disposeString(funcion);
  clang_disposeString(nombre);
}

/* Devuelve 1 si `const` es posible, 0 si no, -1 si no se pudo decidir. */
static int admite_const(const struct analisis *a, const struct candidato *c, CXTranslationUnit base,
                        const char *texto, size_t longitud) {
  struct redeclaraciones r = { .canonica = clang_getCanonicalCursor(c->funcion), .indice = c->indice };
  clang_visitChildren(clang_getTranslationUnitCursor(base), recoger_redeclaraciones, &r);
  int respuesta = -1;
  if (!r.omitir && r.cuantos > 0) {
    size_t nueva = 0;
    char *variante = insertar_const(texto, longitud, &r, &nueva);
    if (variante) {
      CXTranslationUnit unidad = analizar(a, variante, nueva);
      if (unidad) {
        respuesta = contar_errores(unidad) == 0;
        clang_disposeTranslationUnit(unidad);
      }
      free(variante);
    }
  }
  free(r.desplazamientos);
  return respuesta;
}

static int verificar(const struct analisis *a, const char *texto, size_t longitud) {
  CXTranslationUnit base = analizar(a, texto, longitud);
  if (!base) {
    fprintf(stderr, "%s: libclang no pudo analizar el archivo\n", a->ruta);
    return CH_SIN_ANALISIS;
  }
  int resultado = CH_SIN_HALLAZGOS;
  if (contar_errores(base) > 0) {
    mostrar_errores(base);
    resultado = CH_SIN_ANALISIS;
  } else {
    struct candidatos lista = { 0 };
    clang_visitChildren(clang_getTranslationUnitCursor(base), recoger_candidatos, &lista);
    if (lista.sin_memoria) resultado = CH_SIN_ANALISIS;
    for (size_t i = 0; resultado != CH_SIN_ANALISIS && i < lista.cuantos; i++) {
      int respuesta = admite_const(a, &lista.datos[i], base, texto, longitud);
      if (respuesta > 0) {
        informar(a->ruta, &lista.datos[i]);
        resultado = CH_CON_HALLAZGOS;
      } else if (respuesta < 0) {
        fprintf(stderr, "%s: nota: un parámetro quedó sin analizar\n", a->ruta);
      }
    }
    free(lista.datos);
  }
  clang_disposeTranslationUnit(base);
  return resultado;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "uso: %s archivo.c [opciones de compilación]\n", argv[0]);
    return CH_SIN_ANALISIS;
  }
  size_t longitud = 0;
  char *texto = leer_archivo(argv[1], &longitud);
  if (!texto) {
    fprintf(stderr, "%s: no se pudo leer\n", argv[1]);
    return CH_SIN_ANALISIS;
  }
  /* Descartar `const` en una conversión implícita es una escritura posible: cuenta como error. */
  size_t cuantas = (size_t)argc - 2 + 2;
  const char **opciones = malloc(cuantas * sizeof *opciones);
  int resultado = CH_SIN_ANALISIS;
  if (opciones) {
    opciones[0] = "-Werror=incompatible-pointer-types-discards-qualifiers";
    opciones[1] = "-Werror=incompatible-function-pointer-types";
    for (int i = 2; i < argc; i++) opciones[i] = argv[i];
    struct analisis a = { clang_createIndex(0, 0), argv[1], opciones, (int)cuantas };
    resultado = verificar(&a, texto, longitud);
    clang_disposeIndex(a.indice);
    free(opciones);
  }
  free(texto);
  return resultado;
}
