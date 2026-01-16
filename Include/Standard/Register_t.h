//=========================================================================
//                      Register
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Register data strucure. Used to represent many different types of registers.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Register_t
    {
        enum RegisterClass_t : int16_t
        {
            RegisterClass_Invalid = -1,
            RegisterClass_GPR,
            RegisterClass_FPU,
            RegisterClass_SSE, // xxm
            RegisterClass_MMX, // mmx
            RegisterClass_AVX, // ymm
            RegisterClass_AVX512,
            RegisterClass_Segment,
            RegisterClass_Control,
            RegisterClass_Debug,
            RegisterClass_Test,
        };


        Register_t(); // Sets register to invalid.
        Register_t(RegisterClass_t iRegisterClass, int16_t iRegisterIndex, int16_t iRegisterSize);

        const char* ToString() const;

        RegisterClass_t m_iRegisterClass = RegisterClass_Invalid;
        int16_t         m_iRegisterIndex = -1;
        int16_t         m_iRegisterSize  = -1;
    };

}
