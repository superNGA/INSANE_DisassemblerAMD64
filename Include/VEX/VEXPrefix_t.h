//=========================================================================
//                      VEX Prefix
//=========================================================================
// by      : INSANE
// created : 17/01/2026
//
// purpose : Holds VEX prefix and VEX bytes, and provides easy access to bit-field
//           components.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"



namespace INSANE_DASM64_NAMESPACE::VEX_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct VEXPrefix_t
    {
        VEXPrefix_t();

        void Clear();
        void StorePrefix(Byte iVEXPrefix);
        void PushVEXBytes(Byte iVEX);
        int  VEXByteCount();
        Byte GetPrefix();

        // Bit accessors.
        uint64_t m_mmmm() const;
        uint64_t vvvv() const;
        uint64_t L() const;
        uint64_t pp() const;
        uint64_t R() const;
        uint64_t X() const;
        uint64_t B() const;
        uint64_t W() const;

        
        Byte m_iPrefix;
        Byte m_iVEX[Rules::MAX_VEX_PREFIX_BYTES];
        Byte m_nVEXBytes;
    };
}
