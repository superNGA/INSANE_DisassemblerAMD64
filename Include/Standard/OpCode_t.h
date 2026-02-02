//=========================================================================
//                      OpCode
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Holds basic opcode information along with opcode description
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"


// Forward declerations.
namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE{ struct OpCodeDesc_t;   }
namespace INSANE_DASM64_NAMESPACE::LEGACY_NAMESPACE  { struct LegacyPrefix_t; }


namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCode_t
    {
        OpCode_t();
        
        void Clear();
        int  OpByteCount () const;
        int  OperandCount() const;

        Byte GetMostSignificantOpCode() const;
        bool PushOpCode(Byte iByte);
        void StoreOperatorDesc(OpCodeDesc_t* pOperatorInfo);

        bool InitChildVarient(const Legacy::LegacyPrefix_t* pPrefix, Byte iModRM, bool bHasModRM);

        // To manually send in prefix?
        bool InitChildVarient(Byte iModRM, int nPrefixCount, Byte* prefixies, bool bHasModRM);


        // NOTE : One OpCode_t will hold all the OpCodes that are present in one
        //        instruction. That means, if multibyte opcode is present in one instruction,
        //        we will store all bytes ( 1 to 3 ) in a single OpCode_t struct.
        Byte            m_opBytes[Rules::MAX_OPBYTES];
        int32_t         m_nOpBytes        = 0;

        OpCodeDesc_t*   m_pRootOpCodeDesc = nullptr;
        OpCodeDesc_t*   m_pOpCodeDesc     = nullptr;
    };
}
