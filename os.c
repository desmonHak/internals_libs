#define BUILDING_DIN_LYB

#include "din_lib_c.h"
#include "os.h"

#include <stdio.h>



DLL_EXPORT arch_type_t type_arch = arch_type_unknown;

DLL_EXPORT shellcode_t code;

#include "init.c"

static inline void os_constructor(void) { 
    puts("hola desde el constructor"); 
}
static inline void os_destructor(void) { 
    puts("hola desde el destructor"); 
}

void register_os_init(void) {
    constructor = os_constructor;
    destructor = os_destructor;
}



DLL_EXPORT void who_system() {
    code = init_shellcode();

    shellcode_t *ptr_sc = &code;
    ptr_sc->capacity = 16;
    call(ptr_sc, expand);

    printf("who_system!\n");
    EMIT_MOV_R_I(ptr_sc, RBX, 0x12345678deadbeefull);
}
