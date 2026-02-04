//=========================================================================
//                      DASMInst ( Disassembled Instruction )
//=========================================================================
// by      : INSANE
// created : 17/01/2026
//
// purpose : Holds disassembled instruction, as null-terminated strings.
//           Output format for disassembler.
//-------------------------------------------------------------------------
#include "../Include/DASMInst_t.h"
#include <assert.h>
#include <cinttypes>
#include <cstdio>
#include <cstring>


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::DASMInst_t::DASMInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::Clear()
{
    m_szMnemonic[0] = '\0';

    m_nOperands = 0;
    for(int i = 0; i < Rules::MAX_OPERANDS; i++)
        m_szOperands[i][0] = '\0';
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralOperand(uint64_t iLiteralOperand)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    sprintf(&m_szOperands[m_nOperands][0], "0x" PRIX64, iLiteralOperand);

    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralOperand(uint32_t iLiteralOperand)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    sprintf(&m_szOperands[m_nOperands][0], "0x" PRIX32, iLiteralOperand);

    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralOperand(uint16_t iLiteralOperand)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    sprintf(&m_szOperands[m_nOperands][0], "0x" PRIX16, iLiteralOperand);

    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralOperand(uint8_t iLiteralOperand)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    sprintf(&m_szOperands[m_nOperands][0], "0x" PRIX8, iLiteralOperand);

    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralOperand(int iLiteralOperand)
{
    /*
    NOTE : Buffer size for each of the operand and mnemonic is set to more than 40 bytes.
           and writting even an 8 byte interger will only consume 19 bytes ( including "0x" prefix and null terminator ).
           So buffer overflow is no concern here.
    */
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    char* szBuffer  = &m_szOperands[m_nOperands][0];
    int   iIterator = 0;
    szBuffer[iIterator] = '0'; iIterator++;
    szBuffer[iIterator] = 'x'; iIterator++;

    if(iLiteralOperand == 0)
    {
        szBuffer[iIterator] = '0';
        iIterator++;
    }
    else
    {
        bool bLeadingZerosEnded = false;
        int  nBytesStored       = 0;
        for(size_t i = 0; i < sizeof(iLiteralOperand); i++)
        {
            int iBitsToShift = ((sizeof(iLiteralOperand) - (i + 1)) * 8); // Since i + 1 at max can be equal to sizeof(iLiteralOperand) this is safe.
            unsigned char iByte = (static_cast<unsigned int>(iLiteralOperand) >> iBitsToShift) & 0xFF;

            if(iByte != 0)
                bLeadingZerosEnded = true;

            // Skip leading zeros.
            if(bLeadingZerosEnded == false)
                continue;

            nBytesStored++;

            // Write to buffer.
            static const char s_intToHexCharTable[] = "0123456789ABCDEF";

            char iHighBits = s_intToHexCharTable[iByte / 0x10];
            if(nBytesStored > 1 || iHighBits != '0')
            {
                szBuffer[iIterator] = iHighBits; 
                iIterator++;
            }
            szBuffer[iIterator] = s_intToHexCharTable[iByte % 0x10]; iIterator++;
        }
    }

    szBuffer[iIterator] = '\0'; iIterator++;

    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushLiteralFromString(const Byte* szOperand, int iSize, bool bFlipEndian)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");
    assert(szOperand != nullptr && iSize > 0 && "Invalid input received!");


    size_t iBufferSize = Rules::DASMINST_ARG_BUFFER_SIZE;
    char* szBuffer  = &m_szOperands[m_nOperands][0];
    int   iIterator = 0;
    szBuffer[iIterator] = '0'; iIterator++;
    szBuffer[iIterator] = 'x'; iIterator++;

    for(int iByteIndex = 0; iByteIndex < iSize; iByteIndex++)
    {
        int iIndex = bFlipEndian == false ? iByteIndex : iSize - 1 - iByteIndex;

        static const char s_intToHexCharTable[] = "0123456789ABCDEF";
        szBuffer[iIterator] = s_intToHexCharTable[szOperand[iIndex] / 0x10]; iIterator++;
        szBuffer[iIterator] = s_intToHexCharTable[szOperand[iIndex] % 0x10]; iIterator++;

        assert(iIterator < iBufferSize - 1 && "BufferOverflow @ PushLiteralFromString");
    }

    szBuffer[iIterator] = '\0'; iIterator++;


    m_nOperands++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::DASMInst_t::PushBackOperand(const char* szOperand)
{
    assert(m_nOperands >= 0 && m_nOperands < Rules::MAX_OPERANDS && "Too many operands");

    strcpy(&m_szOperands[m_nOperands][0], szOperand);
    // sprintf(&m_szOperands[m_nOperands][0], "%s", szOperand);

    m_nOperands++;
}
