#ifndef OS_H
#define OS_H

#include "x86/amd64/amd64.h"
#include "global_emmit.h"

#include "din_lib_c.h"

typedef enum arch_type_t {
    arch_type_x86_16,
    arch_type_x86_32,
    arch_type_x86_64,

    arch_type_unknown
} arch_type_t;


typedef void (*who_system_t)();

DLL_EXPORT void who_system();

#endif // MYLIB_H