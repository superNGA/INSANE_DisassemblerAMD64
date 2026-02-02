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
#include "VEXPrefix_t.h"



namespace INSANE_DASM64_NAMESPACE::VEX_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct VEXInst_t
    {
        VEXInst_t();

        void Clear();
        uint64_t GetImmRegister() const;

        // These accessor functions account for R,X,B,W bits in 
        // VEX prefix.
        uint64_t ModRM_Mod() const;
        uint64_t ModRM_Reg() const;
        uint64_t ModRM_RM () const;
        uint64_t SIB_Scale() const;
        uint64_t SIB_Index() const;
        uint64_t SIB_Base () const;

        int GetOperandSizeInBytes() const;


        VEXPrefix_t              m_vexPrefix;
        Standard::OpCode_t       m_opcode;
        Standard::ModRM_t        m_modrm;
        Standard::SIB_t          m_SIB;
        bool                     m_bHasSIB = false;
        Standard::Displacement_t m_disp;
        Standard::Immediate_t    m_immediate;
    };
}
