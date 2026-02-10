cmake_minimum_required(VERSION 3.10)

project("INSANE_DisassemblerAMD64" LANGUAGES CXX ASM_NASM)

message(STATUS "C compiler:       ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler:     ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID:      ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

enable_language(ASM_NASM)

if (WIN32)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
else()
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
endif()


add_library (${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/SafeBitWiseOps.asm

    "src/INSANE_DisassemblerAMD64.cpp" 
    "src/Tables/Tables.h" "src/Tables/Tables.cpp"  

    "src/Instruction_t.cpp"
    "src/DASMInst_t.cpp"

    # src/Defs/Operand
    "src/Defs/Standard/Operand/Operand_t.cpp"

    # src/Defs/Legacy
    "src/Defs/Legacy/LegacyInst_t.cpp"
    "src/Defs/Legacy/LegacyOpCode_t.cpp"
    "src/Defs/Legacy/LegacyPrefix_t.cpp"

    # src/Defs/Standard
    "src/Defs/Standard/Displacement_t.cpp"
    "src/Defs/Standard/Immediate_t.cpp"
    "src/Defs/Standard/ModRM_t.cpp"
    "src/Defs/Standard/OpCode_t.cpp"
    "src/Defs/Standard/Register_t.cpp"
    "src/Defs/Standard/SIB_t.cpp"
    "src/Defs/Standard/OpCodeDesc_t.cpp"

    # src/Decoder
    "src/Decoder/Decoder.cpp"

    # src/Disassembler
    "src/Disassembler/Disassembler.cpp"
    "src/Disassembler/InstSummary/InstSummary_t.cpp"

    # src/Defs/VEX
    "src/Defs/VEX/VEXInst_t.cpp"
    "src/Defs/VEX/VEXPrefix_t.cpp"

    # src/Defs/EVEX
    "src/Defs/EVEX/EVEXInst_t.cpp"
    "src/Defs/EVEX/EVEXPrefix_t.cpp"

    # Math
    "src/Math/SafeBitWiseOps.h" 

    # Util
    "src/Util/TestCases/TestCases.h" "src/Util/TestCases/TestCases.cpp" 
    "src/Util/Terminal/Terminal.h"
    "src/Util/Terminal/ConsoleSystem.cpp" "src/Util/Terminal/ConsoleSystem.h")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


# Building demo executable.
add_executable(IDasmDemo ${CMAKE_CURRENT_SOURCE_DIR}/Example/ExampleCLIDasm.cpp)
target_link_libraries(IDasmDemo PRIVATE ${PROJECT_NAME})
