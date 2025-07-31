#include "din_lib_c.h"
#include "x86_64.h"

#include "init.c"


DLL_EXPORT void x86_64_constructor(void) {
    puts("--- x86_64_constructor ---");
}

DLL_EXPORT void x86_64_destructor(void) {
    puts("--- x86_64_destructor  ---");
}

/**
 *
 * @return retorna la ISA que procesa esta libreria dinamica
 */
DLL_EXPORT const char* get_isa_lib_name(void) {
    return "x86_64";
}

static const char* strings_registers[] = {
    [r_al]      = "al",  [r_ah] = "ah",
    [r_bl]      = "bl",  [r_bh] = "bh",
    [r_cl]      = "cl",  [r_ch] = "ch",
    [r_dl]      = "dl",  [r_dh] = "dh",
    [r_sil]     = "sil",
    [r_dil]     = "dil",
    [r_bpl]     = "bpl",
    [r_spl]     = "spl",
    [r_ipl]     = "ipl",
    [r_r8b]     = "r8b",
    [r_r9b]     = "r9b",
    [r_r10b]    = "r10b",
    [r_r11b]    = "r11b",
    [r_r12b]    = "r12b",
    [r_r13b]    = "r13b",
    [r_r14b]    = "r14b",
    [r_r15b]    = "r15b",

    [r_ax]      = "ax",
    [r_bx]      = "bx",
    [r_cx]      = "cx",
    [r_dx]      = "dx",
    [r_si]      = "si",
    [r_di]      = "di",
    [r_bp]      = "bp",
    [r_sp]      = "sp",
    [r_ip]      = "ip",
    [r_r8w]     = "r8w",
    [r_r9w]     = "r9w",
    [r_r10w]    = "r10w",
    [r_r11w]    = "r11w",
    [r_r12w]    = "r12w",
    [r_r13w]    = "r13w",
    [r_r14w]    = "r14w",
    [r_r15w]    = "r15w",

    [r_eax]     = "eax",
    [r_ebx]     = "ebx",
    [r_ecx]     = "ecx",
    [r_edx]     = "edx",
    [r_esi]     = "esi",
    [r_edi]     = "edi",
    [r_ebp]     = "ebp",
    [r_esp]     = "esp",
    [r_eip]     = "eip",
    [r_r8d]     = "r8d",
    [r_r9d]     = "r9d",
    [r_r10d]    = "r10d",
    [r_r11d]    = "r11d",
    [r_r12d]    = "r12d",
    [r_r13d]    = "r13d",
    [r_r14d]    = "r14d",
    [r_r15d]    = "r15d",

    [r_rax]     = "rax",
    [r_rbx]     = "rbx",
    [r_rcx]     = "rcx",
    [r_rdx]     = "rdx",
    [r_rsi]     = "rsi",
    [r_rdi]     = "rdi",
    [r_rbp]     = "rbp",
    [r_rsp]     = "rsp",
    [r_rip]     = "rip",
    [r_r8]      = "r8",
    [r_r9]      = "r9",
    [r_r10]     = "r10",
    [r_r11]     = "r11",
    [r_r12]     = "r12",
    [r_r13]     = "r13",
    [r_r14]     = "r14",
    [r_r15]     = "r15",
    [r_unknown] = "unknown"
};

DLL_EXPORT const char* get_str_register(t_registers_x86_64 reg) {
    if (reg >= r_al && reg <= r_r15) return strings_registers[reg];
    else return strings_registers[r_unknown];
}

DLL_EXPORT t_registers_x86_64 get_id_register(char* const reg) {
    for (uint8_t i = 0; i < r_r15; i++) {
        if (strcmp(reg, strings_registers[i]) == 0) {
            return i;
        }
    }
    return r_unknown;
}

/**
 * @brief permite registrar subrutinas
 * constructoras y destructoras
 */
void register_init(void) {
    constructor = x86_64_constructor;
    destructor  = x86_64_destructor;
}

void *allocate_executable_buffer(size_t size) {
    void *mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!mem) {
        // error
        return NULL;
    }
    return mem;
}

/**
 * Función que ejecutará el hilo real, una vez esta funcion acabe,
 * el estado de la llamada se marcada como acabado, el hilo
 * seguira bloqueado hasta que la VM, la funcion `check_pending_calls`
 * observe que este hilo termino su ejecuccion, en ese caso, volvera
 * a marcar el hilo como READY listo para la siguiente ejecuccion.
 */
static void *call_external_function(void *arg) {
    PendingCall_t *call_data = (PendingCall_t *)arg;
    pthread_mutex_lock(&call_data->lock);

    // debemos emitir codigo para añadir la funcion a llamar
    // emitimos un `mov r12, <direccion de funcion>`
    EMIT_MOV_R_I(call_data->arg, R12, (size_t)call_data->func);

    // emitimos un `call r12`
    call(call_data->arg, Emit8, 0x41);
    call(call_data->arg, Emit8, 0xff);
    call(call_data->arg, Emit8, 0xd4);


    // emitimos `mov r11, <direccion de donde guardar resultado>`
    EMIT_MOV_R_I(call_data->arg, R11, &(call_data->result));

    // emitimos un `mov [r11], rax`, se espera guardar en R11
    // una direccion de variable
    EMIT_M_R(call_data->arg, MOV, R11, RAX);
    call(call_data->arg, dump);

    void *exec_mem = allocate_executable_buffer(call_data->arg->size);
    if (!exec_mem) {
        puts("Error al crear memoria ejecutable");
    }

    ((void (*)())exec_mem)();

    VirtualFree(exec_mem, 0, MEM_RELEASE);


    // copiar el código
    memcpy(exec_mem, call_data->arg, call_data->arg->size);

    call_data->finished = true;
    pthread_mutex_unlock(&call_data->lock);

    return NULL;
}



/**
 * permite invocar en la VM un callthblock, esto bloqueara el hilo virtual
 * que realizo la llamada, debe haberse emitido en `ptr_sc` el codigo
 * necesario para realizar la llamada a la funcion externa, en el proceso
 * se crear un hilo real temporal.
 *
 * La llamada externa debe estar en el miembro `pending_call` de la estructura `ThreadVM_t`,
 * y se debera depositar aqui el valor de retorno.
 *
 * @param ptr_sc
 * @param virtual_thread
 */
DLL_EXPORT void invoke_callthblock(shellcode_t *ptr_sc,  ThreadVM_t *virtual_thread) {
    virtual_thread->state_thread = BLOCKED;          // bloqueamos este hilo
    virtual_thread->err_thread   = THREAD_NO_ERROR;  // indicamos que no hay errores.

    pthread_t thread;
    if (pthread_create(
        &thread, NULL, call_external_function,
        virtual_thread->pending_call // argumentos para la funcion llamada
    ) != 0) {
        virtual_thread->err_thread      = THREAD_UNKNOWN_ERROR;
        virtual_thread->state_thread    = DEAD;
        return;
    }

    pthread_detach(thread); // Desacoplamos el hilo real, no hay join

}

