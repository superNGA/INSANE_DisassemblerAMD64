//=========================================================================
//                      SafeBitWiseOps
//=========================================================================
// by      : INSANE
// created : 01/01/2026
// 
// purpose : Do bit wise ops without the compiler messing with my shit ( int promotions n all )
//-------------------------------------------------------------------------
#pragma once
#include <cstdint>
#include <string.h>
#include "../../Include/Aliases.h"


struct Bits64_t;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE
{
    namespace Maths
    {
        struct Bits64_t { uint64_t m_data; };


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        extern "C" uint64_t SafeBitWiseOps_And(Bits64_t left, Bits64_t right);
        extern "C" uint64_t SafeBitWiseOps_Or (Bits64_t left, Bits64_t right);


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        template <typename L, typename R>
        uint64_t SafeAnd(L iLeft, R iRight)
        {
            Bits64_t iLeftBits{ 0llu }, iRightBits{ 0llu };
            memcpy(&iLeftBits,  &iLeft,  sizeof(L));
            memcpy(&iRightBits, &iRight, sizeof(R));

            return SafeBitWiseOps_And(iLeftBits, iRightBits);
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        template <typename L, typename R>
        uint64_t SafeOr(L iLeft, R iRight)
        {
            Bits64_t iLeftBits{ 0llu }, iRightBits{ 0llu };
            memcpy(&iLeftBits,  &iLeft,  sizeof(L));
            memcpy(&iRightBits, &iRight, sizeof(R));

            return SafeBitWiseOps_Or(iLeftBits, iRightBits);
        }
    }
}
