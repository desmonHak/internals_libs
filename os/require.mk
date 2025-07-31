PATH_lexpar_c 				= $(PATH_LIBRARY)/lexpar_c
PATH_structs_data_C 		= $(PATH_lexpar_c)/lib/structs_data_C
PATH_DebugLibC				= $(PATH_structs_data_C)/DebugLibC
PATH_colors_C_C_plus_plus	= $(PATH_DebugLibC)/colors-C-C-plus-plus

OS_INCLUDES = 	-I$(PATH_lexpar_c)/include 				\
				-I$(PATH_structs_data_C)				\
				-I$(PATH_DebugLibC)						\
				-I$(PATH_colors_C_C_plus_plus)

LINKER_OS	= 	-L$(PATH_lexpar_c) 				-llexpar_c			\
				-L$(PATH_structs_data_C) 		-lstructs_data_c 	\
				-L$(PATH_DebugLibC)  			-ldebug 			\
				-L$(PATH_colors_C_C_plus_plus) 	-lcolors			

INCLUDE_FLAGS += $(OS_INCLUDES)

# Compilar el objeto de cada librería
os.o: $(PATH_os)/os.c $(PATH_os)/os.h libEmmitx86.a liblexpar.a
	$(CC) $(CFLAGS) -DBUILDING_DIN_LYB -DBUILDING_OS -c $< $(LINKER_OS)

liblexpar.a:
	$(MAKE) -C ./$(PATH_lexpar_c) -f $(OS_NAME).mk