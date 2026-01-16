//=========================================================================
//                      Aliases
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Commonly used typedefs.
//-------------------------------------------------------------------------
#pragma once



// These namespace name macros help me avoid spelling mistakes when creating namespaces.

/* Global Insane x86 disassembler namespace. All and any things declared by us will be included in this. No exceptions. */
#define INSANE_DASM64_NAMESPACE InsaneDASM64


/* 
Standard namespace shall hold all the data structures and constants that are used in more than one 
instruction data structures.
*/
#define STANDARD_NAMESPACE      Standard


/*
Legacy namespace holds all data structures that are specific to and used with decoding and disassembling
legacy encoded instructions.
*/
#define LEGACY_NAMESPACE        Legacy


/*
VEX namespace holds all data strucutures and constants that are specific to and used with decoding and disassembling
VEX encoded instructions.
*/
#define VEX_NAMESPACE           VEX


#define DEFINE_GLOBAL_OBJECT(objName, objType) namespace G { inline objType objName; }


namespace INSANE_DASM64_NAMESPACE
{
    typedef unsigned char Byte;
}
