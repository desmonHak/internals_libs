PATH_lexpar_c 				= $(PATH_LIBRARY)/lexpar_c
PATH_structs_data_C 		= $(PATH_lexpar_c)/lib/structs_data_C
PATH_DebugLibC				= $(PATH_structs_data_C)/DebugLibC
PATH_colors_C_C_plus_plus	= $(PATH_DebugLibC)/colors-C-C-plus-plus
PATH_isa					= ISA

X86_64_INCLUDES = 	-I$(PATH_lexpar_c)/include 			\
				-I$(PATH_isa)							\
				-I$(PATH_structs_data_C)				\
				-I$(PATH_DebugLibC)						\
				-I$(PATH_colors_C_C_plus_plus)			\
		

LINKER_X86_64	= 	-L$(PATH_lexpar_c) 		    -llexpar_c			\
				-L$(PATH_structs_data_C) 		-lstructs_data_c 	\
				-L$(PATH_DebugLibC)  			-ldebug 			\
				-L$(PATH_colors_C_C_plus_plus) 	-lcolors \
			

ifeq ($(OS),Windows_NT)
LINKER_X86_64 += -lwinpthread
else
LINKER_X86_64 += -lpthread
endif


INCLUDE_FLAGS += $(X86_64_INCLUDES)

# Compilar el objeto de cada librería
x86_64.o: $(PATH_x86_64)/x86_64.c $(PATH_x86_64)/x86_64.h libEmmitx86.a liblexpar.a
	$(CC) $(CFLAGS) -DBUILDING_DIN_LYB -DBUILDING_X86_64 -c $< $(LINKER_X86_64)

liblexpar.a:
	$(MAKE) -C ./$(PATH_lexpar_c) -f $(OS_NAME).mk