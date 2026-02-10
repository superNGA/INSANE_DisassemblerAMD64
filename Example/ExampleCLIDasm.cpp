//=========================================================================
//                      Cmd Line Disassembler using IDASM
//=========================================================================
// by      : INSANE
// created : 10/02/2026
//
// purpose : To disassemlber stuff.
//-------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <assert.h>
#include <chrono>
// #include <windows.h> // For timing.

#include "../Include/INSANE_DisassemblerAMD64.h"
#include "../Include/Legacy/LegacyInst_t.h"
#include "../Include/Standard/OpCodeDesc_t.h"
#include "../Include/VEX/VEXInst_t.h"
#include "../Include/EVEX/EVEXInst_t.h"
#include "../Include/Rules.h"


using namespace InsaneDASM64;


void PrintInst      (std::vector<Instruction_t>& vecInst);
void PrintInst      (const Instruction_t& inst);
void PrintLegacyInst(InsaneDASM64::Legacy::LegacyInst_t* pInst);
void PrintVEXInst   (InsaneDASM64::VEX::VEXInst_t* pInst);
void PrintEVEXInst  (EVEX::EVEXInst_t* pInst);
void PrintOutput    (std::vector<Instruction_t>& vecDecodedInst, std::vector<DASMInst_t>& vecInst);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int nArgs, char** szArgs)
{
    if(nArgs <= 1)
    {
        std::cout << "No file name specified\n";
        return 1;
    }


    std::chrono::high_resolution_clock::time_point iStartTime = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 1; i++)
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
        printf("Disassembler Initialized!\n");


        // Reading bytes form file.
        std::vector<Byte> vecFileInput; vecFileInput.clear();
        {
            FILE* pFile = fopen(szArgs[1], "rb");
            if(pFile == nullptr)
                return 1;

            int iChar = EOF;
            while((iChar = getc(pFile)) != EOF)
            {
                vecFileInput.push_back(static_cast<Byte>(iChar & 0xFF));
            }

            printf("%zu bytes from file %s", vecFileInput.size(), szArgs[1]);
        }


        
        // Disassemble...
        ArenaAllocator_t allocator(8 * 1024); // 8 KiB
        {
            std::vector<Instruction_t> vecDecodedInst; vecDecodedInst.clear();
            std::vector<DASMInst_t>    vecDasmInst;    vecDasmInst.clear();


            // Decoding...
            IDASMErrorCode_t iDecodingErrCode = Decode(vecFileInput, vecDecodedInst, allocator, false);
            printf("Decoded      [ %zu ] instructions.", vecDecodedInst.size());
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
            printf("Disassembled [ %zu ] instructions.", vecDasmInst.size());
        }


        // Unitialize...
        printf("Arenas : %zu, Memory : 0x%0lX\n Bytes ( %llu KiB, %lu MiB)", allocator.ArenaCount(), allocator.TotalSize(), 
                allocator.TotalSize() / 1024llu,
                allocator.TotalSize() / (1024 * 1024));
        allocator.FreeAll();

        InsaneDASM64::UnInitialize();
    }

    double iElapsedTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - iStartTime).count();
    printf("seconds : %f", iElapsedTime);


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void PrintInst(std::vector<Instruction_t>& vecInst)
{
    for(Instruction_t& inst : vecInst) {
        PrintInst(inst);
        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void PrintInst(const Instruction_t& inst)
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
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void PrintLegacyInst(InsaneDASM64::Legacy::LegacyInst_t* pInst)
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
void PrintVEXInst(InsaneDASM64::VEX::VEXInst_t* pInst)
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
void PrintEVEXInst(EVEX::EVEXInst_t* pInst)
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
void PrintOutput(std::vector<Instruction_t>& vecDecodedInst, std::vector<DASMInst_t>& vecInst)
{
    for(const DASMInst_t& inst : vecInst)
    {
        switch(inst.m_nOperands)
        {
            case 0: printf("%10s\n", inst.m_szMnemonic); break;
            case 1: printf("%10s %s\n", inst.m_szMnemonic, inst.m_szOperands[0]); break;
            case 2: printf("%10s %s, %s\n", inst.m_szMnemonic, inst.m_szOperands[0], inst.m_szOperands[1]); break;
            case 3: printf("%10s %s, %s, %s\n", inst.m_szMnemonic, inst.m_szOperands[0], inst.m_szOperands[1], inst.m_szOperands[2]); break;
            case 4: printf("%10s %s, %s, %s, %s\n", 
                            inst.m_szMnemonic, inst.m_szOperands[0], inst.m_szOperands[1], inst.m_szOperands[2], inst.m_szOperands[3]); break;

            default: break;
        }
        // printf("%10s ", inst.m_szMnemonic);
        //
        // for(int i = 0; i < inst.m_nOperands; i++)
        // {
        //     printf("%s", inst.m_szOperands[i]);
        //
        //     if(i != inst.m_nOperands - 1)
        //         printf(", ");
        // }
        //
        // printf("\n");
    }
}
