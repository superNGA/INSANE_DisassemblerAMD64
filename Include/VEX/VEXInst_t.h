//=========================================================================
//                      VEX Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single VEX encoded instruction and all its components.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"
#include "../Standard/ModRM_t.h"
#include "../Standard/Displacement_t.h"
#include "../Standard/SIB_t.h"
#include "../Standard/Immediate_t.h"
#include "../Standard/OpCode_t.h"



namespace INSANE_DASM64_NAMESPACE::VEX_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct VEXInst_t
    {
        VEXInst_t();
        void Clear();

        Byte                     m_prefix         = 0x00;
        Byte                     m_vex[Rules::MAX_VEX_PREFIX_BYTES];
        int                      m_nVEXBytes      = -1;
        Standard::OpCode_t       m_opcode;
        Standard::ModRM_t        m_modrm;
        Standard::SIB_t          m_SIB;
        bool                     m_bHasSIB        = false;
        Standard::Displacement_t m_disp;
        Standard::Immediate_t    m_immediate;
    };
}
