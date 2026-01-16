//=========================================================================
//                      Legacy Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single legacy encoded instruction and all its components.
//-------------------------------------------------------------------------
#include "../../../Include/Legacy/LegacyOpCode_t.h"
#include "../../../Include/Legacy/LegacyPrefix.h"
#include <assert.h>
#include "../../Tables/Tables.h"
#include "../../../Include/Standard/OpCodeDesc_t.h"


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Legacy::LegacyOpCode_t::LegacyOpCode_t() : Standard::OpCode_t()
{
    // Just call Standard::OpCode_t()'s constructor and be done with it.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Legacy::LegacyOpCode_t::ModRMRequired(const LegacyPrefix_t* pPrefix) const
{
    assert(m_pRootOpCodeDesc != nullptr && "No root OpCode description is stored for this OpCode_t");
    
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    // In case split type is any of these. We will need modRM 100%
    if (m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_ModRM_MOD ||
        m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_ModRM_REG ||
        m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_ModRM_RM)
    {
        return true;
    }


    // In case we have a legacy prefix split, we need to check if we need a
    // modrm or not, according to child OpCodeDesc.
    if (m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_LegacyPrefix)
    {
        // Every legacy prefix split has default entry ( no prefix ) @ index 0.
        Standard::OpCodeDesc_t* pBestDesc = m_pRootOpCodeDesc->m_pVarients[0];
        assert(pBestDesc != nullptr && "Some opcode entry with legacy prefix split has default entry ( index 0 ) as default.");


        for (int i = 0; i < pPrefix->m_nPrefix; i++)
        {
            int iPrefixIndex = G::g_tables.GetLegacyPrefixIndex(pPrefix->m_legacyPrefix[i]);
            
            if (m_pRootOpCodeDesc->m_pVarients[iPrefixIndex] != nullptr)
            {
                pBestDesc = m_pRootOpCodeDesc->m_pVarients[iPrefixIndex];
                break;
            }
        }

        return pBestDesc->m_bModrmRequired;
    }


    // If we reach here, it means there is no split, so we use root.
    return m_pRootOpCodeDesc->m_bModrmRequired;
}
