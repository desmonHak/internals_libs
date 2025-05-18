ifeq ($(OS),Windows_NT)
    OS_NAME := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS_NAME := linux
    endif
    ifeq ($(UNAME_S),Darwin)
        OS_NAME := macos
    endif
endif

# Genera nombres de archivos automáticamente
DLL_NAMES = $(foreach lib,$(LIBS),$(lib).$(EXTENSION_LIB))
DLL_OBJS  = $(foreach lib,$(LIBS),$(lib).o)
EXE_FILE  = $(EXE_NAME).$(EXTENSION_EXEC)
EXE_OBJS  = $(EXE_NAME).o

# Para tests: un ejecutable por cada librería dinámica
TEST_EXES = $(foreach lib,$(LIBS),$(lib)_test.$(EXTENSION_EXEC))
TEST_OBJS = $(foreach lib,$(LIBS),$(lib)_test.o)

PATH_tests              = tests
PATH_os                 = os
PATH_LIBRARY            = lib
PATH_Emmitx86			= $(PATH_LIBRARY)/Emmitx86
PATH_din_lib_c          = din_lib_c 

INCLUDE_FLAGS = 										\
	-I.													\
    -I$(PATH_os)                                        \
    -I$(PATH_din_lib_c)                                 \
	-I$(PATH_Emmitx86)/include

LINKER_FLAGS  	    = 									\
	-L$(PATH_Emmitx86)			-lEmmitx86

ifeq ($(OS_NAME),windows)
    EXTENSION_LIB  = dll
    EXTENSION_EXEC = exe
    CFLAGS         = -Wall -O2 $(INCLUDE_FLAGS) 
    RM             = del /Q
    CFLAGS_EXEC    = $(CFLAGS)
    # esta macro es necesaria al compilar las dlls para indicar que las funciones
    # se exportan y no se importan
    DLL_EXPORT_MACRO = -DBUILDING_DIN_LYB
else
    EXTENSION_LIB  = so
    EXTENSION_EXEC = elf
    CFLAGS         = -Wall -O2 -fPIC $(INCLUDE_FLAGS) 
    RM             = rm -rf
    CFLAGS_EXEC    = $(CFLAGS) -ldl -Wl,-rpath=.
    DLL_EXPORT_MACRO = -DBUILDING_DIN_LYB
endif

.PHONY: all clean tests generate_lib_module generate_lib_static

generate_lib_module: $(DLL_NAMES)

all: generate_lib_module tests #$(EXE_FILE)

# Compilar la librería dinámica
%.dll: %.o
	$(CC) $(DLL_EXPORT_MACRO) $(INCLUDE_FLAGS) $^ $(LINKER_FLAGS) -shared -o $@ 

%.so: %.o
	$(CC) $(DLL_EXPORT_MACRO) $(INCLUDE_FLAGS) $^ $(LINKER_FLAGS) -shared -o $@ 

# Compilar el objeto de cada librería
os.o: $(PATH_os)/os.c $(PATH_os)/os.h
	$(CC) $(CFLAGS) -DBUILDING_OS -c $<

generate_lib_static: libEmmitx86.a

libEmmitx86.a:
	$(MAKE) -C ./$(PATH_Emmitx86) -f $(OS_NAME).mk