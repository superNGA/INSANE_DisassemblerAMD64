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
#include <iostream> // TODO : Don't use this shit, make something better & and debug only!.


// NOTE : Mind this.
using namespace INSANE_DASM64_NAMESPACE;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCodeDesc_t::Init(
    const char* szName, bool bValidOpcd, bool bEscapeOpcd, bool bModrmRequired, Byte iByte, 
    int nOperands, Operand_t operand1, Operand_t operand2, Operand_t operand3, Operand_t operand4)
{
    strcpy_s(m_szName, sizeof(m_szName), szName);
    m_bIsValidCode   = bValidOpcd;
    m_bIsEscapeCode  = bEscapeOpcd;
    m_bModrmRequired = bModrmRequired;
    m_iByte          = iByte;
    m_nOperands      = nOperands;
    m_operands[0]    = operand1;
    m_operands[1]    = operand2;
    m_operands[2]    = operand3;
    m_operands[3]    = operand4;

    // At init, these default to "No varients"
    m_iVarientType   = VarientKey_None;
    m_nVarients      = 0;
    m_pVarients      = nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCodeDesc_t::InsertVarient(int iIndex)
{
    assert(m_pVarients != nullptr && "Varient array is not initialized");
    if (m_pVarients == nullptr)
        return false;


    assert(m_iVarientType != VarientType_t::VarientKey_None && "Varient type can't be null if we are inserting varient");
    if (m_iVarientType == VarientType_t::VarientKey_None)
        return false;


    // Invalid child index?
    assert(iIndex >= 0 && iIndex < GetMaxVarients(m_iVarientType) && "Invalid index for child whos varient type is to be set.");
    if (iIndex < 0 || iIndex >= GetMaxVarients(m_iVarientType))
        return false;


    // prevent mem leak.
    assert(m_pVarients[iIndex] == nullptr && "Element already inserted @ this index!");


    m_pVarients[iIndex] = new OpCodeDesc_t();


    // Failed memory allocation ?
    if (m_pVarients[iIndex] == nullptr)
        return false;


    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
size_t OpCodeDesc_t::GetMaxVarients(VarientType_t iVarientType)
{
    switch (iVarientType)
    {
    case OpCodeDesc_t::VarientKey_ModRM_REG:    return MAX_REG_VARIENTS;
    case OpCodeDesc_t::VarientKey_ModRM_RM:     return MAX_RM_VARIENTS;
    case OpCodeDesc_t::VarientKey_ModRM_MOD:    return MAX_MOD_VARIENTS;
    case OpCodeDesc_t::VarientKey_LegacyPrefix: return MAX_LEGACY_PREFIX_VARIENTS;
    
    default: break;
    }

    return 0llu;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCodeDesc_t::InitVarientType(VarientType_t iVarientType)
{
    // Already initialized as this varient type.
    if (m_iVarientType == iVarientType)
        return true;


    // Must be set to "No varient", else mem leak
    assert(m_iVarientType == VarientType_t::VarientKey_None && m_pVarients == nullptr && "Varient type is already set to some something!");
    if (m_iVarientType != VarientType_t::VarientKey_None || m_pVarients != nullptr)
    {
        printf("Object is already initialized for varient type [ %d ]\n", m_iVarientType);
        return false;
    }


    size_t iVarientArraySize = GetMaxVarients(iVarientType) * sizeof(void*);
    m_pVarients = reinterpret_cast<OpCodeDesc_t**>(malloc(iVarientArraySize));
    

    // Just in case malloc fails.
    if(m_pVarients == nullptr)
    {
        assert(m_pVarients != nullptr && "Failed memory allocation to varient array!");
        std::cout << "Failed memory allocation to varient array!\n";
        return false;
    }


    // Set all entries to nullptr.
    memset(m_pVarients, 0, iVarientArraySize);


    return true;
}


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
int Tables_t::GetLegacyPrefixIndex(Byte iByte)
{
    switch (iByte)
    {
    case 0xF0: return 1;
    case 0xF2: return 2;
    case 0xF3: return 3;
    case 0x2E: return 4;
    case 0x36: return 5;
    case 0x3E: return 6;
    case 0x26: return 7;
    case 0x64: return 8;
    case 0x65: return 9;
    case 0x66: return 10;
    case 0x67: return 11;
    
    default: break;
    }

    return 0;
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
    memset(m_opCodeTable1,    0, sizeof(m_opCodeTable1));
    memset(m_opCodeTable2,    0, sizeof(m_opCodeTable2));
    memset(m_opCodeTable3_38, 0, sizeof(m_opCodeTable3_38));
    memset(m_opCodeTable3_3A, 0, sizeof(m_opCodeTable3_3A));


    for (int i = 0; i <= 0xFF; i++)
    {
        m_opCodeTable1[i].m_bIsValidCode    = false; // we will set all the valid ones manually.
        m_opCodeTable2[i].m_bIsValidCode    = false;
        m_opCodeTable3_38[i].m_bIsValidCode = false;
        m_opCodeTable3_3A[i].m_bIsValidCode = false;
    }


    InitOneByteOpCodeTable();
    InitTwoByteOpCodeTable();
    InitThreeByteOpCodeTable_38();
    InitThreeByteOpCodeTable_3A();

        
    // Setting Escape characters...
    m_opCodeTable1[SpecialChars::ESCAPE_OPCODE_FIRST_INDEX].m_bIsEscapeCode    = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_A].m_bIsEscapeCode = true;
    m_opCodeTable2[SpecialChars::ESCAPE_OPCODE_SECOND_INDEX_B].m_bIsEscapeCode = true;


    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Tables_t::InitOneByteOpCodeTable()
{
    // 0x0
    // Brief : Add
    m_opCodeTable1[0x00].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x0,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1
    // Brief : Add
    m_opCodeTable1[0x01].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2
    // Brief : Add
    m_opCodeTable1[0x02].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x2,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3
    // Brief : Add
    m_opCodeTable1[0x03].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4
    // Brief : Add
    m_opCodeTable1[0x04].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5
    // Brief : Add
    m_opCodeTable1[0x05].Init(
        /*szName         = */"ADD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x6
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x7].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x08].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x09].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x9,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x0A].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xA,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x0B].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xB,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xC
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x0C].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xD
    // Brief : Logical Inclusive OR
    m_opCodeTable1[0x0D].Init(
        /*szName         = */"OR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xD,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xF].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x10
    // Brief : Add with Carry
    m_opCodeTable1[0x10].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x10,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x11
    // Brief : Add with Carry
    m_opCodeTable1[0x11].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x11,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x12
    // Brief : Add with Carry
    m_opCodeTable1[0x12].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x12,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x13
    // Brief : Add with Carry
    m_opCodeTable1[0x13].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x13,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x14
    // Brief : Add with Carry
    m_opCodeTable1[0x14].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x14,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x15
    // Brief : Add with Carry
    m_opCodeTable1[0x15].Init(
        /*szName         = */"ADC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x15,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x16
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x16].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x16,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x17
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x17].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x17,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x18
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x18].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x18,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x19
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x19].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x19,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1A
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x1A].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1A,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1B
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x1B].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1B,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1C
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x1C].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1C,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1D
    // Brief : Integer Subtraction with Borrow
    m_opCodeTable1[0x1D].Init(
        /*szName         = */"SBB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1D,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1E
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x1E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x1F
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x1F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x20
    // Brief : Logical AND
    m_opCodeTable1[0x20].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x20,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x21
    // Brief : Logical AND
    m_opCodeTable1[0x21].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x21,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x22
    // Brief : Logical AND
    m_opCodeTable1[0x22].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x22,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x23
    // Brief : Logical AND
    m_opCodeTable1[0x23].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x23,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x24
    // Brief : Logical AND
    m_opCodeTable1[0x24].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x24,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x25
    // Brief : Logical AND
    m_opCodeTable1[0x25].Init(
        /*szName         = */"AND",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x25,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x26
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x26].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x26,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x27
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x27].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x27,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x28
    // Brief : Subtract
    m_opCodeTable1[0x28].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x28,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x29
    // Brief : Subtract
    m_opCodeTable1[0x29].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x29,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2A
    // Brief : Subtract
    m_opCodeTable1[0x2A].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x2A,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2B
    // Brief : Subtract
    m_opCodeTable1[0x2B].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x2B,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2C
    // Brief : Subtract
    m_opCodeTable1[0x2C].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2C,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2D
    // Brief : Subtract
    m_opCodeTable1[0x2D].Init(
        /*szName         = */"SUB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2D,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2E
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x2E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x2F
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x2F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x30
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x30].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x30,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x31
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x31].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x31,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x32
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x32].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x32,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x33
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x33].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x33,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x34
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x34].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x34,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x35
    // Brief : Logical Exclusive OR
    m_opCodeTable1[0x35].Init(
        /*szName         = */"XOR",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x35,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x36
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x36].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x36,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x37
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x37].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x37,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x38
    // Brief : Compare Two Operands
    m_opCodeTable1[0x38].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x38,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x39
    // Brief : Compare Two Operands
    m_opCodeTable1[0x39].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x39,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3A
    // Brief : Compare Two Operands
    m_opCodeTable1[0x3A].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3A,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3B
    // Brief : Compare Two Operands
    m_opCodeTable1[0x3B].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3B,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3C
    // Brief : Compare Two Operands
    m_opCodeTable1[0x3C].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3C,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3D
    // Brief : Compare Two Operands
    m_opCodeTable1[0x3D].Init(
        /*szName         = */"CMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3D,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3E
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x3E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x3F
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x3F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x40
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x40].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x40,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x41
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x41].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x41,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x42
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x42].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x42,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x43
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x43].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x43,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x44
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x44].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x44,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x45
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x45].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x45,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x46
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x46].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x46,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x47
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x47].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x47,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x48
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x48].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x48,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x49
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x49].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x49,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4A
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4B
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4C
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4D
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4E
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x4F
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x4F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x50
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x50].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x50,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x51
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x51].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x51,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x52
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x52].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x52,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x53
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x53].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x53,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x54
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x54].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x54,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x55
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x55].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x55,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x56
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x56].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x56,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x57
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x57].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x57,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x58
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x58].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x58,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x59
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x59].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x59,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5A
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5A].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5A,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5B
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5B].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5B,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5C
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5C].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5C,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5D
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5D].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5D,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5E
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5E].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5E,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x5F
    // Brief : Pop a Value from the Stack
    m_opCodeTable1[0x5F].Init(
        /*szName         = */"POP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5F,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_64_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x60
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x60].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x60,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x61
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x61].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x61,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x62
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x62].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x62,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x63
    // Brief : Move with Sign-Extension
    m_opCodeTable1[0x63].Init(
        /*szName         = */"MOVSXD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x63,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x64
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x64].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x64,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x65
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x65].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x65,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x66
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x66].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x66,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x67
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x67].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x67,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x68
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x68].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x68,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x69
    // Brief : Signed Multiply
    m_opCodeTable1[0x69].Init(
        /*szName         = */"IMUL",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x69,
        /*nOperands      = */3,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand4       = */Operand_t());

    // 0x6A
    // Brief : Push Word, Doubleword or Quadword Onto the Stack
    m_opCodeTable1[0x6A].Init(
        /*szName         = */"PUSH",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6A,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x6B
    // Brief : Signed Multiply
    m_opCodeTable1[0x6B].Init(
        /*szName         = */"IMUL",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x6B,
        /*nOperands      = */3,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand4       = */Operand_t());

    // 0x6C
    // Brief : Input from Port to String
    m_opCodeTable1[0x6C].Init(
        /*szName         = */"INS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6C,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x6D
    // Brief : Input from Port to String
    m_opCodeTable1[0x6D].Init(
        /*szName         = */"INS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6D,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x6E
    // Brief : Output String to Port
    m_opCodeTable1[0x6E].Init(
        /*szName         = */"OUTS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6E,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x6F
    // Brief : Output String to Port
    m_opCodeTable1[0x6F].Init(
        /*szName         = */"OUTS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6F,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x70
    // Brief : Jump short if overflow (OF=1)
    m_opCodeTable1[0x70].Init(
        /*szName         = */"JO",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x70,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x71
    // Brief : Jump short if not overflow (OF=0)
    m_opCodeTable1[0x71].Init(
        /*szName         = */"JNO",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x71,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x72
    // Brief : Jump short if below/not above or equal/carry (CF=1)
    m_opCodeTable1[0x72].Init(
        /*szName         = */"JB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x72,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x73
    // Brief : Jump short if not below/above or equal/not carry (CF=0)
    m_opCodeTable1[0x73].Init(
        /*szName         = */"JNB",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x73,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x74
    // Brief : Jump short if zero/equal (ZF=1)
    m_opCodeTable1[0x74].Init(
        /*szName         = */"JZ",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x74,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x75
    // Brief : Jump short if not zero/not equal (ZF=0)
    m_opCodeTable1[0x75].Init(
        /*szName         = */"JNZ",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x75,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x76
    // Brief : Jump short if below or equal/not above (CF=1 OR ZF=1)
    m_opCodeTable1[0x76].Init(
        /*szName         = */"JBE",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x76,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x77
    // Brief : Jump short if not below or equal/above (CF=0 AND ZF=0)
    m_opCodeTable1[0x77].Init(
        /*szName         = */"JNBE",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x77,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x78
    // Brief : Jump short if sign (SF=1)
    m_opCodeTable1[0x78].Init(
        /*szName         = */"JS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x78,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x79
    // Brief : Jump short if not sign (SF=0)
    m_opCodeTable1[0x79].Init(
        /*szName         = */"JNS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x79,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7A
    // Brief : Jump short if parity/parity even (PF=1)
    m_opCodeTable1[0x7A].Init(
        /*szName         = */"JP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7A,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7B
    // Brief : Jump short if not parity/parity odd (PF=0)
    m_opCodeTable1[0x7B].Init(
        /*szName         = */"JNP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7B,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7C
    // Brief : Jump short if less/not greater (SF!=OF)
    m_opCodeTable1[0x7C].Init(
        /*szName         = */"JL",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7C,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7D
    // Brief : Jump short if not less/greater or equal (SF=OF)
    m_opCodeTable1[0x7D].Init(
        /*szName         = */"JNL",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7D,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7E
    // Brief : Jump short if less or equal/not greater ((ZF=1) OR (SF!=OF))
    m_opCodeTable1[0x7E].Init(
        /*szName         = */"JLE",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7E,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x7F
    // Brief : Jump short if not less nor equal/greater ((ZF=0) AND (SF=OF))
    m_opCodeTable1[0x7F].Init(
        /*szName         = */"JNLE",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7F,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0x80].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0x80].InsertVarient(0);
    {
        // 0x80
        // Brief : Add
        m_opCodeTable1[0x80].m_pVarients[0x00]->Init(
            /*szName         = */"ADD",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(1);
    {
        // 0x80
        // Brief : Logical Inclusive OR
        m_opCodeTable1[0x80].m_pVarients[0x01]->Init(
            /*szName         = */"OR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(2);
    {
        // 0x80
        // Brief : Add with Carry
        m_opCodeTable1[0x80].m_pVarients[0x02]->Init(
            /*szName         = */"ADC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(3);
    {
        // 0x80
        // Brief : Integer Subtraction with Borrow
        m_opCodeTable1[0x80].m_pVarients[0x03]->Init(
            /*szName         = */"SBB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(4);
    {
        // 0x80
        // Brief : Logical AND
        m_opCodeTable1[0x80].m_pVarients[0x04]->Init(
            /*szName         = */"AND",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(5);
    {
        // 0x80
        // Brief : Subtract
        m_opCodeTable1[0x80].m_pVarients[0x05]->Init(
            /*szName         = */"SUB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(6);
    {
        // 0x80
        // Brief : Logical Exclusive OR
        m_opCodeTable1[0x80].m_pVarients[0x06]->Init(
            /*szName         = */"XOR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x80].InsertVarient(7);
    {
        // 0x80
        // Brief : Compare Two Operands
        m_opCodeTable1[0x80].m_pVarients[0x07]->Init(
            /*szName         = */"CMP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x80,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0x81].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0x81].InsertVarient(0);
    {
        // 0x81
        // Brief : Add
        m_opCodeTable1[0x81].m_pVarients[0x00]->Init(
            /*szName         = */"ADD",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(1);
    {
        // 0x81
        // Brief : Logical Inclusive OR
        m_opCodeTable1[0x81].m_pVarients[0x01]->Init(
            /*szName         = */"OR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(2);
    {
        // 0x81
        // Brief : Add with Carry
        m_opCodeTable1[0x81].m_pVarients[0x02]->Init(
            /*szName         = */"ADC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(3);
    {
        // 0x81
        // Brief : Integer Subtraction with Borrow
        m_opCodeTable1[0x81].m_pVarients[0x03]->Init(
            /*szName         = */"SBB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(4);
    {
        // 0x81
        // Brief : Logical AND
        m_opCodeTable1[0x81].m_pVarients[0x04]->Init(
            /*szName         = */"AND",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(5);
    {
        // 0x81
        // Brief : Subtract
        m_opCodeTable1[0x81].m_pVarients[0x05]->Init(
            /*szName         = */"SUB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(6);
    {
        // 0x81
        // Brief : Logical Exclusive OR
        m_opCodeTable1[0x81].m_pVarients[0x06]->Init(
            /*szName         = */"XOR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x81].InsertVarient(7);
    {
        // 0x81
        // Brief : Compare Two Operands
        m_opCodeTable1[0x81].m_pVarients[0x07]->Init(
            /*szName         = */"CMP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x81,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0x82
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x82].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x82,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0x83].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0x83].InsertVarient(0);
    {
        // 0x83
        // Brief : Add
        m_opCodeTable1[0x83].m_pVarients[0x00]->Init(
            /*szName         = */"ADD",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(1);
    {
        // 0x83
        // Brief : Logical Inclusive OR
        m_opCodeTable1[0x83].m_pVarients[0x01]->Init(
            /*szName         = */"OR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(2);
    {
        // 0x83
        // Brief : Add with Carry
        m_opCodeTable1[0x83].m_pVarients[0x02]->Init(
            /*szName         = */"ADC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(3);
    {
        // 0x83
        // Brief : Integer Subtraction with Borrow
        m_opCodeTable1[0x83].m_pVarients[0x03]->Init(
            /*szName         = */"SBB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(4);
    {
        // 0x83
        // Brief : Logical AND
        m_opCodeTable1[0x83].m_pVarients[0x04]->Init(
            /*szName         = */"AND",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(5);
    {
        // 0x83
        // Brief : Subtract
        m_opCodeTable1[0x83].m_pVarients[0x05]->Init(
            /*szName         = */"SUB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(6);
    {
        // 0x83
        // Brief : Logical Exclusive OR
        m_opCodeTable1[0x83].m_pVarients[0x06]->Init(
            /*szName         = */"XOR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x83].InsertVarient(7);
    {
        // 0x83
        // Brief : Compare Two Operands
        m_opCodeTable1[0x83].m_pVarients[0x07]->Init(
            /*szName         = */"CMP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x83,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0x84
    // Brief : Logical Compare
    m_opCodeTable1[0x84].Init(
        /*szName         = */"TEST",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x84,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x85
    // Brief : Logical Compare
    m_opCodeTable1[0x85].Init(
        /*szName         = */"TEST",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x85,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x86
    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0x86].Init(
        /*szName         = */"XCHG",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x86,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x87
    // Brief : Exchange Register/Memory with Register
    m_opCodeTable1[0x87].Init(
        /*szName         = */"XCHG",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x87,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x88
    // Brief : Move
    m_opCodeTable1[0x88].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x88,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x89
    // Brief : Move
    m_opCodeTable1[0x89].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x89,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8A
    // Brief : Move
    m_opCodeTable1[0x8A].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8A,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8B
    // Brief : Move
    m_opCodeTable1[0x8B].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8B,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8C
    // Brief : Move
    m_opCodeTable1[0x8C].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8C,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_m, OperandType_16 ),
        /*operand2       = */Operand_t( OperandMode_Sreg, OperandType_16 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8D
    // Brief : Load Effective Address
    m_opCodeTable1[0x8D].Init(
        /*szName         = */"LEA",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8D,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_m, OperandType_Invalid ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x8E
    // Brief : Move
    m_opCodeTable1[0x8E].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8E,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_Sreg, OperandType_16 ),
        /*operand2       = */Operand_t( OperandMode_rm, OperandType_16 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0x8F].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0x8F].InsertVarient(0);
    {
        // 0x8F
        // Brief : Pop a Value from the Stack
        m_opCodeTable1[0x8F].m_pVarients[0x00]->Init(
            /*szName         = */"POP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x8F,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_64_16 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0x90].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);

    m_opCodeTable1[0x90].InsertVarient(0);
    {
        // 0x90
        // Brief : No Operation
        m_opCodeTable1[0x90].m_pVarients[0x00]->Init(
            /*szName         = */"NOP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */false,
            /*iByte          = */0x90,
            /*nOperands      = */0,
            /*operand1       = */Operand_t(),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0x90].InsertVarient(3);
    {
        // 0x90
        // Brief : Spin Loop Hint
        m_opCodeTable1[0x90].m_pVarients[0x03]->Init(
            /*szName         = */"PAUSE",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */false,
            /*iByte          = */0x90,
            /*nOperands      = */0,
            /*operand1       = */Operand_t(),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0x91
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x91].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x91,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x92
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x92].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x92,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x93
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x93].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x93,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x94
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x94].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x94,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x95
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x95].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x95,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x96
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x96].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x96,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x97
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x97].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x97,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x98
    // Brief : Convert
    m_opCodeTable1[0x98].Init(
        /*szName         = */"CBW",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x98,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x99
    // Brief : Convert
    m_opCodeTable1[0x99].Init(
        /*szName         = */"CWD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x99,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9A
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x9A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9B
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0x9B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9C
    // Brief : Push rFLAGS Register onto the Stack
    m_opCodeTable1[0x9C].Init(
        /*szName         = */"PUSHF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9D
    // Brief : Pop Stack into rFLAGS Register
    m_opCodeTable1[0x9D].Init(
        /*szName         = */"POPF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9E
    // Brief : Store AH into Flags
    m_opCodeTable1[0x9E].Init(
        /*szName         = */"SAHF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0x9F
    // Brief : Load Status Flags into AH Register
    m_opCodeTable1[0x9F].Init(
        /*szName         = */"LAHF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA0
    // Brief : Move
    m_opCodeTable1[0xA0].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA0,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_moffs, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA1
    // Brief : Move
    m_opCodeTable1[0xA1].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA1,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_moffs, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA2
    // Brief : Move
    m_opCodeTable1[0xA2].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA2,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_moffs, OperandType_8 ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA3
    // Brief : Move
    m_opCodeTable1[0xA3].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA3,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_moffs, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA4
    // Brief : Move Data from String to String
    m_opCodeTable1[0xA4].Init(
        /*szName         = */"MOVS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA4,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA5
    // Brief : Move Data from String to String
    m_opCodeTable1[0xA5].Init(
        /*szName         = */"MOVS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA5,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA6
    // Brief : Compare String Operands
    m_opCodeTable1[0xA6].Init(
        /*szName         = */"CMPS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA6,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA7
    // Brief : Compare String Operands
    m_opCodeTable1[0xA7].Init(
        /*szName         = */"CMPS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA7,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA8
    // Brief : Logical Compare
    m_opCodeTable1[0xA8].Init(
        /*szName         = */"TEST",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA8,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xA9
    // Brief : Logical Compare
    m_opCodeTable1[0xA9].Init(
        /*szName         = */"TEST",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA9,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 64 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAA
    // Brief : Store String
    m_opCodeTable1[0xAA].Init(
        /*szName         = */"STOS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAA,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAB
    // Brief : Store String
    m_opCodeTable1[0xAB].Init(
        /*szName         = */"STOS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAB,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAC
    // Brief : Load String
    m_opCodeTable1[0xAC].Init(
        /*szName         = */"LODS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAC,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAD
    // Brief : Load String
    m_opCodeTable1[0xAD].Init(
        /*szName         = */"LODS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAD,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 6, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAE
    // Brief : Scan String
    m_opCodeTable1[0xAE].Init(
        /*szName         = */"SCAS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAE,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xAF
    // Brief : Scan String
    m_opCodeTable1[0xAF].Init(
        /*szName         = */"SCAS",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAF,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 7, 64 ) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB0
    // Brief : Move
    m_opCodeTable1[0xB0].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB0,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB1
    // Brief : Move
    m_opCodeTable1[0xB1].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB1,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB2
    // Brief : Move
    m_opCodeTable1[0xB2].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB2,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB3
    // Brief : Move
    m_opCodeTable1[0xB3].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB3,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB4
    // Brief : Move
    m_opCodeTable1[0xB4].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB4,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB5
    // Brief : Move
    m_opCodeTable1[0xB5].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB5,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB6
    // Brief : Move
    m_opCodeTable1[0xB6].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB6,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB7
    // Brief : Move
    m_opCodeTable1[0xB7].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB7,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_8 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB8
    // Brief : Move
    m_opCodeTable1[0xB8].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB8,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xB9
    // Brief : Move
    m_opCodeTable1[0xB9].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB9,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBA
    // Brief : Move
    m_opCodeTable1[0xBA].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBA,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBB
    // Brief : Move
    m_opCodeTable1[0xBB].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBB,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBC
    // Brief : Move
    m_opCodeTable1[0xBC].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBC,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBD
    // Brief : Move
    m_opCodeTable1[0xBD].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBD,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBE
    // Brief : Move
    m_opCodeTable1[0xBE].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBE,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xBF
    // Brief : Move
    m_opCodeTable1[0xBF].Init(
        /*szName         = */"MOV",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBF,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_r, OperandType_16_32_64 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32_64 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xC0].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xC0].InsertVarient(0);
    {
        // 0xC0
        // Brief : Rotate
        m_opCodeTable1[0xC0].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(1);
    {
        // 0xC0
        // Brief : Rotate
        m_opCodeTable1[0xC0].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(2);
    {
        // 0xC0
        // Brief : Rotate
        m_opCodeTable1[0xC0].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(3);
    {
        // 0xC0
        // Brief : Rotate
        m_opCodeTable1[0xC0].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(4);
    {
        // 0xC0
        // Brief : Shift
        m_opCodeTable1[0xC0].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(5);
    {
        // 0xC0
        // Brief : Shift
        m_opCodeTable1[0xC0].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(6);
    {
        // 0xC0
        // Brief : Shift
        m_opCodeTable1[0xC0].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC0].InsertVarient(7);
    {
        // 0xC0
        // Brief : Shift
        m_opCodeTable1[0xC0].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xC1].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xC1].InsertVarient(0);
    {
        // 0xC1
        // Brief : Rotate
        m_opCodeTable1[0xC1].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(1);
    {
        // 0xC1
        // Brief : Rotate
        m_opCodeTable1[0xC1].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(2);
    {
        // 0xC1
        // Brief : Rotate
        m_opCodeTable1[0xC1].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(3);
    {
        // 0xC1
        // Brief : Rotate
        m_opCodeTable1[0xC1].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(4);
    {
        // 0xC1
        // Brief : Shift
        m_opCodeTable1[0xC1].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(5);
    {
        // 0xC1
        // Brief : Shift
        m_opCodeTable1[0xC1].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(6);
    {
        // 0xC1
        // Brief : Shift
        m_opCodeTable1[0xC1].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xC1].InsertVarient(7);
    {
        // 0xC1
        // Brief : Shift
        m_opCodeTable1[0xC1].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0xC2
    // Brief : Return from procedure
    m_opCodeTable1[0xC2].Init(
        /*szName         = */"RETN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC2,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xC3
    // Brief : Return from procedure
    m_opCodeTable1[0xC3].Init(
        /*szName         = */"RETN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xC4
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xC4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xC5
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xC5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xC6].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xC6].InsertVarient(0);
    {
        // 0xC6
        // Brief : Move
        m_opCodeTable1[0xC6].m_pVarients[0x00]->Init(
            /*szName         = */"MOV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC6,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xC7].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xC7].InsertVarient(0);
    {
        // 0xC7
        // Brief : Move
        m_opCodeTable1[0xC7].m_pVarients[0x00]->Init(
            /*szName         = */"MOV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC7,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0xC8
    // Brief : Make Stack Frame for Procedure Parameters
    m_opCodeTable1[0xC8].Init(
        /*szName         = */"ENTER",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC8,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_16 ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xC9
    // Brief : High Level Procedure Exit
    m_opCodeTable1[0xC9].Init(
        /*szName         = */"LEAVE",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCA
    // Brief : Return from procedure
    m_opCodeTable1[0xCA].Init(
        /*szName         = */"RETF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCA,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_16 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCB
    // Brief : Return from procedure
    m_opCodeTable1[0xCB].Init(
        /*szName         = */"RETF",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCC
    // Brief : Call to Interrupt Procedure
    m_opCodeTable1[0xCC].Init(
        /*szName         = */"INT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCC,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( 3 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCD
    // Brief : Call to Interrupt Procedure
    m_opCodeTable1[0xCD].Init(
        /*szName         = */"INT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCD,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCE
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xCE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xCF
    // Brief : Interrupt Return
    m_opCodeTable1[0xCF].Init(
        /*szName         = */"IRET",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xD0].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD0].InsertVarient(0);
    {
        // 0xD0
        // Brief : Rotate
        m_opCodeTable1[0xD0].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(1);
    {
        // 0xD0
        // Brief : Rotate
        m_opCodeTable1[0xD0].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(2);
    {
        // 0xD0
        // Brief : Rotate
        m_opCodeTable1[0xD0].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(3);
    {
        // 0xD0
        // Brief : Rotate
        m_opCodeTable1[0xD0].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(4);
    {
        // 0xD0
        // Brief : Shift
        m_opCodeTable1[0xD0].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(5);
    {
        // 0xD0
        // Brief : Shift
        m_opCodeTable1[0xD0].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(6);
    {
        // 0xD0
        // Brief : Shift
        m_opCodeTable1[0xD0].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD0].InsertVarient(7);
    {
        // 0xD0
        // Brief : Shift
        m_opCodeTable1[0xD0].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD0,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xD1].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD1].InsertVarient(0);
    {
        // 0xD1
        // Brief : Rotate
        m_opCodeTable1[0xD1].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(1);
    {
        // 0xD1
        // Brief : Rotate
        m_opCodeTable1[0xD1].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(2);
    {
        // 0xD1
        // Brief : Rotate
        m_opCodeTable1[0xD1].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(3);
    {
        // 0xD1
        // Brief : Rotate
        m_opCodeTable1[0xD1].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(4);
    {
        // 0xD1
        // Brief : Shift
        m_opCodeTable1[0xD1].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(5);
    {
        // 0xD1
        // Brief : Shift
        m_opCodeTable1[0xD1].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(6);
    {
        // 0xD1
        // Brief : Shift
        m_opCodeTable1[0xD1].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD1].InsertVarient(7);
    {
        // 0xD1
        // Brief : Shift
        m_opCodeTable1[0xD1].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD1,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( 1 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xD2].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD2].InsertVarient(0);
    {
        // 0xD2
        // Brief : Rotate
        m_opCodeTable1[0xD2].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(1);
    {
        // 0xD2
        // Brief : Rotate
        m_opCodeTable1[0xD2].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(2);
    {
        // 0xD2
        // Brief : Rotate
        m_opCodeTable1[0xD2].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(3);
    {
        // 0xD2
        // Brief : Rotate
        m_opCodeTable1[0xD2].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(4);
    {
        // 0xD2
        // Brief : Shift
        m_opCodeTable1[0xD2].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(5);
    {
        // 0xD2
        // Brief : Shift
        m_opCodeTable1[0xD2].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(6);
    {
        // 0xD2
        // Brief : Shift
        m_opCodeTable1[0xD2].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD2].InsertVarient(7);
    {
        // 0xD2
        // Brief : Shift
        m_opCodeTable1[0xD2].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD2,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xD3].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD3].InsertVarient(0);
    {
        // 0xD3
        // Brief : Rotate
        m_opCodeTable1[0xD3].m_pVarients[0x00]->Init(
            /*szName         = */"ROL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(1);
    {
        // 0xD3
        // Brief : Rotate
        m_opCodeTable1[0xD3].m_pVarients[0x01]->Init(
            /*szName         = */"ROR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(2);
    {
        // 0xD3
        // Brief : Rotate
        m_opCodeTable1[0xD3].m_pVarients[0x02]->Init(
            /*szName         = */"RCL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(3);
    {
        // 0xD3
        // Brief : Rotate
        m_opCodeTable1[0xD3].m_pVarients[0x03]->Init(
            /*szName         = */"RCR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(4);
    {
        // 0xD3
        // Brief : Shift
        m_opCodeTable1[0xD3].m_pVarients[0x04]->Init(
            /*szName         = */"SHL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(5);
    {
        // 0xD3
        // Brief : Shift
        m_opCodeTable1[0xD3].m_pVarients[0x05]->Init(
            /*szName         = */"SHR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(6);
    {
        // 0xD3
        // Brief : Shift
        m_opCodeTable1[0xD3].m_pVarients[0x06]->Init(
            /*szName         = */"SAL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD3].InsertVarient(7);
    {
        // 0xD3
        // Brief : Shift
        m_opCodeTable1[0xD3].m_pVarients[0x07]->Init(
            /*szName         = */"SAR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD3,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 1, 8 ) ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0xD4
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xD4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xD4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xD5
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xD5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xD5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xD6
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xD6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xD6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xD7
    // Brief : Table Look-up Translation
    m_opCodeTable1[0xD7].Init(
        /*szName         = */"XLAT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xD7,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( Register_t(Register_t::RegisterClass_t::RegisterClass_GPR, 3, 64) ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xD8].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD8].InsertVarient(0);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Add
            m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Add
            m_opCodeTable1[0xD8].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(1);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Multiply
            m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Multiply
            m_opCodeTable1[0xD8].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(2);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Compare Real
            m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FCOM",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STim, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x02]->InsertVarient(3);
        {
            m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD8
                // Brief : Compare Real
                m_opCodeTable1[0xD8].m_pVarients[0x02]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD8,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(3);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Compare Real and Pop
            m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FCOMP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STim, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x03]->InsertVarient(3);
        {
            m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD8
                // Brief : Compare Real and Pop
                m_opCodeTable1[0xD8].m_pVarients[0x03]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FCOMP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD8,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(4);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Subtract
            m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FSUB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x04]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Subtract
            m_opCodeTable1[0xD8].m_pVarients[0x04]->m_pVarients[0x03]->Init(
                /*szName         = */"FSUB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(5);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Reverse Subtract
            m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FSUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x05]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Reverse Subtract
            m_opCodeTable1[0xD8].m_pVarients[0x05]->m_pVarients[0x03]->Init(
                /*szName         = */"FSUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(6);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x06]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x06]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Divide
            m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x00]->Init(
                /*szName         = */"FDIV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x06]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Divide
            m_opCodeTable1[0xD8].m_pVarients[0x06]->m_pVarients[0x03]->Init(
                /*szName         = */"FDIV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD8].InsertVarient(7);
    {
        m_opCodeTable1[0xD8].m_pVarients[0x07]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD8].m_pVarients[0x07]->InsertVarient(0);
        m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x01] = m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x00];
        m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x02] = m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x00];
        {
            // 0xD8
            // Brief : Reverse Divide
            m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x00]->Init(
                /*szName         = */"FDIVR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD8].m_pVarients[0x07]->InsertVarient(3);
        {
            // 0xD8
            // Brief : Reverse Divide
            m_opCodeTable1[0xD8].m_pVarients[0x07]->m_pVarients[0x03]->Init(
                /*szName         = */"FDIVR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD8,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }

    m_opCodeTable1[0xD9].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xD9].InsertVarient(0);
    {
        // 0xD9
        // Brief : Load Floating Point Value
        m_opCodeTable1[0xD9].m_pVarients[0x00]->Init(
            /*szName         = */"FLD",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD9,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_STim, OperandType_32real ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xD9].InsertVarient(1);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Exchange Register Contents
            m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FXCH",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x01]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD9
                // Brief : Exchange Register Contents
                m_opCodeTable1[0xD9].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FXCH",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(2);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Store Floating Point Value
            m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FST",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x02]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xD9
                // Brief : No Operation
                m_opCodeTable1[0xD9].m_pVarients[0x02]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FNOP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(3);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FSTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xD9
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xD9].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FSTP1",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(4);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Load x87 FPU Environment
            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FLDENV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_14_28 ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x04]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xD9
                // Brief : Change Sign
                m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FCHS",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD9
                // Brief : Absolute Value
                m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FABS",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(4);
            {
                // 0xD9
                // Brief : Test
                m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x04]->Init(
                    /*szName         = */"FTST",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(5);
            {
                // 0xD9
                // Brief : Examine
                m_opCodeTable1[0xD9].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x05]->Init(
                    /*szName         = */"FXAM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(5);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Load x87 FPU Control Word
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FLDCW",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16 ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x05]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xD9
                // Brief : Load Constant +1.0
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FLD1",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD9
                // Brief : Load Constant log
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FLDL2T",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(2);
            {
                // 0xD9
                // Brief : Load Constant log
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x02]->Init(
                    /*szName         = */"FLDL2E",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(3);
            {
                // 0xD9
                // Brief : Load Constant 
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x03]->Init(
                    /*szName         = */"FLDPI",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(4);
            {
                // 0xD9
                // Brief : Load Constant log
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x04]->Init(
                    /*szName         = */"FLDLG2",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(5);
            {
                // 0xD9
                // Brief : Load Constant log
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x05]->Init(
                    /*szName         = */"FLDLN2",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(6);
            {
                // 0xD9
                // Brief : Load Constant +0.0
                m_opCodeTable1[0xD9].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x06]->Init(
                    /*szName         = */"FLDZ",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(6);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x06]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x06]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Store x87 FPU Environment
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x00]->Init(
                /*szName         = */"FSTENV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_14_28 ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x06]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xD9
                // Brief : Compute 2
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"F2XM1",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD9
                // Brief : Compute y  log
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FYL2X",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(2);
            {
                // 0xD9
                // Brief : Partial Tangent
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x02]->Init(
                    /*szName         = */"FPTAN",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(3);
            {
                // 0xD9
                // Brief : Partial Arctangent and Pop
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x03]->Init(
                    /*szName         = */"FPATAN",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(4);
            {
                // 0xD9
                // Brief : Extract Exponent and Significand
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x04]->Init(
                    /*szName         = */"FXTRACT",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(5);
            {
                // 0xD9
                // Brief : IEEE Partial Remainder
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x05]->Init(
                    /*szName         = */"FPREM1",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(6);
            {
                // 0xD9
                // Brief : Decrement Stack-Top Pointer
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x06]->Init(
                    /*szName         = */"FDECSTP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xD9
                // Brief : Increment Stack-Top Pointer
                m_opCodeTable1[0xD9].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FINCSTP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xD9].InsertVarient(7);
    {
        m_opCodeTable1[0xD9].m_pVarients[0x07]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xD9].m_pVarients[0x07]->InsertVarient(0);
        m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x01] = m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x00];
        m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x02] = m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x00];
        {
            // 0xD9
            // Brief : Store x87 FPU Control Word
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x00]->Init(
                /*szName         = */"FSTCW",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xD9,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16 ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xD9].m_pVarients[0x07]->InsertVarient(3);
        {
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xD9
                // Brief : Partial Remainder (for compatibility with i8087 and i287)
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FPREM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xD9
                // Brief : Compute y  log
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FYL2XP1",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(2);
            {
                // 0xD9
                // Brief : Square Root
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x02]->Init(
                    /*szName         = */"FSQRT",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(3);
            {
                // 0xD9
                // Brief : Sine and Cosine
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x03]->Init(
                    /*szName         = */"FSINCOS",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(4);
            {
                // 0xD9
                // Brief : Round to Integer
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x04]->Init(
                    /*szName         = */"FRNDINT",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(5);
            {
                // 0xD9
                // Brief : Scale
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x05]->Init(
                    /*szName         = */"FSCALE",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(6);
            {
                // 0xD9
                // Brief : Sine
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x06]->Init(
                    /*szName         = */"FSIN",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xD9
                // Brief : Cosine
                m_opCodeTable1[0xD9].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FCOS",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xD9,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }

    m_opCodeTable1[0xDA].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDA].InsertVarient(0);
    {
        m_opCodeTable1[0xDA].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDA].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDA
            // Brief : Add
            m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FIADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDA].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xDA
            // Brief : FP Conditional Move - below (CF=1)
            m_opCodeTable1[0xDA].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDA].InsertVarient(1);
    {
        m_opCodeTable1[0xDA].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDA].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDA
            // Brief : Multiply
            m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FIMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDA].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xDA
            // Brief : FP Conditional Move - equal (ZF=1)
            m_opCodeTable1[0xDA].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVE",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDA].InsertVarient(2);
    {
        m_opCodeTable1[0xDA].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDA].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDA
            // Brief : Compare Integer
            m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FICOM",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDA].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDA
            // Brief : FP Conditional Move - below or equal (CF=1 or ZF=1)
            m_opCodeTable1[0xDA].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVBE",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDA].InsertVarient(3);
    {
        m_opCodeTable1[0xDA].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDA].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDA
            // Brief : Compare Integer and Pop
            m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FICOMP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDA].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xDA
            // Brief : FP Conditional Move - unordered (PF=1)
            m_opCodeTable1[0xDA].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVU",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDA].InsertVarient(4);
    {
        // 0xDA
        // Brief : Subtract
        m_opCodeTable1[0xDA].m_pVarients[0x04]->Init(
            /*szName         = */"FISUB",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDA,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xDA].InsertVarient(5);
    {
        m_opCodeTable1[0xDA].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDA].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDA
            // Brief : Reverse Subtract
            m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FISUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDA,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDA].m_pVarients[0x05]->InsertVarient(3);
        {
            m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDA
                // Brief : Unordered Compare Floating Point Values and Pop Twice
                m_opCodeTable1[0xDA].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FUCOMPP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDA,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDA].InsertVarient(6);
    {
        // 0xDA
        // Brief : Divide
        m_opCodeTable1[0xDA].m_pVarients[0x06]->Init(
            /*szName         = */"FIDIV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDA,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xDA].InsertVarient(7);
    {
        // 0xDA
        // Brief : Reverse Divide
        m_opCodeTable1[0xDA].m_pVarients[0x07]->Init(
            /*szName         = */"FIDIVR",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDA,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xDB].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDB].InsertVarient(0);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDB].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDB
            // Brief : Load Integer
            m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FILD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDB].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xDB
            // Brief : FP Conditional Move - not below (CF=0)
            m_opCodeTable1[0xDB].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVNB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(1);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDB].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDB
            // Brief : Store Integer with Truncation and Pop
            m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FISTTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDB].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xDB
            // Brief : FP Conditional Move - not equal (ZF=0)
            m_opCodeTable1[0xDB].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVNE",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(2);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDB].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDB
            // Brief : Store Integer
            m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FIST",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDB].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDB
            // Brief : FP Conditional Move - below or equal (CF=0 and ZF=0)
            m_opCodeTable1[0xDB].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVNBE",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(3);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDB].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDB
            // Brief : Store Integer and Pop
            m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FISTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_32int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDB].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xDB
            // Brief : FP Conditional Move - not unordered (PF=0)
            m_opCodeTable1[0xDB].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FCMOVNU",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(4);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        m_opCodeTable1[0xDB].m_pVarients[0x04]->InsertVarient(3);
        {
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xDB
                // Brief : Treated as Integer NOP
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FNENI",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDB
                // Brief : Treated as Integer NOP
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FNDISI",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(2);
            {
                // 0xDB
                // Brief : Clear Exceptions
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x02]->Init(
                    /*szName         = */"FNCLEX",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(3);
            {
                // 0xDB
                // Brief : Initialize Floating-Point Unit
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x03]->Init(
                    /*szName         = */"FNINIT",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(4);
            {
                // 0xDB
                // Brief : Treated as Integer NOP
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x04]->Init(
                    /*szName         = */"FNSETPM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(2);
            {
                // 0xDB
                // Brief : Clear Exceptions
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x02]->Init(
                    /*szName         = */"FCLEX",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(3);
            {
                // 0xDB
                // Brief : Initialize Floating-Point Unit
                m_opCodeTable1[0xDB].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x03]->Init(
                    /*szName         = */"FINIT",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDB,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(5);
    {
        m_opCodeTable1[0xDB].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDB].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDB
            // Brief : Load Floating Point Value
            m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FLD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_80real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDB].m_pVarients[0x05]->InsertVarient(3);
        {
            // 0xDB
            // Brief : Unordered Compare Floating Point Values and Set EFLAGS
            m_opCodeTable1[0xDB].m_pVarients[0x05]->m_pVarients[0x03]->Init(
                /*szName         = */"FUCOMI",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDB,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDB].InsertVarient(6);
    {
        // 0xDB
        // Brief : Compare Floating Point Values and Set EFLAGS
        m_opCodeTable1[0xDB].m_pVarients[0x06]->Init(
            /*szName         = */"FCOMI",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDB,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
            /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xDB].InsertVarient(7);
    {
        // 0xDB
        // Brief : Store Floating Point Value and Pop
        m_opCodeTable1[0xDB].m_pVarients[0x07]->Init(
            /*szName         = */"FSTP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDB,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_80real ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xDC].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDC].InsertVarient(0);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Add
            m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Add
            m_opCodeTable1[0xDC].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(1);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Multiply
            m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Multiply
            m_opCodeTable1[0xDC].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(2);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Compare Real
            m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FCOM",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Compare Real
            m_opCodeTable1[0xDC].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FCOM2",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(3);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Compare Real and Pop
            m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FCOMP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Compare Real and Pop
            m_opCodeTable1[0xDC].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FCOMP3",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(4);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Subtract
            m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FSUB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x04]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Reverse Subtract
            m_opCodeTable1[0xDC].m_pVarients[0x04]->m_pVarients[0x03]->Init(
                /*szName         = */"FSUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(5);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Reverse Subtract
            m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FSUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x05]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Subtract
            m_opCodeTable1[0xDC].m_pVarients[0x05]->m_pVarients[0x03]->Init(
                /*szName         = */"FSUB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(6);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x06]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x06]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Divide
            m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x00]->Init(
                /*szName         = */"FDIV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x06]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Reverse Divide
            m_opCodeTable1[0xDC].m_pVarients[0x06]->m_pVarients[0x03]->Init(
                /*szName         = */"FDIVR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDC].InsertVarient(7);
    {
        m_opCodeTable1[0xDC].m_pVarients[0x07]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDC].m_pVarients[0x07]->InsertVarient(0);
        m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x01] = m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x00];
        m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x02] = m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x00];
        {
            // 0xDC
            // Brief : Reverse Divide
            m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x00]->Init(
                /*szName         = */"FDIVR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDC].m_pVarients[0x07]->InsertVarient(3);
        {
            // 0xDC
            // Brief : Divide and Pop
            m_opCodeTable1[0xDC].m_pVarients[0x07]->m_pVarients[0x03]->Init(
                /*szName         = */"FDIV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDC,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }

    m_opCodeTable1[0xDD].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDD].InsertVarient(0);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Load Floating Point Value
            m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FLD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xDD
            // Brief : Free Floating-Point Register
            m_opCodeTable1[0xDD].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FFREE",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(1);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Store Integer with Truncation and Pop
            m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FISTTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xDD
            // Brief : Exchange Register Contents
            m_opCodeTable1[0xDD].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FXCH4",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(2);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Store Floating Point Value
            m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FST",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDD
            // Brief : Store Floating Point Value
            m_opCodeTable1[0xDD].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FST",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(3);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FSTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64real ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xDD
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xDD].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FSTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(4);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Restore x87 FPU State
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FRSTOR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_94_108 ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x04]->InsertVarient(3);
        {
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values
                m_opCodeTable1[0xDD].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FUCOM",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(5);
    {
        m_opCodeTable1[0xDD].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDD].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDD
            // Brief : Unordered Compare Floating Point Values and Pop
            m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FUCOMP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDD,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDD].m_pVarients[0x05]->InsertVarient(3);
        {
            m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDD
                // Brief : Unordered Compare Floating Point Values and Pop
                m_opCodeTable1[0xDD].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FUCOMP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDD,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDD].InsertVarient(6);
    {
        // 0xDD
        // Brief : Store x87 FPU State
        m_opCodeTable1[0xDD].m_pVarients[0x06]->Init(
            /*szName         = */"FSAVE",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDD,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_94_108 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xDD].InsertVarient(7);
    {
        // 0xDD
        // Brief : Store x87 FPU Status Word
        m_opCodeTable1[0xDD].m_pVarients[0x07]->Init(
            /*szName         = */"FSTSW",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDD,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_16 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xDE].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDE].InsertVarient(0);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Add
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FIADD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x00]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Add and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x00]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FADDP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(1);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Multiply
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FIMUL",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x01]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Multiply and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x01]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FMULP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(2);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Compare Integer
            m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FICOM",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDE
            // Brief : Compare Real and Pop
            m_opCodeTable1[0xDE].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FCOMP5",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(3);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Compare Integer and Pop
            m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FICOMP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x03]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Compare Real and Pop Twice
                m_opCodeTable1[0xDE].m_pVarients[0x03]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FCOMPP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(4);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Subtract
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FISUB",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x04]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(5);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Reverse Subtract
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FISUBR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x05]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Subtract and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x05]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FSUBP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(6);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x06]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x06]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Divide
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x00]->Init(
                /*szName         = */"FIDIV",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x06]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Reverse Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x06]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVRP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDE].InsertVarient(7);
    {
        m_opCodeTable1[0xDE].m_pVarients[0x07]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDE].m_pVarients[0x07]->InsertVarient(0);
        m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x01] = m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x00];
        m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x02] = m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x00];
        {
            // 0xDE
            // Brief : Reverse Divide
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x00]->Init(
                /*szName         = */"FIDIVR",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDE,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDE].m_pVarients[0x07]->InsertVarient(3);
        {
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(1);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x01]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */0,
                    /*operand1       = */Operand_t(),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->InsertVarient(7);
            {
                // 0xDE
                // Brief : Divide and Pop
                m_opCodeTable1[0xDE].m_pVarients[0x07]->m_pVarients[0x03]->m_pVarients[0x07]->Init(
                    /*szName         = */"FDIVP",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDE,
                    /*nOperands      = */2,
                    /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                    /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }

    m_opCodeTable1[0xDF].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xDF].InsertVarient(0);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x00]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x00]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Load Integer
            m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x00]->Init(
                /*szName         = */"FILD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x00]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Free Floating-Point Register and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x00]->m_pVarients[0x03]->Init(
                /*szName         = */"FFREEP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(1);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x01]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x01]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Store Integer with Truncation and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x00]->Init(
                /*szName         = */"FISTTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x01]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Exchange Register Contents
            m_opCodeTable1[0xDF].m_pVarients[0x01]->m_pVarients[0x03]->Init(
                /*szName         = */"FXCH7",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(2);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x02]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x02]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Store Integer
            m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x00]->Init(
                /*szName         = */"FIST",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x02]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x02]->m_pVarients[0x03]->Init(
                /*szName         = */"FSTP8",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(3);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x03]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Store Integer and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x00]->Init(
                /*szName         = */"FISTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_16int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x03]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Store Floating Point Value and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x03]->m_pVarients[0x03]->Init(
                /*szName         = */"FSTP9",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(4);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x04]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x04]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Load Binary Coded Decimal
            m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x00]->Init(
                /*szName         = */"FBLD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_80dec ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x04]->InsertVarient(3);
        {
            m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x03]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM);

            m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xDF
                // Brief : Store x87 FPU Status Word
                m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FNSTSW",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDF,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 16 ) ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
            m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x03]->InsertVarient(0);
            {
                // 0xDF
                // Brief : Store x87 FPU Status Word
                m_opCodeTable1[0xDF].m_pVarients[0x04]->m_pVarients[0x03]->m_pVarients[0x00]->Init(
                    /*szName         = */"FSTSW",
                    /*bValidOpcd     = */true,
                    /*bEscapeOpcd    = */false,
                    /*bModrmRequired = */true,
                    /*iByte          = */0xDF,
                    /*nOperands      = */1,
                    /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 16 ) ),
                    /*operand2       = */Operand_t(),
                    /*operand3       = */Operand_t(),
                    /*operand4       = */Operand_t());
            }
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(5);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x05]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x05]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Load Integer
            m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x00]->Init(
                /*szName         = */"FILD",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_64int ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x05]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Unordered Compare Floating Point Values and Set EFLAGS and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x05]->m_pVarients[0x03]->Init(
                /*szName         = */"FUCOMIP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(6);
    {
        m_opCodeTable1[0xDF].m_pVarients[0x06]->InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);

        // Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents "mem" catagory.
        m_opCodeTable1[0xDF].m_pVarients[0x06]->InsertVarient(0);
        m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x01] = m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x00];
        m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x02] = m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x00];
        {
            // 0xDF
            // Brief : Store BCD Integer and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x00]->Init(
                /*szName         = */"FBSTP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */1,
                /*operand1       = */Operand_t( OperandMode_m, OperandType_80dec ),
                /*operand2       = */Operand_t(),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
        m_opCodeTable1[0xDF].m_pVarients[0x06]->InsertVarient(3);
        {
            // 0xDF
            // Brief : Compare Floating Point Values and Set EFLAGS and Pop
            m_opCodeTable1[0xDF].m_pVarients[0x06]->m_pVarients[0x03]->Init(
                /*szName         = */"FCOMIP",
                /*bValidOpcd     = */true,
                /*bEscapeOpcd    = */false,
                /*bModrmRequired = */true,
                /*iByte          = */0xDF,
                /*nOperands      = */2,
                /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_FPU, -1, 64 ) ),
                /*operand2       = */Operand_t( OperandMode_STi, OperandType_Invalid ),
                /*operand3       = */Operand_t(),
                /*operand4       = */Operand_t());
        }
    }
    m_opCodeTable1[0xDF].InsertVarient(7);
    {
        // 0xDF
        // Brief : Store Integer and Pop
        m_opCodeTable1[0xDF].m_pVarients[0x07]->Init(
            /*szName         = */"FISTP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xDF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_64int ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0xE0
    // Brief : Decrement count; Jump short if count!=0 and ZF=0
    m_opCodeTable1[0xE0].Init(
        /*szName         = */"LOOPNZ",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE0,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE1
    // Brief : Decrement count; Jump short if count!=0 and ZF=1
    m_opCodeTable1[0xE1].Init(
        /*szName         = */"LOOPZ",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE1,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE2
    // Brief : Decrement count; Jump short if count!=0
    m_opCodeTable1[0xE2].Init(
        /*szName         = */"LOOP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE2,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE3
    // Brief : Jump short if rCX register is 0
    m_opCodeTable1[0xE3].Init(
        /*szName         = */"JECXZ",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE3,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE4
    // Brief : Input from Port
    m_opCodeTable1[0xE4].Init(
        /*szName         = */"IN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE4,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE5
    // Brief : Input from Port
    m_opCodeTable1[0xE5].Init(
        /*szName         = */"IN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE5,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 32 ) ),
        /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE6
    // Brief : Output to Port
    m_opCodeTable1[0xE6].Init(
        /*szName         = */"OUT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE6,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE7
    // Brief : Output to Port
    m_opCodeTable1[0xE7].Init(
        /*szName         = */"OUT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE7,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( OperandMode_imm, OperandType_8 ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 32 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE8
    // Brief : Call Procedure
    m_opCodeTable1[0xE8].Init(
        /*szName         = */"CALL",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE8,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_16_32 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xE9
    // Brief : Jump
    m_opCodeTable1[0xE9].Init(
        /*szName         = */"JMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE9,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_16_32 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xEA
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xEA].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xEA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xEB
    // Brief : Jump
    m_opCodeTable1[0xEB].Init(
        /*szName         = */"JMP",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xEB,
        /*nOperands      = */1,
        /*operand1       = */Operand_t( OperandMode_rel, OperandType_8 ),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xEC
    // Brief : Input from Port
    m_opCodeTable1[0xEC].Init(
        /*szName         = */"IN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xEC,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xED
    // Brief : Input from Port
    m_opCodeTable1[0xED].Init(
        /*szName         = */"IN",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xED,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 32 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xEE
    // Brief : Output to Port
    m_opCodeTable1[0xEE].Init(
        /*szName         = */"OUT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xEE,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 8 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xEF
    // Brief : Output to Port
    m_opCodeTable1[0xEF].Init(
        /*szName         = */"OUT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xEF,
        /*nOperands      = */2,
        /*operand1       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 2, 16 ) ),
        /*operand2       = */Operand_t( Register_t( Register_t::RegisterClass_GPR, 0, 32 ) ),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF0
    // Brief : Assert LOCK# Signal Prefix
    m_opCodeTable1[0xF0].Init(
        /*szName         = */"LOCK",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF0,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF1
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xF1].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF1,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF2
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xF2].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF2,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF3
    //? Brief : Invalid instruction in 64-bit mode.
    m_opCodeTable1[0xF3].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF4
    // Brief : Halt
    m_opCodeTable1[0xF4].Init(
        /*szName         = */"HLT",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF5
    // Brief : Complement Carry Flag
    m_opCodeTable1[0xF5].Init(
        /*szName         = */"CMC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xF6].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xF6].InsertVarient(0);
    {
        // 0xF6
        // Brief : Logical Compare
        m_opCodeTable1[0xF6].m_pVarients[0x00]->Init(
            /*szName         = */"TEST",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(1);
    {
        // 0xF6
        // Brief : Logical Compare
        m_opCodeTable1[0xF6].m_pVarients[0x01]->Init(
            /*szName         = */"TEST",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_8 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(2);
    {
        // 0xF6
        // Brief : One's Complement Negation
        m_opCodeTable1[0xF6].m_pVarients[0x02]->Init(
            /*szName         = */"NOT",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(3);
    {
        // 0xF6
        // Brief : Two's Complement Negation
        m_opCodeTable1[0xF6].m_pVarients[0x03]->Init(
            /*szName         = */"NEG",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(4);
    {
        // 0xF6
        // Brief : Unsigned Multiply
        m_opCodeTable1[0xF6].m_pVarients[0x04]->Init(
            /*szName         = */"MUL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(5);
    {
        // 0xF6
        // Brief : Signed Multiply
        m_opCodeTable1[0xF6].m_pVarients[0x05]->Init(
            /*szName         = */"IMUL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(6);
    {
        // 0xF6
        // Brief : Unsigned Divide
        m_opCodeTable1[0xF6].m_pVarients[0x06]->Init(
            /*szName         = */"DIV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF6].InsertVarient(7);
    {
        // 0xF6
        // Brief : Signed Divide
        m_opCodeTable1[0xF6].m_pVarients[0x07]->Init(
            /*szName         = */"IDIV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF6,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xF7].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xF7].InsertVarient(0);
    {
        // 0xF7
        // Brief : Logical Compare
        m_opCodeTable1[0xF7].m_pVarients[0x00]->Init(
            /*szName         = */"TEST",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(1);
    {
        // 0xF7
        // Brief : Logical Compare
        m_opCodeTable1[0xF7].m_pVarients[0x01]->Init(
            /*szName         = */"TEST",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */2,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t( OperandMode_imm, OperandType_16_32 ),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(2);
    {
        // 0xF7
        // Brief : One's Complement Negation
        m_opCodeTable1[0xF7].m_pVarients[0x02]->Init(
            /*szName         = */"NOT",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(3);
    {
        // 0xF7
        // Brief : Two's Complement Negation
        m_opCodeTable1[0xF7].m_pVarients[0x03]->Init(
            /*szName         = */"NEG",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(4);
    {
        // 0xF7
        // Brief : Unsigned Multiply
        m_opCodeTable1[0xF7].m_pVarients[0x04]->Init(
            /*szName         = */"MUL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(5);
    {
        // 0xF7
        // Brief : Signed Multiply
        m_opCodeTable1[0xF7].m_pVarients[0x05]->Init(
            /*szName         = */"IMUL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(6);
    {
        // 0xF7
        // Brief : Unsigned Divide
        m_opCodeTable1[0xF7].m_pVarients[0x06]->Init(
            /*szName         = */"DIV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xF7].InsertVarient(7);
    {
        // 0xF7
        // Brief : Signed Divide
        m_opCodeTable1[0xF7].m_pVarients[0x07]->Init(
            /*szName         = */"IDIV",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xF7,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    // 0xF8
    // Brief : Clear Carry Flag
    m_opCodeTable1[0xF8].Init(
        /*szName         = */"CLC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF8,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xF9
    // Brief : Set Carry Flag
    m_opCodeTable1[0xF9].Init(
        /*szName         = */"STC",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xFA
    // Brief : Clear Interrupt Flag
    m_opCodeTable1[0xFA].Init(
        /*szName         = */"CLI",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xFA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xFB
    // Brief : Set Interrupt Flag
    m_opCodeTable1[0xFB].Init(
        /*szName         = */"STI",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xFB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xFC
    // Brief : Clear Direction Flag
    m_opCodeTable1[0xFC].Init(
        /*szName         = */"CLD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xFC,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    // 0xFD
    // Brief : Set Direction Flag
    m_opCodeTable1[0xFD].Init(
        /*szName         = */"STD",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xFD,
        /*nOperands      = */0,
        /*operand1       = */Operand_t(),
        /*operand2       = */Operand_t(),
        /*operand3       = */Operand_t(),
        /*operand4       = */Operand_t());

    m_opCodeTable1[0xFE].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xFE].InsertVarient(0);
    {
        // 0xFE
        // Brief : Increment by 1
        m_opCodeTable1[0xFE].m_pVarients[0x00]->Init(
            /*szName         = */"INC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFE,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFE].InsertVarient(1);
    {
        // 0xFE
        // Brief : Decrement by 1
        m_opCodeTable1[0xFE].m_pVarients[0x01]->Init(
            /*szName         = */"DEC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFE,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_8 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }

    m_opCodeTable1[0xFF].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG);

    m_opCodeTable1[0xFF].InsertVarient(0);
    {
        // 0xFF
        // Brief : Increment by 1
        m_opCodeTable1[0xFF].m_pVarients[0x00]->Init(
            /*szName         = */"INC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(1);
    {
        // 0xFF
        // Brief : Decrement by 1
        m_opCodeTable1[0xFF].m_pVarients[0x01]->Init(
            /*szName         = */"DEC",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_16_32_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(2);
    {
        // 0xFF
        // Brief : Call Procedure
        m_opCodeTable1[0xFF].m_pVarients[0x02]->Init(
            /*szName         = */"CALL",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(3);
    {
        // 0xFF
        // Brief : Call Procedure
        m_opCodeTable1[0xFF].m_pVarients[0x03]->Init(
            /*szName         = */"CALLF",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_ptp ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(4);
    {
        // 0xFF
        // Brief : Jump
        m_opCodeTable1[0xFF].m_pVarients[0x04]->Init(
            /*szName         = */"JMP",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_64 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(5);
    {
        // 0xFF
        // Brief : Jump
        m_opCodeTable1[0xFF].m_pVarients[0x05]->Init(
            /*szName         = */"JMPF",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_m, OperandType_ptp ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }
    m_opCodeTable1[0xFF].InsertVarient(6);
    {
        // 0xFF
        // Brief : Push Word, Doubleword or Quadword Onto the Stack
        m_opCodeTable1[0xFF].m_pVarients[0x06]->Init(
            /*szName         = */"PUSH",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xFF,
            /*nOperands      = */1,
            /*operand1       = */Operand_t( OperandMode_rm, OperandType_64_16 ),
            /*operand2       = */Operand_t(),
            /*operand3       = */Operand_t(),
            /*operand4       = */Operand_t());
    }


}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Tables_t::InitTwoByteOpCodeTable()
{

}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Tables_t::InitThreeByteOpCodeTable_38()
{
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Tables_t::InitThreeByteOpCodeTable_3A()
{
}
