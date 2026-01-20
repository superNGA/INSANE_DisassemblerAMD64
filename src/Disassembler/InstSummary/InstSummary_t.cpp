//=========================================================================
//                      Instruction Summary
//=========================================================================
// by      : INSANE
// created : 19/01/2026
//
// purpose : Holds the summary of an decoded instruction, to be used internally
//           by the disassembler.
//-------------------------------------------------------------------------
#include "InstSummary_t.h"
#include "../../../Include/Masks.h"
#include "../../../Include/Legacy/LegacyInst_t.h"
#include "../../../Include/VEX/VEXInst_t.h"
#include "../../Math/SafeBitWiseOps.h"



using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::InstSummary_t::InstSummary_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::InstSummary_t::Clear()
{
    m_pOpCode       = nullptr;
    m_pModRM        = nullptr;
    m_bHasModRM     = false;
    m_pSIB          = nullptr;
    m_bHasSIB       = false;
    m_pVEXPrefix    = nullptr;
    m_pImmediate    = nullptr;
    m_pDisplacement = nullptr;

    m_iModRM_Mod    = 0llu;
    m_iModRM_Reg    = 0llu;
    m_iModRM_RM     = 0llu;
    m_iSIB_Scale    = 0llu;
    m_iSIB_Index    = 0llu;
    m_iSIB_Base     = 0llu;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::InstSummary_t::Initialize(const Legacy::LegacyInst_t* pLegacyInst)
{
    m_pOpCode       = &pLegacyInst->m_opCode; // NOTE : pLegacyInst->m_opCode is of type LegacyOpCode_t which inherits from opCode_t. They are pretty much the same.
    m_pModRM        = &pLegacyInst->m_modrm; m_bHasModRM = pLegacyInst->m_bHasModRM;
    m_pSIB          = &pLegacyInst->m_SIB;   m_bHasSIB   = pLegacyInst->m_bHasSIB;
    m_pVEXPrefix    = nullptr;
    m_pDisplacement = &pLegacyInst->m_displacement;
    m_pImmediate    = &pLegacyInst->m_immediate;

    if(m_bHasModRM == true)
    {
        m_iModRM_Mod = pLegacyInst->ModRM_Mod(); 
        m_iModRM_Reg = pLegacyInst->ModRM_Reg(); 
        m_iModRM_RM  = pLegacyInst->ModRM_RM();
    }
    m_iSIB_Scale = pLegacyInst->SIB_Scale(); m_iSIB_Index = pLegacyInst->SIB_Index(); m_iSIB_Base = pLegacyInst->SIB_Base();

    // Operand size & Address size for this instruction in bytes.
    m_iOperandSize = pLegacyInst->GetOperandSizeInBytes(false);
    m_iAddressSize = pLegacyInst->GetAddressSizeInBytes();

    m_iImmRegisterIndex = 0llu;

    m_iREX_B = pLegacyInst->m_bHasREX == false ? 0llu : Maths::SafeAnd(pLegacyInst->m_iREX, Legacy::Masks::REX_B);
}
