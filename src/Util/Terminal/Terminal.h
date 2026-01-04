//=========================================================================
//                      WRITE TO TERMINAL
//=========================================================================
// by      : INSANE
// created : 03/01/2026
// 
// purpose : Writes to terminal, in colors. Quick toggles.
//-------------------------------------------------------------------------
#pragma once
#include "ConsoleSystem.h"


#define EXPAND(X) X

#define CONCAT_HELPER(x,y)      x##y
#define CONCAT(x,y)             CONCAT_HELPER(x,y)


// To manually set color when we are writting something to the console.
#define CONSOLE_RED     "\033[31m"
#define CONSOLE_GREEN   "\033[32m"
#define CONSOLE_YELLOW  "\033[93m"
#define CONSOLE_RESET   "\033[0m"
#define CONSOLE_CYAN    "\033[96m"


// Formatted console output helper macors. So we disable then quickly.
#define ENABLE_CONSOLE_LOGS

#ifdef ENABLE_CONSOLE_LOGS

#define INITIALIZE_CONSOLE()    EXPAND(CONS_INITIALIZE())
#define UNINITIALIZE_CONSOLE()  EXPAND(CONS_UNINITIALIZE())
#define WIN_LOG(msg, ...)       EXPAND(CONS_FASTLOG(FG_GREEN, msg, ##__VA_ARGS__))
#define FAIL_LOG(msg, ...)      EXPAND(CONS_FASTLOG(FG_RED,   msg, ##__VA_ARGS__))
#define LOG(msg, ...)           EXPAND(CONS_FASTLOG(FG_CYAN,  msg, ##__VA_ARGS__))
#define LOG_VEC4(Vector)        EXPAND(CONS_FASTLOG_FLOAT_ARR_CUSTOM(Vector, sizeof(float) * 4))
#define LOG_VEC3(Vector)        EXPAND(CONS_FASTLOG_FLOAT_ARR_CUSTOM(Vector, sizeof(float) * 3))
#define LOG_VEC2(Vector)        EXPAND(CONS_FASTLOG_FLOAT_ARR_CUSTOM(Vector, sizeof(float) * 2))


#else 

#define WIN_LOG(msg, ...)       (void)0
#define FAIL_LOG(msg, ...)      (void)0
#define LOG(msg, ...)           (void)0
#define INITIALIZE_CONSOLE()    (void)0
#define UNINITIALIZE_CONSOLE()  (void)0
#define LOG_VEC4(Vector)        (void)0
#define LOG_VEC3(Vector)        (void)0
#define LOG_VEC2(Vector)        (void)0

#endif // _DEBUG