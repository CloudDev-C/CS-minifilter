#include "Constants.h"
#include "Structures.h"

UNICODE_STRING bSWDLL = RTL_CONSTANT_STRING(L"sw.dll");
UNICODE_STRING bHWDLL = RTL_CONSTANT_STRING(L"hw.dll");

WCHAR CSMName[] = L"\\HL.EXE";
WCHAR CSGOMName[] = L"\\CSGO.EXE";

WCHAR STEAMMName[] = L"STEAM.EXE";
WCHAR STEAMMName_[] = L"\\STEAM.EXE";

UNICODE_STRING shaderExt = RTL_CONSTANT_STRING(L"vcs");

UNICODE_STRING ModelCSExt = RTL_CONSTANT_STRING(L"mdl");

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

UNICODE_STRING WHITE_STEAM_MODULES[STEAM_MODULES_COUNT] =
{
        RTL_CONSTANT_STRING(L"\\GameOverlayRenderer.dll"),
        RTL_CONSTANT_STRING(L"\\steamclient.dll"),
        RTL_CONSTANT_STRING(L"\\tier0_s.dll"),
        RTL_CONSTANT_STRING(L"\\vstdlib_s.dll")
};

UNICODE_STRING WHITE_MODULES_ARRAY_GAME_CS[CS_CLIENT_MODULES_COUNT] =
{
        RTL_CONSTANT_STRING(L"\\FileSystem_Stdio.dll"),
        RTL_CONSTANT_STRING(L"\\hw.dll"),
        RTL_CONSTANT_STRING(L"\\SDL2.dll"),
        RTL_CONSTANT_STRING(L"\\steam_api.dll"),
        RTL_CONSTANT_STRING(L"\\Mss32.dll"),
        RTL_CONSTANT_STRING(L"\\vgui.dll"),
        RTL_CONSTANT_STRING(L"\\cstrike\\cl_dlls\\client.dll"),
        RTL_CONSTANT_STRING(L"\\valve\\cl_dlls\\particleman.dll"),
        RTL_CONSTANT_STRING(L"\\valve\\cl_dlls\\GameUI.dll"),
        RTL_CONSTANT_STRING(L"\\vgui2.dll"),
        RTL_CONSTANT_STRING(L"\\chromehtml.dll"),
        RTL_CONSTANT_STRING(L"\\libcef.dll"),
        RTL_CONSTANT_STRING(L"\\tier0.dll"),
        RTL_CONSTANT_STRING(L"\\icudt.dll"),
        RTL_CONSTANT_STRING(L"\\avcodec-53.dll"),
        RTL_CONSTANT_STRING(L"\\avutil-51.dll"),
        RTL_CONSTANT_STRING(L"\\avformat-53.dll"),
        RTL_CONSTANT_STRING(L"\\DemoPlayer.dll"),
        RTL_CONSTANT_STRING(L"\\Core.dll"),
        RTL_CONSTANT_STRING(L"\\platform\\servers\\ServerBrowser.dll"),
        RTL_CONSTANT_STRING(L"\\vstdlib.dll"),
        RTL_CONSTANT_STRING(L"\\cstrike\\dlls\\mp.dll")
};

UNICODE_STRING WHITE_MODULES_ARRAY_GAME_CSGO[CSGO_CLIENT_MODULES_COUNT] =
{
    RTL_CONSTANT_STRING(L"\\csgo\\bin\\matchmaking.dll"),
    RTL_CONSTANT_STRING(L"\\csgo\\bin\\client.dll"),
    RTL_CONSTANT_STRING(L"\\csgo\\bin\\client_panorama.dll"),
    RTL_CONSTANT_STRING(L"\\csgo\\bin\\server.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\scenefilecache.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\xinput1_3.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\stdshader_dbg.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\stdshader_dx9.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\imemanager.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\localize.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\panorama_text_pango.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libfontconfig-1.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libglib-2.0-0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libgobject-2.0-0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libfreetype-6.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libpango-1.0-0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libpangoft2-1.0-0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libgmodule-2.0-0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\panoramauiclient.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\video.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libavcodec-56.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libavformat-56.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libavresample-2.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libavutil-54.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\libswscale-3.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\panorama.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\cairo.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\parsifal.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\v8_libplatform.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\shaderapidx9.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\vguimatsurface.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\vgui2.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\v8.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\v8_libbase.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\icui18n.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\icuuc.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\phonon.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\steamnetworkingsockets.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\inputsystem.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\vphysics.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\materialsystem.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\datacache.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\studiorender.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\soundemittersystem.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\vscript.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\soundsystem.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\valve_avi.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\steam_api.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\tier0.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\vstdlib.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\filesystem_stdio.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\engine.dll"),
    RTL_CONSTANT_STRING(L"\\bin\\launcher.dll")
};

MODEL_MD5_CS CS_MODEL_DATA[MAX_MODEL_DATA] =
{
    {RTL_CONSTANT_STRING(L"leet.mdl"),RTL_CONSTANT_STRING(L"0241dbb5a93c8694d21fb9980af17d8c"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\leet\\")},
    {RTL_CONSTANT_STRING(L"gign.mdl"),RTL_CONSTANT_STRING(L"52d571170973589af13778226ec1c2e4"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\gign\\")},
    {RTL_CONSTANT_STRING(L"leet.mdl"),RTL_CONSTANT_STRING(L"d104a41e14cf0b7c27929ddf97e9a8a7"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\leet\\")},
    {RTL_CONSTANT_STRING(L"gign.mdl"),RTL_CONSTANT_STRING(L"6eb0273cfa4fcaed22d692508eb3bf59"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\gign\\")},
    {RTL_CONSTANT_STRING(L"arctic.mdl"),RTL_CONSTANT_STRING(L"85a5375e674423a323745e6ca348285d"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\arctic\\")},
    {RTL_CONSTANT_STRING(L"arctic.mdl"),RTL_CONSTANT_STRING(L"f1aa2fc18dcf1fa3f6b8fa73ae75e346"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\arctic\\")},
    {RTL_CONSTANT_STRING(L"gsg9.mdl"),RTL_CONSTANT_STRING(L"b2c61468e60d7cf70d230cd8c956fd8e"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\gsg9\\")},
    {RTL_CONSTANT_STRING(L"gsg9.mdl"),RTL_CONSTANT_STRING(L"48662281b6d5ae2b0b0fcfb4dbf64897"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\gsg9\\")},
    {RTL_CONSTANT_STRING(L"guerilla.mdl"),RTL_CONSTANT_STRING(L"f759f6d24cfedc4e11207b687c7dff5b"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\guerilla\\")},
    {RTL_CONSTANT_STRING(L"guerilla.mdl"),RTL_CONSTANT_STRING(L"14e27570c240c16b7634e4ea67e6ea59"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\guerilla\\")},
    {RTL_CONSTANT_STRING(L"sas.mdl"),RTL_CONSTANT_STRING(L"467b15f2f6fa5f534cd907e024120ef6"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\sas\\")},
    {RTL_CONSTANT_STRING(L"sas.mdl"),RTL_CONSTANT_STRING(L"ba559858bdf16b1e13f17cd6a441816e"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\sas\\")},
    {RTL_CONSTANT_STRING(L"terror.mdl"),RTL_CONSTANT_STRING(L"18a2e940a8041eea6b3a56adaa0dd0cb"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\terror\\")},
    {RTL_CONSTANT_STRING(L"terror.mdl"),RTL_CONSTANT_STRING(L"1544b3d36bfb1f2a1f9d692041530e3c"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\terror\\")},
    {RTL_CONSTANT_STRING(L"urban.mdl"),RTL_CONSTANT_STRING(L"44718fbdd2dba087a15c2270ba1ea8be"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\urban\\")},
    {RTL_CONSTANT_STRING(L"urban.mdl"),RTL_CONSTANT_STRING(L"aa003d2ddb95a7b230073aa60f2c56e6"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\urban\\")},
    {RTL_CONSTANT_STRING(L"vip.mdl"),RTL_CONSTANT_STRING(L"dda8ae43070fddc3bc13da2d787ba807"),RTL_CONSTANT_STRING(L"cstrike\\models\\player\\vip\\")},
    {RTL_CONSTANT_STRING(L"vip.mdl"),RTL_CONSTANT_STRING(L"9a4a3792f497ea9e37114c20856ed392"),RTL_CONSTANT_STRING(L"cstrike_hd\\models\\player\\vip\\")},
};
