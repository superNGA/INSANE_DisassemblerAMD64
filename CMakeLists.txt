cmake_minimum_required(VERSION 3.10)

project("INSANE_DisassemblerAMD64" LANGUAGES CXX ASM_NASM)
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)

set(ASM_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Math/SafeBitWiseOps.asm
)

add_executable (INSANE_DisassemblerAMD64 
${ASM_SOURCES}

# Include
"Include/INSANE_DisassemblerAMD64.h"
"Include/Instruction_t.h"
"Include/Rules.h"
"Include/Aliases.h"
"Include/DASMInst_t.h"
"Include/Masks.h"

# Include/Standard
"Include/Standard/Displacement_t.h"
"Include/Standard/Immediate_t.h"
"Include/Standard/ModRM_t.h"
"Include/Standard/OpCode_t.h"
"Include/Standard/Register_t.h"
"Include/Standard/SIB_t.h"
"Include/Standard/OpCodeDesc_t.h"

# Include/Standard/Operand
"Include/Standard/Operand/CEOperandModes.h"
"Include/Standard/Operand/CEOperandTypes.h"
"Include/Standard/Operand/OperandModes.h"
"Include/Standard/Operand/OperandTypes.h"
"Include/Standard/Operand/Operand_t.h"

# Include/Legacy
"Include/Legacy/LegacyInst_t.h"
"Include/Legacy/LegacyOpCode_t.h"
"Include/Legacy/LegacyPrefix.h"

# Include/VEX
"Include/VEX/VEXInst_t.h"
"Include/VEX/VEXPrefix_t.h"

"main.cpp"
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
"src/Decoder/Decoder.h"
"src/Decoder/Decoder.cpp"

# src/Disassembler
"src/Disassembler/Disassembler.h"
"src/Disassembler/Disassembler.cpp"
"src/Disassembler/InstSummary_t.h"

# src/Defs/VEX
"src/Defs/VEX/VEXInst_t.cpp"
"src/Defs/VEX/VEXPrefix_t.cpp"

# Math
"src/Math/SafeBitWiseOps.h" 

# Util
"src/Util/TestCases/TestCases.h" "src/Util/TestCases/TestCases.cpp" 
"src/Util/Terminal/Terminal.h"
"src/Util/Terminal/ConsoleSystem.cpp" "src/Util/Terminal/ConsoleSystem.h")

target_compile_features(INSANE_DisassemblerAMD64 PRIVATE cxx_std_17)
