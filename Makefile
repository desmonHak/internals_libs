LIBS = os

include config.mk

CC = gcc



# Compilar el ejecutable principal
$(EXE_FILE): $(EXE_OBJS)
	$(CC) $(CFLAGS_EXEC) -o $@ $^

# --- TESTS AUTOMÁTICOS ---

# Regla general para cada ejecutable de test
# Cada test debe tener su propio archivo: os_test.c, mylib_test.c, etc.
tests: generate_lib_static generate_lib_module $(TEST_EXES)

# Regla para compilar ejecutables de test
%_test.$(EXTENSION_EXEC): $(PATH_tests)/%_test.o
	$(CC) $(CFLAGS_EXEC) -o $@ $^


# Regla para compilar objetos de test
%_test.o: $(PATH_tests)/%_test.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) *.o *.$(EXTENSION_EXEC) *.$(EXTENSION_LIB)
	$(MAKE) -C ./$(PATH_Emmitx86) -f $(OS_NAME).mk cleanall
	$(MAKE) -C ./$(PATH_lexpar_c) -f $(OS_NAME).mk cleanall