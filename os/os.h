#ifndef OS_H
#define OS_H

#include "x86/amd64/amd64.h"
#include "global_emmit.h"

#include "../../include/vm.h"

#include "json_c.h"

#include "din_lib_c.h"

/* version
 * | version  | sub version |
 * 0b0000 0001 0000 0000
 */
#define VERSION_OS_MODULE 0b0000000100000000

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
typedef bool (*queery_arch_t)(const char*);



typedef enum err_code_get_type_data_json
{
    ERR_NO_ERROR,
    ARCH_UNKNOWN,
    REGS_UNKNOWN,
    CATEGORY_UNKNOWN,
    TYPE_UNKNOWN,
    CONV_UNKNOWN
} err_code_get_type_data_json;
static const char *errs_get_type_data_json[] = {
    [ARCH_UNKNOWN]      = "Esta arquitectura, no esta registrada",
    [ARCH_UNKNOWN]      = "ARCH_UNKNOWN",
    [REGS_UNKNOWN]      = "REGS_UNKNOWN",
    [CATEGORY_UNKNOWN]  = "CATEGORY_UNKNOWN",
    [TYPE_UNKNOWN]      = "TYPE_UNKNOWN",
    [CONV_UNKNOWN]      = "CONV_UNKNOWN"
};
typedef struct type_data_json_t {
    const char*                   cat_name; /** nombre de la categoria              */
    const char*                  name_type; /** nombre del tipo                     */
    const char**                 data_conv; /** registros de la convencion          */
    const char*                       conv; /** nombre de la convencion             */
    const char*                       arch; /** arquitectura                        */
    err_code_get_type_data_json   err_code; /** codigo de error al obtener datos    */
} type_data_json_t;
type_data_json_t get_type_data_json(const char* arch, const char* name_conv);

typedef void (*f_invoke_callthblock)(shellcode_t *,  ThreadVM_t *);

// Helper: ¿Es un nodo array?
static int is_array_node(ast_node_t* node) {
    return node && node->data && strcmp((char*)node->data, "[") == 0;
}
DLL_EXPORT void os_destructor(void);

DLL_EXPORT const char *getBuild();
DLL_EXPORT void who_system();

//DLL_EXPORT din_lib lib_isa = NULL;

#endif // MYLIB_H