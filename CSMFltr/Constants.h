#ifndef  CONSTANTS_H
#define  CONSTANTS_H
#include <fltKernel.h>
#include "Structures.h"

#define DRVTAG ('CSSC')

extern UNICODE_STRING bSWDLL;
extern UNICODE_STRING bHWDLL;
extern WCHAR CSMName[];
extern WCHAR CSGOMName[];
extern WCHAR STEAMMName[];
extern WCHAR STEAMMName_[];
extern UNICODE_STRING shaderExt;

//check players model CS 1.6
#define MAX_MODEL_DATA 18
extern MODEL_MD5_CS CS_MODEL_DATA[MAX_MODEL_DATA];
//check players model CS 1.6

#define USE_MPDLL 1

#define STEAM_MODULES_COUNT 4
#define CSGO_CLIENT_MODULES_COUNT 53
#if (USE_MPDLL)
#define CS_CLIENT_MODULES_COUNT 22
#else
#define CS_CLIENT_MODULES_COUNT 21
#endif


//#if(_WIN64)
#if (USE_MPDLL)
#define CS16_SYSTEM_MODULES_COUNT 75
#else
#define CS16_SYSTEM_MODULES_COUNT 51
#endif
#define CSGO_SYSTEM_MODULES_COUNT 63
#define WHITE_MODLES_SOFT_COUNT   3

extern UNICODE_STRING WHITE_STEAM_MODULES[STEAM_MODULES_COUNT];
extern UNICODE_STRING WHITE_MODULES_ARRAY_GAME_CS[CS_CLIENT_MODULES_COUNT];
extern UNICODE_STRING WHITE_MODULES_ARRAY_GAME_CSGO[CSGO_CLIENT_MODULES_COUNT];

#endif // ! CONSTANTS_H

