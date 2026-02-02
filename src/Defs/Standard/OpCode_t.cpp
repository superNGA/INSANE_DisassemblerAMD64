//=========================================================================
//                      OpCode
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Holds basic opcode information along with opcode description
//-------------------------------------------------------------------------
#include "../../../Include/Standard/OpCode_t.h"
#include <assert.h>
#include "../../Tables/Tables.h"
#include "../../../Include/Legacy/LegacyPrefix.h"
#include "../../../Include/Standard/OpCodeDesc_t.h"


using namespace InsaneDASM64;


static Standard::OpCodeDesc_t* FindVarientRecurse(const Legacy::LegacyPrefix_t* pPrefix, Byte iModRM, Standard::OpCodeDesc_t* pRootOpCodeDesc, bool bHasModRM);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::OpCode_t::OpCode_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::OpCode_t::Clear()
{
    m_nOpBytes        = 0;
    m_pRootOpCodeDesc = nullptr;
    m_pOpCodeDesc     = nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Standard::OpCode_t::OpByteCount() const
{
    return m_nOpBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Standard::OpCode_t::OperandCount() const
{
    assert(m_pOpCodeDesc != nullptr && "No OpCode description is stored for this OpCode_t");
    return m_pOpCodeDesc == nullptr ? -1 : m_pOpCodeDesc->m_nOperands;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Byte InsaneDASM64::Standard::OpCode_t::GetMostSignificantOpCode() const
{
    assert(m_nOpBytes > 0 && m_nOpBytes <= Rules::MAX_OPBYTES && "OpCode count is invalid!");
    if (m_nOpBytes <= 0 || m_nOpBytes > Rules::MAX_OPBYTES)
        return 0x00;

    return m_opBytes[m_nOpBytes - 1];
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::OpCode_t::PushOpCode(Byte iByte)
{
    assert(m_nOpBytes < Rules::MAX_OPBYTES && "MAX opbytes are already stored!");
    if(m_nOpBytes >= Rules::MAX_OPBYTES)
        return false;

    m_opBytes[m_nOpBytes] = iByte;
    m_nOpBytes++;
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::OpCode_t::StoreOperatorDesc(OpCodeDesc_t* pOperatorInfo)
{
    // Storing root & final opcode description.
    m_pRootOpCodeDesc = pOperatorInfo;
    m_pOpCodeDesc     = nullptr; // Determining final child opcode decription is specific to each encoding type. Inheriting OpCode classes handle that part.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::OpCode_t::InitChildVarient(const Legacy::LegacyPrefix_t* pPrefix, Byte iModRM, bool bHasModRM)
{
    assert(m_pRootOpCodeDesc != nullptr && "We need a root opcode description pointer to init child varient!");
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    m_pOpCodeDesc = FindVarientRecurse(pPrefix, iModRM, m_pRootOpCodeDesc, bHasModRM);
    return m_pOpCodeDesc != nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::OpCode_t::InitChildVarient(Byte iModRM, int nPrefixCount, Byte* prefixies, bool bHasModRM)
{
    // Construct the legacy prefix, to pass into the original InitChildVarient function.
    Legacy::LegacyPrefix_t prefix; prefix.Clear();
    for(int iPrefixIndex = 0; iPrefixIndex < nPrefixCount; iPrefixIndex++)
        prefix.PushPrefix(prefixies[iPrefixIndex]);

    return InitChildVarient(&prefix, iModRM, bHasModRM);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static Standard::OpCodeDesc_t* FindVarientRecurse(const Legacy::LegacyPrefix_t* pPrefix, Byte iModRM, Standard::OpCodeDesc_t* pRootOpCodeDesc, bool bHasModRM)
{
    // Since this is a recursive function, and invalid opcode varients are nullptrs, 
    // incase there is no varient for current combination of ModRM bits / prefixies,
    // this function will call itself with pRootOpCodeDesc as nullpts. But pRootOpCodeDesc
    // will never be nullptr on the first call ( cause it wrote it like that. ).
    if(pRootOpCodeDesc == nullptr)
        return nullptr;


    if (pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_None)
        return pRootOpCodeDesc;

    assert(pRootOpCodeDesc->m_pVarients != nullptr && "Varient array is nullptr, when expected a varient array!.");


    switch (pRootOpCodeDesc->m_iVarientType)
    {
        case Standard::OpCodeDesc_t::VarientKey_ModRM_MOD:
            if(bHasModRM == true)
            {
                return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 6) & 0b11], bHasModRM);
            }
            break;


        case Standard::OpCodeDesc_t::VarientKey_ModRM_REG:
            if(bHasModRM == true)
            {
                return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 3) & 0b111], bHasModRM);
            }
            break;


        case Standard::OpCodeDesc_t::VarientKey_ModRM_RM:
            if(bHasModRM == true)
            {
                return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[iModRM & 0b111], bHasModRM);
            }
            break;


        case Standard::OpCodeDesc_t::VarientKey_LegacyPrefix:
        {
            Standard::OpCodeDesc_t* pOpCodeDesc = pRootOpCodeDesc->m_pVarients[0];

            for (int i = 0; i < pPrefix->PrefixCount(); i++)
            {
                Byte          iPrefix            = pPrefix->m_legacyPrefix[i];
                int           iPrefixIndex       = G::g_tables.GetLegacyPrefixIndex(iPrefix);
                Standard::OpCodeDesc_t* pPrefixOpCodeDesc  = pRootOpCodeDesc->m_pVarients[iPrefixIndex];

                // Check if we have any prefix such that there exists a opcode varient for that
                // prefix. Else we can always use the default entry.
                if (pPrefixOpCodeDesc != nullptr)
                {
                    pOpCodeDesc = pPrefixOpCodeDesc;
                    break;
                }
            }

            // OpCode varient can't be nullptr.
            // if its nullptr that means, this opcode only supports specific perfixies
            // and we don't have that prefix.
            // assert(pOpCodeDesc != nullptr && "Prefix not support by this opcode. Prefix split evaluation failed.");
            if(pOpCodeDesc != nullptr)
                return FindVarientRecurse(pPrefix, iModRM, pOpCodeDesc, bHasModRM);

            break;
        }
    default: break;
    }


    // assert(false && "Invalid opcode varient set!");
    return nullptr;
}
