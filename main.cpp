#include <iostream>
#include <vector>

#include "Include/INSANE_DisassemblerAMD64.h"
#include "src/Math/SafeBitWiseOps.h"

// For testing purposes.
#include "src/Util/TestCases/TestCases.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    std::vector<std::string> vecOutput;
    vecOutput.clear();


    // Initialize disassembler.
    {
        InsaneDASM64::IDASMErrorCode_t iErrorCode = InsaneDASM64::Initialize();
        if (iErrorCode == InsaneDASM64::IDASMErrorCode_FailedInit)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }

    // std::vector<InsaneDASM64::Byte> vecInput = { 0x48, 0x89, 0xC8, 0x48, 0x21, 0xD0, 0xC3 };
    // Disassemble...
    {
        InsaneDASM64::IDASMErrorCode_t iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecOneByteOpCodes_002/*vecInput*/, vecOutput);

        if (iErrorCode != InsaneDASM64::IDASMErrorCode_t::IDASMErrorCode_Success)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    return 0;
}