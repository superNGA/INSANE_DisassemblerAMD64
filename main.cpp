#include <iostream>
#include <vector>

#include "Include/INSANE_DisassemblerAMD64.h"

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

    
    std::vector<InsaneDASM64::Byte> vecInput = { 0x48, 0x89, 0xC8, 0x48, 0x21, 0xD0, 0xC3 };
    // Disassemble...
    {
        InsaneDASM64::IDASMErrorCode_t iErrorCode = InsaneDASM64::IDASMErrorCode_t::IDASMErrorCode_Success;

        iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecVEXTestCase_001, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(vecInput, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecOneByteOpCodes_001, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecOneByteOpCodes_002, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecTwoByteOpCodes_001, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecThreeByteOpCodes_38, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecThreeByteOpCodes_3A, vecOutput);
        // iErrorCode = InsaneDASM64::DecodeAndDisassemble(InsaneDASM64::TestCases::g_vecBubbleSortAsm, vecOutput);

        if (iErrorCode != InsaneDASM64::IDASMErrorCode_t::IDASMErrorCode_Success)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    return 0;
}
