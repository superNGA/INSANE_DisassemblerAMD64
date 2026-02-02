//=========================================================================
//                      Register
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Register data strucure. Used to represent many different types of registers.
//-------------------------------------------------------------------------
#include "../../../Include/Standard/Register_t.h"
#include <assert.h>
#include "../../Util/Terminal/Terminal.h"
#include "../../../Include/Rules.h"


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Register_t::Register_t()
{
    m_iRegisterClass = RegisterClass_Invalid;
    m_iRegisterIndex = -1;
    m_iRegisterSize  = -1;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Register_t::Register_t(RegisterClass_t iRegisterClass, int16_t iRegisterIndex, int16_t iRegisterSizeInBits)
{
    m_iRegisterClass = iRegisterClass;
    m_iRegisterIndex = iRegisterIndex;
    m_iRegisterSize  = iRegisterSizeInBits;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const char* InsaneDASM64::Standard::Register_t::ToString() const
{
    switch (m_iRegisterClass)
    {
    case Register_t::RegisterClass_GPR:
    {
        static const char* s_szGPR64[16] = { "rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","r15" };
        static const char* s_szGPR32[16] = { "eax","ecx","edx","ebx","esp","ebp","esi","edi","r8d","r9d","r10d","r11d","r12d","r13d","r14d","r15d" };
        static const char* s_szGPR16[16] = { "ax","cx","dx","bx","sp","bp","si","di","r8w","r9w","r10w","r11w","r12w","r13w","r14w","r15w" };
        static const char* s_szGPR8 [16] = { "al","cl","dl","bl","spl","bpl","sil","dil","r8b","r9b","r10b","r11b","r12b","r13b","r14b","r15b" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szGPR64) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szGPR64) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;
        

        switch (m_iRegisterSize)
        {
        case 8:  return s_szGPR8[m_iRegisterIndex];
        case 16: return s_szGPR16[m_iRegisterIndex];
        case 32: return s_szGPR32[m_iRegisterIndex];
        case 64: return s_szGPR64[m_iRegisterIndex];
        
        default: 
            FAIL_LOG("Invalid register size : %d\n", m_iRegisterSize);
            assert(false && "Invalid register size"); 
            break;
        }

        return Rules::REGISTER_NAME_SENTINAL;

        break; // Just in case, if I have missed something.
    }

    case Register_t::RegisterClass_FPU:
    {
        static const char* s_szFPU[8] = { "st0","st1","st2","st3","st4","st5","st6","st7" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szFPU) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szFPU) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szFPU[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_SSE: // xxm Registers.
    {
        static const char* s_szXXM[16] = { "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szXXM) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szXXM) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szXXM[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_MMX:
    {
        static const char* s_szMMX[8] = { "mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"};
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szMMX) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szMMX) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szMMX[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_AVX: // yml Registers.
    {
        static const char* s_szYMM[16] = { "ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6","ymm7","ymm8","ymm9","ymm10","ymm11","ymm12","ymm13","ymm14","ymm15" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szYMM) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szYMM) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szYMM[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_AVX512:
    {
        static const char* s_szZMM[32] = {
            "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6", "zmm7",
            "zmm8", "zmm9", "zmm10", "zmm11", "zmm12", "zmm13", "zmm14", "zmm15",
            "zmm16", "zmm17", "zmm18", "zmm19", "zmm20", "zmm21", "zmm22", "zmm23",
            "zmm24", "zmm25", "zmm26", "zmm27", "zmm28", "zmm29", "zmm30", "zmm31"};

        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szZMM) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szZMM) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szZMM[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Segment:
    {
        static const char* s_szSegmentReg[6] = { "es","cs","ss","ds","fs","gs" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szSegmentReg) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szSegmentReg) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szSegmentReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Control:
    {
        static const char* s_szControlReg[9] = { "cr0","cr1","cr2","cr3","cr4","cr5","cr6","cr7","cr8" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szControlReg) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szControlReg) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szControlReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Debug:
    {
        static const char* s_szDebugReg[8] = { "dr0","dr1","dr2","dr3","dr4","dr5","dr6","dr7" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szDebugReg) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szDebugReg) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;

        return s_szDebugReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Test:
    {
        // Although I have been told that only 2 test registers ( TR6 & TR7 ) are accessible,
        // but if I only define those, it might nuke the index. So I define all test registers here.
        static const char* s_szTestReg[8] = { "TR0", "TR1", "TR2", "TR3", "TR4", "TR5", "TR6", "TR7" };
        // assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szTestReg) / sizeof(char*)) && "Invalid register index.");
        if(m_iRegisterIndex < 0 || m_iRegisterIndex >= (sizeof(s_szTestReg) / sizeof(char*)))
            return Rules::REGISTER_NAME_SENTINAL;


        return s_szTestReg[m_iRegisterIndex];
        break;
    }

    default: break;
    }

    return Rules::REGISTER_NAME_SENTINAL;
}
