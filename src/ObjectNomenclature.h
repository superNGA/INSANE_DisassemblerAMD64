//=========================================================================
//                      Object Nomenclature
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : For ease of creating objects in right namespace.
//-------------------------------------------------------------------------
#pragma once


#define DEFINE_GLOBAL_DISASSEMBLER_OBJECT(objName, objType) namespace InsaneDASM64::G { inline objType objName; }