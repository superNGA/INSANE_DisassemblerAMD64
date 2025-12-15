#include <iostream>
#include <vector>

#include "Include/INSANE_DisassemblerAMD64.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Input & Output data for disassembler.
    //std::vector<InsaneDASM64::Byte> vecInput = { 0x48, 0x83, 0xEC, 0x28, 0xC7, 0x44, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xEF, 0xCE, 0xFF, 0xFF, 0x24, 0x01 };
    std::vector<InsaneDASM64::Byte> vecInput = {
        //0x55,
        0x48, 0x89, 0xE5,
        0x48, 0x83, 0xEC, 0x20,
        0xB8, 0x01, 0x00, 0x00, 0x00,
        0x89, 0x45, 0xFC,
        0x83, 0xC0, 0x02,
        0x83, 0xF8, 0x03,
        0x75, 0x05,
        0xB8, 0x00, 0x00, 0x00, 0x00,
        0xC9,
        0xC3
    };

    std::vector<InsaneDASM64::Instruction_t> vecOutput;
    vecOutput.clear();


    // Initialize disassembler.
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Initialize();
        if (iErrorCode == InsaneDASM64::ErrorCode_FailedInit)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    // Disassemble...
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Disassemble(vecInput, vecOutput);

        if (iErrorCode != InsaneDASM64::ErrorCode_t::ErrorCode_Success)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    /*for (int i = 0; i < 10; i++)
        printf("%s\n", InsaneDASM64::GetErrorMessage(static_cast<InsaneDASM64::ErrorCode_t>(i)));*/


    return 0;
}