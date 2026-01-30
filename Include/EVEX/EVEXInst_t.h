//=========================================================================
//                      EVEX Instruction
//=========================================================================
// by      : INSANE
// created : 29/01/2026
//
// purpose : Holds various modular components of a single EVEX encoded instruction.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "EVEXPrefix_t.h" // Must include it.
#include "../Standard/OpCode_t.h"
#include "../Standard/ModRM_t.h"
#include "../Standard/SIB_t.h"
#include "../Standard/Displacement_t.h"
#include "../Standard/Immediate_t.h"


namespace INSANE_DASM64_NAMESPACE::EVEX_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct EVEXInst_t
    {
        EVEXInst_t();
        void Clear();


        // Components...
        EVEXPrefix_t             m_evexPrefix;
        Standard::OpCode_t       m_opcode;
        Standard::ModRM_t        m_modrm;
        Standard::SIB_t          m_SIB;
        bool                     m_bHasSIB = false;
        Standard::Displacement_t m_disp;
        Standard::Immediate_t    m_immediate;
        Byte                     m_iCompressedDisp;
    };
}
