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


        // These accessor functions account for R,X,B,W bits in 
        // VEX prefix.
        uint64_t ModRM_Mod() const;
        uint64_t ModRM_Reg() const;
        uint64_t ModRM_RM () const;
        uint64_t SIB_Scale() const;
        uint64_t SIB_Index() const;
        uint64_t SIB_Base () const;


        uint64_t GetImmRegister() const;
        int GetOperandSizeInBytes() const;

        int GetInstLengthInBytes() const;

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
