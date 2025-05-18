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

    who_system();

    close_lib(handle);
    return 0;
}
