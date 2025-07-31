

#include "os.h"
#include <stdio.h>

#include "isa.h"

DLL_EXPORT extern int mi_variable_global;

int add(int n1, int n2) {
    int i = n1 + n2;
    printf("sumando n1 + n2 = %d\n", i);
    return i;
}

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





    din_lib lib_isa = *(din_lib*)load_simbol(handle, "lib_isa");
    if (lib_isa == NULL) {
        puts("Error get address of lib_isa");
    } else {
        printf("addr base lib isa: %p\n", (void*)lib_isa);
        f_isa_lib_name isa_lib_name = (f_isa_lib_name)load_simbol(lib_isa, "get_isa_lib_name");
        get_error_lib(error);
        if (error != NULL) {
            fprintf(stderr, "No se pudo obtener la libreria(lib_isa): %s\n", error);
        }
        printf("La ISA de esta libreria es: %s\n", isa_lib_name());

        f_invoke_callthblock invoke_callthblock = (f_invoke_callthblock)load_simbol(lib_isa, "invoke_callthblock");
        get_error_lib(error);
        if (error != NULL) {
            fprintf(stderr, "No se pudo obtener la libreria(lib_isa): %s\n", error);
        }

        ThreadVM_t thread = {0};
        thread.pending_call = calloc(1, sizeof(PendingCall_t));

        thread.pending_call->func = add;
        thread.pending_call->arg  = ptr_sc;
        EMIT_MOV_R_I(thread.pending_call->arg, RCX, 2);
        EMIT_MOV_R_I(thread.pending_call->arg, RDX, 3);
        printf("Llamando a la funcion 0x%p, almacenando resultado en 0x%p\n", add, &(thread.pending_call->result));

        if (invoke_callthblock == NULL) {
            puts("Error call invoke_callthblock");
        } else invoke_callthblock(ptr_sc, &thread);

        if (thread.err_thread == THREAD_UNKNOWN_ERROR) {
            puts("Error al crear el hilo");
        }
        _sleep(2000);

        free(thread.pending_call);

    }

    call(ptr_sc, dump);



    destructor_t os_destructor = (destructor_t)load_simbol(handle, "dtor__");
    get_error_lib(error);
    if (error != NULL || os_destructor == NULL) {
        fprintf(stderr, "No se pudo obtener la funcion os_destructor(%p): %s\n", os_destructor, error);
        close_lib(handle);
        return 1;
    }
    os_destructor();

    close_lib(handle);
    puts("Exit...");
    return 0;
}
