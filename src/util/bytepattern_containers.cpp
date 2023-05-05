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

//
// bytepattern_containers.cpp -- Defines byte patterns for various engine/cs builds.
//

#include "precompiled.h"

static const BytePatternContainer bytepattern_conainter_8684 =
{
	{
		// MemoryHook
		{ "cl_funcs"					,{ "hw.dll",		"ClientDLL_Init", { "\xFF\x15\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x1C", 0x2 } } },
		{ "cl_enginefuncs"				,{ "hw.dll",		"ClientDLL_Init", { "\x68\xCC\xCC\xCC\xCC\xFF\x15\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x1C", 0x1} } },
		{ "pmainwindow"					,{ "hw.dll",		"Sys_InitGame",  { "\x8B\x15\xCC\xCC\xCC\xCC\x50\x68\xCC\xCC\xCC\xCC\x51\x8B\x02\x68\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\x50", 0x2 } } },
		{ "host_initialized"			,{ "hw.dll",		"Host_Shutdown", { "\xA1\xCC\xCC\xCC\xCC\x53\x33\xDB\x3B\xC3\x74\x76", 0x1} } },
		{ "sv_player"					,{ "hw.dll",		"Host_God_f", { "\xA1\xCC\xCC\xCC\xCC\x8B\x90\x24\x02\xCC\xCC\x83\xF2\x40", 0x1} } },
		{ "cl"							,{ "hw.dll",		"CL_ClearClientState", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xB8\xCC\xCC\xCC\xCC\x83\xC4\x0C", 0x1} } },
		{ "cls"							,{ "hw.dll",		"CL_ConnectClient", { "\xA1\xCC\xCC\xCC\xCC\x83\xC4\x0C\x83\xF8\x03", 0x1 }}},
		{ "gGlobalVariables"			,{ "hw.dll",		"SaveInit", { "\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x89\x06", 0x2} } },
		{ "scr_fov_value"				,{ "hw.dll",		"ClientDLL_UpdateClientData", { "\x8B\x0D\xCC\xCC\xCC\xCC\x89\x55\xE0", 0x2 }}},
		{ "g_PlayerExtraInfo"			,{ "client.dll",	"CounterStrikeViewport::MsgFunc_ScoreInfo", { "\x66\x89\x99\xCC\xCC\xCC\xCC\x66\x89\xA9\xCC\xCC\xCC\xCC\x66\x89\x91\xCC\xCC\xCC\xCC\x66\x89\x81\xCC\xCC\xCC\xCC\x7D\x09\x66\xC7\x81", 0x3} } },
		{ "engine_studio_api"			,{ "hw.dll",		"ClientDLL_CheckStudioInterface", {"\x68\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\x6A\x01\xFF\xD0\x83\xC4\x0C", 0x1}}},
		{ "cl_parsefuncs"				,{ "hw.dll",		"CL_ParseServerMessage", { "\x8B\xB9\xCC\xCC\xCC\xCC\x85\xFF\x74\x29", 0x2 }}},
		{ "pmove"						,{ "hw.dll",		"CL_PlayerFlashlight", { "\x8B\x15\xCC\xCC\xCC\xCC\x8D\x0C\xC5\x00\x00\x00\x00\x2B\xC8\xC1\xE1\x05\x8D\x04\x11\x8B\x8C\x11\x84\x02", 0x2} } },
		{ "gClientUserMsgs"				,{ "hw.dll",		"AddNewUserMsg", { "\x8B\x35\xCC\xCC\xCC\xCC\x83\xC4\x30", 0x2} } },
		{ "g_iShotsFired"				,{ "client.dll",	"CHudAmmo::DrawCrosshair", { "\xA1\xCC\xCC\xCC\xCC\x3D\x58\x02\x00\x00", 0x1} } },
		{ "r_model"						,{ "hw.dll",		"R_GLStudioDrawPoints", { "\xA1\xCC\xCC\xCC\xCC\x50\xC7\x45\xD8\x00\x00\x00\x00", 0x1} } },
		{ "pstudiohdr"					,{ "hw.dll",		"R_StudioHull", { "\xA3\xCC\xCC\xCC\xCC\x8B\x43\x04\xD9\x03\x89\x55\x90\x8B\x55\x24\x89\x45\x8C", 0x1} } },
		{ "pStudioAPI"					,{ "hw.dll",		"R_DrawViewModel", { "\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x35\xCC\xCC\xCC\xCC\xD1\xEA", 0x2} } },

		// MemoryFnHook
		{ "VGuiWrap2_IsConsoleVisible"	,{ "hw.dll",		"VGuiWrap2_IsConsoleVisible", { "\x8B\x0D\xCC\xCC\xCC\xCC\x85\xC9\x74\x0B\x8B\x01\xFF\x50\x14\x25"} } },
		{ "VGuiWrap2_ConPrintf"			,{ "hw.dll",		"VGuiWrap2_ConPrintf", { "\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x14\x8B\x55\x08\x8B\x08\x52\x68\xCC\xCC\xCC\xCC\x50\xFF\x51\x1C\x83\xC4\x0C\x5D\xC3\x53\x56\x8B\x75\x08\x57\x8B\xFE\x83\xC9\xFF\x33\xC0\xF2\xAE\xA1\xCC\xCC\xCC\xCC\xF7\xD1\x49\x8B\xD8\x8B\xF9\x74\x49\x8B"} } },
		{ "ClearIOStates"				,{ "hw.dll",		"ClearIOStates", { "\x56\x33\xF6\x6A\x00\x56\xE8\xCC\xCC\xCC\xCC\x83\xC4\x08\x46\x81"} } },

		// MemoryFnDetour
		{ "VGui_CallEngineSurfaceAppHandler" ,{ "hw.dll",	"VGui_CallEngineSurfaceAppHandler", { "\x55\x8B\xEC\x56\x8B\x75\x0C\x57\x8B\x7D\x08\x56"} } },
		{ "Key_Event"					,{ "hw.dll",		"Key_Event", { "\x55\x8B\xEC\x81\xEC\x00\x04\x00\x00\x8B\x45\x08\x56\x3D\x00\x01\x00\x00"} }},
		{ "Host_Noclip_f"				,{ "hw.dll",		"Host_Noclip_f", { "\x55\x8B\xEC\x83\xEC\x24\xA1\xCC\xCC\xCC\xCC\xBA\x01\x00\x00\x00"} } },
		{ "ClientDLL_CreateMove"		,{ "hw.dll",		"ClientDLL_CreateMove", { "\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x11\x8B\x4D\x10"} } },
		{ "_Host_Frame"					,{ "hw.dll",		"_Host_Frame", { "\x55\x8B\xEC\x6A\x00\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x08"} } },
		{ "CL_ReallocateDynamicData"	,{ "hw.dll",		"CL_ReallocateDynamicData", { "\x55\x8B\xEC\x8B\x45\x08\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\xA3"} } },
		{ "CL_DeallocateDynamicData"	,{ "hw.dll",		"CL_DeallocateDynamicData", { "\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x13\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xE9" } } },
		{ "MYgluPerspective"			,{ "hw.dll",		"MYgluPerspective", { "\x55\x8B\xEC\x83\xEC\x10\xDD\x45\x08"} } },
		{ "R_ForceCVars"				,{ "hw.dll",		"R_ForceCVars", { "\x55\x8B\xEC\x8B\x45\x08\x85\xC0\x0F\x84\x92\x02\x00\x00"} } },
		{ "V_CalcRefdef"				,{ "client.dll",	"V_CalcRefdef", { "\x8B\x44\x24\x04\x8B\x48\x44"} } },
		{ "EV_HLDM_FireBullets"			,{ "client.dll",	"EV_HLDM_FireBullets", { "\x81\xEC\x08\x01\x00\x00\xB8\x28\x00\x00\x00"} } },
		{ "HUD_Redraw"					,{ "client.dll",	"HUD_Redraw", { "\x8B\x44\x24\x08\x8B\x4C\x24\x04\x50\x51\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xB8\x01\x00\x00\x00"} } },
		{ "R_GLStudioDrawPoints"		,{ "hw.dll",		"R_GLStudioDrawPoints", { "\x55\x8B\xEC\x83\xEC\x48\xA1\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x53\x56\x8B\x70\x54"} } },
		{ "R_LightLambert"				,{ "hw.dll",		"R_LightLambert", { "\x55\x8B\xEC\x83\xEC\x24\x8B\x0D\xCC\xCC\xCC\xCC\x56"} } },
		{ "V_FadeAlpha"					,{ "hw.dll",		"V_FadeAlpha", { "\x55\x8B\xEC\x83\xEC\x08\xD9\x05\xCC\xCC\xCC\xCC\xDC\x1D"} } },
		{ "V_ApplyShake"				,{ "hw.dll",		"V_ApplyShake", { "\x55\x8B\xEC\x8D\x45\x10\x8D\x4D\x0C\x50\x8D\x55\x08\x51\x52\xFF\x15\xCC\xCC\xCC\xCC\x8B\x45\x08\x83\xC4\x0C"} } },
		{ "S_StartDynamicSound"			,{ "hw.dll",		"S_StartDynamicSound", { "\x55\x8B\xEC\x83\xEC\x48\xA1\xCC\xCC\xCC\xCC\x53\x56\x57\x85\xC0\xC7\x45\xFC\x00\x00\x00\x00\x0F\x84\x89\x02\x00\x00" } } },
		{ "R_DrawViewModel"				,{ "hw.dll",		"R_DrawViewModel", { "\x55\x8B\xEC\x83\xEC\x50\xD9\x05"} } },
		{ "CPartSmokeGrenade::Create"	,{ "client.dll",	"CPartSmokeGrenade::Create", { "\x8B\x0D\xCC\xCC\xCC\xCC\x56\x57\x68\x30\x01\x00\x00"} } },
		{ "CreateGasSmoke"				,{ "client.dll",	"CreateGasSmoke", { "\x51\xA1\xCC\xCC\xCC\xCC\x53\x33\xDB"} } },
		{ "CEngine::Unload"				,{ "hw.dll",		"CEngine::Unload", { "\x56\x8B\xF1\xE8\xCC\xCC\xCC\xCC\xC7\x46\x08\x00\x00\x00\x00"} } },
		{ "SCR_CalcRefdef"				,{ "hw.dll",		"SCR_CalcRefdef", { "\x55\x8B\xEC\x83\xEC\x0C\xD9\x05\xCC\xCC\xCC\xCC\xD8\x1D\xCC\xCC\xCC\xCC\x33\xC9"} } },
		{ "SCR_UpdateScreen"			,{ "hw.dll",		"SCR_UpdateScreen", { "\x55\x8B\xEC\x83\xEC\x10\xA1\xCC\xCC\xCC\xCC\x56\x33\xF6\x3B\xC6\x0F\x85\x10\x02\x00\x00"} } },
		{ "SPR_Set"						,{ "hw.dll",		"SPR_Set", { "\x55\x8B\xEC\x83\xEC\x08\x8D\x45\x14"} } },
		{ "CGame::AppActivate"			,{ "hw.dll",		"CGame::AppActivate", { "\x55\x8B\xEC\x51\x53\x8B\x5D\x08\x56\x8B\xF1\x84\xDB"} } },
		{ "CHudAmmo::DrawCrosshair"		,{ "client.dll",	"CHudAmmo::DrawCrosshair", { "\x83\xEC\x08\x8B\x44\x24\x10\x53\x55\x56\x57"} } },
		{ "R_StudioDrawPlayer"			,{ "hw.dll",		"R_StudioDrawPlayer", { "\x55\x8B\xEC\x81\xEC\xDC\x0B\x00\x00\x53\x8B\x5D\x0C\x56\x57\x8B\x4B\x04\x49"} } },

		// HLNetMessageIO
		{ "MSG_StartBitReading"			,{ "hw.dll",		"MSG_StartBitReading", { "\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x33\xC9\x89"} } },
		{ "MSG_EndBitReading"			,{ "hw.dll",		"MSG_EndBitReading", { "\x55\x8B\xEC\x8B\x4D\x08\xA1\xCC\xCC\xCC\xCC\x3B"} } },
		{ "MSG_ReadBits"				,{ "hw.dll",		"MSG_ReadBits", { "\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x07\xB8"} } },
		{ "MSG_ReadSBits"				,{ "hw.dll",		"MSG_ReadSBits", { "\x55\x8B\xEC\x56\xE8\xCC\xCC\xCC\xCC\x8B\xF0\x8B"} } },
		{ "MSG_ReadBitCoord"			,{ "hw.dll",		"MSG_ReadBitCoord", { "\x55\x8B\xEC\x83\xEC\x10\x56\x57\xC7"} } },
		{ "MSG_ReadBitVec3Coord"		,{ "hw.dll",		"MSG_ReadBitVec3Coord", { "\x55\x8B\xEC\x53\x56\x57\xE8\xCC\xCC\xCC\xCC\x8B\xF0"} } },
		{ "MSG_ReadChar"				,{ "hw.dll",		"MSG_ReadChar", { "\xA1\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x8D\x48\x01\x3B\xCA\x7E\x0E\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x83\xC8\xFF\xC3\x8B\x15"} } },
		{ "MSG_ReadByte"				,{ "hw.dll",		"MSG_ReadByte", { "\xA1\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x8D\x48\x01\x3B\xCA\x7E\x0E\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x83\xC8\xFF\xC3\x56"} } },
		{ "MSG_ReadShort"				,{ "hw.dll",		"MSG_ReadShort", { "\xA1\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x8D\x50\x02"} } },
		{ "MSG_ReadWord"				,{ "hw.dll",		"MSG_ReadWord", { "\x8B\x0D\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\x57\x8D\x79\x02"} } },
		{ "MSG_ReadLong"				,{ "hw.dll",		"MSG_ReadLong", { "\x8B\x0D\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\x57\x8D\x79\x04"} } },
		{ "MSG_ReadFloat"				,{ "hw.dll",		"MSG_ReadFloat", { "\x55\x8B\xEC\x51\xA1\xCC\xCC\xCC\xCC\x8B\x0D"} } },
		{ "MSG_ReadString"				,{ "hw.dll",		"MSG_ReadString", { "\x53\x56\x33\xDB\x33\xF6\xE8\xCC\xCC\xCC\xCC\x83\xF8\xFF\x74\x13"} } },
		{ "MSG_ReadCoord"				,{ "hw.dll",		"MSG_ReadCoord", { "\x55\x8B\xEC\x83\xEC\x08\xE8\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x85\xC9\x74\x0A"} } },
		{ "MSG_WriteChar"				,{ "hw.dll",		"MSG_WriteChar", { "\x55\x8B\xEC\x8B\x45\x08\x6A\x01\x50\xE8\xCC\xCC\xCC\xCC\x8A\x4D\x0C\x83\xC4\x08\x88\x08\x5D\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x55\x8B\xEC\x8B\x45\x08\x6A\x01"} } },
		{ "MSG_WriteByte"				,{ "hw.dll",		"MSG_WriteByte", { "\x55\x8B\xEC\x8B\x45\x08\x6A\x01\x50\xE8\xCC\xCC\xCC\xCC\x8A\x4D\x0C\x83\xC4\x08\x88\x08\x5D\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x55\x8B\xEC\x8B\x45\x08\x6A\x02"} } },
		{ "MSG_WriteShort"				,{ "hw.dll",		"MSG_WriteShort", { "\x55\x8B\xEC\x8B\x45\x08\x6A\x02\x50\xE8\xCC\xCC\xCC\xCC\x8B\x4D\x0C\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\x5D\xC3\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x02"} } },
		{ "MSG_WriteWord"				,{ "hw.dll",		"MSG_WriteWord", { "\x55\x8B\xEC\x8B\x45\x08\x6A\x02\x50\xE8\xCC\xCC\xCC\xCC\x8B\x4D\x0C\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\x5D\xC3\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x04"} } },
		{ "MSG_WriteLong"				,{ "hw.dll",		"MSG_WriteLong", { "\x55\x8B\xEC\x8B\x45\x08\x6A\x04"} } },
		{ "MSG_WriteFloat"				,{ "hw.dll",		"MSG_WriteFloat", { "\x55\x8B\xEC\x8B\x45\x0C\x50\x89"} } },
		{ "MSG_WriteString"				,{ "hw.dll",		"MSG_WriteString", { "\x55\x8B\xEC\x56\x8B\x75\x0C\x85\xF6\x75"} } },
		{ "MSG_WriteCoord"				,{ "hw.dll",		"MSG_WriteCoord", { "\x55\x8B\xEC\xD9\x45\x0C\xD8"} } },
		{ "net_message"					,{ "hw.dll",		"CL_FlushEntityPacket", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\x6A\x10", 0x1} } },
		{ "msg_readcount"				,{ "hw.dll",		"CL_ParseServerMessage", { "\xA1\xCC\xCC\xCC\xCC\x89\x45\xFC\xA1", 0x1} } },
		{ "bfread"						,{ "hw.dll",		"COM_Init", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x18\xC3\x55", 0x1} } },

		// MemoryHookCBaseStuff
		{ "ClientWeapons"				,{ "client.dll",	"ClientWeapons", { "\x89\x1C\x85\xCC\xCC\xCC\xCC\x5F", 0x3} } },

	}
};

static const BytePatternContainer bytepattern_conainter_4554 =
{
	{
		// MemoryHook
		{ "cl_funcs"					,{ "hw.dll",		"ClientDLL_Init", { "\xFF\x15\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x1C", 0x2 } } },
		{ "cl_enginefuncs"				,{ "hw.dll",		"ClientDLL_Init", { "\x68\xCC\xCC\xCC\xCC\xFF\x15\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x1C", 0x1 } } },
		{ "pmainwindow"					,{ "hw.dll",		"Sys_InitGame", { "\xA3\xCC\xCC\xCC\xCC\x33\xC0\x89\x0D\xCC\xCC\xCC\xCC\x57\xB9\x8F\x00\x00\x00\xBF", 0x1 } } },
		{ "host_initialized"			,{ "hw.dll",		"Host_Shutdown", { "\xA1\xCC\xCC\xCC\xCC\x53\x33\xDB\x3B\xC3\x74\x76", 0x1 } } },
		{ "sv_player"					,{ "hw.dll",		"Host_God_f", { "\xA1\xCC\xCC\xCC\xCC\x8B\x90\x24\x02\xCC\xCC\x83\xF2\x40", 0x1 } } },
		{ "cl"							,{ "hw.dll",		"CL_ClearClientState", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xB8\xCC\xCC\xCC\xCC\x83\xC4\x0C", 0x1 } } },
		{ "cls"							,{ "hw.dll",		"CL_ConnectClient", { "\xA1\xCC\xCC\xCC\xCC\x83\xC4\x0C\x83\xF8\x03", 0x1 } } },
		{ "gGlobalVariables"			,{ "hw.dll",		"Host_Kill_f", { "\xD9\x1D\xCC\xCC\xCC\xCC\xFF\x15\xCC\xCC\xCC\xCC\x59\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xA1", 0x2 } } },
		{ "scr_fov_value"				,{ "hw.dll",		"ClientDLL_UpdateClientData", { "\x8B\x0D\xCC\xCC\xCC\xCC\x89\x54\x24\x08\x8B\x80\x50\x0B\x00\x00\x8B\x15", 0x2 } } },
		{ "g_PlayerExtraInfoOld"		,{ "client.dll",	"CCSClientScoreBoardDialog::GetPlayerScoreInfo", { "\x0F\xBF\x87\xCC\xCC\xCC\xCC\x8B\x16\x50\x68\xCC\xCC\xCC\xCC\x8B\xCE\xFF\x52\xCC\x8B\xCD\xC1\xE1\x05\x66\x8B\x81\xCC\xCC\xCC\xCC\x66\x3D\x01\x00\x7D\x46", 0x3 } } },
		{ "engine_studio_api"			,{ "hw.dll",		"ClientDLL_CheckStudioInterface", {"\x68\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\x6A\x01\xFF\xD0\x83\xC4\x0C", 0x1} } },
		{ "cl_parsefuncs"				,{ "hw.dll",		"CL_ParseServerMessage", { "\x8B\xB9\xCC\xCC\xCC\xCC\x85\xFF\x74\x29", 0x2 } } },
		{ "pmove"						,{ "hw.dll",		"CL_PlayerFlashlight", { "\x8B\x15\xCC\xCC\xCC\xCC\x8D\x0C\xC5\x00\x00\x00\x00\x2B\xC8\xC1\xE1\x05\x8D\x04\x11\x8B\x8C\x11\x84\x02", 0x2 } } },
		{ "gClientUserMsgs"				,{ "hw.dll",		"AddNewUserMsg", { "\x8B\x35\xCC\xCC\xCC\xCC\x83\xC4\x30", 0x2 } } },
		{ "g_iShotsFired"				,{ "client.dll",	"CHudAmmo::DrawCrosshair", { "\xA1\xCC\xCC\xCC\xCC\x3D\x58\x02\x00\x00", 0x1 } } },
		{ "r_model"						,{ "hw.dll",		"R_StudioSetupSkin", { "\x8B\x0D\xCC\xCC\xCC\xCC\x8B\xF0\x83\xC4\x08\x8B\x46\x0C\x3B\xC1\x75\x1E", 0x2 } } },
		{ "pstudiohdr"					,{ "hw.dll",		"BuildGlowShellVerts", { "\xA1\xCC\xCC\xCC\xCC\x8B\x53\x64\x03\xC8\xDD\x5C\x24\x10\xDD\x44\x24\x10\xDC\x0D\xCC\xCC", 0x1 } } },
		{ "pStudioAPI"					,{ "hw.dll",		"ClientDLL_CheckStudioInterface", { "\x68\xCC\xCC\xCC\xCC\x6A\x01\xFF\xD0\x83\xC4\x0C\x85\xC0\x75\x12\x68\xCC\xCC\xCC\xCC", 0x1 } } },

		// MemoryFnHook
		{ "VGuiWrap2_IsConsoleVisible"	,{ "hw.dll",		"VGuiWrap2_IsConsoleVisible", { "\x8B\x0D\xCC\xCC\xCC\xCC\x85\xC9\x74\x0B\x8B\x01\xFF\x50\x14\x25" } } },
		{ "VGuiWrap2_ConPrintf"			,{ "hw.dll",		"VGuiWrap2_ConPrintf", { "\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x14\x8B\x54\x24\x04\x8B\x08\x52\x68\xCC\xCC\xCC\xCC\x50\xFF\x51\xCC\x83\xC4\x0C\xC3\x53\x56\x8B\x74\x24\x0C\x57\x8B\xFE\x83\xC9\xFF\x33\xC0\xF2\xAE\xA1" } } },
		{ "ClearIOStates"				,{ "hw.dll",		"ClearIOStates", { "\x56\x33\xF6\x6A\x00\x56\xE8\xCC\xCC\xCC\xCC\x83\xC4\x08\x46\x81" } } },

		// MemoryFnDetour
		// this function declaration differs on 4554
		{ "VGui_CallEngineSurfaceAppHandler4554",{ "hw.dll","VGui_CallEngineSurfaceAppHandler4554", { "\x53\x8B\x5C\x24\x0C\x55\x8B\x6C\x24\x0C\x56\x8B\x74\x24\x1C\x57\x8B\x7C\x24\x1C\x56\x57\x53\x55" } } },
		{ "Key_Event"					,{ "hw.dll",		"Key_Event", { "\x81\xEC\x00\x04\x00\x00\x8D\x84\x24\x08\x04\x00\x00\x8D\x8C\x24\x04\x04\x00\x00\x56" } } },
		{ "Host_Noclip_f"				,{ "hw.dll",		"Host_Noclip_f", { "\xA1\xCC\xCC\xCC\xCC\xBA\x01\x00\x00\x00\x83" } } },
		{ "ClientDLL_CreateMove"		,{ "hw.dll",		"ClientDLL_CreateMove", { "\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x14\x8B\x4C\x24\x0C\x8B\x54\x24\x08\x51\x8B\x4C\x24\x08\x52\x51" } } },
		{ "_Host_Frame"					,{ "hw.dll",		"_Host_Frame", { "\x55\x8B\xEC\x6A\x00\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x08" } } },
		{ "CL_ReallocateDynamicData"	,{ "hw.dll",		"CL_ReallocateDynamicData", { "\x8B\x44\x24\x04\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\xA3\xCC\xCC\xCC\xCC\x85\xC0\x7F\x12\x68" } } },
		{ "CL_DeallocateDynamicData"	,{ "hw.dll",		"CL_DeallocateDynamicData", { "\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x13\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\xC7\x05" } } },
		{ "MYgluPerspective"			,{ "hw.dll",		"MYgluPerspective", { "\x83\xEC\x10\xDD\x44\x24\x14\xDC\x0D\xCC\xCC\xCC\xCC\x83\xEC\x08\xDC\x35\xCC\xCC\xCC\xCC" } } },
		{ "R_ForceCVars"				,{ "hw.dll",		"R_ForceCVars", { "\x8B\x44\x24\x04\x85\xC0\x0F\x84\xCC\xCC\xCC\xCC\xD9" } } },
		{ "V_CalcRefdef"				,{ "client.dll",	"V_CalcRefdef", { "\x8B\x44\x24\x04\x8B\x48\x44" } } },
		{ "EV_HLDM_FireBullets"			,{ "client.dll",	"EV_HLDM_FireBullets", { "\x81\xEC\x08\x01\x00\x00\xB8\x28\x00\x00\x00" } } },
		{ "HUD_Redraw"					,{ "client.dll",	"HUD_Redraw", { "\x8B\x44\x24\x08\x8B\x4C\x24\x04\x50\x51\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xB8\x01\x00\x00\x00" } } },
		{ "R_GLStudioDrawPoints"		,{ "hw.dll",		"R_GLStudioDrawPoints", { "\x83\xEC\x48\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x53\x55\x8B\x41\x54" } } },
		{ "R_LightLambert"				,{ "hw.dll",		"R_LightLambert", { "\x8B\x0D\xCC\xCC\xCC\xCC\x83\xEC\x24\x53\x33\xDB\x56\x3B\xCB\x57\x75\x1F" } } },
		{ "V_FadeAlpha"					,{ "hw.dll",		"V_FadeAlpha", { "\xD9\x05\xCC\xCC\xCC\xCC\xDC\x1D\xCC\xCC\xCC\xCC\x8A\x0D\xCC\xCC\xCC\xCC\x83\xEC\x08\xDF\xE0\xF6\xC4\x01" } } },
		{ "V_ApplyShake"				,{ "hw.dll",		"V_ApplyShake", { "\x8D\x44\x24\x0C\x8D\x4C\x24\x08\x50\x8D\x54\x24\x08\x51\x52\xFF\x15\xCC\xCC\xCC\xCC\x8B\x44\x24\x10\x83\xC4\x0C\x85\xC0" } } },
		{ "S_StartDynamicSound"			,{ "hw.dll",		"S_StartDynamicSound", { "\x83\xEC\x48\xA1\xCC\xCC\xCC\xCC\x53\x55\x56\x85\xC0\x57\xC7\x44\x24\x10\x00\x00\x00\x00\x0F\x84\x46\x03\x00\x00" } } },
		{ "R_DrawViewModel"				,{ "hw.dll",		"R_DrawViewModel", { "\x83\xEC\x4C\xD9\x05\xCC\xCC\xCC\xCC\xD8\x1D\xCC\xCC\xCC\xCC\x56\x57\x33\xFF\xC7\x44" } } },
		{ "CPartSmokeGrenade::Create"	,{ "client.dll",	"CPartSmokeGrenade::Create", { "\x8B\x0D\xCC\xCC\xCC\xCC\x56\x68\x30\x01\x00\x00\x8B\x01\xFF\x50\xCC\x8B" } } },
		{ "CreateGasSmoke"				,{ "client.dll",	"CreateGasSmoke", { "\xA1\xCC\xCC\xCC\xCC\x83\xEC\x10\x53\x33\xDB\x3B\xC3\x56\x0F\x84\x76" } } },
		{ "CEngine::Unload"				,{ "hw.dll",		"CEngine::Unload", { "\x56\x8B\xF1\xE8\xCC\xCC\xCC\xCC\xC7\x46\x08\x00\x00\x00\x00" } } },
		{ "SCR_CalcRefdef"				,{ "hw.dll",		"SCR_CalcRefdef", { "\xD9\x05\xCC\xCC\xCC\xCC\xD8\x1D\xCC\xCC\xCC\xCC\x83\xEC\x0C\x33\xC9\x89\x0D" } } },
		{ "SCR_UpdateScreen"			,{ "hw.dll",		"SCR_UpdateScreen", { "\xA1\xCC\xCC\xCC\xCC\x83\xEC\x10\x56\x33\xF6\x3B\xC6\x0F\x85\x13\x02\x00\x00\xC7\x05" } } },
		{ "SPR_Set"						,{ "hw.dll",		"SPR_Set", { "\x55\x8B\xEC\x83\xEC\x08\x8D\x45\x14" } } },
		{ "CGame::AppActivate"			,{ "hw.dll",		"CGame::AppActivate", { "\x53\x8B\x5C\x24\x08\x56\x8B\xF1\x84\xDB\x0F\x84\x8E\x00\x00\x00\x68" } } },
		{ "CHudAmmo::DrawCrosshair"		,{ "client.dll",	"CHudAmmo::DrawCrosshair", { "\x83\xEC\x08\x53\x55\x56\x57\x8B\xF9\xBE\x05\x00\x00\x00\x8B\x4C\x24\x20\x89\x7C\x24\x14\x8D\x69\xFF" } } },
		{ "R_StudioDrawPlayer"			,{ "hw.dll",		"R_StudioDrawPlayer", { "\x55\x8B\xEC\x81\xEC\xDC\x0B\x00\x00\x53\x8B\x5D\x0C\x56\x57\x8B\x4B\x04\x49" } } },

		// HLNetMessageIO
		{ "MSG_StartBitReading"			,{ "hw.dll",		"MSG_StartBitReading", { "\xA1\xCC\xCC\xCC\xCC\x33\xC9\x89\x0D\xCC\xCC\xCC\xCC\x89\x0D\xCC\xCC\xCC\xCC\x8B\x4C\x24\x04" } } },
		{ "MSG_EndBitReading"			,{ "hw.dll",		"MSG_EndBitReading", { "\x8B\x4C\x24\x04\xA1\xCC\xCC\xCC\xCC\x3B\x41\x10\x7E\x0A\xC7\x05" } } },
		{ "MSG_ReadBits"				,{ "hw.dll",		"MSG_ReadBits", { "\xA1\xCC\xCC\xCC\xCC\x85\xC0\x74\x06\xB8\x01\x00\x00\x00\xC3\x53\x56" } } },
		{ "MSG_ReadSBits"				,{ "hw.dll",		"MSG_ReadSBits", { "\x56\xE8\xCC\xCC\xCC\xCC\x8B\xF0\x8B\x44\x24\x08\x48\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\x85\xF6\x5E" } } },
		{ "MSG_ReadBitCoord"			,{ "hw.dll",		"MSG_ReadBitCoord", { "\x83\xEC\x10\x56\x57\xC7\x44\x24\x08\x00\x00\x00\x00\xE8" } } },
		{ "MSG_ReadBitVec3Coord"		,{ "hw.dll",		"MSG_ReadBitVec3Coord", { "\x53\x56\x57\xE8\xCC\xCC\xCC\xCC\x8B\xF0\xE8\xCC\xCC\xCC\xCC\x8B\xF8\xE8" } } },
		{ "MSG_ReadChar"				,{ "hw.dll",		"MSG_ReadChar", { "\xA1\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x8D\x48\x01\x3B\xCA\x7E\x0E\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x83\xC8\xFF\xC3\x8B\x15" } } },
		{ "MSG_ReadByte"				,{ "hw.dll",		"MSG_ReadByte", { "\xA1\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x8D\x48\x01\x3B\xCA\x7E\x0E\xC7\x05\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x83\xC8\xFF\xC3\x56" } } },
		{ "MSG_ReadShort"				,{ "hw.dll",		"MSG_ReadShort", { "\xA1\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x8D\x50\x02" } } },
		{ "MSG_ReadWord"				,{ "hw.dll",		"MSG_ReadWord", { "\x8B\x0D\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\x57\x8D\x79\x02" } } },
		{ "MSG_ReadLong"				,{ "hw.dll",		"MSG_ReadLong", { "\x8B\x0D\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\x57\x8D\x79\x04" } } },
		{ "MSG_ReadFloat"				,{ "hw.dll",		"MSG_ReadFloat", { "\xA1\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x56\x8B\x74\x24\x08\x8D\x0C\x30\x3B\xCA\x7E\x0F" } } },
		{ "MSG_ReadString"				,{ "hw.dll",		"MSG_ReadString", { "\x56\x33\xF6\xE8\xCC\xCC\xCC\xCC\x83\xF8\xFF\x74\x22\x85\xC0\x74\x1E\x83" } } },
		{ "MSG_ReadCoord"				,{ "hw.dll",		"MSG_ReadCoord", { "\x51\xE8\xCC\xCC\xCC\xCC\x89\x44\x24\x00\xDB\x44\x24\x00\xD9\x5C\x24\x00\xD9" } } },
		{ "MSG_WriteChar"				,{ "hw.dll",		"MSG_WriteChar", { "\x8B\x44\x24\x04\x6A\x01\x50\xE8\xCC\xCC\xCC\xCC\x8A\x4C\x24\x10\x83\xC4\x08\x88\x08\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x8B\x44\x24\x04\x6A\x01\x50\xE8" } } },
		{ "MSG_WriteByte"				,{ "hw.dll",		"MSG_WriteByte", { "\x8B\x44\x24\x04\x6A\x01\x50\xE8\xCC\xCC\xCC\xCC\x8A\x4C\x24\x10\x83\xC4\x08\x88\x08\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x8B\x44\x24\x04\x6A\x02\x50\xE8" } } },
		{ "MSG_WriteShort"				,{ "hw.dll",		"MSG_WriteShort", { "\x8B\x44\x24\x04\x6A\x02\x50\xE8\xCC\xCC\xCC\xCC\x8B\x4C\x24\x10\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\xC3" } } },
		{ "MSG_WriteWord"				,{ "hw.dll",		"MSG_WriteWord", { "\x8B\x44\x24\x04\x6A\x02\x50\xE8\xCC\xCC\xCC\xCC\x8B\x4C\x24\x10\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\xC3" } } },
		{ "MSG_WriteLong"				,{ "hw.dll",		"MSG_WriteLong", { "\x8B\x44\x24\x04\x6A\x04\x50\xE8\xCC\xCC\xCC\xCC\x8B\x4C\x24\x10\x83\xC4\x08\x8B\xD1\x88\x08\xC1\xFA\x08\x88\x50\x01\x8B\xD1" } } },
		{ "MSG_WriteFloat"				,{ "hw.dll",		"MSG_WriteFloat", { "\x8B\x44\x24\x08\x50\x89\x44\x24\x0C\xFF\x15\xCC\xCC\xCC\xCC\x8B\x4C\x24\x08\x89\x44\x24\x0C\x8D\x44\x24\x0C" } } },
		{ "MSG_WriteString"				,{ "hw.dll",		"MSG_WriteString", { "\x56\x8B\x74\x24\x0C\x85\xF6\x75\x16\x8B\x44\x24\x08\x6A\x01\x68\xCC\xCC\xCC\xCC\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x0C\x5E" } } },
		{ "MSG_WriteCoord"				,{ "hw.dll",		"MSG_WriteCoord", { "\xD9\x44\x24\x08\xD8\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x50\x8B\x44\x24\x08\x50\xE8\xCC\xCC\xCC\xCC\x83\xC4\x08\xC3" } } },
		{ "net_message"					,{ "hw.dll",		"CL_FlushEntityPacket", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\x6A\x10", 0x1 } } },
		{ "msg_readcount"				,{ "hw.dll",		"CL_ParseServerMessage", { "\x8B\x2D\xCC\xCC\xCC\xCC\x3D\x00\x00\x80\x3F\x56\x75\x15\xA1\xCC\xCC\xCC\xCC\x50", 0x2 } } },
		{ "bfread"						,{ "hw.dll",		"COM_Init", { "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x18\xC3\xA1\xCC\xCC\xCC\xCC\x83\xF8\x08\x7C\x26\xA1\xCC\xCC\xCC\xCC\x6A\x01", 0x1 } } },

		// MemoryHookCBaseStuff
		{ "ClientWeapons"				,{ "client.dll",	"ClientWeapons", { "\x89\x1C\x85\xCC\xCC\xCC\xCC\x5F", 0x3 } } },
	}
};