//=========================================================================
//                      Instruction Summary
//=========================================================================
// by      : INSANE
// created : 19/01/2026
//
// purpose : Holds the summary of an decoded instruction, to be used internally
//           by the disassembler.
//-------------------------------------------------------------------------
#pragma once
#include "../../../Include/Aliases.h"
#include <cstdint>



// Forward declares...
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE { struct OpCode_t;       }
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE { struct ModRM_t;        }
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE { struct SIB_t;          }
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE { struct Displacement_t; }
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE { struct Immediate_t;    }
namespace INSANE_DASM64_NAMESPACE::VEX_NAMESPACE      { struct VEXPrefix_t;    }
namespace INSANE_DASM64_NAMESPACE::VEX_NAMESPACE      { struct VEXInst_t;      }
namespace INSANE_DASM64_NAMESPACE::LEGACY_NAMESPACE   { struct LegacyInst_t;   }


namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct InstSummary_t
    {
        InstSummary_t();

        void Clear();
        void Initialize(const Legacy::LegacyInst_t* pLeagcyInst);
        void Initialize(const VEX::VEXInst_t*       pVEXInst);

        bool IsValid() const;



        const Standard::OpCode_t*       m_pOpCode           = nullptr;
        const Standard::ModRM_t*        m_pModRM            = nullptr;
        bool                            m_bHasModRM         = false;
        const Standard::SIB_t*          m_pSIB              = nullptr;
        bool                            m_bHasSIB           = false;
        const VEX::VEXPrefix_t*         m_pVEXPrefix        = nullptr;
        const Standard::Immediate_t*    m_pImmediate        = nullptr;
        const Standard::Displacement_t* m_pDisplacement     = nullptr;

        uint64_t                        m_iModRM_Mod        = 0llu;
        uint64_t                        m_iModRM_Reg        = 0llu;
        uint64_t                        m_iModRM_RM         = 0llu;
        uint64_t                        m_iSIB_Scale        = 0llu;
        uint64_t                        m_iSIB_Index        = 0llu;
        uint64_t                        m_iSIB_Base         = 0llu;

        int                             m_iOperandSize      = 0;
        int                             m_iAddressSize      = 0;
        uint64_t                        m_iImmRegisterIndex = 0llu;
        uint64_t                        m_iREX_B            = 0llu;
    };
}
