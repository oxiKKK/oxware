/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#include "precompiled.h"

template<class T>
inline void callback_table_hook_t<T>::install(const char* name, T* _ptr, const T& _original)
{
	ptr = _ptr;
	original = _original;

	CConsole::the().info("Found {} at 0x{:08X}", name, (uintptr_t)_ptr);
}

#if 0
#define P(name) CConsole::the().info("{}()", name)
#else
#define P(name)
#endif

//-----------------------------------------------------------------------------
//
// modfuncs_t
//

static void ModFuncs_LoadMod(char* pchModule)
{
	P(__FUNCTION__);
}

static void ModFuncs_CloseMod()
{
	P(__FUNCTION__);
}

static int ModFuncs_NCall(int ijump, int cnArg, ...)
{
	P(__FUNCTION__);
	return 0;
}

static void ModFuncs_GetClDstAddrs(hl::cldll_func_dst_t* pcldstAddrs)
{
	P(__FUNCTION__);
}

static void ModFuncs_GetEngDstAddrs(hl::cl_enginefunc_dst_t* pengdstAddrs)
{
	P(__FUNCTION__);
}

static void ModFuncs_ModuleLoaded()
{
	P(__FUNCTION__);
}

static void ModFuncs_ProcessOutgoingNet(hl::netchan_t* pchan, hl::sizebuf_t* psizebuf)
{
	P(__FUNCTION__);
}

static hl::qboolean ModFuncs_ProcessIncomingNet(hl::netchan_t* pchan, hl::sizebuf_t* psizebuf)
{
	P(__FUNCTION__);

	// called inside Netchan_Process. Has to return TRUE, othwerside the function fails.

	return TRUE;
}

static void ModFuncs_TextureLoad(char* pszName, int dxWidth, int dyHeight, char* pbData)
{
	P(__FUNCTION__);
}

static void ModFuncs_ModelLoad(hl::model_t* pmodel, void* pvBuf)
{
	P(__FUNCTION__);
}

static void ModFuncs_FrameBegin()
{
	P(__FUNCTION__);
}

static void ModFuncs_FrameRender1()
{
	P(__FUNCTION__);
}

static void ModFuncs_FrameRender2()
{
	P(__FUNCTION__);
}

static void ModFuncs_SetModsHelpers(hl::modshelpers_t* pmodshelpers)
{
	P(__FUNCTION__);
}

static void ModFuncs_SetModcHelpers(hl::modchelpers_t* pmodchelpers)
{
	P(__FUNCTION__);
}

static void ModFuncs_SetEngData(hl::engdata_t* pengdata)
{
	P(__FUNCTION__);
	// we can use this function to set our engdata hook

	CSecurityModuleHook::the().set_engdata(pengdata);
}

static void ModFuncs_ConnectClient(int iPlayer)
{
	P(__FUNCTION__);
}

static void ModFuncs_RecordIp(unsigned int pnIP)
{
	P(__FUNCTION__);
}

static void ModFuncs_PlayerStatus(unsigned char* pbData, int cbData)
{
	P(__FUNCTION__);
}

static void ModFuncs_SetEngineVersion(int nVersion)
{
	P(__FUNCTION__);
}

static int ModFuncs_PCMachine()
{
	P(__FUNCTION__);
	// unused
}

static void ModFuncs_SetIp(int ijump)
{
	P(__FUNCTION__);
	// unused
}

static void ModFuncs_Execute()
{
	P(__FUNCTION__);
	// unused
}

//-----------------------------------------------------------------------------
//
// cl_enginefunc_dst_t detour
//
// See https://github.com/ValveSoftware/halflife/issues/2131 for what handlers are missing
//

void EngDst_SPR_Load(const char** pTextureName)
{
	P(__FUNCTION__);
	
}

void EngDst_SPR_Frames(hl::HSPRITE_t*)
{
	P(__FUNCTION__);
}

void EngDst_SPR_Height(hl::HSPRITE_t*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SPR_Width(hl::HSPRITE_t*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SPR_Set(hl::HSPRITE_t*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SPR_Draw(int*, int*, int*, const hl::wrect_t**)
{
	P(__FUNCTION__);
}

void EngDst_SPR_DrawHoles(int*, int*, int*, const hl::wrect_t**)
{
	P(__FUNCTION__);
}

void EngDst_SPR_DrawAdditive(int*, int*, int*, const hl::wrect_t**)
{
	P(__FUNCTION__);
}

void EngDst_SPR_EnableScissor(int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SPR_DisableScissor()
{
	P(__FUNCTION__);
}

void EngDst_SPR_GetList(const char**, int**)
{
	P(__FUNCTION__);
}

void EngDst_FillRGBA(int*, int*, int*, int*, int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_GetScreenInfo(hl::SCREENINFO**)
{
	P(__FUNCTION__);
}

void EngDst_SetCrosshair(hl::HSPRITE_t*, hl::wrect_t*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_RegisterVariable(char**, char**, int*)
{
	P(__FUNCTION__);
}

void EngDst_GetCvarFloat(char**)
{
	P(__FUNCTION__);
}

void EngDst_GetCvarString(char**)
{
	P(__FUNCTION__);
}

void EngDst_AddCommand(char**, void (**()))
{
	P(__FUNCTION__);
}

void EngDst_HookUserMsg(char**, hl::pfnUserMsgHook*)
{
	P(__FUNCTION__);
}

void EngDst_ServerCmd(char**)
{
	P(__FUNCTION__);
}

void EngDst_ClientCmd(char**)
{
	P(__FUNCTION__);
}

void EngDst_PrimeMusicStream(const char**, int*)
{
	P(__FUNCTION__);
}

void EngDst_GetPlayerInfo(int*, hl::hud_player_info_t**)
{
	P(__FUNCTION__);
}

void EngDst_PlaySoundByName(char**, float*)
{
	P(__FUNCTION__);
}

void EngDst_PlaySoundByNameAtPitch(char**, float*, int*)
{
	P(__FUNCTION__);
}

void EngDst_PlaySoundVoiceByName(char**, float*)
{
	P(__FUNCTION__);
}

void EngDst_PlaySoundByIndex(int*, float*)
{
	P(__FUNCTION__);
}

void EngDst_AngleVectors(float**, float**, float**, float**)
{
	P(__FUNCTION__);
}

void EngDst_TextMessageGet(const char**)
{
	P(__FUNCTION__);
}

void EngDst_DrawCharacter(int*, int*, int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_DrawConsoleString(int*, int*, const char**)
{
	P(__FUNCTION__);
}

void EngDst_DrawSetTextColor(float*, float*, float*)
{
	P(__FUNCTION__);
}

void EngDst_DrawConsoleStringLen(const char**, int**, int**)
{
	P(__FUNCTION__);
}

void EngDst_ConsolePrint(const char**)
{
	P(__FUNCTION__);
}

void EngDst_CenterPrint(const char**)
{
	P(__FUNCTION__);
}

void EngDst_GetWindowCenterX()
{
	P(__FUNCTION__);
}

void EngDst_GetWindowCenterY()
{
	P(__FUNCTION__);
}

void EngDst_GetViewAngles(float**)
{
	P(__FUNCTION__);
}

void EngDst_SetViewAngles(float**)
{
	P(__FUNCTION__);
}

void EngDst_GetMaxClients()
{
	P(__FUNCTION__);
}

void EngDst_Cvar_SetValue(char**, float*)
{
	P(__FUNCTION__);
}

void EngDst_Cmd_Argc()
{
	P(__FUNCTION__);
}

void EngDst_Cmd_Argv(int*)
{
	P(__FUNCTION__);
}

void EngDst_Con_Printf(char**)
{
	P(__FUNCTION__);
}

void EngDst_Con_DPrintf(char**)
{
	P(__FUNCTION__);
}

void EngDst_Con_NPrintf(int*, char**)
{
	P(__FUNCTION__);
}

void EngDst_Con_NXPrintf(hl::con_nprint_t**, char**)
{
	P(__FUNCTION__);
}

void EngDst_PhysInfo_ValueForKey(const char**)
{
	P(__FUNCTION__);
}

void EngDst_ServerInfo_ValueForKey(const char**)
{
	P(__FUNCTION__);
}

void EngDst_GetClientMaxspeed()
{
	P(__FUNCTION__);
}

void EngDst_CheckParm(char**, char***)
{
	P(__FUNCTION__);
}

void EngDst_Key_Event(int*, hl::qboolean*)
{
	P(__FUNCTION__);
}

void EngDst_GetMousePosition(int**, int**)
{
	P(__FUNCTION__);
}

void EngDst_IsNoClipping()
{
	P(__FUNCTION__);
}

void EngDst_GetLocalPlayer()
{
	P(__FUNCTION__);
}

void EngDst_GetViewModel()
{
	P(__FUNCTION__);
}

void EngDst_GetEntityByIndex(int*)
{
	P(__FUNCTION__);
}

void EngDst_GetClientTime()
{
	P(__FUNCTION__);
}

void EngDst_V_CalcShake()
{
	P(__FUNCTION__);
}

void EngDst_V_ApplyShake(float**, float**, float*)
{
	P(__FUNCTION__);
}

void EngDst_PM_PointContents(float**, int**)
{
	P(__FUNCTION__);
}

void EngDst_PM_WaterEntity(float**)
{
	P(__FUNCTION__);
}

void EngDst_PM_TraceLine(float**, float**, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_CL_LoadModel(const char**, int**)
{
	P(__FUNCTION__);
}

void EngDst_CL_CreateVisibleEntity(int*, hl::cl_entity_t**)
{
	P(__FUNCTION__);
}

void EngDst_GetSpritePointer(hl::HSPRITE_t*)
{
	P(__FUNCTION__);
}

void EngDst_PlaySoundByNameAtLocation(char**, float*, float**)
{
	P(__FUNCTION__);
}

void EngDst_PrecacheEvent(int*, const char**)
{
	P(__FUNCTION__);
}

void EngDst_PlaybackEvent(int*, const hl::edict_t**, unsigned short*, float*, float**, float**, float*, float*, int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_WeaponAnim(int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_RandomFloat(float*, float*)
{
	P(__FUNCTION__);
}

void EngDst_RandomLong(hl::int32*, hl::int32*)
{
	P(__FUNCTION__);
}

void EngDst_HookEvent(char**, void (**(hl::event_args_t*)))
{
	P(__FUNCTION__);
}

void EngDst_Con_IsVisible()
{
	P(__FUNCTION__);
}

void EngDst_GetGameDirectory()
{
	P(__FUNCTION__);
}

void EngDst_GetCvarPointer(char**)
{
	P(__FUNCTION__);
}

void EngDst_Key_LookupBinding(const char**)
{
	P(__FUNCTION__);
}

void EngDst_GetLevelName()
{
	P(__FUNCTION__);
}

void EngDst_GetScreenFade(hl::screenfade_t**)
{
	P(__FUNCTION__);
}

void EngDst_SetScreenFade(hl::screenfade_t**)
{
	P(__FUNCTION__);
}

void EngDst_VGui_GetPanel()
{
	P(__FUNCTION__);
}

void EngDst_VGui_ViewportPaintBackground(int**)
{
	P(__FUNCTION__);
}

void EngDst_COM_LoadFile(const char**, int*, int**)
{
	P(__FUNCTION__);
}

void EngDst_COM_ParseFile(char**, char**)
{
	P(__FUNCTION__);
}

void EngDst_COM_FreeFile(void**)
{
	P(__FUNCTION__);
}

void EngDst_IsSpectateOnly()
{
	P(__FUNCTION__);
}

void EngDst_LoadMapSprite(const char**)
{
	P(__FUNCTION__);
}

void EngDst_COM_AddAppDirectoryToSearchPath(const char**, const char**)
{
	P(__FUNCTION__);
}

void EngDst_COM_ExpandFilename(const char**, char**, int*)
{
	P(__FUNCTION__);
}

void EngDst_PlayerInfo_ValueForKey(int*, const char**)
{
	P(__FUNCTION__);
}

void EngDst_PlayerInfo_SetValueForKey(const char**, const char**)
{
	P(__FUNCTION__);
}

void EngDst_GetPlayerUniqueID(int*, char**)
{
	P(__FUNCTION__);
}

void EngDst_GetTrackerIDForPlayer(int*)
{
	P(__FUNCTION__);
}

void EngDst_GetPlayerForTrackerID(int*)
{
	P(__FUNCTION__);
}

void EngDst_ServerCmdUnreliable(char**)
{
	P(__FUNCTION__);
}

void EngDst_GetMousePos(POINT**)
{
	P(__FUNCTION__);
}

void EngDst_SetMousePos(int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SetMouseEnable(hl::qboolean*)
{
	P(__FUNCTION__);
}

void EngDst_SetFilterMode(int*)
{
	P(__FUNCTION__);
}

void EngDst_SetFilterColor(float*, float*, float*)
{
	P(__FUNCTION__);
}

void EngDst_SetFilterBrightness(float*)
{
	P(__FUNCTION__);
}

void EngDst_SequenceGet(const char**, const char**)
{
	P(__FUNCTION__);
}

void EngDst_SPR_DrawGeneric(int*, int*, int*, const hl::wrect_t**, int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_SequencePickSentence(const char**, int*, int**)
{
	P(__FUNCTION__);
}

void EngDst_DrawString(int*, int*, const char*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_DrawStringReverse(int*, int*, const char*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_LocalPlayerInfo_ValueForKey(const char**)
{
	P(__FUNCTION__);
}

void EngDst_VGUI2DrawCharacter(int*, int*, int*, unsigned int*)
{
	P(__FUNCTION__);
}

void EngDst_VGUI2DrawCharacterAdd(int*, int*, int*, int*, int*, int*, unsigned int*)
{
	P(__FUNCTION__);
}

void EngDst_ProcessTutorMessageDecayBuffer(int**, int*)
{
	P(__FUNCTION__);
}

void EngDst_ConstructTutorMessageDecayBuffer(int**, int*)
{
	P(__FUNCTION__);
}

void EngDst_ResetTutorMessageDecayData()
{
	P(__FUNCTION__);
}

void EngDst_FillRGBABlend(int*, int*, int*, int*, int*, int*, int*, int*)
{
	P(__FUNCTION__);
}

void EngDst_GetAppID()
{
	P(__FUNCTION__);
}

void EngDst_GetAliases()
{
	P(__FUNCTION__);
}

void EngDst_VguiWrap2_GetMouseDelta(int* x, int* y)
{
	P(__FUNCTION__);
}

void EngDst_FilteredClientCmd(char**)
{
	P(__FUNCTION__);
}

//-----------------------------------------------------------------------------

bool CSecurityModuleHook::install_hooks()
{
	CConsole::the().info("Initializng security module hook...");
	
	if (!g_modfuncs().install())
	{
		return false;
	}
	
	if (!CommonLoadSecurityModule().install())
	{
		return false;
	}

	if (!CloseSecurityModule().install())
	{
		return false;
	}

	setup();

	CConsole::the().info("Security module hook initialized.");

	m_initialized = true;
	return true;
}

void CSecurityModuleHook::uninstall_hooks()
{
	if (!m_initialized)
	{
		return;
	}

	s_enginefunc_hook.restore();
	s_enginefunc_dst_hook.restore();
	s_cl_funcs_hook.restore();

	CloseSecurityModule().uninstall();

	restore_modfuncs_callback_table();
}

void CSecurityModuleHook::setup()
{
	// fill modfuncs_t wtih non-null data that can be used when CommonLoadSecurityModule is called.
	fill_modfuncs_callback_table();

	CConsole::the().info("Calling CommonLoadSecurityModule to retreive and modify pointers...");

	// Now call this function, which will eventually call g_modfuncs callbacks, that we hook. From these callbacks
	// we can get useful pointers to information.
	CommonLoadSecurityModule().call(NULL);

	//
	// Ok, now we have engdata_t pointer, let's fill others.
	//

	// callback tables
	s_enginefunc_hook.install("cl_enginefunc_t", s_engdata.pcl_enginefuncs, *s_engdata.pcl_enginefuncs);
	s_enginefunc_dst_hook.install("cl_enginefunc_dst_t", s_engdata.pg_engdstAddrs, *s_engdata.pg_engdstAddrs);
	s_cl_funcs_hook.install("cldll_func_t", s_engdata.pcl_funcs, *s_engdata.pcl_funcs);

	// regular pointer to pointer hooks
	s_keybindings = (char**)s_engdata.pkeybindings;
	s_cls = (hl::client_static_t*)s_engdata.pcls;
}

void CSecurityModuleHook::fill_modfuncs_callback_table()
{
	auto pg_modfuncs = g_modfuncs().get();

	pg_modfuncs->m_nVersion = k_nModuleVersionCur;
	pg_modfuncs->m_pfnLoadMod = ModFuncs_LoadMod;
	pg_modfuncs->m_pfnCloseMod = ModFuncs_CloseMod;
	pg_modfuncs->m_pfnNCall = ModFuncs_NCall;
	pg_modfuncs->m_pfnGetClDstAddrs = ModFuncs_GetClDstAddrs;
	pg_modfuncs->m_pfnGetEngDstAddrs = ModFuncs_GetEngDstAddrs;
	pg_modfuncs->m_pfnModuleLoaded = ModFuncs_ModuleLoaded;
	pg_modfuncs->m_pfnProcessOutgoingNet = ModFuncs_ProcessOutgoingNet;
	pg_modfuncs->m_pfnProcessIncomingNet = ModFuncs_ProcessIncomingNet;
	pg_modfuncs->m_pfnTextureLoad = ModFuncs_TextureLoad;
	pg_modfuncs->m_pfnModelLoad = ModFuncs_ModelLoad;
	pg_modfuncs->m_pfnFrameBegin = ModFuncs_FrameBegin;
	pg_modfuncs->m_pfnFrameRender1 = ModFuncs_FrameRender1;
	pg_modfuncs->m_pfnFrameRender2 = ModFuncs_FrameRender2;
	pg_modfuncs->m_pfnSetModSHelpers = ModFuncs_SetModsHelpers;
	pg_modfuncs->m_pfnSetModCHelpers = ModFuncs_SetModcHelpers;
	pg_modfuncs->m_pfnSetEngData = ModFuncs_SetEngData;
	pg_modfuncs->m_pfnConnectClient = ModFuncs_ConnectClient;
	pg_modfuncs->m_pfnRecordIP = ModFuncs_RecordIp;
	pg_modfuncs->m_pfnPlayerStatus = ModFuncs_PlayerStatus;
	pg_modfuncs->m_pfnSetEngineVersion = ModFuncs_SetEngineVersion;
}

void CSecurityModuleHook::restore_modfuncs_callback_table()
{
	auto pg_modfuncs = g_modfuncs().get();

	memset(pg_modfuncs, NULL, sizeof(hl::modfuncs_t));
}

void CSecurityModuleHook::fill_engdst_callback_table()
{
	auto cl_enginefunc_dst = s_enginefunc_dst_hook.ptr;
	cl_enginefunc_dst->pfnSPR_Load = EngDst_SPR_Load;
	cl_enginefunc_dst->pfnSPR_Frames = EngDst_SPR_Frames;
	cl_enginefunc_dst->pfnSPR_Height = EngDst_SPR_Height;
	cl_enginefunc_dst->pfnSPR_Width = EngDst_SPR_Width;
	cl_enginefunc_dst->pfnSPR_Set = EngDst_SPR_Set;
	cl_enginefunc_dst->pfnSPR_Draw = EngDst_SPR_Draw;
	cl_enginefunc_dst->pfnSPR_DrawHoles = EngDst_SPR_DrawHoles;
	cl_enginefunc_dst->pfnSPR_DrawAdditive = EngDst_SPR_DrawAdditive;
	cl_enginefunc_dst->pfnSPR_EnableScissor = EngDst_SPR_EnableScissor;
	cl_enginefunc_dst->pfnSPR_DisableScissor = EngDst_SPR_DisableScissor;
	cl_enginefunc_dst->pfnSPR_GetList = EngDst_SPR_GetList;
	cl_enginefunc_dst->pfnFillRGBA = EngDst_FillRGBA;
	cl_enginefunc_dst->pfnGetScreenInfo = EngDst_GetScreenInfo;
	cl_enginefunc_dst->pfnSetCrosshair = EngDst_SetCrosshair;
	cl_enginefunc_dst->pfnRegisterVariable = EngDst_RegisterVariable;
	cl_enginefunc_dst->pfnGetCvarFloat = EngDst_GetCvarFloat;
	cl_enginefunc_dst->pfnGetCvarString = EngDst_GetCvarString;
	cl_enginefunc_dst->pfnAddCommand = (hl::pfnDst_AddCommand_t)EngDst_AddCommand;
	cl_enginefunc_dst->pfnHookUserMsg = EngDst_HookUserMsg;
	cl_enginefunc_dst->pfnServerCmd = EngDst_ServerCmd;
	cl_enginefunc_dst->pfnClientCmd = EngDst_ClientCmd;
	cl_enginefunc_dst->pfnGetPlayerInfo = EngDst_GetPlayerInfo;
	cl_enginefunc_dst->pfnPlaySoundByName = EngDst_PlaySoundByName;
	cl_enginefunc_dst->pfnPlaySoundByIndex = EngDst_PlaySoundByIndex;
	cl_enginefunc_dst->pfnAngleVectors = EngDst_AngleVectors;
	cl_enginefunc_dst->pfnTextMessageGet = EngDst_TextMessageGet;
	cl_enginefunc_dst->pfnDrawCharacter = EngDst_DrawCharacter;
	cl_enginefunc_dst->pfnDrawConsoleString = EngDst_DrawConsoleString;
	cl_enginefunc_dst->pfnDrawSetTextColor = EngDst_DrawSetTextColor;
	cl_enginefunc_dst->pfnDrawConsoleStringLen = EngDst_DrawConsoleStringLen;
	cl_enginefunc_dst->pfnConsolePrint = EngDst_ConsolePrint;
	cl_enginefunc_dst->pfnCenterPrint = EngDst_CenterPrint;
	cl_enginefunc_dst->GetWindowCenterX = EngDst_GetWindowCenterX;
	cl_enginefunc_dst->GetWindowCenterY = EngDst_GetWindowCenterY;
	cl_enginefunc_dst->GetViewAngles = EngDst_GetViewAngles;
	cl_enginefunc_dst->SetViewAngles = EngDst_SetViewAngles;
	cl_enginefunc_dst->GetMaxClients = EngDst_GetMaxClients;
	cl_enginefunc_dst->Cvar_SetValue = EngDst_Cvar_SetValue;
	cl_enginefunc_dst->Cmd_Argc = EngDst_Cmd_Argc;
	cl_enginefunc_dst->Cmd_Argv = EngDst_Cmd_Argv;
	cl_enginefunc_dst->Con_Printf = EngDst_Con_Printf;
	cl_enginefunc_dst->Con_DPrintf = EngDst_Con_DPrintf;
	cl_enginefunc_dst->Con_NPrintf = EngDst_Con_NPrintf;
	cl_enginefunc_dst->Con_NXPrintf = EngDst_Con_NXPrintf;
	cl_enginefunc_dst->PhysInfo_ValueForKey = EngDst_PhysInfo_ValueForKey;
	cl_enginefunc_dst->ServerInfo_ValueForKey = EngDst_ServerInfo_ValueForKey;
	cl_enginefunc_dst->GetClientMaxspeed = EngDst_GetClientMaxspeed;
	cl_enginefunc_dst->CheckParm = EngDst_CheckParm;
	cl_enginefunc_dst->Key_Event = EngDst_Key_Event;
	cl_enginefunc_dst->GetMousePosition = EngDst_GetMousePosition;
	cl_enginefunc_dst->IsNoClipping = EngDst_IsNoClipping;
	cl_enginefunc_dst->GetLocalPlayer = EngDst_GetLocalPlayer;
	cl_enginefunc_dst->GetViewModel = EngDst_GetViewModel;
	cl_enginefunc_dst->GetEntityByIndex = EngDst_GetEntityByIndex;
	cl_enginefunc_dst->GetClientTime = EngDst_GetClientTime;
	cl_enginefunc_dst->V_CalcShake = EngDst_V_CalcShake;
	cl_enginefunc_dst->V_ApplyShake = EngDst_V_ApplyShake;
	cl_enginefunc_dst->PM_PointContents = EngDst_PM_PointContents;
	cl_enginefunc_dst->PM_WaterEntity = EngDst_PM_WaterEntity;
	cl_enginefunc_dst->PM_TraceLine = EngDst_PM_TraceLine;
	cl_enginefunc_dst->CL_LoadModel = EngDst_CL_LoadModel;
	cl_enginefunc_dst->CL_CreateVisibleEntity = EngDst_CL_CreateVisibleEntity;
	cl_enginefunc_dst->GetSpritePointer = EngDst_GetSpritePointer;
	cl_enginefunc_dst->pfnPlaySoundByNameAtLocation = EngDst_PlaySoundByNameAtLocation;
	cl_enginefunc_dst->pfnPrecacheEvent = EngDst_PrecacheEvent;
	cl_enginefunc_dst->pfnPlaybackEvent = EngDst_PlaybackEvent;
	cl_enginefunc_dst->pfnWeaponAnim = EngDst_WeaponAnim;
	cl_enginefunc_dst->pfnRandomFloat = EngDst_RandomFloat;
	cl_enginefunc_dst->pfnRandomLong = EngDst_RandomLong;
	cl_enginefunc_dst->pfnHookEvent = (hl::pfnDst_HookEvent_t)EngDst_HookEvent;
	cl_enginefunc_dst->Con_IsVisible = EngDst_Con_IsVisible;
	cl_enginefunc_dst->pfnGetGameDirectory = EngDst_GetGameDirectory;
	cl_enginefunc_dst->pfnGetCvarPointer = EngDst_GetCvarPointer;
	cl_enginefunc_dst->Key_LookupBinding = EngDst_Key_LookupBinding;
	cl_enginefunc_dst->pfnGetLevelName = EngDst_GetLevelName;
	cl_enginefunc_dst->pfnGetScreenFade = EngDst_GetScreenFade;
	cl_enginefunc_dst->pfnSetScreenFade = EngDst_SetScreenFade;
	cl_enginefunc_dst->VGui_GetPanel = EngDst_VGui_GetPanel;
	cl_enginefunc_dst->VGui_ViewportPaintBackground = EngDst_VGui_ViewportPaintBackground;
	cl_enginefunc_dst->COM_LoadFile = EngDst_COM_LoadFile;
	cl_enginefunc_dst->COM_ParseFile = EngDst_COM_ParseFile;
	cl_enginefunc_dst->COM_FreeFile = EngDst_COM_FreeFile;
	cl_enginefunc_dst->IsSpectateOnly = EngDst_IsSpectateOnly;
	cl_enginefunc_dst->LoadMapSprite = EngDst_LoadMapSprite;
	cl_enginefunc_dst->COM_AddAppDirectoryToSearchPath = EngDst_COM_AddAppDirectoryToSearchPath;
	cl_enginefunc_dst->COM_ExpandFilename = EngDst_COM_ExpandFilename;
	cl_enginefunc_dst->PlayerInfo_ValueForKey = EngDst_PlayerInfo_ValueForKey;
	cl_enginefunc_dst->PlayerInfo_SetValueForKey = EngDst_PlayerInfo_SetValueForKey;
	cl_enginefunc_dst->GetPlayerUniqueID = EngDst_GetPlayerUniqueID;
	cl_enginefunc_dst->GetTrackerIDForPlayer = EngDst_GetTrackerIDForPlayer;
	cl_enginefunc_dst->GetPlayerForTrackerID = EngDst_GetPlayerForTrackerID;
	cl_enginefunc_dst->pfnServerCmdUnreliable = EngDst_ServerCmdUnreliable;
	cl_enginefunc_dst->pfnGetMousePos = EngDst_GetMousePos;
	cl_enginefunc_dst->pfnSetMousePos = EngDst_SetMousePos;
	cl_enginefunc_dst->pfnSetMouseEnable = EngDst_SetMouseEnable;
	cl_enginefunc_dst->pfnSetFilterMode = EngDst_SetFilterMode;
	cl_enginefunc_dst->pfnSetFilterColor = EngDst_SetFilterColor;
	cl_enginefunc_dst->pfnSetFilterBrightness = EngDst_SetFilterBrightness;
	cl_enginefunc_dst->pfnSequenceGet = EngDst_SequenceGet;
	cl_enginefunc_dst->pfnSPR_DrawGeneric = EngDst_SPR_DrawGeneric;
	cl_enginefunc_dst->pfnSequencePickSentence = EngDst_SequencePickSentence;
	cl_enginefunc_dst->pfnDrawString = EngDst_DrawString;
	cl_enginefunc_dst->pfnDrawStringReverse = EngDst_DrawStringReverse;
	cl_enginefunc_dst->LocalPlayerInfo_ValueForKey = EngDst_LocalPlayerInfo_ValueForKey;
	cl_enginefunc_dst->pfnVGUI2DrawCharacter = EngDst_VGUI2DrawCharacter;
	cl_enginefunc_dst->pfnVGUI2DrawCharacterAdd = EngDst_VGUI2DrawCharacterAdd;
	cl_enginefunc_dst->pfnPlaySoundVoiceByName = EngDst_PlaySoundVoiceByName;
	cl_enginefunc_dst->pfnPrimeMusicStream = EngDst_PrimeMusicStream;
	cl_enginefunc_dst->pfnProcessTutorMessageDecayBuffer = EngDst_ProcessTutorMessageDecayBuffer;
	cl_enginefunc_dst->pfnConstructTutorMessageDecayBuffer = EngDst_ConstructTutorMessageDecayBuffer;
	cl_enginefunc_dst->pfnResetTutorMessageDecayData = EngDst_ResetTutorMessageDecayData;
	cl_enginefunc_dst->pfnPlaySoundByNameAtPitch = EngDst_PlaySoundByNameAtPitch;
	cl_enginefunc_dst->pfnFillRGBABlend = EngDst_FillRGBABlend;
	cl_enginefunc_dst->pfnGetAppID = EngDst_GetAppID;
	cl_enginefunc_dst->pfnGetAliasList = EngDst_GetAliases;
	cl_enginefunc_dst->pfnVguiWrap2_GetMouseDelta = EngDst_VguiWrap2_GetMouseDelta;
	cl_enginefunc_dst->pfnFilteredClientCmd = EngDst_FilteredClientCmd;
}

//-----------------------------------------------------------------------------

bool CommonLoadSecurityModule_FnHook_t::install()
{
	initialize("CommonLoadSecurityModule", L"hw.dll");
	return install_using_bytepattern(0);
}

//-----------------------------------------------------------------------------

bool g_modfuncs_MemoryHook::install()
{
	initialize("g_modfuncs", L"hw.dll");
	return install_using_bytepattern(1);
}

void g_modfuncs_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
	{
		return p->m_nVersion != NULL;
	});
}

//-----------------------------------------------------------------------------

bool CloseSecurityModule_FnDetour_t::install()
{
	initialize("CloseSecurityModule", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CloseSecurityModule);
}

void CloseSecurityModule_FnDetour_t::CloseSecurityModule()
{
	// This function still gets called inside CL_Disconnect. I have nodiea why, it's just stupid, but whatever.
	// If it's called, it resets all of the pointers we've set. Unless we'd set g_module.fLoaded back to zero
	// (which is set inside CommonLoadSecurityModule), we're fucked. :D
	return;

	CSecurityModuleHook::the().CloseSecurityModule().call();
}

//-----------------------------------------------------------------------------