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

    
    // Setting Escale characters...
    m_opCodeTable1[SpecialChars::ESCAPE_OPCODE_FIRST_INDEX].m_bIsEscapeOpCode    = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_A].m_bIsEscapeOpCode = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_B].m_bIsEscapeOpCode = true;


    // TODO : Fix this, this is just for testing parsing algorithm.
    for (int i = 0; i <= 0xFF; i++)
    {
        m_opCodeTable1[i].m_bIsValidOpCode = false; // we will set all the valid ones manually.
        m_opCodeTable2[i].m_bIsValidOpCode = true;
        m_opCodeTable3[i].m_bIsValidOpCode = true;
    }


    // ADD...
    m_opCodeTable1[0x00].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x01].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x02].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x03].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x04].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x05].SetOperatorInfo("ADD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    // ADC...
    m_opCodeTable1[0x10].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x11].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x12].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x13].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x14].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x15].SetOperatorInfo("ADC", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    // AND...
    m_opCodeTable1[0x20].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x21].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x22].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x23].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x24].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x25].SetOperatorInfo("AND", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    // XOR...
    m_opCodeTable1[0x30].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x31].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x32].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x33].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x34].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x35].SetOperatorInfo("XOR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    
    // Row 0x40 to 0x47 is not encodable in 64 bit mode. ( REX bytes )
    
    m_opCodeTable1[0x50].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x51].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x52].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x53].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x54].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x55].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x56].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x57].SetOperatorInfo("PUSH", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid));


    m_opCodeTable1[0x63].SetOperatorInfo("MOVSXD", 2, OpCodeFlag_t::OpCodeFlag_o64, OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    

    // Don't be fooled by the f64 flag on these instructions. These are short distance jump instructions...
    m_opCodeTable1[0x70].SetOperatorInfo("O",        1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x71].SetOperatorInfo("NO",       1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x72].SetOperatorInfo("B/NAE/C",  1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x73].SetOperatorInfo("NB/AE/NC", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x74].SetOperatorInfo("Z/E",      1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x75].SetOperatorInfo("NZ/NE",    1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x76].SetOperatorInfo("BE/NA",    1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x77].SetOperatorInfo("NBE/A",    1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));


    m_opCodeTable1[0x80].SetOperatorInfo("Immd. Grp1", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x81].SetOperatorInfo("Immd. Grp1", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    m_opCodeTable1[0x82].SetOperatorInfo("Immd. Grp1", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x83].SetOperatorInfo("Immd. Grp1", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));


    m_opCodeTable1[0x84].SetOperatorInfo("TEST", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x85].SetOperatorInfo("TEST", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));

    m_opCodeTable1[0x86].SetOperatorInfo("XCHG", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x87].SetOperatorInfo("XCHG", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));


    // These are XCHG instructions where one of the operands is rax. Other can be either from ( rcx to rdi ) or ( r8 to r15 ) 
    // Determined by REX.b bit.
    m_opCodeTable1[0x90].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x91].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x92].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x93].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x94].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x95].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x96].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x97].SetOperatorInfo("XCHG", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid));


    m_opCodeTable1[0xA0].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_O,   OpCodeOperandType_b));
    m_opCodeTable1[0xA1].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_O,   OpCodeOperandType_v));
    m_opCodeTable1[0xA2].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_O,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid));
    m_opCodeTable1[0xA3].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_O,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid));

    m_opCodeTable1[0xA4].SetOperatorInfo("MOVS/B",     2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_X, OpCodeOperandType_b));
    m_opCodeTable1[0xA5].SetOperatorInfo("MOVS/W/D/Q", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_X, OpCodeOperandType_v));
    m_opCodeTable1[0xA6].SetOperatorInfo("CMPS/B",     2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_X, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_Y, OpCodeOperandType_b));
    m_opCodeTable1[0xA7].SetOperatorInfo("CMPS/W/D/Q", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_X, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_Y, OpCodeOperandType_v));


    m_opCodeTable1[0xB0].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB1].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_CL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB2].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB3].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_BL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB4].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AH,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB5].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_CH,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB6].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DH,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xB7].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_BH,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));


    m_opCodeTable1[0xC0].SetOperatorInfo("Shift Grp2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E,  OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xC1].SetOperatorInfo("Shift Grp2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E,  OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    
    m_opCodeTable1[0xC2].SetOperatorInfo("RET", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_w));
    m_opCodeTable1[0xC3].SetOperatorInfo("RET", 0, OpCodeFlag_t::OpCodeFlag_f64);

    m_opCodeTable1[0xC6].SetOperatorInfo("Grp 11", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xC7].SetOperatorInfo("Grp 11", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    
    // Put second operand as literal 1. 0xd0 & 0xd1
    m_opCodeTable1[0xD0].SetOperatorInfo("Shift Grp 2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    m_opCodeTable1[0xD1].SetOperatorInfo("Shift Grp 2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    m_opCodeTable1[0xD2].SetOperatorInfo("Shift Grp 2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_CL, OpCodeOperandType_Invalid));
    m_opCodeTable1[0xD3].SetOperatorInfo("Shift Grp 2", 2, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_CL, OpCodeOperandType_Invalid));

    m_opCodeTable1[0xD6].SetOperatorInfo("XLAT/XLATB", 0, OpCodeFlag_t::OpCodeFlag_None);


    m_opCodeTable1[0xE0].SetOperatorInfo("LOOPNE/LOOPNZ", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_b));
    m_opCodeTable1[0xE1].SetOperatorInfo("LOOPE/LOOPZ",   1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_b));
    m_opCodeTable1[0xE2].SetOperatorInfo("LOOP",          1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_b));
    m_opCodeTable1[0xE3].SetOperatorInfo("jrCXZ",         1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_b));

    m_opCodeTable1[0xE4].SetOperatorInfo("IN",         2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xE5].SetOperatorInfo("IN",         2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));

    m_opCodeTable1[0xE6].SetOperatorInfo("OUT",         2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid));
    m_opCodeTable1[0xE7].SetOperatorInfo("OUT",         2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid));


    m_opCodeTable1[0xF1].SetOperatorInfo("INT1", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xF4].SetOperatorInfo("HLT",  0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xF5].SetOperatorInfo("CMC",  0, OpCodeFlag_t::OpCodeFlag_None);
    
    m_opCodeTable1[0xF6].SetOperatorInfo("Unary Grp 3",  1, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0xF7].SetOperatorInfo("Unary Grp 3",  1, OpCodeFlag_t::OpCodeFlag_1A, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));


    // Other half of table1
    m_opCodeTable1[0x08].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x09].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x0A].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x0B].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x0C].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x0D].SetOperatorInfo("OR", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    m_opCodeTable1[0x18].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x19].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x1A].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x1B].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x1C].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x1D].SetOperatorInfo("SBB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    m_opCodeTable1[0x28].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x29].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x2A].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x2B].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x2C].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x2D].SetOperatorInfo("SUB", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    m_opCodeTable1[0x38].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x39].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x3A].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x3B].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    m_opCodeTable1[0x3C].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x3D].SetOperatorInfo("CMP", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));


    m_opCodeTable1[0x58].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x59].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RCX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5A].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RDX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5B].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RBX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5C].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RSP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5D].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RBP, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5E].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RSI, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x5F].SetOperatorInfo("POP", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_RDI, OpCodeOperandType_Invalid));


    m_opCodeTable1[0x68].SetOperatorInfo("PUSH",             1, OpCodeFlag_t::OpCodeFlag_d64,  OpCodeOperand_t(OpCodeAddressingMethod_I,  OpCodeOperandType_z));
    m_opCodeTable1[0x69].SetOperatorInfo("IMUL",             3, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,  OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));
    m_opCodeTable1[0x6A].SetOperatorInfo("PUSH",             1, OpCodeFlag_t::OpCodeFlag_d64,  OpCodeOperand_t(OpCodeAddressingMethod_I,  OpCodeOperandType_b));
    m_opCodeTable1[0x6B].SetOperatorInfo("IMUL",             3, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G,  OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_E,  OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0x6C].SetOperatorInfo("INS/INSB",         2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y,  OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x6D].SetOperatorInfo("INS/INSW/INSD",    2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y,  OpCodeOperandType_z),       OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x6E].SetOperatorInfo("OUTS/OUTSB",       2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_X,  OpCodeOperandType_b));
    m_opCodeTable1[0x6F].SetOperatorInfo("OUTS/OUTSW/OUTSD", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_X,  OpCodeOperandType_z));


    // Don't be fooled by the f64 flag on these instructions. These are short distance jump instructions...
    m_opCodeTable1[0x78].SetOperatorInfo("S",     1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x79].SetOperatorInfo("NS",    1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7A].SetOperatorInfo("P/PE",  1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7B].SetOperatorInfo("NP/PO", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7C].SetOperatorInfo("L/NGE", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7D].SetOperatorInfo("NL/GE", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7E].SetOperatorInfo("LE/NG", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));
    m_opCodeTable1[0x7F].SetOperatorInfo("NLE/G", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_rel8));


    m_opCodeTable1[0x88].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b));
    m_opCodeTable1[0x89].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v));
    m_opCodeTable1[0x8A].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_b), OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_b));
    m_opCodeTable1[0x8B].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));
    
    m_opCodeTable1[0x8C].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_S, OpCodeOperandType_w));
    m_opCodeTable1[0x8D].SetOperatorInfo("LEA", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_G, OpCodeOperandType_v), OpCodeOperand_t(OpCodeAddressingMethod_M, OpCodeOperandType_Invalid));
    
    m_opCodeTable1[0x8E].SetOperatorInfo("LEA", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_S, OpCodeOperandType_w), OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_w));
    m_opCodeTable1[0x8F].SetOperatorInfo("POP", 2, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_E, OpCodeOperandType_v));


    m_opCodeTable1[0x98].SetOperatorInfo("CBW/CWDE/CDQE", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x99].SetOperatorInfo("CWD/CDQ/CQO", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x9A].SetOperatorInfo("CALL", 2, OpCodeFlag_t::OpCodeFlag_i64, OpCodeOperand_t(OpCodeAddressingMethod_A, OpCodeOperandType_p));
    m_opCodeTable1[0x9B].SetOperatorInfo("FWAIT/WAIT", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0x9C].SetOperatorInfo("PUSHF/D/Q", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_F, OpCodeOperandType_v));
    m_opCodeTable1[0x9D].SetOperatorInfo("POPF/D/Q", 1, OpCodeFlag_t::OpCodeFlag_d64, OpCodeOperand_t(OpCodeAddressingMethod_F, OpCodeOperandType_v));
    
    m_opCodeTable1[0x9E].SetOperatorInfo("SAHF", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AH, OpCodeOperandType_Invalid));
    m_opCodeTable1[0x9F].SetOperatorInfo("LAHF", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AH, OpCodeOperandType_Invalid));


    m_opCodeTable1[0xA8].SetOperatorInfo("TEST", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xA9].SetOperatorInfo("TEST", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_z));

    m_opCodeTable1[0xAA].SetOperatorInfo("STOS/B",     2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y,   OpCodeOperandType_b),       OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid));
    m_opCodeTable1[0xAB].SetOperatorInfo("STOS/W/D/Q", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_Y,   OpCodeOperandType_v),       OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0xAC].SetOperatorInfo("LODS/B",     2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_X,   OpCodeOperandType_b));
    m_opCodeTable1[0xAD].SetOperatorInfo("LODS/W/D/Q", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_X,   OpCodeOperandType_v));
    m_opCodeTable1[0xAE].SetOperatorInfo("SCAS/B",     2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_Y,   OpCodeOperandType_b));
    m_opCodeTable1[0xAF].SetOperatorInfo("SCAS/W/D/Q", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_Y,   OpCodeOperandType_v));


    m_opCodeTable1[0xB8].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RAX,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xB9].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RCX,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBA].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RDX,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBB].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RBX,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBC].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RSP,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBD].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RBP,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBE].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RSI,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));
    m_opCodeTable1[0xBF].SetOperatorInfo("MOV", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_RDI,  OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_v));


    m_opCodeTable1[0xC8].SetOperatorInfo("ENTER",   2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_w), OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    m_opCodeTable1[0xC9].SetOperatorInfo("LEAVE",   0, OpCodeFlag_t::OpCodeFlag_d64);
    m_opCodeTable1[0xCA].SetOperatorInfo("far RET", 1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_w));
    m_opCodeTable1[0xCB].SetOperatorInfo("far RET", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xCC].SetOperatorInfo("INT3",    0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xCD].SetOperatorInfo("INT",     1, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_I, OpCodeOperandType_b));
    // 0xCE is invalid in 64 bit mode.
    m_opCodeTable1[0xCF].SetOperatorInfo("IRET/D/Q", 0, OpCodeFlag_t::OpCodeFlag_None);


    m_opCodeTable1[0xE8].SetOperatorInfo("near CALL", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_z));
    m_opCodeTable1[0xE9].SetOperatorInfo("JMP near", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_z));
    // 0xEA is invalid in 64 bit mode.
    m_opCodeTable1[0xEB].SetOperatorInfo("JMP short", 1, OpCodeFlag_t::OpCodeFlag_f64, OpCodeOperand_t(OpCodeAddressingMethod_J, OpCodeOperandType_b));
    m_opCodeTable1[0xEC].SetOperatorInfo("IN", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid));
    m_opCodeTable1[0xED].SetOperatorInfo("IN", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid));
    
    m_opCodeTable1[0xEE].SetOperatorInfo("OUT", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_AL, OpCodeOperandType_Invalid));
    m_opCodeTable1[0xEF].SetOperatorInfo("OUT", 2, OpCodeFlag_t::OpCodeFlag_None, OpCodeOperand_t(OpCodeAddressingMethod_DX, OpCodeOperandType_Invalid), OpCodeOperand_t(OpCodeAddressingMethod_EAX, OpCodeOperandType_Invalid));


    m_opCodeTable1[0xF8].SetOperatorInfo("CLC", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xF9].SetOperatorInfo("STC", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xFA].SetOperatorInfo("CLI", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xFB].SetOperatorInfo("STI", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xFC].SetOperatorInfo("CLD", 0, OpCodeFlag_t::OpCodeFlag_None);
    m_opCodeTable1[0xFD].SetOperatorInfo("STD", 0, OpCodeFlag_t::OpCodeFlag_None);
    
    m_opCodeTable1[0xFE].SetOperatorInfo("Grp 4", 0, OpCodeFlag_t::OpCodeFlag_1A);
    m_opCodeTable1[0xFF].SetOperatorInfo("Grp 5", 0, OpCodeFlag_t::OpCodeFlag_1A);


    return ErrorCode_t::ErrorCode_Success;
}
