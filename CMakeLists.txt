cmake_minimum_required(VERSION 3.10)

project("INSANE_DisassemblerAMD64" LANGUAGES CXX ASM_NASM)
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)

set(ASM_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Math/SafeBitWiseOps.asm
)

add_executable (INSANE_DisassemblerAMD64 
${ASM_SOURCES}

"Include/INSANE_DisassemblerAMD64.h"
"main.cpp"
"src/INSANE_DisassemblerAMD64.cpp" 
"src/Tables/Tables.h" "src/Tables/Tables.cpp"  
"src/ObjectNomenclature.h"
"src/Math/SafeBitWiseOps.h" "src/Util/TestCases/TestCases.h" "src/Util/TestCases/TestCases.cpp")

target_compile_features(INSANE_DisassemblerAMD64 PRIVATE cxx_std_17)