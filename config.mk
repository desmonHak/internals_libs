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

ifeq ($(OS_NAME),windows)
    EXTENSION_LIB  = dll
    EXTENSION_EXEC = exe
    CFLAGS         = -Wall -O2 -Idin_lib_c
    RM             = del /Q
    CFLAGS_EXEC    = $(CFLAGS)
    DLL_EXPORT_MACRO = -DBUILDING_DLL
else
    EXTENSION_LIB  = so
    EXTENSION_EXEC = elf
    CFLAGS         = -Wall -O2 -fPIC -Idin_lib_c
    RM             = rm -rf
    CFLAGS_EXEC    = $(CFLAGS) -ldl -Wl,-rpath=.
    DLL_EXPORT_MACRO = -DBUILDING_DLL
endif

# Genera nombres de archivos automáticamente
DLL_NAMES = $(foreach lib,$(LIBS),$(lib).$(EXTENSION_LIB))
DLL_OBJS  = $(foreach lib,$(LIBS),$(lib).o)
EXE_FILE  = $(EXE_NAME).$(EXTENSION_EXEC)
EXE_OBJS  = $(EXE_NAME).o

# Para tests: un ejecutable por cada librería dinámica
TEST_EXES = $(foreach lib,$(LIBS),$(lib)_test.$(EXTENSION_EXEC))
TEST_OBJS = $(foreach lib,$(LIBS),$(lib)_test.o)

.PHONY: all clean

all: $(DLL_NAMES) tests #$(EXE_FILE)

# Compilar la librería dinámica
%.dll: %.o
	$(CC) -Idin_lib_c -shared -o $@ $^

%.so: %.o
	$(CC) -Idin_lib_c -shared -o $@ $^

# Compilar el objeto de cada librería
os.o: os.c os.h
	$(CC) $(CFLAGS) -DBUILDING_OS -c $<