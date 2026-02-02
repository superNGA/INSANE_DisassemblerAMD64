//=========================================================================
//                      Legacy Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single legacy encoded instruction and all its components.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"
#include "../Standard/OpCode_t.h" // Inherited from this.



namespace INSANE_DASM64_NAMESPACE::LEGACY_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct LegacyOpCode_t : public Standard::OpCode_t
    {
        LegacyOpCode_t();

        bool TryResolveModRMNeed(const LegacyPrefix_t* pPrefix, bool* bSucceded) const;
    };
}
