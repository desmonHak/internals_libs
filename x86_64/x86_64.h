#ifndef X86_64_H
#define X86_64_H

#include "x86/amd64/amd64.h"
#include "global_emmit.h"

#include "../../include/vm.h"

#include "isa.h"


#include "din_lib_c.h"

/* version
 * | version  | sub version |
 * 0b0000 0001 0000 0000
 */
#define VERSION_OS_MODULE 0b0000000100000000

typedef enum t_registers_x86_64 {
    // registers 8bits
    r_al, r_ah,
    r_bl, r_bh,
    r_cl, r_ch,
    r_dl, r_dh,
    r_sil,
    r_dil,
    r_bpl,
    r_spl,
    r_ipl,
    r_r8b,
    r_r9b,
    r_r10b,
    r_r11b,
    r_r12b,
    r_r13b,
    r_r14b,
    r_r15b,

    // 16 bits
    r_ax,
    r_bx,
    r_cx,
    r_dx,
    r_si,
    r_di,
    r_bp,
    r_sp,
    r_ip,
    r_r8w,
    r_r9w,
    r_r10w,
    r_r11w,
    r_r12w,
    r_r13w,
    r_r14w,
    r_r15w,

    // 32bits
    r_eax,
    r_ebx,
    r_ecx,
    r_edx,
    r_esi,
    r_edi,
    r_ebp,
    r_esp,
    r_eip,
    r_r8d,
    r_r9d,
    r_r10d,
    r_r11d,
    r_r12d,
    r_r13d,
    r_r14d,
    r_r15d,

    // 64bits
    r_rax,
    r_rbx,
    r_rcx,
    r_rdx,
    r_rsi,
    r_rdi,
    r_rbp,
    r_rsp,
    r_rip,
    r_r8,
    r_r9,
    r_r10,
    r_r11,
    r_r12,
    r_r13,
    r_r14,
    r_r15,
    r_unknown
} t_registers_x86_64;
DLL_EXPORT const char* get_str_register(t_registers_x86_64 reg);
DLL_EXPORT t_registers_x86_64 get_id_register(char* const reg);

DLL_EXPORT void invoke_callthblock(shellcode_t *ptr_sc,  ThreadVM_t *virtual_thread);

DLL_EXPORT void x86_64_constructor(void);
DLL_EXPORT void x86_64_destructor(void);
void register_init(void);

#endif //X86_64_H
