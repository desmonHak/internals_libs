#ifndef OS_H
#define OS_H

#include "x86/amd64/amd64.h"
#include "global_emmit.h"

#include "din_lib_c.h"

/* version
 * | version  | sub version |
 * 0b0000 0001 0000 0000
 */
#define VERSION_OS_MODULE 0b0000 0001 0000 0000

typedef enum arch_type_t {
    arch_type_x86_16,
    arch_type_x86_32,
    arch_type_x86_64,

    arch_type_unknown
} arch_type_t;

static const char* arch_type_string[] = {
    [arch_type_x86_16]  = "x86_16",
    [arch_type_x86_32]  = "x86_32",
    [arch_type_x86_64]  = "x86_64",
    [arch_type_unknown] = "unknown"
};

/**
 * @brief permite crear dos macros que indiquen la arquitectura y tamaño de la
 * CPU en la que se compilo el codigo actual.
 */
#if defined(__x86_64__) || defined(_M_X64)
    #define WORD_ARCH   64
    #define THIS_ARCH   arch_type_x86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define WORD_ARCH   32
    #define THIS_ARCH   arch_type_x86_32
#elif defined(__16BIT__) || defined(_M_I86)
    #define WORD_ARCH   16
    #define THIS_ARCH   arch_type_x86_16
#else
    #error "Arquitectura desconocida. No se puede determinar si es 16, 32 o 64 bits."
#endif

typedef enum os_type_t {
    os_type_windows,
    os_type_linux,
    os_type_mac,

    os_type_unknown
} os_type_t;

#if defined(_WIN32) || defined(_WIN64)
    #define THIS_OS os_type_windows
#elif defined(__APPLE__) && defined(__MACH__)
    #define THIS_OS os_type_windows
#elif defined(__linux__)
    #define THIS_OS os_type_linux
#elif defined(__unix__) || defined(__unix)
    #define THIS_OS os_type_mac
#else
    #define THIS_OS os_type_unknown
#endif

static const char* os_type_string[] = {
    [os_type_windows]       = "windows",
    [os_type_linux]         = "linux",
    [os_type_mac]           = "mac",
    [os_type_unknown]       = "unknown"
};

typedef struct info_destination_module {
    os_type_t       os;
    arch_type_t     arch;
} info_destination_module;

typedef void (*who_system_t)();

DLL_EXPORT void who_system();

#endif // MYLIB_H