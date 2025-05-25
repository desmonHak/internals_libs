cmake_minimum_required(VERSION 3.15)
project(MyProject C)

# Detectar sistema operativo
if(WIN32)
    set(OS_NAME "windows")
    set(SHARED_LIB_EXT "dll")
    set(EXEC_EXT "exe")
    set(CFLAGS -Wall -O2)
    set(DLL_EXPORT_MACRO "-DBUILDING_DIN_LYB")
else()
    # Linux o macOS
    if(APPLE)
        set(OS_NAME "macos")
    else()
        set(OS_NAME "linux")
    endif()
    set(SHARED_LIB_EXT "so")
    set(EXEC_EXT "elf")
    set(CFLAGS -Wall -O2 -fPIC)
    set(DLL_EXPORT_MACRO "-DBUILDING_DIN_LYB")
endif()

# Paths base
set(PATH_tests "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(PATH_os "${CMAKE_CURRENT_SOURCE_DIR}/os")
set(PATH_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(PATH_Emmitx86 "${PATH_LIBRARY}/Emmitx86")
set(PATH_lexpar_c "${PATH_LIBRARY}/lexpar_c")
set(PATH_structs_data_C "${PATH_lexpar_c}/lib/structs_data_C")
set(PATH_DebugLibC "${PATH_structs_data_C}/DebugLibC")
set(PATH_colors_C_C_plus_plus "${PATH_DebugLibC}/colors-C-C-plus-plus")
set(PATH_din_lib_c "${CMAKE_CURRENT_SOURCE_DIR}/din_lib_c")

# Includes
include_directories(
        ${PATH_os}
        ${PATH_din_lib_c}
        ${PATH_Emmitx86}/include
        ${PATH_lexpar_c}/include
        ${PATH_structs_data_C}
        ${PATH_DebugLibC}
        ${PATH_colors_C_C_plus_plus}
)

# Compiler flags
add_compile_options(${CFLAGS} ${DLL_EXPORT_MACRO})

# Linker flags (donde buscar las librerias estaticas)
link_directories(${PATH_Emmitx86} ${PATH_lexpar_c} ${PATH_structs_data_C} ${PATH_colors_C_C_plus_plus})
# Librerías
set(COMMON_LIBS Emmitx86 lexpar_c structs_data_c colors)


# --- Librerías dinámicas ---
set(LIBS os)
set(DLL_NAMES "")
foreach(lib IN LISTS LIBS)
    list(APPEND DLL_NAMES "${lib}.${SHARED_LIB_EXT}")
endforeach()

# Objetos
set(EXE_NAME "main_executable") # Cambia si tienes otro nombre
set(EXE_SRC "${EXE_NAME}.c")
set(EXE_OBJ "${EXE_NAME}.o")

# Ejecutable principal
# add_executable(${EXE_NAME} ${EXE_SRC})
# target_link_libraries(${EXE_NAME} ${COMMON_LIBS})

# --- Librerías compartidas ---
foreach(lib IN LISTS LIBS)
    if(lib STREQUAL "os")
        add_library(${lib} SHARED
                "${PATH_os}/${lib}.c"
                ${LEXPAR_C_SOURCES}
        )
        # Ahora que 'os' existe, se puede agregar dependencia y linkear
        add_dependencies(${lib} generate_lib_static_Emmitx86)
        add_dependencies(${lib} generate_lib_static_lexpar_c)
        target_link_libraries(${lib} ${COMMON_LIBS})
    else()
        add_library(${lib} SHARED "${PATH_os}/${lib}.c")
        target_link_libraries(${lib} ${COMMON_LIBS})
    endif()

    target_compile_definitions(${lib} PRIVATE BUILDING_DIN_LYB)
    target_include_directories(${lib} PRIVATE
            ${PATH_os}
            ${PATH_din_lib_c}
            ${PATH_Emmitx86}/include
            ${PATH_lexpar_c}/include
            ${PATH_structs_data_C}
            ${PATH_DebugLibC}
            ${PATH_colors_C_C_plus_plus}
    )
    set_target_properties(${lib} PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${lib}
            SUFFIX ".${SHARED_LIB_EXT}"
    )
endforeach()

file(GLOB LEXPAR_C_SOURCES "${PATH_lexpar_c}/*.c")

# --- Tests ---
set(TEST_EXES "")
foreach(lib IN LISTS LIBS)
    set(test_exe "${lib}_test")
    add_executable(${test_exe} "${PATH_tests}/${lib}_test.c")
    target_include_directories(${test_exe} PRIVATE
            ${PATH_os}
            ${PATH_din_lib_c}
            ${PATH_Emmitx86}/include
            ${PATH_lexpar_c}/include
            ${PATH_structs_data_C}
            ${PATH_DebugLibC}
            ${PATH_colors_C_C_plus_plus}
    )
    target_link_libraries(${test_exe} ${COMMON_LIBS})
    list(APPEND TEST_EXES ${test_exe})
endforeach()

# --- Custom target: tests ---
add_custom_target(tests DEPENDS ${TEST_EXES})

# --- Generar librería estática libEmmitx86.a ---
add_custom_target(generate_lib_static_Emmitx86
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_Emmitx86} -f ${OS_NAME}.mk
        COMMENT "Generando libEmmitx86.a ejecutando make en ${PATH_Emmitx86}"
)

# --- Generar librería estática liblexpar_c.a ---
add_custom_target(generate_lib_static_lexpar_c
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_lexpar_c} -f ${OS_NAME}.mk generate_lib
        COMMENT "Generando liblexpar_c.a ejecutando make en ${PATH_lexpar_c}"
)

# --- Generar librería estática libstructs_data_c.a ---
add_custom_target(generate_lib_static_structs_data_c
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_structs_data_C} -f ${OS_NAME}.mk generate_lib
        COMMENT "Generando libstructs_data_c.a ejecutando make en ${PATH_structs_data_C}"
)

# --- Generar librería estática libcolors.a ---
add_custom_target(generate_lib_static_libcolors
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_colors_C_C_plus_plus} -f ${OS_NAME}.mk generate_lib
        COMMENT "Generando libcolors.a ejecutando make en ${PATH_colors_C_C_plus_plus}"
)

# --- Generar librerías compartidas ---
add_custom_target(generate_lib_module DEPENDS ${DLL_NAMES})

# --- all ---
add_custom_target(build_all DEPENDS generate_lib_module tests)
add_custom_target(default ALL DEPENDS build_all)

# --- clean ---
add_custom_target(clean_all
        COMMAND ${CMAKE_COMMAND} -E rm -f *.o *.${EXEC_EXT} *.${SHARED_LIB_EXT}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_Emmitx86} -f ${OS_NAME}.mk cleanall
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${PATH_lexpar_c} -f ${OS_NAME}.mk cleanall
        COMMENT "Limpiando archivos objeto y ejecutables"
)
