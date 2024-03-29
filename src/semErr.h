#ifndef _SEMERR_H_
#define _SEMERR_H_

typedef enum SEMERR {
    NOP,
    VAR_UNDEF,
    FUN_UNDEF,
    VAR_REDEF,
    FUN_REDEF,
    ASSIGN_TYPE,
    ASSIGN_RVAL,
    OP_TYPE,
    RETURN_TYPE,
    ARG_INCOMPATIBLE,
    NOT_ARRAY,
    NOT_FUN,
    NOT_INT,
    NOT_STRUCT,
    FIELD_UNDEF,
    FIELD_REDEF,
    STRUCT_REDEF,
    STRUCT_UNDEF,
} SEMERR;

#endif