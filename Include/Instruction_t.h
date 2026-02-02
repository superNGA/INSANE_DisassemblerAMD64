//=========================================================================
//                      Instruction
//=========================================================================
// by      : INSANE
// created : 16/01/2026
//
// purpose : Simple "union" like data strcture to hold instruction of many different 
//           encoding types. ( one at a time )
//-------------------------------------------------------------------------
#pragma once
#include "Aliases.h"



namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Instruction_t
    {
        enum InstEncodingTypes_t
        {
            InstEncodingType_Invalid = -1,
            InstEncodingType_Legacy = 0,
            InstEncodingType_VEX,
            InstEncodingType_EVEX,
            InstEncodingType_XOP,

            InstEncodingType_Count
        };

        Instruction_t() {};
        Instruction_t(InstEncodingTypes_t iEncoding)
        { 
            InitEncodingType(iEncoding); 
        }

        bool InitEncodingType(InstEncodingTypes_t iEncoding);
        bool Free();


        void*               m_pInst             = nullptr;
        InstEncodingTypes_t m_iInstEncodingType = InstEncodingTypes_t::InstEncodingType_Invalid;
    };
}
