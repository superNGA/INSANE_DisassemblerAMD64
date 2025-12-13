#include <iostream>
#include <vector>

#include "Include/INSANE_DisassemblerAMD64.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Input & Output data for disassembler.
    std::vector<InsaneDASM64::Byte> vecInput = { 0x48, 0x83, 0xEC, 0x28, 0xC7, 0x44, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xEF, 0xCE, 0xFF, 0xFF, 0x24, 0x01 };
    std::vector<InsaneDASM64::Instruction_t> vecOutput;
    vecOutput.clear();


    // Initialize disassembler.
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Initialize();
        if (iErrorCode == InsaneDASM64::ErrorCode_FailedInit)
        {
            std::cout << "Failed Initialization\n";
            return 1;
        }
    }


    // Disassemble...
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Disassemble(vecInput, vecOutput);

        if (iErrorCode != InsaneDASM64::ErrorCode_t::ErrorCode_Success)
        {
            std::cout << "Failed disassembly!!\n";
            return 1;
        }
    }


    return 0;
}