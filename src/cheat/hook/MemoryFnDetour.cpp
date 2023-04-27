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

bool CMemoryFnDetourMgr::install_hooks()
{
	// let's install and detour individual hooks.
	if (!wglSwapBuffers().install()) return false;
	if (!VGui_CallEngineSurfaceAppHandler().install()) return false;
	if (!Key_Event().install()) return false;
	if (!Host_Noclip_f().install()) return false;
	if (!ClientDLL_CreateMove().install()) return false;
	if (!_Host_Frame().install()) return false;
	if (!CL_ReallocateDynamicData().install()) return false;
	if (!CL_DeallocateDynamicData().install()) return false;
	if (!MYgluPerspective().install()) return false;
	if (!R_ForceCVars().install()) return false;
	if (!V_CalcRefdef().install()) return false;
	//if (!EV_HLDM_FireBullets().install()) return false;
	if (!HUD_Redraw().install()) return false;
	if (!R_StudioDrawPoints().install()) return false;
	if (!R_LightLambert().install()) return false;
	if (!V_FadeAlpha().install()) return false;
	if (!V_ApplyShake().install()) return false;
	if (!S_StartStaticSound().install()) return false;
	if (!R_DrawViewModel().install()) return false;
	if (!CPartSmokeGrenade__Create().install()) return false;
	if (!CreateGasSmoke().install()) return false;
	if (!CEngine__Unload().install()) return false;
	if (!SCR_CalcRefdef().install()) return false;
	if (!SCR_UpdateScreen().install()) return false;
	if (!SPR_Set().install()) return false;
	if (!CGame__AppActivate().install()) return false;
	if (!CHudAmmo__DrawCrosshair().install()) return false;

	return true;
}

void CMemoryFnDetourMgr::uninstall_hooks()
{
	m_unloading_hooks_mutex = true;

	// now, uninstall the detour from each function.
	wglSwapBuffers().uninstall();
	VGui_CallEngineSurfaceAppHandler().uninstall();
	Key_Event().uninstall();
	Host_Noclip_f().uninstall();
	ClientDLL_CreateMove().uninstall();
	_Host_Frame().uninstall();
	CL_ReallocateDynamicData().uninstall();
	CL_DeallocateDynamicData().uninstall();
	MYgluPerspective().uninstall();
	R_ForceCVars().uninstall();
	V_CalcRefdef().uninstall();
	//EV_HLDM_FireBullets().uninstall();
	HUD_Redraw().uninstall();
	R_StudioDrawPoints().uninstall();
	R_LightLambert().uninstall();
	V_FadeAlpha().uninstall();
	V_ApplyShake().uninstall();
	S_StartStaticSound().uninstall();
	R_DrawViewModel().uninstall();
	CPartSmokeGrenade__Create().uninstall();
	CreateGasSmoke().uninstall();

	if (!m_unloading_from_CEngine__Unload)
	{
		// this gets uninstalled when called, otherwise it would crash, because we unload the cheat inside of this function.
		CEngine__Unload().uninstall();
	}

	SCR_CalcRefdef().uninstall();
	SCR_UpdateScreen().uninstall();
	SPR_Set().uninstall();
	CGame__AppActivate().uninstall();
	CHudAmmo__DrawCrosshair().uninstall();

	m_unloading_hooks_mutex = false;
}

//---------------------------------------------------------------------------------

bool wglSwapBuffersFnHook_t::install()
{
	initialize("wglSwapBuffers", L"opengl32.dll");
	return generic_exported_procname_detour(wglSwapBuffers, "wglSwapBuffers");
}

BOOL wglSwapBuffersFnHook_t::wglSwapBuffers(HDC hdc)
{
	OX_PROFILE_SCOPE("swapbuffers");

	COxWareUI::the().swapbuffers_detour(hdc);

	return CMemoryFnDetourMgr::the().wglSwapBuffers().call(hdc);
}

//---------------------------------------------------------------------------------

bool VGui_CallEngineSurfaceAppHandlerFnHook_t::install()
{
	initialize("VGui_CallEngineSurfaceAppHandler", L"hw.dll");
	return generic_bytepattern_detour(VGui_CallEngineSurfaceAppHandler, "\x55\x8B\xEC\x56\x8B\x75\x0C\x57\x8B\x7D\x08\x56\x57\xE8\x00\x00\x00\x00\x83\xC4\x08\x85\xC0\x75\x0A\x56\x57\xE8\x00\x00\x00\x00\x83\xC4\x08\x5F\x5E\x5D\xC3\x90\x90\x90\x90\x90\x90\x90\x90\x90\xE9\x00\x00\x00\x00\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\xE8\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x90\x90\x90\x90\x90\x90\xE8");
}

void VGui_CallEngineSurfaceAppHandlerFnHook_t::VGui_CallEngineSurfaceAppHandler(void* event, void* userData)
{
	// This function is responsible for processing user keyboard and mouse input.

	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().VGui_CallEngineSurfaceAppHandler().call(event, userData);
}

//---------------------------------------------------------------------------------

bool Key_EventFnHook_t::install()
{
	initialize("Key_Event", L"hw.dll");
	return generic_bytepattern_detour(Key_Event, "\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x45\x08\x56\x3D");
}

void Key_EventFnHook_t::Key_Event(int key, hl::qboolean down)
{
	// this function is responsible for key input ingame

	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().Key_Event().call(key, down);
}

//---------------------------------------------------------------------------------

bool Host_Noclip_fFnHook_t::install()
{
	initialize("Host_Noclip_f", L"hw.dll");
	return generic_bytepattern_detour(Host_Noclip_f, "\x55\x8B\xEC\x83\xEC\x24\xA1\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x3B\xC2\x75\x00\xE8\x00\x00\x00\x00\x8B\xE5\x5D\xC3");
}

void Host_Noclip_fFnHook_t::Host_Noclip_f()
{
	CForceEnableDisabled::the().force_enable_noclip_pre();

	CMemoryFnDetourMgr::the().Host_Noclip_f().call();

	CForceEnableDisabled::the().force_enable_noclip_post();
}

//---------------------------------------------------------------------------------

bool ClientDLL_CreateMoveFnHook_t::install()
{
	initialize("ClientDLL_CreateMove", L"hw.dll");
	return generic_bytepattern_detour(ClientDLL_CreateMove, { "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x85\xC0\x74\x00\x8B\x4D\x10\x8B\x55\x0C\x51\x8B\x4D\x08\x52\x51\xFF\xD0\x83\xC4\x0C\x5D\xC3\x90" });
}

void ClientDLL_CreateMoveFnHook_t::ClientDLL_CreateMove(float frametime, hl::usercmd_t *cmd, int active)
{
	CMemoryFnDetourMgr::the().ClientDLL_CreateMove().call(frametime, cmd, active);

	if (COxWareUI::the().should_disable_ingame_input())
	{
		// disable all movement when the menu is up. This is called by CL_Move() when the game expects
		// client dll to modify usercmd_t for current command so it can be sent to the server.
		cmd->buttons = 0;
		cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;
	}
}

//---------------------------------------------------------------------------------

bool _Host_FrameFnHook_t::install()
{
	initialize("_Host_Frame", L"hw.dll");
	return generic_bytepattern_detour(_Host_Frame, { "\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83" });
}

void _Host_FrameFnHook_t::_Host_Frame(float time)
{
	if (CMemoryFnDetourMgr::the().exit_if_uninstalling())
	{
		return;
	}

	// GoldSrc engine frame function. There goes all code that has to work with ingame things.
	OX_PROFILE_SCOPE("engine_frame");

	CEntityMgr::the().update();

	CForceEnableDisabled::the().update_disable_sponly_cvars();

	CMemoryFnDetourMgr::the()._Host_Frame().call(time);
}

//---------------------------------------------------------------------------------

bool CL_ReallocateDynamicDataFnHook_t::install()
{
	initialize("CL_ReallocateDynamicData", L"hw.dll");
	return generic_bytepattern_detour(CL_ReallocateDynamicData, { "\x55\x8B\xEC\x8B\x45\x08\x50\xE8\x00\x00\x00\x00\x83\xC4\x04\xA3" });
}

void CL_ReallocateDynamicDataFnHook_t::CL_ReallocateDynamicData(int nMaxClients)
{
	// Called on level transition, the main entity array, cl_entities[] is being reallocated.

	CMemoryFnDetourMgr::the().CL_ReallocateDynamicData().call(nMaxClients);

	// must be called after CL_ReallocateDynamicData(), therefore we have stuff like cl.max_edicts set.
	CEntityMgr::the().erase();
	CEntityMgr::the().init();
}

//---------------------------------------------------------------------------------

bool CL_DeallocateDynamicDataFnHook_t::install()
{
	initialize("CL_DeallocateDynamicData", L"hw.dll");
	return generic_bytepattern_detour(CL_DeallocateDynamicData, { "\xA1\x00\x00\x00\x00\x85\xC0\x74\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x04\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE9" });
}

void CL_DeallocateDynamicDataFnHook_t::CL_DeallocateDynamicData()
{
	// called before allocation or at level disconnect. the cl_entities[] array is being deallocated and set to NULL.

	CEntityMgr::the().erase();

	CMemoryFnDetourMgr::the().CL_DeallocateDynamicData().call();
}

//---------------------------------------------------------------------------------

bool MYgluPerspectiveFnHook_t::install()
{
	initialize("MYgluPerspective", L"hw.dll");
	return generic_bytepattern_detour(MYgluPerspective, { "\x55\x8B\xEC\x83\xEC\x10\xDD\x45" });
}

void MYgluPerspectiveFnHook_t::MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble our_zFar = CForceEnableDisabled::the().force_max_viewable_renderdistance();
	if (our_zFar == -1.0)
		our_zFar = zFar;

	CMemoryFnDetourMgr::the().MYgluPerspective().call(fovy, aspect, zNear, our_zFar);
}

//---------------------------------------------------------------------------------

bool R_ForceCVarsFnHook_t::install()
{
	initialize("R_ForceCVars", L"hw.dll");
	return generic_bytepattern_detour(R_ForceCVars, { "\x55\x8B\xEC\x8B\x45\x08\x85\xC0\x0F\x84\x00\x00\x00\x00\xD9" });
}

void R_ForceCVarsFnHook_t::R_ForceCVars(hl::qboolean mp)
{
	if (CForceEnableDisabled::the().disable_renderer_cvar_constrain())
	{
		return;
	}

	CMemoryFnDetourMgr::the().R_ForceCVars().call(mp);
}

//---------------------------------------------------------------------------------

bool V_CalcRefdefFnHook_t::install()
{
	initialize("V_CalcRefdef", L"client.dll");
	return generic_functionaddr_detour(V_CalcRefdef, (uintptr_t*)CMemoryHookMgr::the().cldllfunc().get()->pfnHUD_CalcRef);
}

void V_CalcRefdefFnHook_t::V_CalcRefdef(hl::ref_params_t *pparams)
{
	CMemoryFnDetourMgr::the().V_CalcRefdef().call(pparams);

	// no-recoil
	//pparams->viewangles = pparams->cl_viewangles + pparams->punchangle;
}
 
//---------------------------------------------------------------------------------

bool EV_HLDM_FireBulletsFnHook_t::install()
{
	initialize("EV_HLDM_FireBullets", L"client.dll");
	return generic_bytepattern_detour(EV_HLDM_FireBullets, { "\x81\xEC\x00\x00\x00\x00\xB8" });
}

void EV_HLDM_FireBulletsFnHook_t::EV_HLDM_FireBullets(int idx, float* forward, float* right, float* up, int cShots, float* vecSrc, 
													  float* vecDirShooting, float* vecSpread, float flDistance, int iBulletType, 
													  int iTracerFreq, int* tracerCount, int iPenetration)
{
	CBulletTrace::the().on_bullet_fired(idx, forward, right, up, vecSrc, vecDirShooting, vecSpread, flDistance);

	CMemoryFnDetourMgr::the().EV_HLDM_FireBullets().call(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance,
														 iBulletType, iTracerFreq, tracerCount, iPenetration);

	//Vector vec_start = /*CGameUtil::the().is_local_player(idx) ? CEntityMgr::the().get_local_player().get_view_height() :*/ vecSrc;

}

//---------------------------------------------------------------------------------

bool HUD_RedrawFnHook_t::install()
{
	initialize("HUD_Redraw", L"client.dll");
	return generic_functionaddr_detour(HUD_Redraw, (uintptr_t*)CMemoryHookMgr::the().cldllfunc().get()->pfnHUD_Redraw);
}

int HUD_RedrawFnHook_t::HUD_Redraw(float time, int intermission)
{
	OX_PROFILE_SCOPE("clientdll_hud_redraw");
	
	CRemovals::the().remove_hud_modifier();

	CSpriteMgr::the().update();

	return CMemoryFnDetourMgr::the().HUD_Redraw().call(time, intermission);
}

//---------------------------------------------------------------------------------

bool R_StudioDrawPointsFnHook_t::install()
{
	initialize("R_StudioDrawPoints", L"hw.dll");
	return generic_bytepattern_detour(R_StudioDrawPoints, { "\x55\x8B\xEC\x83\xEC\x48\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x53\x56\x8B\x70" });
}

void R_StudioDrawPointsFnHook_t::R_StudioDrawPoints()
{
	// function responsible for the actual rendering of the studio model
	OX_PROFILE_SCOPE("studio_drawpoints");

	CModelChams::the().executeall_studio_pre();

	CMemoryFnDetourMgr::the().R_StudioDrawPoints().call();

	CModelChams::the().executeall_studio_post();
}

//---------------------------------------------------------------------------------

bool R_LightLambertFnHook_t::install()
{
	initialize("R_LightLambert", L"hw.dll");
	return generic_bytepattern_detour(R_LightLambert, { "\x55\x8B\xEC\x83\xEC\x24\x8B\x00\x00\x00\x00\x00\x56" });
}

void R_LightLambertFnHook_t::R_LightLambert(float** light, float *normal, float *src, float *lambert)
{
	// function called inside R_StudioDrawPoints() for modifying the color of the rendered model.

	CMemoryFnDetourMgr::the().R_LightLambert().call(light, normal, src, lambert);

	CModelChams::the().executeall_color(lambert);
}

//---------------------------------------------------------------------------------

bool V_FadeAlphaFnHook_t::install()
{
	initialize("V_FadeAlpha", L"hw.dll");
	return generic_bytepattern_detour(V_FadeAlpha, { "\x55\x8B\xEC\x83\xEC\x08\xD9\x05\x00\x00\x00\x00\xDC\x1D" });
}

int V_FadeAlphaFnHook_t::V_FadeAlpha()
{
	// function for drawing screen fade.

	int alpha = CMemoryFnDetourMgr::the().V_FadeAlpha().call();

	CFlashbangFadeModifier::the().update(alpha);

	return alpha;
}

//---------------------------------------------------------------------------------

bool V_ApplyShakeFnHook_t::install()
{
	initialize("V_ApplyShake", L"hw.dll");
	return generic_bytepattern_detour(V_ApplyShake, { "\x55\x8B\xEC\x8D\x45\x10\x8D\x4D\x0C\x50\x8D\x55\x08\x51\x52\xFF\x15\x00\x00\x00\x00\x8B\x45\x08\x83" });
}

void V_ApplyShakeFnHook_t::V_ApplyShake(float* origin, float* angles, float factor)
{
	if (CRemovals::the().remove_screenshake())
	{
		return; // bye bye
	}

	CMemoryFnDetourMgr::the().V_ApplyShake().call(origin, angles, factor);
}

//---------------------------------------------------------------------------------

bool S_StartStaticSoundFnHook_t::install()
{
	initialize("S_StartStaticSound", L"hw.dll");
	return generic_bytepattern_detour(S_StartStaticSound, { "\x55\x8B\xEC\x83\xEC\x48\xA1\x00\x00\x00\x00\x53" });
}

void S_StartStaticSoundFnHook_t::S_StartStaticSound(int entnum, int entchannel, hl::sfx_t* sfx, hl::vec_t* origin, 
													float fvol, float attenuation, int flags, int pitch)
{
	if (sfx)
	{
		if (strstr(sfx->name, "step"))
		{
			CESP::the().register_player_step(origin, entnum);
		}
	}

	CMemoryFnDetourMgr::the().S_StartStaticSound().call(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
}

//---------------------------------------------------------------------------------

bool R_DrawViewModelFnHook_t::install()
{
	initialize("R_DrawViewModel", L"hw.dll");
	return generic_bytepattern_detour(R_DrawViewModel, { "\x55\x8B\xEC\x83\xEC\x50\xD9" });
}

void R_DrawViewModelFnHook_t::R_DrawViewModel()
{
	CViewmodelOffset::the().update();

	CMemoryFnDetourMgr::the().R_DrawViewModel().call();
}

//---------------------------------------------------------------------------------

bool CPartSmokeGrenade__CreateFnHook_t::install()
{
	initialize("CPartSmokeGrenade__Create", L"client.dll");
	return generic_bytepattern_detour(CPartSmokeGrenade__Create, { "\x8B\x00\x00\x00\x00\x00\x56\x57\x68\x00\x00\x00\x00\x8B\x01" });
}

hl::CPartSmokeGrenade* __cdecl CPartSmokeGrenade__CreateFnHook_t::CPartSmokeGrenade__Create(
	hl::CPartSmokeGrenade* _this, Vector org, Vector normal, hl::model_t* sprite, 
	float size, float brightness, const char* classname)
{
	// CPartSmokeGrenade::Create, although it doesn't look like it, is a static member function, so its calling convention
	// may be (and it is in this case) __cdecl, instead of __thiscall. It is called every time new smoke is created. And
	// it is being called by CreateGasSmoke.

	auto part = CMemoryFnDetourMgr::the().CPartSmokeGrenade__Create().call(_this, org, normal, sprite, size/* * 0.5*/, brightness, classname);

	CSmokeVisuals::the().on_smoke_particleeffect_create(part);

	return part;
}

//---------------------------------------------------------------------------------

bool CreateGasSmokeFnHook_t::install()
{
	initialize("CreateGasSmoke", L"client.dll");
	return generic_bytepattern_detour(CreateGasSmoke, { "\x51\xA1\x00\x00\x00\x00\x53\x33" });
}

void CreateGasSmokeFnHook_t::CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable)
{
	CMemoryFnDetourMgr::the().CreateGasSmoke().call(origin, vVelocity, bInsideSmoke, bSpawnInside, bBlowable);

	CSmokeVisuals::the().post_gas_smoke_create();
}

//---------------------------------------------------------------------------------

bool CEngine__UnloadFnHook_t::install()
{
	initialize("CEngine__Unload", L"hw.dll");
	return generic_bytepattern_detour(CEngine__Unload, { "\x56\x8B\xF1\xE8\x00\x00\x00\x00\xC7\x46" });
}

void __thiscall CEngine__UnloadFnHook_t::CEngine__Unload(hl::CEngine* ecx)
{
	CConsole::the().info("Game is closing, shutting down cheat...");

	CMemoryFnDetourMgr::the().toggle_unloading_from_CEngine__Unload();

	// shut down the cheat in the next iteration of the topmost update function and when the loop will
	// actually break, don't tell the loader we're unloading, because we wouldn't have enough time to finish
	// with this code - the loader would unload our module..
	CoXWARE::the().end_cheat_execution(true);

	// ok now we SHOULD fully shutted down and we can redirect this to the engine again.
	// I say should, because if the update function somehow hangs before it can proceed to the next iteration, we can fail here.
	// Hence, wait a little.
	std::this_thread::sleep_for(0.250s);
	CMemoryFnDetourMgr::the().CEngine__Unload().call(ecx);
	CMemoryFnDetourMgr::the().CEngine__Unload().uninstall();

	CInjectedDllIPCLayerClient::the().write_code(C2I_Unloading);
}

//---------------------------------------------------------------------------------

bool SCR_CalcRefdefFnHook_t::install()
{
	initialize("SCR_CalcRefdef", L"hw.dll");
	return generic_bytepattern_detour(SCR_CalcRefdef, { "\x55\x8B\xEC\x83\xEC\x0C\xD9\x05\x00\x00\x00\x00\xD8\x1D\x00\x00\x00\x00\x33" });
}

void SCR_CalcRefdefFnHook_t::SCR_CalcRefdef()
{
	// fov gets capped inside this function.

	CFieldOfViewChanger::the().scale_fov();

	CMemoryFnDetourMgr::the().SCR_CalcRefdef().call();
}

//---------------------------------------------------------------------------------

bool SCR_UpdateScreenFnHook_t::install()
{
	initialize("SCR_UpdateScreen", L"hw.dll");
	return generic_bytepattern_detour(SCR_UpdateScreen, { "\x55\x8B\xEC\x83\xEC\x10\xA1\x00\x00\x00\x00\x56\x33\xF6\x3B\xC6\x0F" });
}

void SCR_UpdateScreenFnHook_t::SCR_UpdateScreen()
{
	if (CFrameSkipper::the().skip_current_frame())
	{
		return;
	}

	CMemoryFnDetourMgr::the().SCR_UpdateScreen().call();
}

//---------------------------------------------------------------------------------

bool SPR_SetFnHook_t::install()
{
	initialize("SPR_Set", L"hw.dll");
	return generic_functionaddr_detour(SPR_Set, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs().get()->pfnSPR_Set);
}

void SPR_SetFnHook_t::SPR_Set(hl::HSPRITE_t hSprite, int r, int g, int b)
{
	// Handles sprite color
	
	// there isn't any way around this (how to change all sprites color), because this function
	// directly calls glColorf().

	CSpriteMgr::the().handle_color_change(hSprite, r, g, b);

	CMemoryFnDetourMgr::the().SPR_Set().call(hSprite, r, g, b);
}

//---------------------------------------------------------------------------------

bool CGame__AppActivateFnHook_t::install()
{
	initialize("CGame__AppActivate", L"hw.dll");
	return generic_bytepattern_detour(CGame__AppActivate, { "\x55\x8B\xEC\x51\x53\x8B\x5D\x08\x56\x8B\xF1\x84" });
}

void __thiscall CGame__AppActivateFnHook_t::CGame__AppActivate(void* ecx, bool fActive)
{
	// is called when the window is focused/unfocused.

	// if we let this called while the menu is up, it can override the ingame cursor that we hid.
	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().CGame__AppActivate().call(ecx, fActive);
}

//---------------------------------------------------------------------------------

bool CHudAmmo__DrawCrosshairFnHook_t::install()
{
	initialize("CHudAmmo__DrawCrosshair", L"client.dll");
	return generic_bytepattern_detour(CHudAmmo__DrawCrosshair, { "\x83\xEC\x08\x8B\x44\x24\x10\x53\x55\x56\x57" });
}

int __thiscall CHudAmmo__DrawCrosshairFnHook_t::CHudAmmo__DrawCrosshair(void* ecx, float flTime, int weaponid)
{
	if (crosshair_enable.get_value())
	{
		CVanillaCrosshair::the().draw();
		return 1;
	}

	return CMemoryFnDetourMgr::the().CHudAmmo__DrawCrosshair().call(ecx, flTime, weaponid);
}

//---------------------------------------------------------------------------------