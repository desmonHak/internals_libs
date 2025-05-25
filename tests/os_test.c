#include "os.h"
#include <stdio.h>

DLL_EXPORT extern int mi_variable_global;

int main() {
    char *error = NULL;

    din_lib handle = load_lib("./os");
    if (!handle) {
        get_error_lib(error);
        fprintf(stderr, "No se pudo cargar la SO: %s\n", error);
        return 1;
    }

    get_error_lib(error); // Limpiar errores previos
    printf("./os" EXT_LIB " se cargo correctamente.\n");

    who_system_t who_system = (who_system_t)load_simbol(handle, "who_system");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion who_system: %s\n", error);
        close_lib(handle);
        return 1;
    }

    queery_arch_t queery_arch = (queery_arch_t)load_simbol(handle, "queery_arch");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion who_system: %s\n", error);
        close_lib(handle);
        return 1;
    }

    who_system();

    printf("Esta x86_64 ? = %d\n", queery_arch("x86_64"));

    shellcode_t *ptr_sc = (shellcode_t *)load_simbol(handle, "code");

    call(ptr_sc, dump);

    destructor_t os_destructor = (destructor_t)load_simbol(handle, "dtor__");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion os_destructor(%p): %s\n", os_destructor, error);
        close_lib(handle);
        return 1;
    }
    os_destructor();

    close_lib(handle);
    puts("Exit...");
    return 0;
}
