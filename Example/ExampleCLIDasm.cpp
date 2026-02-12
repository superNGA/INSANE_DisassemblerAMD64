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

#include "../Include/INSANE_DisassemblerAMD64.h"


void PrintOutput(std::vector<InsaneDASM64::Instruction_t>& vecDecodedInst, std::vector<InsaneDASM64::DASMInst_t>& vecInst);


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
        std::vector<InsaneDASM64::Byte> vecFileInput; vecFileInput.clear();
        {
            FILE* pFile = fopen(szArgs[1], "rb");
            if(pFile == nullptr)
                return 1;

            int iChar = EOF;
            while((iChar = getc(pFile)) != EOF)
            {
                vecFileInput.push_back(static_cast<InsaneDASM64::Byte>(iChar & 0xFF));
            }

            printf("%zu bytes from file %s", vecFileInput.size(), szArgs[1]);
        }


        
        // Disassemble...
        ArenaAllocator_t allocator(8 * 1024); // 8 KiB
        {
            std::vector<InsaneDASM64::Instruction_t> vecDecodedInst; vecDecodedInst.clear();
            std::vector<InsaneDASM64::DASMInst_t>    vecDasmInst;    vecDasmInst.clear();


            // Decoding...
            InsaneDASM64::IDASMErrorCode_t iDecodingErrCode = Decode(vecFileInput, vecDecodedInst, allocator, false);
            printf("Decoded      [ %zu ] instructions.", vecDecodedInst.size());
            if(iDecodingErrCode != InsaneDASM64::IDASMErrorCode_t::IDASMErrorCode_Success)
            {
                printf("%s\n", GetErrorMessage(iDecodingErrCode));
                return 1;
            }
            
            // Disassemlbing...
            InsaneDASM64::IDASMErrorCode_t iDASMErrCode = Disassemble(vecDecodedInst, vecDasmInst);
            PrintOutput(vecDecodedInst, vecDasmInst);
            
            if(iDASMErrCode != InsaneDASM64::IDASMErrorCode_t::IDASMErrorCode_Success)
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
void PrintOutput(std::vector<InsaneDASM64::Instruction_t>& vecDecodedInst, std::vector<InsaneDASM64::DASMInst_t>& vecInst)
{
    for(const InsaneDASM64::DASMInst_t& inst : vecInst)
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
    }
}
