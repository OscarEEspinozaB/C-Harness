#!/usr/bin/env python3
"""Counts C coding practices in source files. Exploratory: not a harness stage.

Usage: practices.py file.c... -- compiler options
"""
import sys
from clang import cindex
from clang.cindex import CursorKind as K, TypeKind as T

BOUNDED = {"snprintf", "vsnprintf", "strncpy", "strncat", "memcpy", "memmove", "memset"}
UNBOUNDED = {"strcpy", "strcat", "sprintf", "vsprintf", "gets"}
ALLOC = {"malloc", "calloc", "realloc", "free"}
SIZE_WORDS = ("len", "size", "capacity", "max", "count", "length", "bytes")


def tokens_of(cursor):
    return [t.spelling for t in cursor.get_tokens()]


def checks_null(tokens, name):
    for i, token in enumerate(tokens):
        if token != name:
            continue
        before = tokens[i - 1] if i else ""
        after = tokens[i + 1:i + 3]
        if before == "!" or after in (["==", "NULL"], ["!=", "NULL"], ["==", "0"], ["!=", "0"]):
            return True
    return False


def add_file(counts, path, args):
    unit = cindex.Index.create().parse(path, args=args)
    for cursor in unit.cursor.walk_preorder():
        if not cursor.location.file or cursor.location.file.name != path:
            continue
        kind = cursor.kind
        if kind == K.FUNCTION_DECL and cursor.is_definition():
            counts["functions"] += 1
            counts["static_functions"] += cursor.storage_class == cindex.StorageClass.STATIC
            size = cursor.extent.end.line - cursor.extent.start.line + 1
            counts["lines"] += size
            counts["longest"] = max(counts["longest"], size)
            tokens = tokens_of(cursor)
            params = list(cursor.get_arguments())
            for position, param in enumerate(params):
                if param.type.kind != T.POINTER:
                    continue
                pointee = param.type.get_pointee()
                if pointee.get_canonical().kind in (T.FUNCTIONPROTO, T.FUNCTIONNOPROTO):
                    continue
                counts["pointer_params"] += 1
                counts["const_pointer_params"] += pointee.is_const_qualified()
                counts["null_checked_params"] += checks_null(tokens, param.spelling)
                if pointee.get_canonical().kind in (T.CHAR_S, T.UCHAR, T.SCHAR, T.VOID):
                    counts["buffer_params"] += 1
                    rest = [p.spelling.lower() for p in params if p != param]
                    counts["buffer_params_with_size"] += any(w in r for r in rest for w in SIZE_WORDS)
        elif kind == K.VAR_DECL and cursor.semantic_parent.kind == K.TRANSLATION_UNIT or (
            kind == K.VAR_DECL and cursor.storage_class == cindex.StorageClass.STATIC):
            element = cursor.type.get_array_element_type() if cursor.type.kind == T.CONSTANTARRAY else cursor.type
            counts["const_tables" if element.is_const_qualified() else "mutable_state"] += 1
        elif kind == K.CSTYLE_CAST_EXPR:
            counts["casts"] += 1
        elif kind == K.GOTO_STMT:
            counts["gotos"] += 1
        elif kind == K.UNION_DECL:
            counts["unions"] += 1
        elif kind in (K.FIELD_DECL, K.PARM_DECL) and cursor.type.kind == T.POINTER and \
                cursor.type.get_pointee().get_canonical().kind == T.FUNCTIONPROTO:
            counts["function_pointers"] += 1
        elif kind == K.CALL_EXPR:
            counts["bounded_calls"] += cursor.spelling in BOUNDED
            counts["unbounded_calls"] += cursor.spelling in UNBOUNDED
            counts["direct_alloc"] += cursor.spelling in ALLOC


def main(argv):
    split = argv.index("--") if "--" in argv else len(argv)
    counts = dict.fromkeys(
        ["functions", "static_functions", "lines", "longest", "pointer_params", "const_pointer_params",
         "null_checked_params", "buffer_params", "buffer_params_with_size", "mutable_state", "const_tables",
         "casts", "gotos", "unions", "function_pointers", "bounded_calls", "unbounded_calls", "direct_alloc"], 0)
    for path in argv[:split]:
        add_file(counts, path, argv[split + 1:])
    return counts


if __name__ == "__main__":
    for key, value in main(sys.argv[1:]).items():
        print(f"{key} {value}")
