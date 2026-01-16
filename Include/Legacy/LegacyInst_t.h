//=========================================================================
//                      Legacy Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single legacy encoded instruction and all of its components.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>
#include "LegacyPrefix.h"
#include "LegacyOpCode_t.h"
#include "../Standard/ModRM_t.h"
#include "../Standard/Immediate_t.h"
#include "../Standard/Displacement_t.h"
#include "../Standard/SIB_t.h"



namespace INSANE_DASM64_NAMESPACE::LEGACY_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct LegacyInst_t
    {
        LegacyInst_t();

        void Clear();
        int GetOperandSizeInBytes(bool bIgnoreREXW) const;
        int GetAddressSizeInBytes() const;

        // NOTE : These functions account for the REX bits. 
        //        And the indivigual functions of ModRM_t or SIB_t don't account for that.
        //        So take precaution while using these functions.
        uint64_t ModRM_Reg() const;
        uint64_t ModRM_Mod() const;
        uint64_t ModRM_RM()  const;
        uint64_t SIB_Scale() const;
        uint64_t SIB_Index() const;
        uint64_t SIB_Base()  const;


        LegacyPrefix_t           m_legacyPrefix;
        Legacy::LegacyOpCode_t   m_opCode;

        bool                     m_bHasREX   = false;
        Byte                     m_iREX      = 0x00;
        int32_t                  m_iREXIndex = -1;

        bool                     m_bHasModRM = false;
        Standard::ModRM_t        m_modrm;

        bool                     m_bHasSIB   = false;
        Standard::SIB_t          m_SIB;

        Standard::Displacement_t m_displacement;
        Standard::Immediate_t    m_immediate;
    };
}
