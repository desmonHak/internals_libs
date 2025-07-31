#include "x86_64.h"

int main() {
    char *error = NULL;

    din_lib handle = load_lib("./x86_64");
    if (!handle) {
        get_error_lib(error);
        fprintf(stderr, "No se pudo cargar la SO: %s\n", error);
        return 1;
    }

    get_error_lib(error); // Limpiar errores previos
    printf("./x86_64" EXT_LIB " se cargo correctamente.\n");




    f_isa_lib_name isa_lib_name = (f_isa_lib_name)load_simbol(handle, "get_isa_lib_name");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion get_isa_lib_name(%p): %s\n", isa_lib_name, error);
        close_lib(handle);
        return 1;
    }
    printf("La ISA de esta libreria es: %s\n", isa_lib_name());


    destructor_t x86_64_destructor = (destructor_t)load_simbol(handle, "dtor__");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion x86_64_destructor(%p): %s\n", x86_64_destructor, error);
        close_lib(handle);
        return 1;
    }
    if (x86_64_destructor != NULL) x86_64_destructor();
    else puts("not found x86_64_destructor");


    typedef const char*(*f_get_str_register)(t_registers_x86_64);
    f_get_str_register* get_str_register = (f_get_str_register)load_simbol(handle, "get_str_register");
    get_error_lib(error);
    if (error != NULL) {
        fprintf(stderr, "No se pudo obtener la funcion get_str_register(%p): %s\n", get_str_register, error);
        close_lib(handle);
        return 1;
    }

    printf("id %d = %s\n", r_ah, ((f_get_str_register)get_str_register)(r_ah));

    close_lib(handle);
    puts("Exit...");
    return 0;
}