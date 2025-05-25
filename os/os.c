#define BUILDING_DIN_LYB

#include "din_lib_c.h"
#include "os.h"

#include <stdio.h>

DLL_EXPORT info_destination_module info_module = {
    .arch   = THIS_ARCH,
    .os     = THIS_OS
};

DLL_EXPORT shellcode_t code;

/**
 * @brief puntero al json con informacion de las especificaciones de la 
 * arquitecturas
 */
DLL_EXPORT ast_node_t* json_specs;

#include "init.c"

static void print_type_data_json(const type_data_json_t* data) {
    if (!data) {
        printf("type_data_json_t: (null)\n");
        return;
    }
    printf_color("\rarch: {#{FG:lpurple}%10s#{reset}}, ", data->arch ? data->arch : "(null)");
    printf_color("name_conv: {#{FG:lgreen}%16s#{reset}}, ", data->conv ? data->conv : "(null)");
    printf_color("category: {#{FG:lcyan}%10s#{reset}}, ", data->cat_name ? data->cat_name : "(null)");
    printf_color("type: {#{FG:lcyan}%10s#{reset}}, ", data->name_type ? data->name_type : "(null)");
    //printf("Código error: %d\n", data->err_code);

    if (data->data_conv) {
        printf("regs: ");
        for (const char** r = data->data_conv; *r; ++r) {


            printf_color(FOREGROUND_COLOR_CUSTOM("%d")"%05s#{reset}%s ", ((int)(((r - data->data_conv) + 0x33) *2) & 0xff) ,
                *r, (*(r + 1) != NULL) ? (",") : "\n" );
        }
    } else {
        printf("Registros: (ninguno)\n");
    }
    //printf("==========================\n");
}
static inline void print_type_data_json_with_free(const type_data_json_t* data, void* NULL_DATA) {
    print_type_data_json(data);
    free(data->data_conv);
    free(data);
}
static inline void push_type_data_json(const type_data_json_t* data, ArrayList* arr) {
    push_back_a(arr, data);
}

/**
 *
 *"x86_64": {                                                   ---> arch_data
 *        "msfcall": {                                          ---> name_conv_data
 *            "number": {                                       ---> type_data
 *                "uint64_t": ["rcx",     "rdx",      ....      ---> size_data
 *                "float"  :  ["xmm0",    "xmm1",     ....      ---> type_data
 *            }
 *        },
 *        "syscall_linux": {                                    ---> name_conv_data
 *            "number": {                                       ---> type_data
 *                "uint64_t": ["rdi", "rsi", "rdx", ....        ---> size_data
 *            }
 *        },
 *        "syscall_windows": {                                  ---> name_conv_data
 *            "number": {                                       ---> type_data
 *                "uint64_t": ["rcx", "rdx", ...                ---> size_data
 *            }
 *        }
 *    },
 *
 * @param arch
 * @param name_conv
 * @return
 */
static inline void dump_all_conventions(type_data_json_t* data) {
    print_type_data_json(data);
}

static inline void get_elements_specs_json(ArrayList *arr, type_data_json_t* data) {
    push_back_a(arr, data);
}

void *build_type_data_json_array(ast_node_t *json_specs, void (*func)(void*, void*), void *user_data) {
    ArrayList *arr = NULL;
    if ((void*)func == (void*)get_elements_specs_json) {arr = createArrayList(1, NULL);}

    size_t n_archs = size_a(json_specs->ramas);
    for (size_t i = 0; i < n_archs; ++i) {
        ast_node_t* arch_key = (ast_node_t*)get_element_a(json_specs->ramas, i);
        const char* arch_name = (const char*)arch_key->data;
        if (!arch_name) continue;
        // Salta al nodo valor de arquitectura
        if (size_a(arch_key->ramas) == 0) continue;
        ast_node_t* arch_val = (ast_node_t*)get_element_a(arch_key->ramas, 0); // nodo ':'
        if (size_a(arch_val->ramas) == 0) continue;
        ast_node_t* arch_obj = (ast_node_t*)get_element_a(arch_val->ramas, 0); // objeto

        // Convenciones
        size_t n_convs = size_a(arch_obj->ramas);
        for (size_t j = 0; j < n_convs; ++j) {
            ast_node_t* conv_key = (ast_node_t*)get_element_a(arch_obj->ramas, j);
            const char* conv_name = (const char*)conv_key->data;
            if (!conv_name) continue;
            if (size_a(conv_key->ramas) == 0) continue;
            ast_node_t* conv_val = (ast_node_t*)get_element_a(conv_key->ramas, 0);
            if (size_a(conv_val->ramas) == 0) continue;
            ast_node_t* conv_obj = (ast_node_t*)get_element_a(conv_val->ramas, 0);

            // Categorías (ej: "number")
            size_t n_cats = size_a(conv_obj->ramas);
            for (size_t k = 0; k < n_cats; ++k) {
                ast_node_t* cat_key = (ast_node_t*)get_element_a(conv_obj->ramas, k);
                const char* cat_name = (const char*)cat_key->data;
                if (!cat_name) continue;
                if (size_a(cat_key->ramas) == 0) continue;
                ast_node_t* cat_val = (ast_node_t*)get_element_a(cat_key->ramas, 0);
                if (size_a(cat_val->ramas) == 0) continue;
                ast_node_t* cat_obj = (ast_node_t*)get_element_a(cat_val->ramas, 0);

                // Tipos de dato (ej: "uint64_t", "float", ...)
                size_t n_types = size_a(cat_obj->ramas);
                for (size_t t = 0; t < n_types; ++t) {
                    ast_node_t* type_key = (ast_node_t*)get_element_a(cat_obj->ramas, t);
                    const char* type_name = (const char*)type_key->data;
                    if (!type_name) continue;
                    if (size_a(type_key->ramas) == 0) continue;
                    ast_node_t* type_val = (ast_node_t*)get_element_a(type_key->ramas, 0);
                    if (size_a(type_val->ramas) == 0) continue;
                    ast_node_t* regs_array = (ast_node_t*)get_element_a(type_val->ramas, 0);

                    size_t nregs = size_a(regs_array->ramas);
                    const char** regs = malloc((nregs + 1) * sizeof(char*));
                    for (size_t r = 0; r < nregs; ++r) {
                        ast_node_t* reg_node = (ast_node_t*)get_element_a(regs_array->ramas, r);
                        regs[r] = (const char*)reg_node->data;
                    }
                    regs[nregs] = NULL;


                    // Prepara la estructura
                    type_data_json_t *data = calloc(sizeof(type_data_json_t), 1);
                    data->conv = conv_name;
                    data->name_type = type_name;
                    data->err_code = 0;
                    data->data_conv = regs;
                    data->cat_name = cat_name;
                    data->arch = arch_name;

                    func(data, user_data);
                }
            }
        }
    }
}

// Libera un array de type_data_json_t
void free_type_data_json_array(type_data_json_t* arr, size_t n) {
    if (!arr) return;
    for (size_t i = 0; i < n; ++i) {
        if (arr[i].data_conv) free((void*)arr[i].data_conv);
    }
    free(arr);
}


// Helper: Salta clave -> : -> valor
static ast_node_t* get_value_node(ast_node_t* key_node) {
    if (!key_node || size_a(key_node->ramas) == 0) return NULL;
    ast_node_t* colon_node = (ast_node_t*)get_element_a(key_node->ramas, 0);
    if (!colon_node || size_a(colon_node->ramas) == 0) return NULL;
    return (ast_node_t*)get_element_a(colon_node->ramas, 0);
}

static inline void os_constructor(void) { 
    puts("--- os_constructor ---"); 
    printf("- type_arch: %s\n", arch_type_string[info_module.arch]);
    printf("- type_os:   %s\n", os_type_string[info_module.os]);

    json_specs = json_load_file("json_specs.abimap.json");
    if (!json_specs) {
        puts("json_specs.abimap.json no encontrado");
        exit(1);
    }


    build_type_data_json_array(json_specs, (void*)print_type_data_json_with_free, NULL);

    ArrayList*arr = createArrayList(0, NULL);
    build_type_data_json_array(json_specs, (void*)push_type_data_json, arr);
    for (size_t i = 0; i < size_a(arr); ++i) {
        print_type_data_json_with_free(arr->Array[i], NULL);
    }
    freeArrayList_struct(&arr);

}
static inline void os_destructor(void) { 
    puts("--- os_destructor  ---"); 
    json_free(json_specs);
}

/**
 * @brief permite registrar subrutinas
 * constructoras y destructoras
 */
void register_init(void) {
    constructor = os_constructor;
    destructor  = os_destructor;
}






DLL_EXPORT void who_system() {
    code = init_shellcode();

    shellcode_t *ptr_sc = &code;
    ptr_sc->capacity = 16;
    call(ptr_sc, expand);

    printf("who_system!\n");
    EMIT_MOV_R_I(ptr_sc, RBX, 0x12345678deadbeefull);
}

DLL_EXPORT bool queery_arch(const char* name_arch) {
    // si se obtiene algo distinto a NULL es que la arch existe en el json

    return json_get_string(json_specs, name_arch) != NULL;
}

DLL_EXPORT const char *getBuild() { //Get current architecture, detectx nearly every architecture. Coded by Freak
    #if defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
    #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    return "x86_32";
    #elif defined(__ARM_ARCH_2__)
    return "ARM2";
    #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
    return "ARM3";
    #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
    return "ARM4T";
    #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
    return "ARM5"
    #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
    return "ARM6T2";
    #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
    return "ARM6";
    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    return "ARM7";
    #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    return "ARM7A";
    #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    return "ARM7R";
    #elif defined(__ARM_ARCH_7M__)
    return "ARM7M";
    #elif defined(__ARM_ARCH_7S__)
    return "ARM7S";
    #elif defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64";
    #elif defined(mips) || defined(__mips__) || defined(__mips)
    return "MIPS";
    #elif defined(__sh__)
    return "SUPERH";
    #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
    return "POWERPC";
    #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
    return "POWERPC64";
    #elif defined(__sparc__) || defined(__sparc)
    return "SPARC";
    #elif defined(__m68k__)
    return "M68K";
    #else
    return "UNKNOWN";
    #endif
}
