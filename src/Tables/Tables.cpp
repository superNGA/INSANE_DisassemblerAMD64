//=========================================================================
//                      Tables
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Holds Look-up-tables of instruction information.
//-------------------------------------------------------------------------
#include "Tables.h"
#include "assert.h"


// NOTE : Mind this.
using namespace INSANE_DASM64_NAMESPACE;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Tables_t::Tables_t()
{
    m_bInstTypeLUTInit = false;
    m_bOpCodeTableInit = false;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t Tables_t::Initialize()
{
    _InitializeInstTypeLUT(); m_bInstTypeLUTInit = true; // can't fail, so no checks required here.
    _InitializeOpCodeTable(); m_bOpCodeTableInit = true; // can't fail, so no checks required here.


    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Tables_t::IsInitialized() const
{
    return m_bInstTypeLUTInit == true && m_bOpCodeTableInit == true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint16_t Tables_t::GetInstType(Byte iOpCode) const
{
    assert(m_bInstTypeLUTInit == true && "[Tables_t] Instruction type LUT is not initialized!");

    return m_instTypeLUT[iOpCode];
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const OperatorInfo_t* Tables_t::GetOperatorInfo(Byte iOpCode, int iTableIndex) const
{
    assert(m_bOpCodeTableInit == true && "OpCode Table is not initialized!.");

    switch (iTableIndex)
    {
    case 1: return &m_OpCodeTable1[iOpCode];
    case 2: return &m_OpCodeTable2[iOpCode];
    case 3: return &m_OpCodeTable3[iOpCode];
    
    default: assert(iTableIndex >= 1 && iTableIndex <= 3 && "Table index is out of range!!!"); break;
    }

    return nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t Tables_t::_InitializeInstTypeLUT()
{
    memset(m_instTypeLUT, 0, sizeof(m_instTypeLUT));

    // Legacy Prefix...
    {
        // Group 1 Legacy Prefix...
        m_instTypeLUT[0xF0] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0xF2] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0xF3] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;


        // Group 2 Legacy Prefix...
        m_instTypeLUT[0x2E] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0x36] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0x3E] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0x26] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0x64] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;
        m_instTypeLUT[0x65] |= InstTypes_t::InstTypes_LegacyPrefixGrp1;


        // Group 3 Legacy Prefix...
        m_instTypeLUT[0x66] |= InstTypes_t::InstTypes_LegacyPrefixGrp3;


        // Group 4 Legacy Prefix...
        m_instTypeLUT[0x67] |= InstTypes_t::InstTypes_LegacyPrefixGrp4;
    }


    // REX... ( [0x40 - 0x4F], i.e. all bytes with pattern '0100 xxxx' )
    {
        for(size_t i = 0x40; i <= 0x4F; i++)
            m_instTypeLUT[i] |= InstTypes_t::InstTypes_REX;
    }


    // Op Codes... ( everything except REX & legacy prefixies )
    {
        for(size_t i = 0x00; i <= 0xFF; i++)
        {
            if(m_instTypeLUT[i] == 0)
                m_instTypeLUT[i] |= InstTypes_t::InstTypes_OpCode;
        }
    }


    return InsaneDASM64::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t Tables_t::_InitializeOpCodeTable()
{
    memset(m_OpCodeTable1, 0, sizeof(m_OpCodeTable1));
    memset(m_OpCodeTable2, 0, sizeof(m_OpCodeTable2));
    memset(m_OpCodeTable3, 0, sizeof(m_OpCodeTable3));

    
    // Setting Escale characters...
    m_OpCodeTable1[SpecialChars::ESCAPE_OPCODE_FIRST_INDEX].m_bIsEscapeOpCode    = true;
    m_OpCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_A].m_bIsEscapeOpCode = true;
    m_OpCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_B].m_bIsEscapeOpCode = true;


    // TODO : Fix this, this is just for testing parsing algorithm.
    for (int i = 0; i <= 0xFF; i++)
    {
        m_OpCodeTable1[i].m_bIsValidOpCode = true;
        m_OpCodeTable2[i].m_bIsValidOpCode = true;
        m_OpCodeTable3[i].m_bIsValidOpCode = true;
    }


    return ErrorCode_t::ErrorCode_Success;
}
