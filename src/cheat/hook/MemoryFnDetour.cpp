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
	wglSwapBuffers().install();
	if (CoXWARE::the().get_build_number() <= 4554) // TODO: figure out how far does this go, to which version
	{
		VGui_CallEngineSurfaceAppHandler4554().install();
	}
	else
	{
		VGui_CallEngineSurfaceAppHandler().install();
	}
	Key_Event().install();
	Host_Noclip_f().install();
	ClientDLL_CreateMove().install();
	_Host_Frame().install();
	CL_ReallocateDynamicData().install();
	CL_DeallocateDynamicData().install();
	MYgluPerspective().install();
	R_ForceCVars().install();
	V_CalcRefdef().install();
	//EV_HLDM_FireBullets().install();
	HUD_Redraw().install();
	R_GLStudioDrawPoints().install();
	R_LightLambert().install();
	V_FadeAlpha().install();
	V_ApplyShake().install();
	S_StartDynamicSound().install();
	R_DrawViewModel().install();
	CPartSmokeGrenade__Create().install();
	CreateGasSmoke().install();
	CEngine__Unload().install();
	SCR_CalcRefdef().install();
	SCR_UpdateScreen().install();
	SPR_Set().install();
	CGame__AppActivate().install();
	CHudAmmo__DrawCrosshair().install();
	R_StudioDrawPlayer().install();
	CL_SendConsistencyInfo().install();
	SCR_DrawFPS().install();
	Cmd_AddMallocCommand().install();
	hudRegisterVariable().install();
	MSG_WriteUsercmd().install();
	CHudSniperScope__Draw().install();
	IN_ScaleMouse().install();
	CL_IsThirdPerson().install();

	return true;
}

void CMemoryFnDetourMgr::uninstall_hooks()
{
	m_unloading_hooks_mutex = true;

	// now, uninstall the detour from each function.
	wglSwapBuffers().uninstall();
	if (CoXWARE::the().get_build_number() <= 4554) // TODO: figure out how far does this go, to which version
	{
		VGui_CallEngineSurfaceAppHandler4554().uninstall();
	}
	else
	{
		VGui_CallEngineSurfaceAppHandler().uninstall();
	}
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
	R_GLStudioDrawPoints().uninstall();
	R_LightLambert().uninstall();
	V_FadeAlpha().uninstall();
	V_ApplyShake().uninstall();
	S_StartDynamicSound().uninstall();
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
	R_StudioDrawPlayer().uninstall();
	CL_SendConsistencyInfo().uninstall();
	SCR_DrawFPS().uninstall();
	Cmd_AddMallocCommand().uninstall();
	hudRegisterVariable().uninstall();
	MSG_WriteUsercmd().uninstall();
	CHudSniperScope__Draw().uninstall();
	IN_ScaleMouse().uninstall();
	CL_IsThirdPerson().uninstall();

	m_unloading_hooks_mutex = false;
}

//---------------------------------------------------------------------------------

bool wglSwapBuffers_FnDetour_t::install()
{
	initialize("wglSwapBuffers", L"opengl32.dll");
	return detour_using_exported_name((uintptr_t*)wglSwapBuffers, "wglSwapBuffers");
}

BOOL wglSwapBuffers_FnDetour_t::wglSwapBuffers(HDC hdc)
{
	OX_PROFILE_SCOPE("swapbuffers");

	COxWareUI::the().swapbuffers_detour(hdc);

	return CMemoryFnDetourMgr::the().wglSwapBuffers().call(hdc);
}

//---------------------------------------------------------------------------------

bool VGui_CallEngineSurfaceAppHandler_FnDetour_t::install()
{
	initialize("VGui_CallEngineSurfaceAppHandler", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)VGui_CallEngineSurfaceAppHandler);
}

void VGui_CallEngineSurfaceAppHandler_FnDetour_t::VGui_CallEngineSurfaceAppHandler(void* event, void* userData)
{
	// This function is responsible for processing user keyboard and mouse input.

	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().VGui_CallEngineSurfaceAppHandler().call(event, userData);
}

//---------------------------------------------------------------------------------

bool VGui_CallEngineSurfaceAppHandler4554_FnDetour_t::install()
{
	initialize("VGui_CallEngineSurfaceAppHandler4554", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)VGui_CallEngineSurfaceAppHandler4554);
}

LRESULT VGui_CallEngineSurfaceAppHandler4554_FnDetour_t::VGui_CallEngineSurfaceAppHandler4554(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// This function is responsible for processing user keyboard and mouse input.

	if (COxWareUI::the().should_disable_ingame_input())
	{
		return 0;
	}

	return CMemoryFnDetourMgr::the().VGui_CallEngineSurfaceAppHandler4554().call(hwnd, msg, wparam, lparam);
}

//---------------------------------------------------------------------------------

bool Key_Event_FnDetour_t::install()
{
	initialize("Key_Event", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Key_Event);
}

void Key_Event_FnDetour_t::Key_Event(int key, hl::qboolean down)
{
	// this function is responsible for key input ingame

	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	// stop executing the engine key, if our key is bound to an incommand.
	// TODO: Make this customizable in the future...?
	if (g_in_commands_i->is_key_bound_and_active(g_user_input_i->engine_key_to_virtual_key(key)))
	{
		return;
	}

	// see if this key is bound inside our cheat. if yes, prefer executing our key over engine key.
	if (g_bindmgr_i->is_key_bound(g_user_input_i->engine_key_to_virtual_key(key)))
	{
		return;
	}

	CMemoryFnDetourMgr::the().Key_Event().call(key, down);
}

//---------------------------------------------------------------------------------

bool Host_Noclip_f_FnDetour_t::install()
{
	initialize("Host_Noclip_f", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Host_Noclip_f);
}

void Host_Noclip_f_FnDetour_t::Host_Noclip_f()
{
	CForceEnableDisabled::the().force_enable_noclip_pre();

	CMemoryFnDetourMgr::the().Host_Noclip_f().call();

	CForceEnableDisabled::the().force_enable_noclip_post();
}

//---------------------------------------------------------------------------------

bool ClientDLL_CreateMove_FnDetour_t::install()
{
	initialize("ClientDLL_CreateMove", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)ClientDLL_CreateMove);
}

void ClientDLL_CreateMove_FnDetour_t::ClientDLL_CreateMove(float frametime, hl::usercmd_t *cmd, int active)
{
	// reset this crap back to it's original state.
	// TODO: Find a better way of doing this kind of speedhack, i think that modifying the multiplier 
	//       in CL_Move and CL_RunUsercmd would create the same effect...
	CGameUtil::the().classic_cs16_cheating_scene_speedhack(1.0);

	CClientMovementPacket::the().update_clientmove();

	CMemoryFnDetourMgr::the().ClientDLL_CreateMove().call(frametime, cmd, active);

	CClientMovementPacket::the().set_current_cmd_for_manipulation(cmd);

	CLocalState::the().update_clientmove(frametime, cmd);

	if (COxWareUI::the().should_disable_ingame_input())
	{
		// disable all movement when the menu is up. This is called by CL_Move() when the game expects
		// client dll to modify usercmd_t for current command so it can be sent to the server.
		cmd->buttons = 0;
		cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;
	}

	CMovement::the().update_clientmove(frametime, cmd);
}

//---------------------------------------------------------------------------------

bool _Host_Frame_FnDetour_t::install()
{
	initialize("_Host_Frame", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)_Host_Frame);
}

void _Host_Frame_FnDetour_t::_Host_Frame(float time)
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

bool CL_ReallocateDynamicData_FnDetour_t::install()
{
	initialize("CL_ReallocateDynamicData", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_ReallocateDynamicData);
}

void CL_ReallocateDynamicData_FnDetour_t::CL_ReallocateDynamicData(int nMaxClients)
{
	// Called on level transition, the main entity array, cl_entities[] is being reallocated.

	CMemoryFnDetourMgr::the().CL_ReallocateDynamicData().call(nMaxClients);

	// must be called after CL_ReallocateDynamicData(), therefore we have stuff like cl.max_edicts set.
	CEntityMgr::the().erase();
	CEntityMgr::the().init();
}

//---------------------------------------------------------------------------------

bool CL_DeallocateDynamicData_FnDetour_t::install()
{
	initialize("CL_DeallocateDynamicData", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_DeallocateDynamicData);
}

void CL_DeallocateDynamicData_FnDetour_t::CL_DeallocateDynamicData()
{
	// called before allocation or at level disconnect. the cl_entities[] array is being deallocated and set to NULL.

	CEntityMgr::the().erase();

	CMemoryFnDetourMgr::the().CL_DeallocateDynamicData().call();
}

//---------------------------------------------------------------------------------

bool MYgluPerspective_FnDetour_t::install()
{
	initialize("MYgluPerspective", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)MYgluPerspective);
}

void MYgluPerspective_FnDetour_t::MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble our_zFar = CForceEnableDisabled::the().force_max_viewable_renderdistance();
	if (our_zFar == -1.0)
		our_zFar = zFar;

	CMemoryFnDetourMgr::the().MYgluPerspective().call(fovy, aspect, zNear, our_zFar);
}

//---------------------------------------------------------------------------------

bool R_ForceCVars_FnDetour_t::install()
{
	initialize("R_ForceCVars", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_ForceCVars);
}

void R_ForceCVars_FnDetour_t::R_ForceCVars(hl::qboolean mp)
{
	if (CForceEnableDisabled::the().disable_renderer_cvar_constrain())
	{
		return;
	}

	CMemoryFnDetourMgr::the().R_ForceCVars().call(mp);
}

//---------------------------------------------------------------------------------

bool V_CalcRefdef_FnDetour_t::install()
{
	initialize("V_CalcRefdef", L"client.dll");
	return detour_using_memory_address((uintptr_t*)V_CalcRefdef, (uintptr_t*)CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_CalcRef);
}

void V_CalcRefdef_FnDetour_t::V_CalcRefdef(hl::ref_params_t *pparams)
{
	CMemoryFnDetourMgr::the().V_CalcRefdef().call(pparams);

	CThirdPerson::the().update(pparams);

	// no-recoil
	//pparams->viewangles = pparams->cl_viewangles + pparams->punchangle;
}
 
//---------------------------------------------------------------------------------

bool EV_HLDM_FireBullets_FnDetour_t::install()
{
	initialize("EV_HLDM_FireBullets", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)EV_HLDM_FireBullets);
}

void EV_HLDM_FireBullets_FnDetour_t::EV_HLDM_FireBullets(int idx, float* forward, float* right, float* up, int cShots, float* vecSrc, 
													  float* vecDirShooting, float* vecSpread, float flDistance, int iBulletType, 
													  int iTracerFreq, int* tracerCount, int iPenetration)
{
	CBulletTrace::the().on_bullet_fired(idx, forward, right, up, vecSrc, vecDirShooting, vecSpread, flDistance);

	CMemoryFnDetourMgr::the().EV_HLDM_FireBullets().call(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance,
														 iBulletType, iTracerFreq, tracerCount, iPenetration);

	//Vector vec_start = /*CGameUtil::the().is_local_player(idx) ? CEntityMgr::the().get_local_player().get_eye_pos() :*/ vecSrc;

}

//---------------------------------------------------------------------------------

bool HUD_Redraw_FnDetour_t::install()
{
	initialize("HUD_Redraw", L"client.dll");
	return detour_using_memory_address((uintptr_t*)HUD_Redraw, (uintptr_t*)CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_Redraw);
}

int HUD_Redraw_FnDetour_t::HUD_Redraw(float time, int intermission)
{
	OX_PROFILE_SCOPE("clientdll_hud_redraw");
	
	CRemovals::the().remove_hud_modifier();

	CSpriteMgr::the().update();

	return CMemoryFnDetourMgr::the().HUD_Redraw().call(time, intermission);
}

//---------------------------------------------------------------------------------

bool R_GLStudioDrawPoints_FnDetour_t::install()
{
	initialize("R_GLStudioDrawPoints", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_GLStudioDrawPoints);
}

void R_GLStudioDrawPoints_FnDetour_t::R_GLStudioDrawPoints()
{
	if (CMemoryFnDetourMgr::the().exit_if_uninstalling())
	{
		return;
	}

	// function responsible for the actual rendering of the studio model
	OX_PROFILE_SCOPE("studio_drawpoints");

	CModelChams::the().render_playerhead_hitbox();

	if (CModelChams::the().studio_draw_skeleton())
	{
		return;
	}

	CModelChams::the().executeall_studio_pre();

	CMemoryFnDetourMgr::the().R_GLStudioDrawPoints().call();

	CModelChams::the().executeall_studio_post();
}

//---------------------------------------------------------------------------------

bool R_LightLambert_FnDetour_t::install()
{
	initialize("R_LightLambert", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_LightLambert);
}

void R_LightLambert_FnDetour_t::R_LightLambert(float** light, float *normal, float *src, float *lambert)
{
	if (CMemoryFnDetourMgr::the().exit_if_uninstalling())
	{
		return;
	}

	// function called inside R_GLStudioDrawPoints() for modifying the color of the rendered model.

	CMemoryFnDetourMgr::the().R_LightLambert().call(light, normal, src, lambert);

	CModelChams::the().executeall_color(lambert);
}

//---------------------------------------------------------------------------------

bool V_FadeAlpha_FnDetour_t::install()
{
	initialize("V_FadeAlpha", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)V_FadeAlpha);
}

int V_FadeAlpha_FnDetour_t::V_FadeAlpha()
{
	// function for drawing screen fade.

	int alpha = CMemoryFnDetourMgr::the().V_FadeAlpha().call();

	CFlashbangFadeModifier::the().update(alpha);

	return alpha;
}

//---------------------------------------------------------------------------------

bool V_ApplyShake_FnDetour_t::install()
{
	initialize("V_ApplyShake", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)V_ApplyShake);
}

void V_ApplyShake_FnDetour_t::V_ApplyShake(float* origin, float* angles, float factor)
{
	if (CRemovals::the().remove_screenshake())
	{
		return; // bye bye
	}

	CMemoryFnDetourMgr::the().V_ApplyShake().call(origin, angles, factor);
}

//---------------------------------------------------------------------------------

bool S_StartDynamicSound_FnDetour_t::install()
{
	initialize("S_StartDynamicSound", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)S_StartDynamicSound);
}

void S_StartDynamicSound_FnDetour_t::S_StartDynamicSound(int entnum, int entchannel, hl::sfx_t* sfx, hl::vec_t* origin, 
													float fvol, float attenuation, int flags, int pitch)
{
	if (sfx)
	{
		if (strstr(sfx->name, "step"))
		{
			CESP::the().register_player_step(origin, entnum);
		}
	}

	CMemoryFnDetourMgr::the().S_StartDynamicSound().call(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
}

//---------------------------------------------------------------------------------

bool R_DrawViewModel_FnDetour_t::install()
{
	initialize("R_DrawViewModel", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_DrawViewModel);
}

void R_DrawViewModel_FnDetour_t::R_DrawViewModel()
{
	if (CRemovals::the().remove_viewmodel())
	{
		return;
	}

	CViewmodelOffset::the().update();

	CModelChams::the().force_default_models();
	
	CMemoryFnDetourMgr::the().R_DrawViewModel().call();
}

//---------------------------------------------------------------------------------

bool CPartSmokeGrenade__Create_FnDetour_t::install()
{
	initialize("CPartSmokeGrenade::Create", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CPartSmokeGrenade__Create);
}

hl::CPartSmokeGrenade* __cdecl CPartSmokeGrenade__Create_FnDetour_t::CPartSmokeGrenade__Create(
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

bool CreateGasSmoke_FnDetour_t::install()
{
	initialize("CreateGasSmoke", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CreateGasSmoke);
}

void CreateGasSmoke_FnDetour_t::CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable)
{
	CMemoryFnDetourMgr::the().CreateGasSmoke().call(origin, vVelocity, bInsideSmoke, bSpawnInside, bBlowable);

	CSmokeVisuals::the().post_gas_smoke_create();
}

//---------------------------------------------------------------------------------

bool CEngine__Unload_FnDetour_t::install()
{
	initialize("CEngine::Unload", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CEngine__Unload);
}

void __thiscall CEngine__Unload_FnDetour_t::CEngine__Unload(hl::CEngine* ecx)
{
	CConsole::the().info("Game is closing, shutting down cheat...");

	// don't unload this detour when calling CMemoryFnDetourMgr::uninstall_hooks().
	CMemoryFnDetourMgr::the().toggle_unloading_from_CEngine__Unload();

	bool engine_restarting = ecx->m_nQuitting == hl::QUIT_RESTART;
	
	// set up the shutdown of the cheat. this triggers a check inside the main loop that is executing
	// on another thread. worst possible time to wait is 0.5s (if there's no hang), because the main loop updates
	// every 0.5s.
	CoXWARE::the().end_cheat_execution(true);

	while (!CoXWARE::the().is_shutted_down())
	{
		// hang till we fully shut down, if we hang in the shutdown process, we're fucked.
		std::this_thread::sleep_for(0.1s);
	}

	CMemoryFnDetourMgr::the().CEngine__Unload().call(ecx);
	CMemoryFnDetourMgr::the().CEngine__Unload().uninstall();

	if (engine_restarting)
	{
		// restart the cheat. this will fire a signal inside the loader and it will unload the module 
		// first and then load it back in.
		CInjectedDllIPCLayerClient::the().write_code(C2I_Restarting);
	}
	else
	{
		// engine qutting, we're fully shutted down, so just tell the loader to fully unload us.
		CInjectedDllIPCLayerClient::the().write_code(C2I_Unloading);

		// wait a little bit so that the loader can keep up with the unload code...
		std::this_thread::sleep_for(std::chrono::milliseconds(CBaseInjector::k_update_threshold_ms * 4));
	}
}

//---------------------------------------------------------------------------------

bool SCR_CalcRefdef_FnDetour_t::install()
{
	initialize("SCR_CalcRefdef", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_CalcRefdef);
}

void SCR_CalcRefdef_FnDetour_t::SCR_CalcRefdef()
{
	// fov gets capped inside this function.

	CFieldOfViewChanger::the().scale_fov();

	CMemoryFnDetourMgr::the().SCR_CalcRefdef().call();
}

//---------------------------------------------------------------------------------

bool SCR_UpdateScreen_FnDetour_t::install()
{
	initialize("SCR_UpdateScreen", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_UpdateScreen);
}

void SCR_UpdateScreen_FnDetour_t::SCR_UpdateScreen()
{
	if (CFrameSkipper::the().skip_current_frame())
	{
		return;
	}

	CMemoryFnDetourMgr::the().SCR_UpdateScreen().call();
}

//---------------------------------------------------------------------------------

bool SPR_Set_FnDetour_t::install()
{
	initialize("SPR_Set", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)SPR_Set, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs().get()->pfnSPR_Set);
}

void SPR_Set_FnDetour_t::SPR_Set(hl::HSPRITE_t hSprite, int r, int g, int b)
{
	// Handles sprite color
	
	// there isn't any way around this (how to change all sprites color), because this function
	// directly calls glColorf().

	CSpriteMgr::the().handle_color_change(hSprite, r, g, b);

	CMemoryFnDetourMgr::the().SPR_Set().call(hSprite, r, g, b);
}

//---------------------------------------------------------------------------------

bool CGame__AppActivate_FnDetour_t::install()
{
	initialize("CGame::AppActivate", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CGame__AppActivate);
}

void __thiscall CGame__AppActivate_FnDetour_t::CGame__AppActivate(void* ecx, bool fActive)
{
	// is called when the window is focused/unfocused.

	CoXWARE::the().set_at_least_once_focused();

	// if we let this called while the menu is up, it can override the ingame cursor that we hid.
	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().CGame__AppActivate().call(ecx, fActive);
}

//---------------------------------------------------------------------------------

bool CHudAmmo__DrawCrosshair_FnDetour_t::install()
{
	initialize("CHudAmmo::DrawCrosshair", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CHudAmmo__DrawCrosshair);
}

int __thiscall CHudAmmo__DrawCrosshair_FnDetour_t::CHudAmmo__DrawCrosshair(void* ecx, float flTime, int weaponid)
{
	if (crosshair_enable.get_value())
	{
		CVanillaCrosshair::the().draw();
		return 1;
	}

	return CMemoryFnDetourMgr::the().CHudAmmo__DrawCrosshair().call(ecx, flTime, weaponid);
}

//---------------------------------------------------------------------------------

bool R_StudioDrawPlayer_FnDetour_t::install()
{
	initialize("R_StudioDrawPlayer", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)R_StudioDrawPlayer, (uintptr_t*)(*CMemoryHookMgr::the().pStudioAPI().get())->StudioDrawPlayer);
}

int R_StudioDrawPlayer_FnDetour_t::R_StudioDrawPlayer(int flags, hl::entity_state_t* pplayer)
{
	if (CRemovals::the().remove_player(pplayer->number - 1))
	{
		return 0;
	}

	if (mdlchams_player_skeleton.get_value())
	{
		// disable p_* models when using skeletal chams. This model gets merged into the playermodel, hence createing unecessary
		// bones then we then render and that stays in the way.
		pplayer->weaponmodel = 0;
	}

	if (mdlchams_disable_animations.get_value())
	{
		pplayer->gaitsequence = 1;
	}

	// draw first so we don't clip through original model
	if (mdlchams_render_real_playermodel.get_value())
	{
		static hl::cvar_t* cl_minmodels = nullptr;
		if (!cl_minmodels)
		{
			cl_minmodels = CGoldSrcCommandMgr::the().get_cvar("cl_minmodels");
		}

		cl_minmodels->value = 1;

		CModelChams::the().toggle_rendering_real_playermodel();

		CMemoryFnDetourMgr::the().R_StudioDrawPlayer().call(flags, pplayer);

		CModelChams::the().toggle_rendering_real_playermodel();

		cl_minmodels->value = 0;
	}

	int ret = CMemoryFnDetourMgr::the().R_StudioDrawPlayer().call(flags, pplayer);

	return ret;
}

//---------------------------------------------------------------------------------

bool CL_SendConsistencyInfo_FnDetour_t::install()
{
	initialize("CL_SendConsistencyInfo", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_SendConsistencyInfo);
}

void CL_SendConsistencyInfo_FnDetour_t::CL_SendConsistencyInfo(hl::sizebuf_t* msg)
{
	if (!CResourceConsistencyBypass::the().detour_consistency_info(msg))
	{
		CMemoryFnDetourMgr::the().CL_SendConsistencyInfo().call(msg);
	}
}

//---------------------------------------------------------------------------------

bool SCR_DrawFPS_FnDetour_t::install()
{
	initialize("SCR_DrawFPS", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_DrawFPS);
}

void SCR_DrawFPS_FnDetour_t::SCR_DrawFPS()
{
	// this is in fact a good place to render custom engine stuff from

	CEngineRendering::the().repaint();

	CMemoryFnDetourMgr::the().SCR_DrawFPS().call();
}

//---------------------------------------------------------------------------------

bool Cmd_AddMallocCommand_FnDetour_t::install()
{
	initialize("Cmd_AddMallocCommand", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Cmd_AddMallocCommand);
}

void Cmd_AddMallocCommand_FnDetour_t::Cmd_AddMallocCommand(char* cmd_name, hl::xcommand_t function, int flag)
{
	// sadly, the function doesn't return the new command entry pointer, so we have to search for it...

	//
	// so far what I had gathered from studying the GS command code:
	// 
	// there are two functions that can add commands:
	// 1) Cmd_AddCommandWithFlags	- is only called from the host initialization code and cannot be called after.
	// 2) Cmd_AddMallocCommand		- can be called after initialization, is used by mods. This one is the only one
	//								  that I am aware of that can add commands at runtime.
	// 
	// commands can be also removed, but they're only at initialization or shutdown. From mod you aren't able to do that AFAIk.
	// 
	// commands with FCMD_HUD flag are added by the client dll (mod).
	// commands with FCMD_GAME flag are added by the game dll.
	// commands with FCMD_WRAPPER flag are added by the SystemWrapper interface. (this is used by gameUI)
	//

	CMemoryFnDetourMgr::the().Cmd_AddMallocCommand().call(cmd_name, function, flag);

	// now the cmd has been added, search for it.
	//
	// the function goes like this:
	//
	// cmd->next = cmd_functions;
	// cmd_functions = cmd;
	//
	// so I assume that cmd_functions points to the newly added entry.
	auto cmd = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetFirstCmdFunctionHandle(); // cannot be null
	CGoldSrcCommandMgr::the().register_cmd(cmd->name, cmd);
}

//---------------------------------------------------------------------------------

bool hudRegisterVariable_FnDetour_t::install()
{
	initialize("hudRegisterVariable", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)hudRegisterVariable, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs().get()->pfnRegisterVariable);
}

hl::cvar_t* hudRegisterVariable_FnDetour_t::hudRegisterVariable(char* szName, char* szValue, int flags)
{
	auto cvar = CMemoryFnDetourMgr::the().hudRegisterVariable().call(szName, szValue, flags);

	//
	// with cvars its easier than with commands. Cvars are added only by Cvar_RegisterVariable, which is
	// exported to clientdlls via pfnRegisterVariable. There's no other way to register cvars.
	// 
	// cvars cannot be unregistered. they are only unregistered at shutdown or initialization. same with cmds.
	//

	if (cvar)
	{
		CGoldSrcCommandMgr::the().register_cvar(cvar->name, cvar);
	}

	return cvar;
}

//---------------------------------------------------------------------------------

bool MSG_WriteUsercmd_FnDetour_t::install()
{
	initialize("MSG_WriteUsercmd", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)MSG_WriteUsercmd);
}

void MSG_WriteUsercmd_FnDetour_t::MSG_WriteUsercmd(hl::sizebuf_t* buf, hl::usercmd_t* to, hl::usercmd_t* from)
{
	CClientMovementPacket::the().update_msg_writeusercmd(to);

	CMemoryFnDetourMgr::the().MSG_WriteUsercmd().call(buf, to, from);
}

//---------------------------------------------------------------------------------

bool CHudSniperScope__Draw_FnDetour_t::install()
{
	initialize("CHudSniperScope::Draw", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CHudSniperScope__Draw);
}

int __thiscall CHudSniperScope__Draw_FnDetour_t::CHudSniperScope__Draw(void* ecx, float flTime)
{
	if (CRemovals::the().remove_sniper_scope())
	{
		return 1;
	}

	return CMemoryFnDetourMgr::the().CHudSniperScope__Draw().call(ecx, flTime);
}

//---------------------------------------------------------------------------------

bool IN_ScaleMouse_FnDetour_t::install()
{
	initialize("IN_ScaleMouse", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)IN_ScaleMouse);
}

float *mouse_x_ptr, *mouse_y_ptr;

void IN_ScaleMouse_FnDetour_t::IN_ScaleMouse(float* x, float* y)
{
	// hooked in order to retreive these two pointers that are passed into this function.
	// this func is called only with these two passed in.

	if (!mouse_x_ptr)
	{
		mouse_x_ptr = x;
	}

	if (!mouse_y_ptr)
	{
		mouse_y_ptr = x;
	}

	CMemoryFnDetourMgr::the().IN_ScaleMouse().call(x, y);
}

//---------------------------------------------------------------------------------

bool CL_IsThirdPerson_FnDetour_t::install()
{
	initialize("CL_IsThirdPerson", L"client.dll");
	return detour_using_memory_address((uintptr_t*)CL_IsThirdPerson, (uintptr_t*)CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_CL_IsThirdperson);
}

int CL_IsThirdPerson_FnDetour_t::CL_IsThirdPerson()
{
	return thirdperson_dist.get_value() != 0;

	if (thirdperson_dist.get_value() != 0)
	{
		return 1;
	}

	return CMemoryFnDetourMgr::the().CL_IsThirdPerson().call();
}

//---------------------------------------------------------------------------------