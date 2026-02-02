#include <iostream>
#include <vector>
#include <assert.h>

#include "Include/INSANE_DisassemblerAMD64.h"
#include "Include/Legacy/LegacyInst_t.h"
#include "Include/Standard/OpCodeDesc_t.h"
#include "Include/VEX/VEXInst_t.h"
#include "Include/EVEX/EVEXInst_t.h"
#include "Include/Rules.h"

// For testing purposes.
#include "src/Util/TestCases/TestCases.h"
#include "src/Util/Terminal/Terminal.h"


using namespace InsaneDASM64;


static void PrintInst(std::vector<Instruction_t>& vecInst);
static void PrintLegacyInst(InsaneDASM64::Legacy::LegacyInst_t* pInst);
static void PrintVEXInst(InsaneDASM64::VEX::VEXInst_t* pInst);
static void PrintEVEXInst(EVEX::EVEXInst_t* pInst);
static void PrintOutput(std::vector<Instruction_t>& vecDecodedInst, std::vector<DASMInst_t>& vecInst);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int nArgs, char** szArgs)
{
    // Initialize disassembler.
    {
        InsaneDASM64::IDASMErrorCode_t iErrorCode = InsaneDASM64::Initialize();
        if (iErrorCode == InsaneDASM64::IDASMErrorCode_FailedInit)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }
    std::cout << "Disassembler Initialized!\n";

    // delete this...
    FILE* pFile = fopen(szArgs[1], "rb");
    if(pFile == nullptr)
        return 1;

    std::vector<Byte> vecFileInput; vecFileInput.clear();
    int iChar = EOF;
    while((iChar = getc(pFile)) != EOF)
    {
        vecFileInput.push_back(static_cast<Byte>(iChar & 0xFF));
    }

    LOG("%zu bytes from file %s", vecFileInput.size(), szArgs[1]);


    // Disassemble...
    {
        std::vector<Instruction_t> vecDecodedInst; vecDecodedInst.clear();
        std::vector<DASMInst_t>    vecDasmInst;    vecDasmInst.clear();


        // Decoding...
        IDASMErrorCode_t iDecodingErrCode = Decode(vecFileInput, vecDecodedInst, false);
        WIN_LOG("Decoded      [ %zu ] instructions.", vecDecodedInst.size());
        if(iDecodingErrCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        {
            printf("%s\n", GetErrorMessage(iDecodingErrCode));
            PrintInst(vecDecodedInst); // print whatever we got incase we fail.
            return 1;
        }


        // Disassemlbing...
        IDASMErrorCode_t iDASMErrCode = Disassemble(vecDecodedInst, vecDasmInst);
        PrintOutput(vecDecodedInst, vecDasmInst);

        if(iDASMErrCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        {
            printf("%s", GetErrorMessage(iDASMErrCode));
            return 1;
        }
        WIN_LOG("Disassembled [ %zu ] instructions.", vecDasmInst.size());


    }


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void PrintInst(std::vector<Instruction_t>& vecInst)
{
    for(Instruction_t& inst : vecInst)
    {
        switch (inst.m_iInstEncodingType) 
        {
            case Instruction_t::InstEncodingType_Legacy: PrintLegacyInst(reinterpret_cast<Legacy::LegacyInst_t*>(inst.m_pInst)); break;
            case Instruction_t::InstEncodingType_VEX:    PrintVEXInst(reinterpret_cast<VEX::VEXInst_t*>(inst.m_pInst));          break;
            case Instruction_t::InstEncodingType_EVEX:   PrintEVEXInst(reinterpret_cast<EVEX::EVEXInst_t*>(inst.m_pInst));       break;

            // Not done yet.
            case Instruction_t::InstEncodingType_XOP:
            case Instruction_t::InstEncodingType_Invalid:
            default: assert(false && "Invalid encoding type!"); break;
        }

        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintLegacyInst(InsaneDASM64::Legacy::LegacyInst_t* pInst)
{
    if (pInst->m_opCode.OpByteCount() == 0)
    {
        printf("EMTPY INSTRUCTION!\n");
        return;
    }


    printf("[ %12s ]", pInst->m_opCode.m_pOpCodeDesc->m_szName);

    for (int i = 0; i < Rules::MAX_LEGACY_PREFIX; i++)
    {
        if (i < pInst->m_legacyPrefix.m_nPrefix)
            printf("0x%02X ", pInst->m_legacyPrefix.m_legacyPrefix[i]);
        else
            printf("~~~~ ");
    }
    printf(" . ");


    if(pInst->m_bHasREX == true)
    {
        printf("0x%02X ", pInst->m_iREX);
    }
    else
    {
        printf("~~~~ ");
    }
    printf(" . ");


    for (int i = 0; i < Rules::MAX_OPBYTES; i++)
    {
        if(i < pInst->m_opCode.OpByteCount())
        {
            printf("0x%02X ", pInst->m_opCode.m_opBytes[i]);
        }
        else
        {
            printf("~~~~ ");
        }
    }
    printf(" . ");


    if (pInst->m_bHasModRM == true)
    {
        printf("0x%02X ", pInst->m_modrm.Get());
    }
    else
    {
        printf("~~~~ ");
    }
    printf(" . ");


    if (pInst->m_bHasSIB == true)
    {
        printf("0x%02X ", pInst->m_SIB.Get());
    }
    else
    {
        printf("~~~~ ");
    }
    printf(" . ");


    for (int i = 0; i < Rules::MAX_DISPLACEMENT_BYTES; i++)
    {
        if(i < pInst->m_displacement.ByteCount())
        {
            printf("0x%02X ", pInst->m_displacement.m_iDispBytes[i]);
        }
        else
        {
            printf("~~~~ ");
        }
    }
    printf(" . ");


    for (int i = 0; i < Rules::MAX_IMMEDIATE_BYTES; i++)
    {
        if (i < pInst->m_immediate.ByteCount())
        {
            printf("0x%02X ", pInst->m_immediate.m_immediateByte[i]);
        }
        else
        {
            printf("~~~~ ");
        }
    }
    printf(" . ");
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintVEXInst(InsaneDASM64::VEX::VEXInst_t* pInst)
{
    printf("[ %16s ] ", pInst->m_opcode.m_pOpCodeDesc->m_szName);
    printf("0x%02X", pInst->m_vexPrefix.GetPrefix());
    printf(" . ");

    for(int i = 0; i < 2; i++)
        printf("0x%02X ", i < pInst->m_vexPrefix.VEXByteCount() ? pInst->m_vexPrefix.m_iVEX[i] : 0);
    printf(". ");

    printf("0x%02X", pInst->m_opcode.GetMostSignificantOpCode());
    printf(" . ");

    printf("0x%02X", pInst->m_modrm.Get());
    printf(" . ");

    printf("0x%02X", pInst->m_SIB.Get());
    printf(" . ");

    for(int iDispIndex = 0; iDispIndex < Rules::MAX_DISPLACEMENT_BYTES ; iDispIndex++)
        if(iDispIndex < pInst->m_disp.ByteCount())
            printf("0x%02X ", pInst->m_disp.m_iDispBytes[iDispIndex]);
        else 
            printf("~~~~ ");
    printf(". ");

    printf("0x%02X", pInst->m_immediate.m_immediateByte[0]);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintEVEXInst(EVEX::EVEXInst_t* pInst)
{
    printf("[ %16s ] ", pInst->m_opcode.m_pOpCodeDesc->m_szName);
    printf("0x%02X", pInst->m_evexPrefix.m_iPrefix);
    printf(" . ");

    printf("0x%02X 0x%02X 0x%02X ", pInst->m_evexPrefix.m_iPayload1, pInst->m_evexPrefix.m_iPayload2, pInst->m_evexPrefix.m_iPayload3);

    printf(". ");

    printf("0x%02X", pInst->m_opcode.GetMostSignificantOpCode());
    printf(" . ");

    printf("0x%02X", pInst->m_modrm.Get());
    printf(" . ");

    printf("0x%02X", pInst->m_SIB.Get());
    printf(" . ");

    for(int iDispIndex = 0; iDispIndex < Rules::MAX_DISPLACEMENT_BYTES ; iDispIndex++)
        if(iDispIndex < pInst->m_disp.ByteCount())
            printf("0x%02X ", pInst->m_disp.m_iDispBytes[iDispIndex]);
        else 
            printf("~~~~ ");
    printf(". ");

    printf("0x%02X", pInst->m_immediate.m_immediateByte[0]);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintOutput(std::vector<Instruction_t>& vecDecodedInst, std::vector<DASMInst_t>& vecInst)
{
    for(const DASMInst_t& inst : vecInst)
    {
        printf("%10s ", inst.m_szMnemonic);

        for(int i = 0; i < inst.m_nOperands; i++)
        {
            printf("%s", inst.m_szOperands[i]);

            if(i != inst.m_nOperands - 1)
                printf(", ");
        }

        printf("\n");
    }
}
