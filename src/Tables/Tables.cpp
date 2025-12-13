//=========================================================================
//                      Tables
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Holds Look-up-tables of instruction information.
//-------------------------------------------------------------------------
#include "Tables.h"



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::ErrorCode_t Tables_t::Initialize()
{
    _InitializeInstTypeLUT(); // can't fail, so no checks required here.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::ErrorCode_t Tables_t::_InitializeInstTypeLUT()
{
    memset(m_instTypeLUT, 0, sizeof(m_instTypeLUT));


    return InsaneDASM64::ErrorCode_Success;
}
