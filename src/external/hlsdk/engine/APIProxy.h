/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef APIPROXY_H
#define APIPROXY_H
#pragma once

#include "client_textmessage.h"

// Routine prototype for parsing message data
typedef void (*svc_parsefn_t)();

// A single SVC object description
typedef struct
{
	// Opcode is the command number
	byte			opcode;

	// Display Name
	char*			pszname;

	// Routine that will be dispatched
	svc_parsefn_t	pfnParse;
} svc_func_t;

struct SCREENINFO
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];
};

struct client_data_t
{
	// fields that cannot be modified  (ie. have no effect if changed)
	vec3_t origin;

	// fields that can be changed by the cldll
	vec3_t viewangles;
	int		iWeaponBits;
	//	int		iAccessoryBits;
	float	fov;	// field of view
};

struct client_sprite_t
{
	char	szName[64];
	char	szSprite[64];
	int		hspr;
	int		iRes;
	wrect_t rc;
};

struct hud_player_info_t
{
	char*	name;
	short	ping;
	byte	thisplayer;  // TRUE if this is the calling player

	byte	spectator;
	byte	packetloss;

	char*	model;
	short	topcolor;
	short	bottomcolor;

	uint64	m_nSteamID;
};

struct module_t
{
	unsigned char				ucMD5Hash[16];	// hash over code
	qboolean					fLoaded;		// true if successfully loaded
};

typedef int				HSPRITE_t;	// handle to graphics
#define NULL_SPRITE		((hl::HSPRITE_t)NULL)

//-----------------------------------------------------------------------------
// Purpose: So that we don't have to include every single header file that 
//			containts these and this would probably lead to including even 
//			more header files in the end. These types are used only as pointers
//			inside this file so they don't need to have known declaration.
//-----------------------------------------------------------------------------
struct cl_enginefunc_t;
struct modfuncs_t;
struct IVoiceTweak;
struct playermove_t;

//-----------------------------------------------------------------------------
//
// Functions exported by the client.dll
//
//-----------------------------------------------------------------------------

#define CLDLL_INTERFACE_VERSION	7

//-----------------------------------------------------------------------------
// Purpose: Function type declarations for client exports
//-----------------------------------------------------------------------------
typedef int				(*pfnSrc_Initalize_t)				(cl_enginefunc_t*, int);
typedef void			(*pfnSrc_HUD_Init_t)				();
typedef int				(*pfnSrc_HUD_VIDInit_t)				();
typedef int				(*pfnSrc_HUD_Redraw_t)				(float, int);
typedef int				(*pfnSrc_HUD_UpdateClientData_t)	(client_data_t*, float);
typedef void			(*pfnSrc_HUD_Reset_t)				();
typedef void			(*pfnSrc_HUD_ClientMove_t)			(playermove_t*, qboolean);
typedef void			(*pfnSrc_HUD_ClientMoveInit_t)		(playermove_t*);
typedef char			(*pfnSrc_HUD_TextureType_t)			(char*);
typedef void			(*pfnSrc_HUD_IN_ActivateMouse_t)	();
typedef void			(*pfnSrc_HUD_IN_DeactivateMouse_t)	();
typedef void			(*pfnSrc_HUD_IN_MouseEvent_t)		(int);
typedef void			(*pfnSrc_HUD_IN_ClearStates_t)		();
typedef void			(*pfnSrc_HUD_IN_Accumulate_t)		();
typedef void			(*pfnSrc_HUD_CL_CreateMove_t)		(float, usercmd_t*, int);
typedef int				(*pfnSrc_HUD_CL_IsThirdperson_t)	();
typedef void			(*pfnSrc_HUD_CL_GetCameraOffsets_t)	(float*);
typedef kbutton_t*		(*pfnSrc_HUD_KB_Find_t)				(const char*);
typedef void			(*pfnSrc_HUD_CAMThink_t)			();
typedef void			(*pfnSrc_HUD_CalcRef_t)				(ref_params_t*);
typedef int				(*pfnSrc_HUD_AddEntity_t)			(int, cl_entity_t*, const char*);
typedef void			(*pfnSrc_HUD_CreateEntities_t)		();
typedef void			(*pfnSrc_HUD_DrawNormalTris_t)		();
typedef void			(*pfnSrc_HUD_DrawTransTris_t)		();
typedef void			(*pfnSrc_HUD_StudioEvent_t)			(const mstudioevent_t*, const cl_entity_t*);
typedef void			(*pfnSrc_HUD_PostRunCmd_t)			(local_state_t*, local_state_t*, usercmd_t*, int, double, unsigned int);
typedef void			(*pfnSrc_HUD_ShutDown_t)			();
typedef void			(*pfnSrc_HUD_TxferLocalOverrides_t)	(entity_state_t*, const clientdata_t*);
typedef void			(*pfnSrc_HUD_ProcessPlayerState_t)	(entity_state_t*, const entity_state_t*);
typedef void			(*pfnSrc_HUD_TxferPredictionData_t)	(entity_state_t*, const entity_state_t*, clientdata_t*, const clientdata_t*, weapon_data_t*, const weapon_data_t*);
typedef void			(*pfnSrc_HUD_DemoRead_t)			(int, unsigned char*);
typedef int				(*pfnSrc_HUD_ConnectionLess_t)		(const netadr_t*, const char*, char*, int*);
typedef int				(*pfnSrc_HUD_GetHullBounds_t)		(int, float*, float*);
typedef void			(*pfnSrc_HUD_Frame_t)				(double);
typedef int				(*pfnSrc_HUD_Key_Event_t)			(int, int, const char*);
typedef void			(*pfnSrc_HUD_TempentUpdate_t)		(double, double, double, tempent_t**, tempent_t**, int (*)(cl_entity_t*), void (*)(tempent_t*, float));
typedef cl_entity_t*	(*pfnSrc_HUD_GetUserEntity_t)		(int);
typedef void			(*pfnSrc_HUD_VoiceStatus_t)			(int, qboolean);
typedef void			(*pfnSrc_HUD_DirectorMessage_t)		(int, void*);
typedef int				(*pfnSrc_HUD_Studio_Interface_t)	(int, r_studio_interface_t**, engine_studio_api_t*);
typedef void			(*pfnSrc_HUD_ChatinputPosition_t)	(int*, int*);
typedef int				(*pfnSrc_HUD_GetPlayerTeam_t)		(int);
typedef void*			(*pfnSrc_ClientFactory_t)			();	// this should be CreateInterfaceFn but that means including interface.h
// which is a C++ file and some of the client files a C only... 
// so we return a void * which we then do a typecast on later.

//-----------------------------------------------------------------------------
// Purpose: Pointers to the exported client functions themselves
//-----------------------------------------------------------------------------
struct cldll_func_t
{
	pfnSrc_Initalize_t					pfnInitalize;
	pfnSrc_HUD_Init_t					pfnHUD_Init;
	pfnSrc_HUD_VIDInit_t				pfnHUD_VIDInit;
	pfnSrc_HUD_Redraw_t					pfnHUD_Redraw;
	pfnSrc_HUD_UpdateClientData_t		pfnHUD_UpdateClientData;
	pfnSrc_HUD_Reset_t					pfnHUD_Reset;
	pfnSrc_HUD_ClientMove_t				pfnHUD_ClientMove;
	pfnSrc_HUD_ClientMoveInit_t			pfnHUD_ClientMoveInit;
	pfnSrc_HUD_TextureType_t			pfnHUD_TextureType;
	pfnSrc_HUD_IN_ActivateMouse_t		pfnHUD_IN_ActivateMouse;
	pfnSrc_HUD_IN_DeactivateMouse_t		pfnHUD_IN_DeactivateMouse;
	pfnSrc_HUD_IN_MouseEvent_t			pfnHUD_IN_MouseEvent;
	pfnSrc_HUD_IN_ClearStates_t			pfnHUD_IN_ClearStates;
	pfnSrc_HUD_IN_Accumulate_t			pfnHUD_IN_Accumulate;
	pfnSrc_HUD_CL_CreateMove_t			pfnHUD_CL_CreateMove;
	pfnSrc_HUD_CL_IsThirdperson_t		pfnHUD_CL_IsThirdperson;
	pfnSrc_HUD_CL_GetCameraOffsets_t	pfnHUD_CL_GetCameraOffsets;
	pfnSrc_HUD_KB_Find_t				pfnHUD_KB_Find;
	pfnSrc_HUD_CAMThink_t				pfnHUD_CAMThink;
	pfnSrc_HUD_CalcRef_t				pfnHUD_CalcRef;
	pfnSrc_HUD_AddEntity_t				pfnHUD_AddEntity;
	pfnSrc_HUD_CreateEntities_t			pfnHUD_CreateEntities;
	pfnSrc_HUD_DrawNormalTris_t			pfnHUD_DrawNormalTris;
	pfnSrc_HUD_DrawTransTris_t			pfnHUD_DrawTransTris;
	pfnSrc_HUD_StudioEvent_t			pfnHUD_StudioEvent;
	pfnSrc_HUD_PostRunCmd_t				pfnHUD_PostRunCmd;
	pfnSrc_HUD_ShutDown_t				pfnHUD_ShutDown;
	pfnSrc_HUD_TxferLocalOverrides_t	pfnHUD_TxferLocalOverrides;
	pfnSrc_HUD_ProcessPlayerState_t		pfnHUD_ProcessPlayerState;
	pfnSrc_HUD_TxferPredictionData_t	pfnHUD_TxferPredictionData;
	pfnSrc_HUD_DemoRead_t				pfnHUD_DemoRead;
	pfnSrc_HUD_ConnectionLess_t			pfnHUD_ConnectionLess;
	pfnSrc_HUD_GetHullBounds_t			pfnHUD_GetHullBounds;
	pfnSrc_HUD_Frame_t					pfnHUD_Frame;
	pfnSrc_HUD_Key_Event_t				pfnHUD_Key_Event;
	pfnSrc_HUD_TempentUpdate_t			pfnHUD_TempentUpdate;
	pfnSrc_HUD_GetUserEntity_t			pfnHUD_GetUserEntity;
	pfnSrc_HUD_VoiceStatus_t			pfnHUD_VoiceStatus;
	pfnSrc_HUD_DirectorMessage_t		pfnHUD_DirectorMessage;
	pfnSrc_HUD_Studio_Interface_t		pfnHUD_Studio_Interface;
	pfnSrc_HUD_ChatinputPosition_t		pfnHUD_ChatinputPosition;
	pfnSrc_HUD_GetPlayerTeam_t			pfnHUD_GetPlayerTeam;
	pfnSrc_ClientFactory_t				pfnClientFactory;
};

//-----------------------------------------------------------------------------
// Purpose: Function type declarations for client destination functions
//-----------------------------------------------------------------------------
typedef int				(*pfnDst_Initalize_t)				(cl_enginefunc_t**, int*);
typedef void			(*pfnDst_HUD_Init_t)				();
typedef int				(*pfnDst_HUD_VIDInit_t)				();
typedef int				(*pfnDst_HUD_Redraw_t)				(float*, int*);
typedef int				(*pfnDst_HUD_UpdateClientData_t)	(client_data_t**, float*);
typedef void			(*pfnDst_HUD_Reset_t)				();
typedef void			(*pfnDst_HUD_ClientMove_t)			(playermove_t**, qboolean*);
typedef void			(*pfnDst_HUD_ClientMoveInit_t)		(playermove_t**);
typedef char			(*pfnDst_HUD_TextureType_t)			(char**);
typedef void			(*pfnDst_HUD_IN_ActivateMouse_t)	();
typedef void			(*pfnDst_HUD_IN_DeactivateMouse_t)	();
typedef void			(*pfnDst_HUD_IN_MouseEvent_t)		(int*);
typedef void			(*pfnDst_HUD_IN_ClearStates_t)		();
typedef void			(*pfnDst_HUD_IN_Accumulate_t)		();
typedef void			(*pfnDst_HUD_CL_CreateMove_t)		(float*, usercmd_t**, int*);
typedef int				(*pfnDst_HUD_CL_IsThirdperson_t)	();
typedef void			(*pfnDst_HUD_CL_GetCameraOffsets_t)	(float**);
typedef kbutton_t*		(*pfnDst_HUD_KB_Find_t)				(char**);
typedef void			(*pfnDst_HUD_CAMThink_t)			();
typedef void			(*pfnDst_HUD_CalcRef_t)				(ref_params_t**);
typedef int				(*pfnDst_HUD_AddEntity_t)			(int*, cl_entity_t**, const char**);
typedef void			(*pfnDst_HUD_CreateEntities_t)		();
typedef void			(*pfnDst_HUD_DrawNormalTris_t)		();
typedef void			(*pfnDst_HUD_DrawTransTris_t)		();
typedef void			(*pfnDst_HUD_StudioEvent_t)			(const mstudioevent_t**, const cl_entity_t**);
typedef void			(*pfnDst_HUD_PostRunCmd_t)			(local_state_t**, local_state_t**, usercmd_t**, int*, double*, unsigned int*);
typedef void			(*pfnDst_HUD_ShutDown_t)			();
typedef void			(*pfnDst_HUD_TxferLocalOverrides_t)	(entity_state_t**, const clientdata_t**);
typedef void			(*pfnDst_HUD_ProcessPlayerState_t)	(entity_state_t**, const entity_state_t**);
typedef void			(*pfnDst_HUD_TxferPredictionData_t)	(entity_state_t**, const entity_state_t**, clientdata_t**, const clientdata_t**, weapon_data_t**, const weapon_data_t**);
typedef void			(*pfnDst_HUD_DemoRead_t)			(int*, unsigned char**);
typedef int				(*pfnDst_HUD_ConnectionLess_t)		(const netadr_t**, const char**, char**, int**);
typedef int				(*pfnDst_HUD_GetHullBounds_t)		(int*, float**, float**);
typedef void			(*pfnDst_HUD_Frame_t)				(double*);
typedef int				(*pfnDst_HUD_Key_Event_t)			(int*, int*, const char**);
typedef void			(*pfnDst_HUD_TempentUpdate_t)		(double*, double*, double*, tempent_t***, tempent_t***, int (*)(cl_entity_t*), void (*)(tempent_t*, float));
typedef cl_entity_t*	(*pfnDst_HUD_GetUserEntity_t)		(int*);
typedef void			(*pfnDst_HUD_VoiceStatus_t)			(int*, qboolean*);
typedef void			(*pfnDst_HUD_DirectorMessage_t)		(int*, void**);
typedef int				(*pfnDst_HUD_Studio_Interface_t)	(int*, r_studio_interface_t***, engine_studio_api_t**);
typedef void			(*pfnDst_HUD_ChatinputPosition_t)	(int**, int**);
typedef int				(*pfnDst_HUD_GetPlayerTeam_t)		(int*);
typedef void*			(*pfnDst_ClientFactory_t)			();	// this should be CreateInterfaceFn but that means including interface.h
// which is a C++ file and some of the client files a C only... 
// so we return a void * which we then do a typecast on later.

//-----------------------------------------------------------------------------
// Purpose: Pointers to the client destination functions
//-----------------------------------------------------------------------------
struct cldll_func_dst_t
{
	pfnDst_Initalize_t					pfnInitalize;
	pfnDst_HUD_Init_t					pfnHUD_Init;
	pfnDst_HUD_VIDInit_t				pfnHUD_VIDInit;
	pfnDst_HUD_Redraw_t					pfnHUD_Redraw;
	pfnDst_HUD_UpdateClientData_t		pfnHUD_UpdateClientData;
	pfnDst_HUD_Reset_t					pfnHUD_Reset;
	pfnDst_HUD_ClientMove_t				pfnHUD_ClientMove;
	pfnDst_HUD_ClientMoveInit_t			pfnHUD_ClientMoveInit;
	pfnDst_HUD_TextureType_t			pfnHUD_TextureType;
	pfnDst_HUD_IN_ActivateMouse_t		pfnHUD_IN_ActivateMouse;
	pfnDst_HUD_IN_DeactivateMouse_t		pfnHUD_IN_DeactivateMouse;
	pfnDst_HUD_IN_MouseEvent_t			pfnHUD_IN_MouseEvent;
	pfnDst_HUD_IN_ClearStates_t			pfnHUD_IN_ClearStates;
	pfnDst_HUD_IN_Accumulate_t			pfnHUD_IN_Accumulate;
	pfnDst_HUD_CL_CreateMove_t			pfnHUD_CL_CreateMove;
	pfnDst_HUD_CL_IsThirdperson_t		pfnHUD_CL_IsThirdperson;
	pfnDst_HUD_CL_GetCameraOffsets_t	pfnHUD_CL_GetCameraOffsets;
	pfnDst_HUD_KB_Find_t				pfnHUD_KB_Find;
	pfnDst_HUD_CAMThink_t				pfnHUD_CAMThink;
	pfnDst_HUD_CalcRef_t				pfnHUD_CalcRef;
	pfnDst_HUD_AddEntity_t				pfnHUD_AddEntity;
	pfnDst_HUD_CreateEntities_t			pfnHUD_CreateEntities;
	pfnDst_HUD_DrawNormalTris_t			pfnHUD_DrawNormalTris;
	pfnDst_HUD_DrawTransTris_t			pfnHUD_DrawTransTris;
	pfnDst_HUD_StudioEvent_t			pfnHUD_StudioEvent;
	pfnDst_HUD_PostRunCmd_t				pfnHUD_PostRunCmd;
	pfnDst_HUD_ShutDown_t				pfnHUD_ShutDown;
	pfnDst_HUD_TxferLocalOverrides_t	pfnHUD_TxferLocalOverrides;
	pfnDst_HUD_ProcessPlayerState_t		pfnHUD_ProcessPlayerState;
	pfnDst_HUD_TxferPredictionData_t	pfnHUD_TxferPredictionData;
	pfnDst_HUD_DemoRead_t				pfnHUD_DemoRead;
	pfnDst_HUD_ConnectionLess_t			pfnHUD_ConnectionLess;
	pfnDst_HUD_GetHullBounds_t			pfnHUD_GetHullBounds;
	pfnDst_HUD_Frame_t					pfnHUD_Frame;
	pfnDst_HUD_Key_Event_t				pfnHUD_Key_Event;
	pfnDst_HUD_TempentUpdate_t			pfnHUD_TempentUpdate;
	pfnDst_HUD_GetUserEntity_t			pfnHUD_GetUserEntity;
	pfnDst_HUD_VoiceStatus_t			pfnHUD_VoiceStatus;
	pfnDst_HUD_DirectorMessage_t		pfnHUD_DirectorMessage;
	pfnDst_HUD_Studio_Interface_t		pfnHUD_Studio_Interface;
	pfnDst_HUD_ChatinputPosition_t		pfnHUD_ChatinputPosition;
	pfnDst_HUD_GetPlayerTeam_t			pfnHUD_GetPlayerTeam;
	pfnDst_ClientFactory_t				pfnClientFactory;
};

//-----------------------------------------------------------------------------
//
// Functions exported by the engine 
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Function type declarations for engine exports
//-----------------------------------------------------------------------------
typedef HSPRITE_t					(*pfnSrc_SPR_Load_t)					(const char*);
typedef int							(*pfnSrc_SPR_Frames_t)					(HSPRITE_t);
typedef int							(*pfnSrc_SPR_Height_t)					(HSPRITE_t, int);
typedef int							(*pfnSrc_SPR_Width_t)					(HSPRITE_t, int);
typedef void						(*pfnSrc_SPR_Set_t)						(HSPRITE_t, int, int, int);
typedef void						(*pfnSrc_SPR_Draw_t)					(int, int, int, const wrect_t*);
typedef void						(*pfnSrc_SPR_DrawHoles_t)				(int, int, int, const wrect_t*);
typedef void						(*pfnSrc_SPR_DrawAdditive_t)			(int, int, int, const wrect_t*);
typedef void						(*pfnSrc_SPR_EnableScissor_t)			(int, int, int, int);
typedef void						(*pfnSrc_SPR_DisableScissor_t)			();
typedef client_sprite_t*			(*pfnSrc_SPR_GetList_t)					(const char*, int*);
typedef void						(*pfnSrc_FillRGBA_t)					(int, int, int, int, int, int, int, int);
typedef int							(*pfnSrc_GetScreenInfo_t) 				(SCREENINFO*);
typedef void						(*pfnSrc_SetCrosshair_t)				(HSPRITE_t, wrect_t, int, int, int);
typedef cvar_t*						(*pfnSrc_RegisterVariable_t)			(char*, char*, int);
typedef float						(*pfnSrc_GetCvarFloat_t)				(char*);
typedef char*						(*pfnSrc_GetCvarString_t)				(char*);
typedef int							(*pfnSrc_AddCommand_t)					(char*, void (*)());
typedef int							(*pfnSrc_HookUserMsg_t)					(char*, pfnUserMsgHook);
typedef int							(*pfnSrc_ServerCmd_t)					(char*);
typedef int							(*pfnSrc_ClientCmd_t)					(char*);
typedef void						(*pfnSrc_PrimeMusicStream_t)			(const char*, int);
typedef void						(*pfnSrc_GetPlayerInfo_t)				(int, hud_player_info_t*);
typedef void						(*pfnSrc_PlaySoundByName_t)				(char*, float);
typedef void						(*pfnSrc_PlaySoundByNameAtPitch_t)		(char*, float, int);
typedef void						(*pfnSrc_PlaySoundVoiceByName_t)		(char*, float, int);
typedef void						(*pfnSrc_PlaySoundByIndex_t)			(int, float);
typedef void						(*pfnSrc_AngleVectors_t)				(float*, float*, float*, float*);
typedef client_textmessage_s*		(*pfnSrc_TextMessageGet_t)				(const char*);
typedef int							(*pfnSrc_DrawCharacter_t)				(int, int, int, int, int, int);
typedef int							(*pfnSrc_DrawConsoleString_t)			(int, int, char*);
typedef void						(*pfnSrc_DrawSetTextColor_t)			(float, float, float);
typedef void						(*pfnSrc_DrawConsoleStringLen_t)		(const char*, int*, int*);
typedef void						(*pfnSrc_ConsolePrint_t)				(const char*);
typedef void						(*pfnSrc_CenterPrint_t)					(const char*);
typedef int							(*pfnSrc_GetWindowCenterX_t)			();
typedef int							(*pfnSrc_GetWindowCenterY_t)			();
typedef void						(*pfnSrc_GetViewAngles_t)				(float*);
typedef void						(*pfnSrc_SetViewAngles_t)				(float*);
typedef int							(*pfnSrc_GetMaxClients_t)				();
typedef void						(*pfnSrc_Cvar_SetValue_t)				(char*, float);
typedef int       					(*pfnSrc_Cmd_Argc_t)					();
typedef char*						(*pfnSrc_Cmd_Argv_t)					(int);
typedef void						(*pfnSrc_Con_Printf_t)					(char*, ...);
typedef void						(*pfnSrc_Con_DPrintf_t)					(char*, ...);
typedef void						(*pfnSrc_Con_NPrintf_t)					(int, char*, ...);
typedef void						(*pfnSrc_Con_NXPrintf_t)				(con_nprint_t*, char*, ...);
typedef const char*					(*pfnSrc_PhysInfo_ValueForKey_t)		(const char*);
typedef const char*					(*pfnSrc_ServerInfo_ValueForKey_t)		(const char*);
typedef float						(*pfnSrc_GetClientMaxspeed_t)			();
typedef int							(*pfnSrc_CheckParm_t)					(char*, char**);
typedef void						(*pfnSrc_Key_Event_t)					(int, qboolean);
typedef void						(*pfnSrc_GetMousePosition_t)			(int*, int*);
typedef int							(*pfnSrc_IsNoClipping_t)				();
typedef cl_entity_t*				(*pfnSrc_GetLocalPlayer_t)				();
typedef cl_entity_t*				(*pfnSrc_GetViewModel_t)				();
typedef cl_entity_t*				(*pfnSrc_GetEntityByIndex_t)			(int);
typedef float						(*pfnSrc_GetClientTime_t)				();
typedef void						(*pfnSrc_V_CalcShake_t)					();
typedef void						(*pfnSrc_V_ApplyShake_t)				(float*, float*, float);
typedef int							(*pfnSrc_PM_PointContents_t)			(float*, int*);
typedef int							(*pfnSrc_PM_WaterEntity_t)				(float*);
typedef pmtrace_t*					(*pfnSrc_PM_TraceLine_t)				(float*, float*, int, int, int);
typedef model_t*					(*pfnSrc_CL_LoadModel_t)				(const char*, int*);
typedef int							(*pfnSrc_CL_CreateVisibleEntity_t)		(int, cl_entity_t*);
typedef model_t*					(*pfnSrc_GetSpritePointer_t)			(HSPRITE_t);
typedef void						(*pfnSrc_PlaySoundByNameAtLocation_t)	(char*, float, float*);
typedef unsigned short				(*pfnSrc_PrecacheEvent_t)				(int, const char*);
typedef void						(*pfnSrc_PlaybackEvent_t)				(int, const edict_t*, unsigned short, float, float*, float*, float, float, int, int, int, int);
typedef void						(*pfnSrc_WeaponAnim_t)					(int, int);
typedef float						(*pfnSrc_RandomFloat_t)					(float, float);
typedef int32						(*pfnSrc_RandomLong_t)					(int32, int32);
typedef void						(*pfnSrc_HookEvent_t)					(char*, void (*)(event_args_t*));
typedef int							(*pfnSrc_Con_IsVisible_t)				();
typedef const char*					(*pfnSrc_GetGameDirectory_t)			();
typedef cvar_t*						(*pfnSrc_GetCvarPointer_t)				(char*);
typedef const char*					(*pfnSrc_Key_LookupBinding_t)			(const char*);
typedef const char*					(*pfnSrc_GetLevelName_t)				();
typedef void						(*pfnSrc_GetScreenFade_t)				(screenfade_t*);
typedef void						(*pfnSrc_SetScreenFade_t)				(screenfade_t*);
typedef void*						(*pfnSrc_VGui_GetPanel_t)				();
typedef void						(*pfnSrc_VGui_ViewportPaintBackground_t)(int[4]);
typedef byte*						(*pfnSrc_COM_LoadFile_t)				(const char*, int, int*);
typedef char*						(*pfnSrc_COM_ParseFile_t)				(char*, char*);
typedef void						(*pfnSrc_COM_FreeFile_t)				(void*);
typedef struct triangleapi_t*		pSrc_TriAPI;
typedef struct efx_api_t*			pSrc_EfxAPI;
typedef event_api_t*				pSrc_EventAPI;
typedef struct demo_api_t*			pSrc_DemoAPI;
typedef struct net_api_t*			pSrc_NetAPI;
typedef struct IVoiceTweak*			pSrc_VoiceTweak;
typedef int							(*pfnSrc_IsSpectateOnly_t)				();
typedef model_t*					(*pfnSrc_LoadMapSprite_t)				(const char*);
typedef void						(*pfnSrc_COM_AddAppDirectoryToSearchPath_t)(const char*, const char*);
typedef int							(*pfnSrc_COM_ExpandFilename_t)			(const char*, char*, int);
typedef const char*					(*pfnSrc_PlayerInfo_ValueForKey_t)		(int, const char*);
typedef void						(*pfnSrc_PlayerInfo_SetValueForKey_t)	(const char*, const char*);
typedef qboolean					(*pfnSrc_GetPlayerUniqueID_t)			(int, char[16]);
typedef int							(*pfnSrc_GetTrackerIDForPlayer_t)		(int);
typedef int							(*pfnSrc_GetPlayerForTrackerID_t)		(int);
typedef int							(*pfnSrc_ServerCmdUnreliable_t)			(char*);
typedef void						(*pfnSrc_GetMousePos_t)					(POINT*);
typedef void						(*pfnSrc_SetMousePos_t)					(int, int);
typedef void						(*pfnSrc_SetMouseEnable_t)				(qboolean);
typedef cvar_t*						(*pfnSrc_GetFirstCVarPtr_t)				();
typedef cmd_function_t*				(*pfnSrc_GetFirstCmdFunctionHandle_t)	();
typedef cmd_function_t*				(*pfnSrc_GetNextCmdFunctionHandle_t)	(cmd_function_t*);
typedef const char*					(*pfnSrc_GetCmdFunctionName_t)			(cmd_function_t*);
typedef float						(*pfnSrc_GetClientOldTime_t)			();
typedef float						(*pfnSrc_GetServerGravityValue_t)		();
typedef model_t*					(*pfnSrc_GetModelByIndex_t)				(int);
typedef void						(*pfnSrc_SetFilterMode_t)				(int);
typedef void						(*pfnSrc_SetFilterColor_t)				(float, float, float);
typedef void						(*pfnSrc_SetFilterBrightness_t)			(float);
typedef struct sequenceEntry_s*		(*pfnSrc_SequenceGet_t)					(const char*, const char*);
typedef void						(*pfnSrc_SPR_DrawGeneric_t)				(int, int, int, const wrect_t*, int, int, int, int);
typedef struct sentenceEntry_s*		(*pfnSrc_SequencePickSentence_t)		(const char*, int, int*);
typedef int							(*pfnSrc_DrawString_t)					(int, int, const char*, int, int, int);
typedef int							(*pfnSrc_DrawStringReverse_t)			(int, int, const char*, int, int, int);
typedef const char*					(*pfnSrc_LocalPlayerInfo_ValueForKey_t)	(const char*);
typedef int							(*pfnSrc_VGUI2DrawCharacter_t)			(int, int, int, unsigned int);
typedef int							(*pfnSrc_VGUI2DrawCharacterAdd_t)		(int, int, int, int, int, int, unsigned int);
typedef unsigned int				(*pfnSrc_COM_GetApproxWavePlayLength)	(const char*);
typedef void*						(*pfnSrc_GetCareerUI_t)					();
typedef void						(*pfnSrc_Cvar_Set_t)					(char*, char*);
typedef int							(*pfnSrc_IsPlayingCareerMatch_t)		();
typedef double						(*pfnSrc_GetAbsoluteTime_t)				();
typedef void						(*pfnSrc_ProcessTutorMessageDecayBuffer_t)(int*, int);
typedef void						(*pfnSrc_ConstructTutorMessageDecayBuffer_t)(int*, int);
typedef void						(*pfnSrc_ResetTutorMessageDecayData_t)	();
typedef void						(*pfnSrc_FillRGBABlend_t)				(int, int, int, int, int, int, int, int);
typedef int							(*pfnSrc_GetAppID_t)					();
typedef cmdalias_t*					(*pfnSrc_GetAliases_t)					();
typedef void						(*pfnSrc_VguiWrap2_GetMouseDelta_t)		(int*, int*);
typedef int							(*pfnSrc_FilteredClientCmd_t)			(char*);

//-----------------------------------------------------------------------------
// Purpose: Pointers to the exported engine functions themselves
//-----------------------------------------------------------------------------
struct cl_enginefunc_t
{
	pfnSrc_SPR_Load_t							pfnSPR_Load;
	pfnSrc_SPR_Frames_t							pfnSPR_Frames;
	pfnSrc_SPR_Height_t							pfnSPR_Height;
	pfnSrc_SPR_Width_t							pfnSPR_Width;
	pfnSrc_SPR_Set_t							pfnSPR_Set;
	pfnSrc_SPR_Draw_t							pfnSPR_Draw;
	pfnSrc_SPR_DrawHoles_t						pfnSPR_DrawHoles;
	pfnSrc_SPR_DrawAdditive_t					pfnSPR_DrawAdditive;
	pfnSrc_SPR_EnableScissor_t					pfnSPR_EnableScissor;
	pfnSrc_SPR_DisableScissor_t					pfnSPR_DisableScissor;
	pfnSrc_SPR_GetList_t						pfnSPR_GetList;
	pfnSrc_FillRGBA_t							pfnFillRGBA;
	pfnSrc_GetScreenInfo_t						pfnGetScreenInfo;
	pfnSrc_SetCrosshair_t						pfnSetCrosshair;
	pfnSrc_RegisterVariable_t					pfnRegisterVariable;
	pfnSrc_GetCvarFloat_t						pfnGetCvarFloat;
	pfnSrc_GetCvarString_t						pfnGetCvarString;
	pfnSrc_AddCommand_t							pfnAddCommand;
	pfnSrc_HookUserMsg_t						pfnHookUserMsg;
	pfnSrc_ServerCmd_t							pfnServerCmd;
	pfnSrc_ClientCmd_t							pfnClientCmd;
	pfnSrc_GetPlayerInfo_t						pfnGetPlayerInfo;
	pfnSrc_PlaySoundByName_t					pfnPlaySoundByName;
	pfnSrc_PlaySoundByIndex_t					pfnPlaySoundByIndex;
	pfnSrc_AngleVectors_t						pfnAngleVectors;
	pfnSrc_TextMessageGet_t						pfnTextMessageGet;
	pfnSrc_DrawCharacter_t						pfnDrawCharacter;
	pfnSrc_DrawConsoleString_t					pfnDrawConsoleString;
	pfnSrc_DrawSetTextColor_t					pfnDrawSetTextColor;
	pfnSrc_DrawConsoleStringLen_t				pfnDrawConsoleStringLen;
	pfnSrc_ConsolePrint_t						pfnConsolePrint;
	pfnSrc_CenterPrint_t						pfnCenterPrint;
	pfnSrc_GetWindowCenterX_t					pfnGetWindowCenterX;
	pfnSrc_GetWindowCenterY_t					pfnGetWindowCenterY;
	pfnSrc_GetViewAngles_t						pfnGetViewAngles;
	pfnSrc_SetViewAngles_t						pfnSetViewAngles;
	pfnSrc_GetMaxClients_t						pfnGetMaxClients;
	pfnSrc_Cvar_SetValue_t						pfnCvar_SetValue;
	pfnSrc_Cmd_Argc_t							pfnCmd_Argc;
	pfnSrc_Cmd_Argv_t							pfnCmd_Argv;
	pfnSrc_Con_Printf_t							pfnCon_Printf;
	pfnSrc_Con_DPrintf_t						pfnCon_DPrintf;
	pfnSrc_Con_NPrintf_t						pfnCon_NPrintf;
	pfnSrc_Con_NXPrintf_t						pfnCon_NXPrintf;
	pfnSrc_PhysInfo_ValueForKey_t				pfnPhysInfo_ValueForKey;
	pfnSrc_ServerInfo_ValueForKey_t				pfnServerInfo_ValueForKey;
	pfnSrc_GetClientMaxspeed_t					pfnGetClientMaxspeed;
	pfnSrc_CheckParm_t							pfnCheckParm;
	pfnSrc_Key_Event_t							pfnKey_Event;
	pfnSrc_GetMousePosition_t					pfnGetMousePosition;
	pfnSrc_IsNoClipping_t						pfnIsNoClipping;
	pfnSrc_GetLocalPlayer_t						pfnGetLocalPlayer;
	pfnSrc_GetViewModel_t						pfnGetViewModel;
	pfnSrc_GetEntityByIndex_t					pfnGetEntityByIndex;
	pfnSrc_GetClientTime_t						pfnGetClientTime;
	pfnSrc_V_CalcShake_t						pfnV_CalcShake;
	pfnSrc_V_ApplyShake_t						pfnV_ApplyShake;
	pfnSrc_PM_PointContents_t					pfnPM_PointContents;
	pfnSrc_PM_WaterEntity_t						pfnPM_WaterEntity;
	pfnSrc_PM_TraceLine_t						pfnPM_TraceLine;
	pfnSrc_CL_LoadModel_t						pfnCL_LoadModel;
	pfnSrc_CL_CreateVisibleEntity_t				pfnCL_CreateVisibleEntity;
	pfnSrc_GetSpritePointer_t					pfnGetSpritePointer;
	pfnSrc_PlaySoundByNameAtLocation_t			pfnPlaySoundByNameAtLocation;
	pfnSrc_PrecacheEvent_t						pfnPrecacheEvent;
	pfnSrc_PlaybackEvent_t						pfnPlaybackEvent;
	pfnSrc_WeaponAnim_t							pfnWeaponAnim;
	pfnSrc_RandomFloat_t						pfnRandomFloat;
	pfnSrc_RandomLong_t							pfnRandomLong;
	pfnSrc_HookEvent_t							pfnHookEvent;
	pfnSrc_Con_IsVisible_t						pfnCon_IsVisible;
	pfnSrc_GetGameDirectory_t					pfnGetGameDirectory;
	pfnSrc_GetCvarPointer_t						pfnGetCvarPointer;
	pfnSrc_Key_LookupBinding_t					pfnKey_LookupBinding;
	pfnSrc_GetLevelName_t						pfnGetLevelName;
	pfnSrc_GetScreenFade_t						pfnGetScreenFade;
	pfnSrc_SetScreenFade_t						pfnSetScreenFade;
	pfnSrc_VGui_GetPanel_t						pfnVGui_GetPanel;
	pfnSrc_VGui_ViewportPaintBackground_t		pfnVGui_ViewportPaintBackground;
	pfnSrc_COM_LoadFile_t						pfnCOM_LoadFile;
	pfnSrc_COM_ParseFile_t						pfnCOM_ParseFile;
	pfnSrc_COM_FreeFile_t						pfnCOM_FreeFile;
	pSrc_TriAPI									pTriAPI;
	pSrc_EfxAPI									pEfxAPI;
	pSrc_EventAPI								pEventAPI;
	pSrc_DemoAPI								pDemoAPI;
	pSrc_NetAPI									pNetAPI;
	pSrc_VoiceTweak								pVoiceTweak;
	pfnSrc_IsSpectateOnly_t						pfnIsSpectateOnly;
	pfnSrc_LoadMapSprite_t						pfnLoadMapSprite;
	pfnSrc_COM_AddAppDirectoryToSearchPath_t	pfnCOM_AddAppDirectoryToSearchPath;
	pfnSrc_COM_ExpandFilename_t					pfnCOM_ExpandFilename;
	pfnSrc_PlayerInfo_ValueForKey_t				pfnPlayerInfo_ValueForKey;
	pfnSrc_PlayerInfo_SetValueForKey_t			pfnPlayerInfo_SetValueForKey;
	pfnSrc_GetPlayerUniqueID_t					pfnGetPlayerUniqueID;
	pfnSrc_GetTrackerIDForPlayer_t				pfnGetTrackerIDForPlayer;
	pfnSrc_GetPlayerForTrackerID_t				pfnGetPlayerForTrackerID;
	pfnSrc_ServerCmdUnreliable_t				pfnServerCmdUnreliable;
	pfnSrc_GetMousePos_t						pfnGetMousePos;
	pfnSrc_SetMousePos_t						pfnSetMousePos;
	pfnSrc_SetMouseEnable_t						pfnSetMouseEnable;
	pfnSrc_GetFirstCVarPtr_t					pfnGetFirstCvarPtr;
	pfnSrc_GetFirstCmdFunctionHandle_t			pfnGetFirstCmdFunctionHandle;
	pfnSrc_GetNextCmdFunctionHandle_t			pfnGetNextCmdFunctionHandle;
	pfnSrc_GetCmdFunctionName_t					pfnGetCmdFunctionName;
	pfnSrc_GetClientOldTime_t					pfnGetClientOldTime;
	pfnSrc_GetServerGravityValue_t				pfnGetServerGravityValue;
	pfnSrc_GetModelByIndex_t					pfnGetModelByIndex;
	pfnSrc_SetFilterMode_t						pfnSetFilterMode;
	pfnSrc_SetFilterColor_t						pfnSetFilterColor;
	pfnSrc_SetFilterBrightness_t				pfnSetFilterBrightness;
	pfnSrc_SequenceGet_t						pfnSequenceGet;
	pfnSrc_SPR_DrawGeneric_t					pfnSPR_DrawGeneric;
	pfnSrc_SequencePickSentence_t				pfnSequencePickSentence;
	pfnSrc_DrawString_t							pfnDrawString;
	pfnSrc_DrawStringReverse_t					pfnDrawStringReverse;
	pfnSrc_LocalPlayerInfo_ValueForKey_t		pfnLocalPlayerInfo_ValueForKey;
	pfnSrc_VGUI2DrawCharacter_t					pfnVGUI2DrawCharacter;
	pfnSrc_VGUI2DrawCharacterAdd_t				pfnVGUI2DrawCharacterAdd;
	pfnSrc_COM_GetApproxWavePlayLength			pfnCOM_GetApproxWavePlayLength;
	pfnSrc_GetCareerUI_t						pfnGetCareerUI;
	pfnSrc_Cvar_Set_t							pfnCvar_Set;
	pfnSrc_IsPlayingCareerMatch_t				pfnIsCareerMatch;
	pfnSrc_PlaySoundVoiceByName_t				pfnPlaySoundVoiceByName;
	pfnSrc_PrimeMusicStream_t					pfnPrimeMusicStream;
	pfnSrc_GetAbsoluteTime_t					pfnGetAbsoluteTime;
	pfnSrc_ProcessTutorMessageDecayBuffer_t		pfnProcessTutorMessageDecayBuffer;
	pfnSrc_ConstructTutorMessageDecayBuffer_t	pfnConstructTutorMessageDecayBuffer;
	pfnSrc_ResetTutorMessageDecayData_t			pfnResetTutorMessageDecayData;
	pfnSrc_PlaySoundByNameAtPitch_t				pfnPlaySoundByNameAtPitch;
	pfnSrc_FillRGBABlend_t						pfnFillRGBABlend;
	pfnSrc_GetAppID_t							pfnGetAppID;
	pfnSrc_GetAliases_t							pfnGetAliasList;
	pfnSrc_VguiWrap2_GetMouseDelta_t			pfnVguiWrap2_GetMouseDelta;
	pfnSrc_FilteredClientCmd_t					pfnFilteredClientCmd;
};

//-----------------------------------------------------------------------------
// Purpose: Function type declarations for engine destination functions
//-----------------------------------------------------------------------------
typedef void (*pfnDst_SPR_Load_t)						(const char**);
typedef void (*pfnDst_SPR_Frames_t)						(HSPRITE_t*);
typedef void (*pfnDst_SPR_Height_t)						(HSPRITE_t*, int*);
typedef void (*pfnDst_SPR_Width_t)						(HSPRITE_t*, int*);
typedef void (*pfnDst_SPR_Set_t)						(HSPRITE_t*, int*, int*, int*);
typedef void (*pfnDst_SPR_Draw_t)						(int*, int*, int*, const wrect_t**);
typedef void (*pfnDst_SPR_DrawHoles_t)					(int*, int*, int*, const wrect_t**);
typedef void (*pfnDst_SPR_DrawAdditive_t)				(int*, int*, int*, const wrect_t**);
typedef void (*pfnDst_SPR_EnableScissor_t)				(int*, int*, int*, int*);
typedef void (*pfnDst_SPR_DisableScissor_t)				();
typedef void (*pfnDst_SPR_GetList_t)					(const char**, int**);
typedef void (*pfnDst_FillRGBA_t)						(int*, int*, int*, int*, int*, int*, int*, int*);
typedef void (*pfnDst_GetScreenInfo_t) 					(SCREENINFO**);
typedef void (*pfnDst_SetCrosshair_t)					(HSPRITE_t*, wrect_t*, int*, int*, int*);
typedef void (*pfnDst_RegisterVariable_t)				(char**, char**, int*);
typedef void (*pfnDst_GetCvarFloat_t)					(char**);
typedef void (*pfnDst_GetCvarString_t)					(char**);
typedef void (*pfnDst_AddCommand_t)						(char**, void (**)());
typedef void (*pfnDst_HookUserMsg_t)					(char**, pfnUserMsgHook*);
typedef void (*pfnDst_ServerCmd_t)						(char**);
typedef void (*pfnDst_ClientCmd_t)						(char**);
typedef void (*pfnDst_PrimeMusicStream_t)				(const char**, int*);
typedef void (*pfnDst_GetPlayerInfo_t)					(int*, hud_player_info_t**);
typedef void (*pfnDst_PlaySoundByName_t)				(char**, float*);
typedef void (*pfnDst_PlaySoundByNameAtPitch_t)			(char**, float*, int*);
typedef void (*pfnDst_PlaySoundVoiceByName_t)			(char**, float*);
typedef void (*pfnDst_PlaySoundByIndex_t)				(int*, float*);
typedef void (*pfnDst_AngleVectors_t)					(float**, float**, float**, float**);
typedef void (*pfnDst_TextMessageGet_t)					(const char**);
typedef void (*pfnDst_DrawCharacter_t)					(int*, int*, int*, int*, int*, int*);
typedef void (*pfnDst_DrawConsoleString_t)				(int*, int*, const char**);
typedef void (*pfnDst_DrawSetTextColor_t)				(float*, float*, float*);
typedef void (*pfnDst_DrawConsoleStringLen_t)			(const char**, int**, int**);
typedef void (*pfnDst_ConsolePrint_t)					(const char**);
typedef void (*pfnDst_CenterPrint_t)					(const char**);
typedef void (*pfnDst_GetWindowCenterX_t)				();
typedef void (*pfnDst_GetWindowCenterY_t)				();
typedef void (*pfnDst_GetViewAngles_t)					(float**);
typedef void (*pfnDst_SetViewAngles_t)					(float**);
typedef void (*pfnDst_GetMaxClients_t)					();
typedef void (*pfnDst_Cvar_SetValue_t)					(char**, float*);
typedef void (*pfnDst_Cmd_Argc_t)						();
typedef void (*pfnDst_Cmd_Argv_t)						(int*);
typedef void (*pfnDst_Con_Printf_t)						(char**);
typedef void (*pfnDst_Con_DPrintf_t)					(char**);
typedef void (*pfnDst_Con_NPrintf_t)					(int*, char**);
typedef void (*pfnDst_Con_NXPrintf_t)					(con_nprint_t**, char**);
typedef void (*pfnDst_PhysInfo_ValueForKey_t)			(const char**);
typedef void (*pfnDst_ServerInfo_ValueForKey_t)			(const char**);
typedef void (*pfnDst_GetClientMaxspeed_t)				();
typedef void (*pfnDst_CheckParm_t)						(char**, char***);
typedef void (*pfnDst_Key_Event_t)						(int*, qboolean*);
typedef void (*pfnDst_GetMousePosition_t)				(int**, int**);
typedef void (*pfnDst_IsNoClipping_t)					();
typedef void (*pfnDst_GetLocalPlayer_t)					();
typedef void (*pfnDst_GetViewModel_t)					();
typedef void (*pfnDst_GetEntityByIndex_t)				(int*);
typedef void (*pfnDst_GetClientTime_t)					();
typedef void (*pfnDst_V_CalcShake_t)					();
typedef void (*pfnDst_V_ApplyShake_t)					(float**, float**, float*);
typedef void (*pfnDst_PM_PointContents_t)				(float**, int**);
typedef void (*pfnDst_PM_WaterEntity_t)					(float**);
typedef void (*pfnDst_PM_TraceLine_t)					(float**, float**, int*, int*, int*);
typedef void (*pfnDst_CL_LoadModel_t)					(const char**, int**);
typedef void (*pfnDst_CL_CreateVisibleEntity_t)			(int*, cl_entity_t**);
typedef void (*pfnDst_GetSpritePointer_t)				(HSPRITE_t*);
typedef void (*pfnDst_PlaySoundByNameAtLocation_t)		(char**, float*, float**);
typedef void (*pfnDst_PrecacheEvent_t)					(int*, const char**);
typedef void (*pfnDst_PlaybackEvent_t)					(int*, const edict_t**, unsigned short*, float*, float**, float**, float*, float*, int*, int*, int*, int*);
typedef void (*pfnDst_WeaponAnim_t)						(int*, int*);
typedef void (*pfnDst_RandomFloat_t)					(float*, float*);
typedef void (*pfnDst_RandomLong_t)						(int32*, int32*);
typedef void (*pfnDst_HookEvent_t)						(char**, void (**)(event_args_t*));
typedef void (*pfnDst_Con_IsVisible_t)					();
typedef void (*pfnDst_GetGameDirectory_t)				();
typedef void (*pfnDst_GetCvarPointer_t)					(char**);
typedef void (*pfnDst_Key_LookupBinding_t)				(const char**);
typedef void (*pfnDst_GetLevelName_t)					();
typedef void (*pfnDst_GetScreenFade_t)					(screenfade_t**);
typedef void (*pfnDst_SetScreenFade_t)					(screenfade_t**);
typedef void (*pfnDst_VGui_GetPanel_t)					();
typedef void (*pfnDst_VGui_ViewportPaintBackground_t)	(int**);
typedef void (*pfnDst_COM_LoadFile_t)					(const char**, int*, int**);
typedef void (*pfnDst_COM_ParseFile_t)					(char**, char**);
typedef void (*pfnDst_COM_FreeFile_t)					(void**);
typedef triangleapi_t*									pDst_TriAPI;
typedef efx_api_t*										pDst_EfxAPI;
typedef event_api_t*									pDst_EventAPI;
typedef demo_api_t*										pDst_DemoAPI;
typedef net_api_t*										pDst_NetAPI;
typedef IVoiceTweak*									pDst_VoiceTweak;
typedef void (*pfnDst_IsSpectateOnly_t)					();
typedef void (*pfnDst_LoadMapSprite_t)					(const char**);
typedef void (*pfnDst_COM_AddAppDirectoryToSearchPath_t)(const char**, const char**);
typedef void (*pfnDst_COM_ExpandFilename_t)				(const char**, char**, int*);
typedef void (*pfnDst_PlayerInfo_ValueForKey_t)			(int*, const char**);
typedef void (*pfnDst_PlayerInfo_SetValueForKey_t)		(const char**, const char**);
typedef void (*pfnDst_GetPlayerUniqueID_t)				(int*, char**);
typedef void (*pfnDst_GetTrackerIDForPlayer_t)			(int*);
typedef void (*pfnDst_GetPlayerForTrackerID_t)			(int*);
typedef void (*pfnDst_ServerCmdUnreliable_t)			(char**);
typedef void (*pfnDst_GetMousePos_t)					(POINT**);
typedef void (*pfnDst_SetMousePos_t)					(int*, int*);
typedef void (*pfnDst_SetMouseEnable_t)					(qboolean*);
typedef void (*pfnDst_SetFilterMode_t)					(int*);
typedef void (*pfnDst_SetFilterColor_t)					(float*, float*, float*);
typedef void (*pfnDst_SetFilterBrightness_t)			(float*);
typedef void (*pfnDst_SequenceGet_t)					(const char**, const char**);
typedef void (*pfnDst_SPR_DrawGeneric_t)				(int*, int*, int*, const wrect_t**, int*, int*, int*, int*);
typedef void (*pfnDst_SequencePickSentence_t)			(const char**, int*, int**);
typedef void (*pfnDst_DrawString_t)						(int*, int*, const char*, int*, int*, int*);
typedef void (*pfnDst_DrawStringReverse_t)				(int*, int*, const char*, int*, int*, int*);
typedef void (*pfnDst_LocalPlayerInfo_ValueForKey_t)	(const char**);
typedef void (*pfnDst_VGUI2DrawCharacter_t)				(int*, int*, int*, unsigned int*);
typedef void (*pfnDst_VGUI2DrawCharacterAdd_t)			(int*, int*, int*, int*, int*, int*, unsigned int*);
typedef void (*pfnDst_ProcessTutorMessageDecayBuffer_t)	(int**, int*);
typedef void (*pfnDst_ConstructTutorMessageDecayBuffer_t)(int**, int*);
typedef void (*pfnDst_ResetTutorMessageDecayData_t)		();
typedef void (*pfnDst_FillRGBABlend_t)					(int*, int*, int*, int*, int*, int*, int*, int*);
typedef void (*pfnDst_GetAppID_t)						();
typedef void (*pfnDst_GetAliases_t)						();
typedef void (*pfnDst_VguiWrap2_GetMouseDelta_t)		(int* x, int* y);
typedef void (*pfnDst_FilteredClientCmd_t)				(char**);

//-----------------------------------------------------------------------------
// Purpose: Pointers to the engine destination functions
//-----------------------------------------------------------------------------
struct cl_enginefunc_dst_t
{
	pfnDst_SPR_Load_t							pfnSPR_Load;
	pfnDst_SPR_Frames_t							pfnSPR_Frames;
	pfnDst_SPR_Height_t							pfnSPR_Height;
	pfnDst_SPR_Width_t							pfnSPR_Width;
	pfnDst_SPR_Set_t							pfnSPR_Set;
	pfnDst_SPR_Draw_t							pfnSPR_Draw;
	pfnDst_SPR_DrawHoles_t						pfnSPR_DrawHoles;
	pfnDst_SPR_DrawAdditive_t					pfnSPR_DrawAdditive;
	pfnDst_SPR_EnableScissor_t					pfnSPR_EnableScissor;
	pfnDst_SPR_DisableScissor_t					pfnSPR_DisableScissor;
	pfnDst_SPR_GetList_t						pfnSPR_GetList;
	pfnDst_FillRGBA_t							pfnFillRGBA;
	pfnDst_GetScreenInfo_t						pfnGetScreenInfo;
	pfnDst_SetCrosshair_t						pfnSetCrosshair;
	pfnDst_RegisterVariable_t					pfnRegisterVariable;
	pfnDst_GetCvarFloat_t						pfnGetCvarFloat;
	pfnDst_GetCvarString_t						pfnGetCvarString;
	pfnDst_AddCommand_t							pfnAddCommand;
	pfnDst_HookUserMsg_t						pfnHookUserMsg;
	pfnDst_ServerCmd_t							pfnServerCmd;
	pfnDst_ClientCmd_t							pfnClientCmd;
	pfnDst_GetPlayerInfo_t						pfnGetPlayerInfo;
	pfnDst_PlaySoundByName_t					pfnPlaySoundByName;
	pfnDst_PlaySoundByIndex_t					pfnPlaySoundByIndex;
	pfnDst_AngleVectors_t						pfnAngleVectors;
	pfnDst_TextMessageGet_t						pfnTextMessageGet;
	pfnDst_DrawCharacter_t						pfnDrawCharacter;
	pfnDst_DrawConsoleString_t					pfnDrawConsoleString;
	pfnDst_DrawSetTextColor_t					pfnDrawSetTextColor;
	pfnDst_DrawConsoleStringLen_t				pfnDrawConsoleStringLen;
	pfnDst_ConsolePrint_t						pfnConsolePrint;
	pfnDst_CenterPrint_t						pfnCenterPrint;
	pfnDst_GetWindowCenterX_t					GetWindowCenterX;
	pfnDst_GetWindowCenterY_t					GetWindowCenterY;
	pfnDst_GetViewAngles_t						GetViewAngles;
	pfnDst_SetViewAngles_t						SetViewAngles;
	pfnDst_GetMaxClients_t						GetMaxClients;
	pfnDst_Cvar_SetValue_t						Cvar_SetValue;
	pfnDst_Cmd_Argc_t							Cmd_Argc;
	pfnDst_Cmd_Argv_t							Cmd_Argv;
	pfnDst_Con_Printf_t							Con_Printf;
	pfnDst_Con_DPrintf_t						Con_DPrintf;
	pfnDst_Con_NPrintf_t						Con_NPrintf;
	pfnDst_Con_NXPrintf_t						Con_NXPrintf;
	pfnDst_PhysInfo_ValueForKey_t				PhysInfo_ValueForKey;
	pfnDst_ServerInfo_ValueForKey_t				ServerInfo_ValueForKey;
	pfnDst_GetClientMaxspeed_t					GetClientMaxspeed;
	pfnDst_CheckParm_t							CheckParm;
	pfnDst_Key_Event_t							Key_Event;
	pfnDst_GetMousePosition_t					GetMousePosition;
	pfnDst_IsNoClipping_t						IsNoClipping;
	pfnDst_GetLocalPlayer_t						GetLocalPlayer;
	pfnDst_GetViewModel_t						GetViewModel;
	pfnDst_GetEntityByIndex_t					GetEntityByIndex;
	pfnDst_GetClientTime_t						GetClientTime;
	pfnDst_V_CalcShake_t						V_CalcShake;
	pfnDst_V_ApplyShake_t						V_ApplyShake;
	pfnDst_PM_PointContents_t					PM_PointContents;
	pfnDst_PM_WaterEntity_t						PM_WaterEntity;
	pfnDst_PM_TraceLine_t						PM_TraceLine;
	pfnDst_CL_LoadModel_t						CL_LoadModel;
	pfnDst_CL_CreateVisibleEntity_t				CL_CreateVisibleEntity;
	pfnDst_GetSpritePointer_t					GetSpritePointer;
	pfnDst_PlaySoundByNameAtLocation_t			pfnPlaySoundByNameAtLocation;
	pfnDst_PrecacheEvent_t						pfnPrecacheEvent;
	pfnDst_PlaybackEvent_t						pfnPlaybackEvent;
	pfnDst_WeaponAnim_t							pfnWeaponAnim;
	pfnDst_RandomFloat_t						pfnRandomFloat;
	pfnDst_RandomLong_t							pfnRandomLong;
	pfnDst_HookEvent_t							pfnHookEvent;
	pfnDst_Con_IsVisible_t						Con_IsVisible;
	pfnDst_GetGameDirectory_t					pfnGetGameDirectory;
	pfnDst_GetCvarPointer_t						pfnGetCvarPointer;
	pfnDst_Key_LookupBinding_t					Key_LookupBinding;
	pfnDst_GetLevelName_t						pfnGetLevelName;
	pfnDst_GetScreenFade_t						pfnGetScreenFade;
	pfnDst_SetScreenFade_t						pfnSetScreenFade;
	pfnDst_VGui_GetPanel_t						VGui_GetPanel;
	pfnDst_VGui_ViewportPaintBackground_t		VGui_ViewportPaintBackground;
	pfnDst_COM_LoadFile_t						COM_LoadFile;
	pfnDst_COM_ParseFile_t						COM_ParseFile;
	pfnDst_COM_FreeFile_t						COM_FreeFile;
	pDst_TriAPI									pTriAPI;
	pDst_EfxAPI									pEfxAPI;
	pDst_EventAPI								pEventAPI;
	pDst_DemoAPI								pDemoAPI;
	pDst_NetAPI									pNetAPI;
	pDst_VoiceTweak								pVoiceTweak;
	pfnDst_IsSpectateOnly_t						IsSpectateOnly;
	pfnDst_LoadMapSprite_t						LoadMapSprite;
	pfnDst_COM_AddAppDirectoryToSearchPath_t	COM_AddAppDirectoryToSearchPath;
	pfnDst_COM_ExpandFilename_t					COM_ExpandFilename;
	pfnDst_PlayerInfo_ValueForKey_t				PlayerInfo_ValueForKey;
	pfnDst_PlayerInfo_SetValueForKey_t			PlayerInfo_SetValueForKey;
	pfnDst_GetPlayerUniqueID_t					GetPlayerUniqueID;
	pfnDst_GetTrackerIDForPlayer_t				GetTrackerIDForPlayer;
	pfnDst_GetPlayerForTrackerID_t				GetPlayerForTrackerID;
	pfnDst_ServerCmdUnreliable_t				pfnServerCmdUnreliable;
	pfnDst_GetMousePos_t						pfnGetMousePos;
	pfnDst_SetMousePos_t						pfnSetMousePos;
	pfnDst_SetMouseEnable_t						pfnSetMouseEnable;
	pfnDst_SetFilterMode_t						pfnSetFilterMode;
	pfnDst_SetFilterColor_t						pfnSetFilterColor;
	pfnDst_SetFilterBrightness_t				pfnSetFilterBrightness;
	pfnDst_SequenceGet_t						pfnSequenceGet;
	pfnDst_SPR_DrawGeneric_t					pfnSPR_DrawGeneric;
	pfnDst_SequencePickSentence_t				pfnSequencePickSentence;
	pfnDst_DrawString_t							pfnDrawString;
	pfnDst_DrawString_t							pfnDrawStringReverse;
	pfnDst_LocalPlayerInfo_ValueForKey_t		LocalPlayerInfo_ValueForKey;
	pfnDst_VGUI2DrawCharacter_t					pfnVGUI2DrawCharacter;
	pfnDst_VGUI2DrawCharacterAdd_t				pfnVGUI2DrawCharacterAdd;
	pfnDst_PlaySoundVoiceByName_t				pfnPlaySoundVoiceByName;
	pfnDst_PrimeMusicStream_t					pfnPrimeMusicStream;
	pfnDst_ProcessTutorMessageDecayBuffer_t		pfnProcessTutorMessageDecayBuffer;
	pfnDst_ConstructTutorMessageDecayBuffer_t	pfnConstructTutorMessageDecayBuffer;
	pfnDst_ResetTutorMessageDecayData_t			pfnResetTutorMessageDecayData;
	pfnDst_PlaySoundByNameAtPitch_t				pfnPlaySoundByNameAtPitch;
	pfnDst_FillRGBABlend_t						pfnFillRGBABlend;
	pfnDst_GetAppID_t							pfnGetAppID;
	pfnDst_GetAliases_t							pfnGetAliasList;
	pfnDst_VguiWrap2_GetMouseDelta_t			pfnVguiWrap2_GetMouseDelta;
	pfnDst_FilteredClientCmd_t					pfnFilteredClientCmd;
};

//-----------------------------------------------------------------------------
//
// Functions exposed by the engine to the module
//
//-----------------------------------------------------------------------------

typedef void (*pfnModHlprs_KickPlayer) (int nPlayerSlot, int nReason);

//-----------------------------------------------------------------------------
// Purpose: Functions for ModuleS
//-----------------------------------------------------------------------------
struct modshelpers_t
{
	pfnModHlprs_KickPlayer m_pfnKickPlayer;

	// Reserved for future expansion
	int m_nVoid1;
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
};

//-----------------------------------------------------------------------------
// Purpose: Functions for moduleC
//-----------------------------------------------------------------------------
struct modchelpers_t
{
	// Reserved for future expansion
	int m_nVoid0;
	int m_nVoid1;
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
};

typedef void		(*pfnEngData_ConPrintf_t)			(char* fmt, ...);
typedef void*		(*pfnEngData_SZ_GetSpace_t)			(sizebuf_t* buf, int length);
typedef void		(*pfnEngData_SV_DropClient_t)		(void* cl, qboolean crash, const char* pszFormat, ...);
typedef void		(*pfnEngData_Netchan_Transmit_t)	(netchan_t* chan, int length, byte* data);
typedef void		(*pfnEngData_NET_SendPacket_t)		(netsrc_t sock, int length, void* data, const netadr_t& to);
typedef cvar_t*		(*pfnEngData_CvarFindVar_t)			(char* pchName);
typedef void*		pfnEngData_Sys_GetProcAddress_t;
typedef void*		pfnEngData_GetModuleHandle_t;

//-----------------------------------------------------------------------------
// Purpose: Information about the engine
//-----------------------------------------------------------------------------
struct engdata_t
{
	cl_enginefunc_t*				pcl_enginefuncs;		// functions exported by the engine
	cl_enginefunc_dst_t*			pg_engdstAddrs;			// destination handlers for engine exports

	cldll_func_t*					pcl_funcs;				// client exports
	cldll_func_dst_t*				pg_cldstAddrs;			// client export destination handlers

	modfuncs_t*						pg_modfuncs;			// engine's pointer to module functions
	cmd_function_t**				pcmd_functions;			// list of all registered commands

	void*							pkeybindings;			// all key bindings (not really a void *, but easier this way)

	pfnEngData_ConPrintf_t			pfnConPrintf;			// dump to console

	cvar_t**						pcvar_vars;				// pointer to head of cvar list

	struct glwstate_t*				pglwstate;				// OpenGl information

	pfnEngData_SZ_GetSpace_t		pfnSZ_GetSpace;			// pointer to SZ_GetSpace

	modfuncs_t*						pmodfuncs;				// &g_modfuncs

	pfnEngData_Sys_GetProcAddress_t	pfnGetProcAddress;		// &Sys_GetProcAddress
	pfnEngData_GetModuleHandle_t	pfnGetModuleHandle;		// &GetModuleHandle

	void*							psvs;					// &svs
	void*							pcls;					// &cls

	pfnEngData_SV_DropClient_t		pfnSV_DropClient;		// pointer to SV_DropClient
	pfnEngData_Netchan_Transmit_t	pfnNetchan_Transmit;	// pointer to Netchan_Transmit
	pfnEngData_NET_SendPacket_t		pfnNET_SendPacket;		// &NET_SendPacket
	pfnEngData_CvarFindVar_t		pfnCvarFindVar;			// pointer to Cvar_FindVar

	int*							phinstOpenGlEarly;		// &g_hinstOpenGlEarly

	// Reserved for future expansion
	void*							pVoid0;
	void*							pVoid1;
	void*							pVoid2;
	void*							pVoid3;
	void*							pVoid4;
	void*							pVoid5;
	void*							pVoid6;
	void*							pVoid7;
	void*							pVoid8;
	void*							pVoid9;
};

//-----------------------------------------------------------------------------
//
// Functions exposed by the security module
//
//-----------------------------------------------------------------------------

typedef void	(*pfnModFuncs_LoadMod)				(char* pchModule);
typedef void	(*pfnModFuncs_CloseMod)				();
typedef int		(*pfnModFuncs_NCall)				(int ijump, int cnArg, ...);

typedef void	(*pfnModFuncs_GetClDstAddrs)		(cldll_func_dst_t* pcldstAddrs);
typedef void	(*pfnModFuncs_GetEngDstAddrs)		(cl_enginefunc_dst_t* pengdstAddrs);
typedef void	(*pfnModFuncs_ModuleLoaded)			();

typedef void	(*pfnModFuncs_ProcessOutgoingNet)	(netchan_t* pchan, sizebuf_t* psizebuf);
typedef qboolean(*pfnModFuncs_ProcessIncomingNet)	(netchan_t* pchan, sizebuf_t* psizebuf);

typedef void	(*pfnModFuncs_TextureLoad)			(char* pszName, int dxWidth, int dyHeight, char* pbData);
typedef void	(*pfnModFuncs_ModelLoad)			(model_t* pmodel, void* pvBuf);

typedef void	(*pfnModFuncs_FrameBegin)			();
typedef void	(*pfnModFuncs_FrameRender1)			();
typedef void	(*pfnModFuncs_FrameRender2)			();

typedef void	(*pfnModFuncs_SetModsHelpers)		(modshelpers_t* pmodshelpers);
typedef void	(*pfnModFuncs_SetModcHelpers)		(modchelpers_t* pmodchelpers);
typedef void	(*pfnModFuncs_SetEngData)			(engdata_t* pengdata);

typedef void	(*pfnModFuncs_ConnectClient)		(int iPlayer);
typedef void	(*pfnModFuncs_RecordIp)				(unsigned int pnIP);
typedef void	(*pfnModFuncs_PlayerStatus)			(unsigned char* pbData, int cbData);

typedef void	(*pfnModFuncs_SetEngineVersion)		(int nVersion);

typedef int		(*pfnModFuncs_PCMachine)			();
typedef void	(*pfnModFuncs_SetIp)				(int ijump);
typedef void	(*pfnModFuncs_Execute)				();

struct modfuncs_t
{
	// Functions for the pcode interpreter
	pfnModFuncs_LoadMod				m_pfnLoadMod;
	pfnModFuncs_CloseMod			m_pfnCloseMod;
	pfnModFuncs_NCall				m_pfnNCall;

	// API destination functions
	pfnModFuncs_GetClDstAddrs		m_pfnGetClDstAddrs;
	pfnModFuncs_GetEngDstAddrs		m_pfnGetEngDstAddrs;

	// Miscellaneous functions
	pfnModFuncs_ModuleLoaded		m_pfnModuleLoaded;   // Called right after the module is loaded

	// Functions for processing network traffic
	pfnModFuncs_ProcessOutgoingNet	m_pfnProcessOutgoingNet; // Every outgoing packet gets run through this
	pfnModFuncs_ProcessIncomingNet	m_pfnProcessIncomingNet; // Every incoming packet gets run through this

	// Resource functions
	pfnModFuncs_TextureLoad			m_pfnTextureLoad;    // Called as each texture is loaded
	pfnModFuncs_ModelLoad			m_pfnModelLoad;      // Called as each model is loaded

	// Functions called every frame
	pfnModFuncs_FrameBegin			m_pfnFrameBegin;     // Called at the beginning of each frame cycle
	pfnModFuncs_FrameRender1		m_pfnFrameRender1;   // Called at the beginning of the render loop
	pfnModFuncs_FrameRender2		m_pfnFrameRender2;   // Called at the end of the render loop

	// Module helper transfer
	pfnModFuncs_SetModsHelpers		m_pfnSetModSHelpers;
	pfnModFuncs_SetModcHelpers		m_pfnSetModCHelpers;
	pfnModFuncs_SetEngData			m_pfnSetEngData;

	// Which version of the module is this?
	int								m_nVersion;

	// Miscellaneous game stuff
	pfnModFuncs_ConnectClient		m_pfnConnectClient;	// Called whenever a new client connects
	pfnModFuncs_RecordIp			m_pfnRecordIP;		// Secure master has reported a new IP for us
	pfnModFuncs_PlayerStatus		m_pfnPlayerStatus;	// Called whenever we receive a PlayerStatus packet

	// Recent additions
	pfnModFuncs_SetEngineVersion	m_pfnSetEngineVersion;	// 1 = patched engine

	// reserved for future expansion
	int m_nVoid2;
	int m_nVoid3;
	int m_nVoid4;
	int m_nVoid5;
	int m_nVoid6;
	int m_nVoid7;
	int m_nVoid8;
	int m_nVoid9;
};

#define k_nEngineVersion15Base		0
#define k_nEngineVersion15Patch		1
#define k_nEngineVersion16Base		2
#define k_nEngineVersion16Validated	3 // 1.6 engine with built-in validation

struct validator_t
{
	int m_nRandomizer;			// Random number to be XOR'd into all subsequent fields
	int m_nSignature1;			// First signature that identifies this structure
	int m_nSignature2;			// Second signature
	int m_pbCode;				// Beginning of the code block
	int m_cbCode;				// Size of the code block
	int m_nChecksum;			// Checksum of the code block
	int m_nSpecial;				// For engine, 1 if hw.dll, 0 if sw.dll.  For client, pclfuncs checksum
	int m_nCompensator;			// Keeps the checksum correct
};

#define k_nChecksumCompensator		0x36A8F09C // Don't change this value: it's hardcorded in cdll_int.cpp, 

#define k_nModuleVersionCur			0x43210004

// These are used within shared bot, client, and server code.
//
// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL

#endif // APIPROXY_H