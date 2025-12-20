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
    case 1: return &m_opCodeTable1[iOpCode];
    case 2: return &m_opCodeTable2[iOpCode];
    case 3: return &m_opCodeTable3[iOpCode];
    
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
    memset(m_opCodeTable1, 0, sizeof(m_opCodeTable1));
    memset(m_opCodeTable2, 0, sizeof(m_opCodeTable2));
    memset(m_opCodeTable3, 0, sizeof(m_opCodeTable3));


    // TODO : Fix this, this is just for testing parsing algorithm.
    for (int i = 0; i <= 0xFF; i++)
    {
        m_opCodeTable1[i].m_bIsValidOpCode = false; // we will set all the valid ones manually.
        m_opCodeTable2[i].m_bIsValidOpCode = true;
        m_opCodeTable3[i].m_bIsValidOpCode = true;
    }


    // Brief : Add
    m_opCodeTable1[0].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X1].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X1, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X2].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X2, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X3].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X3, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X4].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X4, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X5].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X5, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X6].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X7].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0X8].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0X8, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0X9].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0X9, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0XA].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0XA, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0XB].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0XB, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0XC].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0XC, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Inclusive OR
    m_opCodeTable1[0XD].SetOperatorInfo(
        /* Operator Name = */"OR", 
        /* Byte          = */0XD, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XE].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XF].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X10].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X10, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X11].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X11, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X12].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X12, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X13].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X13, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X14].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X14, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add with Carry
    m_opCodeTable1[0X15].SetOperatorInfo(
        /* Operator Name = */"ADC", 
        /* Byte          = */0X15, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X16].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X17].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X18].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X18, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X19].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X19, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X1A].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X1A, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X1B].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X1B, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X1C].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X1C, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0X1D].SetOperatorInfo(
        /* Operator Name = */"SBB", 
        /* Byte          = */0X1D, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X1E].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X1F].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X20].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X20, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X21].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X21, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X22].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X22, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X23].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X23, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X24].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X24, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical AND
    m_opCodeTable1[0X25].SetOperatorInfo(
        /* Operator Name = */"AND", 
        /* Byte          = */0X25, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : ES segment override prefix
    m_opCodeTable1[0X26].SetOperatorInfo(
        /* Operator Name = */"ES", 
        /* Byte          = */0X26, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X27].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X28].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X28, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X29].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X29, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X2A].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X2A, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X2B].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X2B, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X2C].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X2C, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Subtract
    m_opCodeTable1[0X2D].SetOperatorInfo(
        /* Operator Name = */"SUB", 
        /* Byte          = */0X2D, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : CS segment override prefix
    m_opCodeTable1[0X2E].SetOperatorInfo(
        /* Operator Name = */"CS", 
        /* Byte          = */0X2E, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X2F].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X30].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X30, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X31].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X31, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X32].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X32, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X33].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X33, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X34].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X34, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Exclusive OR
    m_opCodeTable1[0X35].SetOperatorInfo(
        /* Operator Name = */"XOR", 
        /* Byte          = */0X35, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : SS segment override prefix
    m_opCodeTable1[0X36].SetOperatorInfo(
        /* Operator Name = */"SS", 
        /* Byte          = */0X36, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X37].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X38].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X38, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X39].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X39, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X3A].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X3A, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X3B].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X3B, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X3C].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X3C, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare Two Operands
    m_opCodeTable1[0X3D].SetOperatorInfo(
        /* Operator Name = */"CMP", 
        /* Byte          = */0X3D, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : DS segment override prefix
    m_opCodeTable1[0X3E].SetOperatorInfo(
        /* Operator Name = */"DS", 
        /* Byte          = */0X3E, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X3F].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X40].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X41].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X42].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X43].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X44].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X45].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X46].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X47].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X48].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X49].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4A].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4B].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4C].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4D].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4E].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X4F].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0X50].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0X50, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X51].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X52].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X53].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X54].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X55].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X56].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X57].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0X58].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0X58, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X59].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5A].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5B].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5C].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5D].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5E].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X5F].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X60].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X61].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X62].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Adjust RPL Field of Segment Selector
    m_opCodeTable1[0X63].SetOperatorInfo(
        /* Operator Name = */"ARPL", 
        /* Byte          = */0X63, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_w ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : FS segment override prefix
    m_opCodeTable1[0X64].SetOperatorInfo(
        /* Operator Name = */"FS", 
        /* Byte          = */0X64, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : GS segment override prefix
    m_opCodeTable1[0X65].SetOperatorInfo(
        /* Operator Name = */"GS", 
        /* Byte          = */0X65, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X66].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X67].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0X68].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0X68, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Signed Multiply
    m_opCodeTable1[0X69].SetOperatorInfo(
        /* Operator Name = */"IMUL", 
        /* Byte          = */0X69, 
        /* Operand Count = */3, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0X6A].SetOperatorInfo(
        /* Operator Name = */"PUSH", 
        /* Byte          = */0X6A, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Signed Multiply
    m_opCodeTable1[0X6B].SetOperatorInfo(
        /* Operator Name = */"IMUL", 
        /* Byte          = */0X6B, 
        /* Operand Count = */3, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port to String
    m_opCodeTable1[0X6C].SetOperatorInfo(
        /* Operator Name = */"INS", 
        /* Byte          = */0X6C, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port to String
    m_opCodeTable1[0X6D].SetOperatorInfo(
        /* Operator Name = */"INS", 
        /* Byte          = */0X6D, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output String to Port
    m_opCodeTable1[0X6E].SetOperatorInfo(
        /* Operator Name = */"OUTS", 
        /* Byte          = */0X6E, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output String to Port
    m_opCodeTable1[0X6F].SetOperatorInfo(
        /* Operator Name = */"OUTS", 
        /* Byte          = */0X6F, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_SI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if overflow (OF=1)
    m_opCodeTable1[0X70].SetOperatorInfo(
        /* Operator Name = */"JO", 
        /* Byte          = */0X70, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not overflow (OF=0)
    m_opCodeTable1[0X71].SetOperatorInfo(
        /* Operator Name = */"JNO", 
        /* Byte          = */0X71, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if below/not above or equal/carry (CF=1)
    m_opCodeTable1[0X72].SetOperatorInfo(
        /* Operator Name = */"JB", 
        /* Byte          = */0X72, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not below/above or equal/not carry (CF=0)
    m_opCodeTable1[0X73].SetOperatorInfo(
        /* Operator Name = */"JNB", 
        /* Byte          = */0X73, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if zero/equal (ZF=1)
    m_opCodeTable1[0X74].SetOperatorInfo(
        /* Operator Name = */"JZ", 
        /* Byte          = */0X74, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not zero/not equal (ZF=0)
    m_opCodeTable1[0X75].SetOperatorInfo(
        /* Operator Name = */"JNZ", 
        /* Byte          = */0X75, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if below or equal/not above (CF=1 OR ZF=1)
    m_opCodeTable1[0X76].SetOperatorInfo(
        /* Operator Name = */"JBE", 
        /* Byte          = */0X76, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not below or equal/above (CF=0 AND ZF=0)
    m_opCodeTable1[0X77].SetOperatorInfo(
        /* Operator Name = */"JNBE", 
        /* Byte          = */0X77, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if sign (SF=1)
    m_opCodeTable1[0X78].SetOperatorInfo(
        /* Operator Name = */"JS", 
        /* Byte          = */0X78, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not sign (SF=0)
    m_opCodeTable1[0X79].SetOperatorInfo(
        /* Operator Name = */"JNS", 
        /* Byte          = */0X79, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if parity/parity even (PF=1)
    m_opCodeTable1[0X7A].SetOperatorInfo(
        /* Operator Name = */"JP", 
        /* Byte          = */0X7A, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not parity/parity odd (PF=0)
    m_opCodeTable1[0X7B].SetOperatorInfo(
        /* Operator Name = */"JNP", 
        /* Byte          = */0X7B, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if less/not greater (SF!=OF)
    m_opCodeTable1[0X7C].SetOperatorInfo(
        /* Operator Name = */"JL", 
        /* Byte          = */0X7C, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not less/greater or equal (SF=OF)
    m_opCodeTable1[0X7D].SetOperatorInfo(
        /* Operator Name = */"JNL", 
        /* Byte          = */0X7D, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if less or equal/not greater ((ZF=1) OR (SF!=OF))
    m_opCodeTable1[0X7E].SetOperatorInfo(
        /* Operator Name = */"JLE", 
        /* Byte          = */0X7E, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if not less nor equal/greater ((ZF=0) AND (SF=OF))
    m_opCodeTable1[0X7F].SetOperatorInfo(
        /* Operator Name = */"JNLE", 
        /* Byte          = */0X7F, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X80].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X80, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X81].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X81, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X82].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Add
    m_opCodeTable1[0X83].SetOperatorInfo(
        /* Operator Name = */"ADD", 
        /* Byte          = */0X83, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0X84].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0X84, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0X85].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0X85, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X86].SetOperatorInfo(
        /* Operator Name = */"XCHG", 
        /* Byte          = */0X86, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X87].SetOperatorInfo(
        /* Operator Name = */"XCHG", 
        /* Byte          = */0X87, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X88].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X88, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X89].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X89, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X8A].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X8A, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X8B].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X8B, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X8C].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X8C, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_M, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_S, OpCodeOperandType_w ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Load Effective Address
    m_opCodeTable1[0X8D].SetOperatorInfo(
        /* Operator Name = */"LEA", 
        /* Byte          = */0X8D, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_G, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_M, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0X8E].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0X8E, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_S, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_w ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0X8F].SetOperatorInfo(
        /* Operator Name = */"POP", 
        /* Byte          = */0X8F, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X90].SetOperatorInfo(
        /* Operator Name = */"XCHG", 
        /* Byte          = */0X90, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_R8, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X91].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X92].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X93].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X94].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X95].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X96].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0X97].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Convert Byte to Word
    m_opCodeTable1[0X98].SetOperatorInfo(
        /* Operator Name = */"CBW", 
        /* Byte          = */0X98, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Convert Word to Doubleword
    m_opCodeTable1[0X99].SetOperatorInfo(
        /* Operator Name = */"CWD", 
        /* Byte          = */0X99, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0X9A].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Check pending unmasked floating-point exceptions
    m_opCodeTable1[0X9B].SetOperatorInfo(
        /* Operator Name = */"FWAIT", 
        /* Byte          = */0X9B, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Push FLAGS Register onto the Stack
    m_opCodeTable1[0X9C].SetOperatorInfo(
        /* Operator Name = */"PUSHF", 
        /* Byte          = */0X9C, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Pop Stack into FLAGS Register
    m_opCodeTable1[0X9D].SetOperatorInfo(
        /* Operator Name = */"POPF", 
        /* Byte          = */0X9D, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Store AH into Flags
    m_opCodeTable1[0X9E].SetOperatorInfo(
        /* Operator Name = */"SAHF", 
        /* Byte          = */0X9E, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Load Status Flags into AH Register
    m_opCodeTable1[0X9F].SetOperatorInfo(
        /* Operator Name = */"LAHF", 
        /* Byte          = */0X9F, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XA0].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XA0, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_O, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XA1].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XA1, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_O, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XA2].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XA2, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_O, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XA3].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XA3, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_O, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move Data from String to String
    m_opCodeTable1[0XA4].SetOperatorInfo(
        /* Operator Name = */"MOVS", 
        /* Byte          = */0XA4, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move Data from String to String
    m_opCodeTable1[0XA5].SetOperatorInfo(
        /* Operator Name = */"MOVS", 
        /* Byte          = */0XA5, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_SI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare String Operands
    m_opCodeTable1[0XA6].SetOperatorInfo(
        /* Operator Name = */"CMPS", 
        /* Byte          = */0XA6, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Compare String Operands
    m_opCodeTable1[0XA7].SetOperatorInfo(
        /* Operator Name = */"CMPS", 
        /* Byte          = */0XA7, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_SI, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0XA8].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0XA8, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0XA9].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0XA9, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Store String
    m_opCodeTable1[0XAA].SetOperatorInfo(
        /* Operator Name = */"STOS", 
        /* Byte          = */0XAA, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Store String
    m_opCodeTable1[0XAB].SetOperatorInfo(
        /* Operator Name = */"STOS", 
        /* Byte          = */0XAB, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Load String
    m_opCodeTable1[0XAC].SetOperatorInfo(
        /* Operator Name = */"LODS", 
        /* Byte          = */0XAC, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Load String
    m_opCodeTable1[0XAD].SetOperatorInfo(
        /* Operator Name = */"LODS", 
        /* Byte          = */0XAD, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_SI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Scan String
    m_opCodeTable1[0XAE].SetOperatorInfo(
        /* Operator Name = */"SCAS", 
        /* Byte          = */0XAE, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Scan String
    m_opCodeTable1[0XAF].SetOperatorInfo(
        /* Operator Name = */"SCAS", 
        /* Byte          = */0XAF, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XB0].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XB0, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB1].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_CL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB2].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB3].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_BL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB4].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AH, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB5].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_CH, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB6].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DH, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB7].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_BH, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XB8].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XB8, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RAX, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XB9].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBA].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBB].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBC].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBD].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBE].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XBF].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XC0].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XC0, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XC1].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XC1, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Return from procedure
    m_opCodeTable1[0XC2].SetOperatorInfo(
        /* Operator Name = */"RETN", 
        /* Byte          = */0XC2, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Return from procedure
    m_opCodeTable1[0XC3].SetOperatorInfo(
        /* Operator Name = */"RETN", 
        /* Byte          = */0XC3, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XC4].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XC5].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XC6].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XC6, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Move
    m_opCodeTable1[0XC7].SetOperatorInfo(
        /* Operator Name = */"MOV", 
        /* Byte          = */0XC7, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_z ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Make Stack Frame for Procedure Parameters
    m_opCodeTable1[0XC8].SetOperatorInfo(
        /* Operator Name = */"ENTER", 
        /* Byte          = */0XC8, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : High Level Procedure Exit
    m_opCodeTable1[0XC9].SetOperatorInfo(
        /* Operator Name = */"LEAVE", 
        /* Byte          = */0XC9, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Return from procedure
    m_opCodeTable1[0XCA].SetOperatorInfo(
        /* Operator Name = */"RETF", 
        /* Byte          = */0XCA, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_w ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Return from procedure
    m_opCodeTable1[0XCB].SetOperatorInfo(
        /* Operator Name = */"RETF", 
        /* Byte          = */0XCB, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Call to Interrupt Procedure
    m_opCodeTable1[0XCC].SetOperatorInfo(
        /* Operator Name = */"INT", 
        /* Byte          = */0XCC, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Call to Interrupt Procedure
    m_opCodeTable1[0XCD].SetOperatorInfo(
        /* Operator Name = */"INT", 
        /* Byte          = */0XCD, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XCE].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Interrupt Return
    m_opCodeTable1[0XCF].SetOperatorInfo(
        /* Operator Name = */"IRET", 
        /* Byte          = */0XCF, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XD0].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XD0, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XD1].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XD1, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XD2].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XD2, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_CL, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Rotate
    m_opCodeTable1[0XD3].SetOperatorInfo(
        /* Operator Name = */"ROL", 
        /* Byte          = */0XD3, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_CL, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XD4].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XD5].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Intentional UDB
    m_opCodeTable1[0XD6].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Table Look-up Translation
    m_opCodeTable1[0XD7].SetOperatorInfo(
        /* Operator Name = */"XLAT", 
        /* Byte          = */0XD7, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XD8].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XD9].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDA].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDB].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDC].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDD].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDE].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Escape to coprocessor instruction set
    m_opCodeTable1[0XDF].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0XD8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Decrement count; Jump short if count!=0 and ZF=0
    m_opCodeTable1[0XE0].SetOperatorInfo(
        /* Operator Name = */"LOOPNZ", 
        /* Byte          = */0XE0, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Decrement count; Jump short if count!=0 and ZF=1
    m_opCodeTable1[0XE1].SetOperatorInfo(
        /* Operator Name = */"LOOPZ", 
        /* Byte          = */0XE1, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Decrement count; Jump short if count!=0
    m_opCodeTable1[0XE2].SetOperatorInfo(
        /* Operator Name = */"LOOP", 
        /* Byte          = */0XE2, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump short if eCX register is 0
    m_opCodeTable1[0XE3].SetOperatorInfo(
        /* Operator Name = */"JCXZ", 
        /* Byte          = */0XE3, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port
    m_opCodeTable1[0XE4].SetOperatorInfo(
        /* Operator Name = */"IN", 
        /* Byte          = */0XE4, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port
    m_opCodeTable1[0XE5].SetOperatorInfo(
        /* Operator Name = */"IN", 
        /* Byte          = */0XE5, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output to Port
    m_opCodeTable1[0XE6].SetOperatorInfo(
        /* Operator Name = */"OUT", 
        /* Byte          = */0XE6, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output to Port
    m_opCodeTable1[0XE7].SetOperatorInfo(
        /* Operator Name = */"OUT", 
        /* Byte          = */0XE7, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Call Procedure
    m_opCodeTable1[0XE8].SetOperatorInfo(
        /* Operator Name = */"CALL", 
        /* Byte          = */0XE8, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_z ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump
    m_opCodeTable1[0XE9].SetOperatorInfo(
        /* Operator Name = */"JMP", 
        /* Byte          = */0XE9, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_z ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XEA].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Jump
    m_opCodeTable1[0XEB].SetOperatorInfo(
        /* Operator Name = */"JMP", 
        /* Byte          = */0XEB, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_J, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port
    m_opCodeTable1[0XEC].SetOperatorInfo(
        /* Operator Name = */"IN", 
        /* Byte          = */0XEC, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Input from Port
    m_opCodeTable1[0XED].SetOperatorInfo(
        /* Operator Name = */"IN", 
        /* Byte          = */0XED, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output to Port
    m_opCodeTable1[0XEE].SetOperatorInfo(
        /* Operator Name = */"OUT", 
        /* Byte          = */0XEE, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Output to Port
    m_opCodeTable1[0XEF].SetOperatorInfo(
        /* Operator Name = */"OUT", 
        /* Byte          = */0XEF, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Assert LOCK# Signal Prefix
    m_opCodeTable1[0XF0].SetOperatorInfo(
        /* Operator Name = */"LOCK", 
        /* Byte          = */0XF0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Instruction Invalid in 64-bit Mode
    m_opCodeTable1[0XF1].SetOperatorInfo(
        /* Operator Name = */"xx_INVALID_xx", 
        /* Byte          = */0, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */false, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Repeat String Operation Prefix
    m_opCodeTable1[0XF2].SetOperatorInfo(
        /* Operator Name = */"REPNZ", 
        /* Byte          = */0XF2, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Repeat String Operation Prefix
    m_opCodeTable1[0XF3].SetOperatorInfo(
        /* Operator Name = */"REPZ", 
        /* Byte          = */0XF3, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Halt
    m_opCodeTable1[0XF4].SetOperatorInfo(
        /* Operator Name = */"HLT", 
        /* Byte          = */0XF4, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Complement Carry Flag
    m_opCodeTable1[0XF5].SetOperatorInfo(
        /* Operator Name = */"CMC", 
        /* Byte          = */0XF5, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0XF6].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0XF6, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_b ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Logical Compare
    m_opCodeTable1[0XF7].SetOperatorInfo(
        /* Operator Name = */"TEST", 
        /* Byte          = */0XF7, 
        /* Operand Count = */2, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_I, OpCodeOperandType_v ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Clear Carry Flag
    m_opCodeTable1[0XF8].SetOperatorInfo(
        /* Operator Name = */"CLC", 
        /* Byte          = */0XF8, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Set Carry Flag
    m_opCodeTable1[0XF9].SetOperatorInfo(
        /* Operator Name = */"STC", 
        /* Byte          = */0XF9, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Clear Interrupt Flag
    m_opCodeTable1[0XFA].SetOperatorInfo(
        /* Operator Name = */"CLI", 
        /* Byte          = */0XFA, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Set Interrupt Flag
    m_opCodeTable1[0XFB].SetOperatorInfo(
        /* Operator Name = */"STI", 
        /* Byte          = */0XFB, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Clear Direction Flag
    m_opCodeTable1[0XFC].SetOperatorInfo(
        /* Operator Name = */"CLD", 
        /* Byte          = */0XFC, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Set Direction Flag
    m_opCodeTable1[0XFD].SetOperatorInfo(
        /* Operator Name = */"STD", 
        /* Byte          = */0XFD, 
        /* Operand Count = */0, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Increment by 1
    m_opCodeTable1[0XFE].SetOperatorInfo(
        /* Operator Name = */"INC", 
        /* Byte          = */0XFE, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_b ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );

    // Brief : Increment by 1
    m_opCodeTable1[0XFF].SetOperatorInfo(
        /* Operator Name = */"INC", 
        /* Byte          = */0XFF, 
        /* Operand Count = */1, 
        /* Valid OpCode  = */true, 
        /* Escape Code   = */false, 
        /* Operand 1     = */OpCodeOperand_t( OpCodeAddressingMethod_E, OpCodeOperandType_v ),
        /* Operand 2     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 3     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ),
        /* Operand 4     = */OpCodeOperand_t( OpCodeAddressingMethod_Invalid, OpCodeOperandType_Invalid ) 
    );




        
    // Setting Escape characters...
    m_opCodeTable1[SpecialChars::ESCAPE_OPCODE_FIRST_INDEX].m_bIsEscapeOpCode    = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_A].m_bIsEscapeOpCode = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_B].m_bIsEscapeOpCode = true;



    return ErrorCode_t::ErrorCode_Success;
}
