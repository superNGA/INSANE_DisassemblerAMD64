//=========================================================================
//                      OpCode Description
//=========================================================================
// by      : INSANE
// created : 16/01/2026
//
// purpose : OpCode description, holds all operands, Modrm information, OpCode validity 
//          other information for an opcode, along with pointers to any varients and 
//          split method.
//-------------------------------------------------------------------------
#include "../../../Include/Standard/OpCodeDesc_t.h"
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include "../../Util/Terminal/Terminal.h"


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::OpCodeDesc_t::OpCodeDesc_t()
{
    Reset();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::OpCodeDesc_t::Reset()
{

    m_iVarientType   = VarientKey_None;
    m_nVarients      = 0;
    m_pVarients      = nullptr;
    strcpy(m_szName, Rules::OPCODE_NAME_SENTINAL);
    m_bIsValidCode   = false;
    m_bIsEscapeCode  = false;
    m_bModrmRequired = false;
    m_iByte          = 0x00;
    m_nOperands      = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::OpCodeDesc_t::Init(
    const char* szName, bool bValidOpcd, bool bEscapeOpcd, bool bModrmRequired, Byte iByte, 
    int nOperands, Operand_t operand1, Operand_t operand2, Operand_t operand3, Operand_t operand4)
{
    strcpy(m_szName, szName);
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


    //? Delete this
    // printf("Initialized 0x%02X { %s }\n", m_iByte, m_szName);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::OpCodeDesc_t::InsertVarient(int iIndex)
{
    //? Delete this
    // printf("Inserting varient @ [ %d ] index for byte [ 0x%02X ]\n", iIndex, m_iByte);

    assert(m_pVarients != nullptr && "Varient array is not initialized");
    if (m_pVarients == nullptr)
        return false;


    assert(m_iVarientType != VarientType_t::VarientKey_None && "Varient type can't be null if we are inserting varient");
    if (m_iVarientType == VarientType_t::VarientKey_None)
        return false;


    //? Delete this
    // Invalid child index?
    //printf("Varient : %d, iIndex = %d, Min : %d, Max : %zu\n", m_iVarientType, iIndex, 0, GetMaxVarients(m_iVarientType));


    assert(iIndex >= 0 && iIndex < GetMaxVarients(m_iVarientType) && "Invalid index while inserting varient.");
    if (iIndex < 0 || iIndex >= GetMaxVarients(m_iVarientType))
        return false;


    // prevent mem leak.
    // assert(m_pVarients[iIndex] == nullptr && "Element already inserted @ this index!");
    if(m_pVarients[iIndex] != nullptr)
    {
        //? Delete this
        // printf(RED "Trying to insert varient @ index [ %d ], varinent { %s } already inserted @ this index." RESET, iIndex, m_pVarients[iIndex]->m_szName);
        return true;
    }


    m_pVarients[iIndex] = new OpCodeDesc_t();


    // Failed memory allocation ?
    if (m_pVarients[iIndex] == nullptr)
        return false;


    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
size_t InsaneDASM64::Standard::OpCodeDesc_t::GetMaxVarients(VarientType_t iVarientType)
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
bool InsaneDASM64::Standard::OpCodeDesc_t::InitVarientType(VarientType_t iVarientType)
{
    // Already initialized as this varient type.
    if (m_iVarientType == iVarientType)
        return true;


    // Must be set to "No varient", else mem leak
    assert(m_iVarientType == VarientType_t::VarientKey_None && m_pVarients == nullptr && "Varient type is already set to some something!");
    if (m_iVarientType != VarientType_t::VarientKey_None || m_pVarients != nullptr)
    {
        LOG("Object is already initialized for varient type [ %d ]\n", m_iVarientType);
        return false;
    }


    size_t iVarientArraySize = GetMaxVarients(iVarientType) * sizeof(void*);
    m_pVarients              = reinterpret_cast<OpCodeDesc_t**>(malloc(iVarientArraySize));
    m_iVarientType           = iVarientType;
    

    // Just in case malloc fails.
    if(m_pVarients == nullptr)
    {
        assert(m_pVarients != nullptr && "Failed memory allocation to varient array!");
        FAIL_LOG("Failed memory allocation to varient array!\n");
        return false;
    }


    // Set all entries to nullptr.
    memset(m_pVarients, 0, iVarientArraySize);

    //? Delete this
    // printf("Inititalized varient type : %d, Varient Array : %p\n", m_iVarientType, m_pVarients);

    return true;
}
