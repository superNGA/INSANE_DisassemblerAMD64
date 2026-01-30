//=========================================================================
//                      Instruction
//=========================================================================
// by      : INSANE
// created : 16/01/2026
//
// purpose : Simple "union" like data strcture to hold instruction of many different 
//           encoding types. ( one at a time )
//-------------------------------------------------------------------------
#include "../Include/Instruction_t.h"
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "Util/Terminal/Terminal.h"

// Instruction data structures.
#include "../Include/Legacy/LegacyInst_t.h"
#include "../Include/VEX/VEXInst_t.h"
#include "../Include/EVEX/EVEXInst_t.h"



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Instruction_t::InitEncodingType(InstEncodingTypes_t iEncoding)
{
    // Is the encoding type valid.
    bool bValidEncoding = iEncoding >= InstEncodingTypes_t::InstEncodingType_Legacy && iEncoding < InstEncodingTypes_t::InstEncodingType_Count;
    assert(bValidEncoding == true && "Invalid encoding type passed for initializing instruction.");
    if(bValidEncoding == false)
        return false;


    // Instruction encoding must not be set.
    assert(m_iInstEncodingType == InstEncodingType_Invalid && "Instruction is already initialized to some type");
    if (m_iInstEncodingType != InstEncodingType_Invalid)
        return false;


    // Instruction pointer must be NULL.
    assert(m_pInst == nullptr && "Instruction pointer is pointing to some memory address before initializing");
    if(m_pInst != nullptr)
        return false;


    // Get the instruction size.
    size_t iInstSize = 0llu;
    switch (iEncoding) 
    {
        case InstEncodingType_Legacy: iInstSize = sizeof(Legacy::LegacyInst_t); break;
        case InstEncodingType_VEX:    iInstSize = sizeof(VEX::VEXInst_t);       break;
        case InstEncodingType_EVEX:   iInstSize = sizeof(EVEX::EVEXInst_t);     break;

        default: assert(false && "Size of this instruction encoding type is not know. Not implemented yet!"); break;
    }
    assert(iInstSize > 0llu && iInstSize < 0x100 && "Invalid instruction size."); // Sanity checks. So we don't accidently blow up.


    // Boom!
    m_iInstEncodingType = iEncoding;
    m_pInst             = malloc(iInstSize);


    // TODO We will make a arena allocator soon, so will need to be rewritten.
    if(m_pInst == nullptr)
    {
        FAIL_LOG("Failed memory allocation for inst encoding [ %d ] and size [ %zu ]", iEncoding, iInstSize);
        return false;
    }

    // If we don't clear it, bad things will happen.
    memset(m_pInst, 0, iInstSize);
    return true;
}
