cmake_minimum_required(VERSION 3.10)

project("INSANE_DisassemblerAMD64" LANGUAGES CXX)

add_executable (INSANE_DisassemblerAMD64 
"Include/INSANE_DisassemblerAMD64.h"
"main.cpp"
"src/INSANE_DisassemblerAMD64.cpp" 
"src/Tables/Tables.h" "src/Tables/Tables.cpp"  
"src/ObjectNomenclature.h"
)

target_compile_features(INSANE_DisassemblerAMD64 PRIVATE cxx_std_17)