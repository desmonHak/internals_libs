#ifndef ISA_H
#define ISA_H

#include "din_lib_c.h"

typedef const char* (*f_isa_lib_name)(void);
DLL_EXPORT const char* get_isa_lib_name(void);

#endif //ISA_H
