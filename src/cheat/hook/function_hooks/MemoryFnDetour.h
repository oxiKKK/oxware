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
// MemoryFnDetour.h -- Resposibility of re-detouring the function code flow to our local implementation 
//					   when the it is called.
//

#ifndef MEMORYFNDETOUR_H
#define MEMORYFNDETOUR_H
#pragma once

template<typename T = void, typename ...Args>
class GenericMemoryFnDetour : public GenericMemoryHook<uintptr_t>
{
public:
	virtual bool install() = 0;

	// if the restore fails, a message is printed to the console. this isn't fatal,
	// don't exit the application if we fail restoring the detour..
	void uninstall()
	{
		if (!is_installed())
		{
			CConsole::the().warning("Trying to uninstall non-installed detour!");
			return;
		}

		if (!m_detoured)
		{
			return;
		}

		CConsole::the().info("Removing detoured '{}' from {}...",
							 m_name, CStringTools::the().utf16_to_utf8(m_module_name));

		restore_internal();
	}

	// the m_address pointer gets changed when the function is detoured. Use this to retreive the original function
	// address.
	uintptr_t* get_original_function_address() const { return m_address; }

protected:
	template <typename U>
	inline auto as_func() const { return reinterpret_cast<U>(m_detoured_address); };

public:
	//
	// testing
	//

	virtual void add_to_test() override
	{
		CHookTests::the().add_for_testing("MemoryFnDetour", this);
	}

protected:
	bool detour_internal()
	{
		m_detoured_address = m_address;

		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourAttach(&reinterpret_cast<PVOID&>(m_detoured_address), m_detour_func_addr);

		uint32_t error = commit_detour_transaction();
		if (error != NO_ERROR)
		{
			CConsole::the().error("Failed to apply detour to a function '{}'. (error code: {})", m_name, error);
			return false;
		}

		m_detoured = true;
		return true;
	}

	bool restore_internal()
	{
		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourDetach(&reinterpret_cast<PVOID&>(m_detoured_address), m_detour_func_addr);

		m_address = m_detoured_address = nullptr;
		m_detour_func_addr = nullptr;

		static int restore_fail_counter = 0; // to not be annoying
		uint32_t error = commit_detour_transaction();
		if (error != NO_ERROR && restore_fail_counter < 5)
		{
			restore_fail_counter++;

			if (restore_fail_counter == 5)
			{
				CMessageBox::display_error("Failed to restore more than 5 detours in a row. This error will not be shown again. For more details, see the console.", m_name, error);
			}
			else
			{
				CMessageBox::display_error("{}: Failed to restore detour from a function '{}'. (error code: {})", restore_fail_counter, m_name, error);
			}

			return false;
		}

		m_detoured = true;
		return true;
	}

	bool detour_using_bytepattern(uintptr_t* detour_fn);
	bool detour_using_exported_name(uintptr_t* detour_fn, const char* export_name);
	bool detour_using_memory_address(uintptr_t* detour_fn, uintptr_t* memory_address);

	uint32_t commit_detour_transaction()
	{
		// save address where the original function was, the value of m_address gets changed by internal detour code after this function call.
		uint32_t error = DetourTransactionCommit(); // this modifies the original address and I don't want to even know why.
		return error;
	}

protected:
	uintptr_t* m_detour_func_addr = nullptr;

	// at first, this is the address of the function we want to detour, but after we detour it, the detour code change pointer of this
	// variable to point to special memory region allocated by detours and this memory is supposed to be called when you're in the detoured function.
	uintptr_t* m_detoured_address = nullptr;

	// true if successfully detoured
	bool m_detoured = false;
};

template<typename T, typename ...Args>
inline bool GenericMemoryFnDetour<T, Args...>::detour_using_bytepattern(uintptr_t* detour_fn)
{
	m_detour_func_addr = detour_fn;

	// find the function via byte pattern 
	if (!install_using_bytepattern(0))
	{
		return false;
	}

	// detour it
	if (!detour_internal())
	{
		return false;
	}

	return true;
}

template<typename T, typename ...Args>
inline bool GenericMemoryFnDetour<T, Args...>::detour_using_exported_name(uintptr_t* detour_fn, const char* export_name)
{
	m_detour_func_addr = detour_fn;

	// find the function via exported name
	if (!install_using_exported_name(export_name))
	{
		return false;
	}

	// detour it
	if (!detour_internal())
	{
		return false;
	}

	return true;
}

template<typename T, typename ...Args>
inline bool GenericMemoryFnDetour<T, Args...>::detour_using_memory_address(uintptr_t* detour_fn, uintptr_t* memory_address)
{
	m_detour_func_addr = detour_fn;

	// find the function via memory address
	if (!install_using_memory_address(memory_address))
	{
		return false;
	}

	// detour it
	if (!detour_internal())
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------

template<typename T = void, typename ...Args>
struct GenericMemoryFnDetour_cdecl : public GenericMemoryFnDetour<uintptr_t>
{
	virtual inline T __cdecl call(Args... args) const { return as_func<T(__cdecl*)(Args...)>()(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnDetour_stdcall : public GenericMemoryFnDetour<uintptr_t>
{
	virtual inline T __stdcall call(Args... args) const { return as_func<T(__stdcall*)(Args...)>()(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnDetour_thiscall : public GenericMemoryFnDetour<uintptr_t>
{
	virtual inline T __thiscall call(Args... args) const { return as_func<T(__thiscall*)(Args...)>()(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnDetour_fastcall : public GenericMemoryFnDetour<uintptr_t>
{
	virtual inline T __fastcall call(Args... args) const { return as_func<T(__fastcall*)(Args...)>()(args...); }
};

//---------------------------------------------------------------------------------

struct wglSwapBuffers_FnDetour_t final : public GenericMemoryFnDetour_stdcall<BOOL, HDC>
{
	bool install();
	static BOOL APIENTRY wglSwapBuffers(HDC hdc);
};

struct glBegin_FnDetour_t final : public GenericMemoryFnDetour_stdcall<void, GLenum>
{
	bool install();
	static void APIENTRY glBegin(GLenum mode);
};

struct glReadPixels_FnDetour_t final : public GenericMemoryFnDetour_stdcall<void, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*>
{
	bool install();
	static void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
};

struct glColor4f_FnDetour_t final : public GenericMemoryFnDetour_stdcall<void, GLfloat, GLfloat, GLfloat, GLfloat>
{
	bool install();
	static void APIENTRY glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
};

// void __cdecl VGui_CallEngineSurfaceAppHandler(void *event, void *userData)
struct VGui_CallEngineSurfaceAppHandler_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, void *, void *>
{
	bool install();
	static void VGui_CallEngineSurfaceAppHandler(void *event, void *userData);
};

// old function that uses winapi rather than SDL2
// note that the function declaration has been completely guessed.
// LRESULT __cdecl VGui_CallEngineSurfaceAppHandler4554(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
struct VGui_CallEngineSurfaceAppHandler4554_FnDetour_t final : public GenericMemoryFnDetour_cdecl<LRESULT, HWND, UINT, WPARAM, LPARAM>
{
	bool install();
	static LRESULT VGui_CallEngineSurfaceAppHandler4554(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

// void __cdecl Key_Event(int key, bool down)
struct Key_Event_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int, hl::qboolean>
{
	bool install();
	static void Key_Event(int key, hl::qboolean down);
};

// void Host_Noclip_f(void)
struct Host_Noclip_f_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void Host_Noclip_f();
};

// void __cdecl CL_CreateMove(float frametime, usercmd_t *cmd, int active)
struct CL_CreateMove_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, float, hl::usercmd_t*, int>
{
	bool install();
	static void CL_CreateMove(float frametime, hl::usercmd_t *cmd, int active);
};

// void _Host_Frame(float time)
struct _Host_Frame_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, float>
{
	bool install();
	static void _Host_Frame(float time);
};

// void __cdecl CL_ReallocateDynamicData(int nMaxClients)
struct CL_ReallocateDynamicData_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int>
{
	bool install();
	static void CL_ReallocateDynamicData(int nMaxClients);
};

// void __cdecl CL_DeallocateDynamicData()
struct CL_DeallocateDynamicData_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void CL_DeallocateDynamicData();
};

// void __cdecl MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
struct MYgluPerspective_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, GLdouble, GLdouble, GLdouble, GLdouble>
{
	bool install();
	static void MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
};

// void __cdecl R_ForceCVars(qboolean mp)
struct R_ForceCVars_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::qboolean>
{
	bool install();
	static void R_ForceCVars(hl::qboolean mp);
};

// void __cdecl V_CalcRefdef(ref_params_s *pparams)
struct V_CalcRefdef_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::ref_params_t*>
{
	bool install();
	static void V_CalcRefdef(hl::ref_params_t *pparams);
};

// void __cdecl EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration)
struct EV_HLDM_FireBullets_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int, float *, float *, float *, int, float *, float *, float*, float, int, int, int*, int>
{
	bool install();
	static void EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration);
};

// int HUD_Redraw(float time, int intermission)
struct HUD_Redraw_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, float, int>
{
	bool install();

	static int HUD_Redraw(float time, int intermission);
};

// void R_GLStudioDrawPoints()
struct R_GLStudioDrawPoints_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void R_GLStudioDrawPoints();
};

// int V_FadeAlpha()
struct V_FadeAlpha_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int>
{
	bool install();
	static int V_FadeAlpha();
};

// void __cdecl V_ApplyShake(float *origin, float *angles, float factor);
struct V_ApplyShake_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, float*, float*, float>
{
	bool install();
	static void V_ApplyShake(float *origin, float *angles, float factor);
};

// void S_StartDynamicSound(int entnum, int entchannel, sfx_t* sfx, vec_t* origin, float fvol, float attenuation, int flags, int pitch)
struct S_StartDynamicSound_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int, int, hl::sfx_t*, hl::vec_t*, float, float, int, int>
{
	bool install();
	static void S_StartDynamicSound(int entnum, int entchannel, hl::sfx_t* sfx, hl::vec_t* origin, float fvol, float attenuation, int flags, int pitch);
};

// void R_DrawViewModel(h)
struct R_DrawViewModel_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void R_DrawViewModel();
};

// static CPartSmokeGrenade *__cdeclcall CPartSmokeGrenade::Create(CPartSmokeGrenade* this, Vector org, Vector normal, model_s *sprite, float size, float brightness, const char *classname)
struct CPartSmokeGrenade__Create_FnDetour_t final : public GenericMemoryFnDetour_cdecl<hl::CPartSmokeGrenade*, hl::CPartSmokeGrenade*, Vector, Vector, hl::model_t*, float, float, const char*>
{
	bool install();
	static hl::CPartSmokeGrenade* __cdecl CPartSmokeGrenade__Create(hl::CPartSmokeGrenade* _this, Vector org, Vector normal, hl::model_t* sprite, float size, float brightness, const char* classname);
};

// void __cdecl CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable)
struct CreateGasSmoke_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, Vector, Vector, bool, bool, bool>
{
	bool install();
	static void CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable);
};

// void __thiscall CEngine::Unload(CEngine* this)
struct CEngine__Unload_FnDetour_t final : public GenericMemoryFnDetour_thiscall<void, hl::CEngine*>
{
	bool install();
	static void __thiscall CEngine__Unload(hl::CEngine* ecx);
};

// void __cdecl SCR_CalcRefdef()
struct SCR_CalcRefdef_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void SCR_CalcRefdef();
};

// void __cdecl SCR_UpdateScreen()
struct SCR_UpdateScreen_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void>
{
	bool install();
	static void SCR_UpdateScreen();
};

// void SPR_Set(HSPRITE_t hSprite, int r, int g, int b)
struct SPR_Set_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::HSPRITE_t, int, int, int>
{
	bool install();
	static void SPR_Set(hl::HSPRITE_t hSprite, int r, int g, int b);
};

// void __thiscall CGame__AppActivate(CGame *this, bool fActive)
struct CGame__AppActivate_FnDetour_t final : public GenericMemoryFnDetour_thiscall<void, void*, bool>
{
	bool install();
	static void __thiscall CGame__AppActivate(void* ecx, bool fActive);
};

// int __cdecl CHudAmmo__DrawCrosshair(CHudAmmo *const this, float flTime, int weaponid)
struct CHudAmmo__DrawCrosshair_FnDetour_t final : public GenericMemoryFnDetour_thiscall<int, void*, float, int>
{
	bool install();
	static int __thiscall CHudAmmo__DrawCrosshair(void* ecx, float flTime, int weaponid);
};

// int R_StudioDrawPlayer(int flags, entity_state_t* pplayer)
struct R_StudioDrawPlayer_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, int, hl::entity_state_t*>
{
	bool install();
	static int R_StudioDrawPlayer(int flags, hl::entity_state_t* pplayer);
};

// void CL_SendConsistencyInfo(sizebuf_t* msg)
struct CL_SendConsistencyInfo_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, hl::sizebuf_t*>
{
	bool install();
	static void CL_SendConsistencyInfo(hl::sizebuf_t* msg);
};

// void SCR_DrawFPS()
struct SCR_DrawFPS_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void SCR_DrawFPS();
};

// void Cmd_AddMallocCommand(char* cmd_name, xcommand_t function, int flag)
struct Cmd_AddMallocCommand_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, char*, hl::xcommand_t, int>
{
	bool install();
	static void Cmd_AddMallocCommand(char* cmd_name, hl::xcommand_t function, int flag);
};

// cvar_t* hudRegisterVariable(char* szName, char* szValue, int flags)
struct hudRegisterVariable_FnDetour_t final : public GenericMemoryFnDetour_cdecl<hl::cvar_t*, char*, char*, int>
{
	bool install();
	static hl::cvar_t* hudRegisterVariable(char* szName, char* szValue, int flags);
};

// void MSG_WriteUsercmd(sizebuf_t* buf, usercmd_t* to, usercmd_t* from);
struct MSG_WriteUsercmd_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::sizebuf_t*, hl::usercmd_t*, hl::usercmd_t*>
{
	bool install();
	static void MSG_WriteUsercmd(hl::sizebuf_t* buf, hl::usercmd_t* to, hl::usercmd_t* from);
};

// int __thiscall CHudSniperScope__Draw(CHudSniperScope *const this, float flTime)
struct CHudSniperScope__Draw_FnDetour_t final : public GenericMemoryFnDetour_thiscall<int, void*, float>
{
	bool install();
	static int __thiscall CHudSniperScope__Draw(void* ecx, float flTime);
};

// int CL_IsThirdPerson()
struct CL_IsThirdPerson_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int>
{
	bool install();
	static int CL_IsThirdPerson();
};

// void __cdecl CL_ProcessEntityUpdate(cl_entity_t *ent)
struct CL_ProcessEntityUpdate_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::cl_entity_t*>
{
	bool install();
	static void CL_ProcessEntityUpdate(hl::cl_entity_t *ent);
};

// void HUD_PostRunCmd(local_state_t* from, local_state_t* to, usercmd_t* cmd, int runfuncs, double time, unsigned int random_seed)
struct HUD_PostRunCmd_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::local_state_t*, hl::local_state_t*, hl::usercmd_t*, int, double, unsigned int>
{
	bool install();
	static void HUD_PostRunCmd(hl::local_state_t* from, hl::local_state_t* to, hl::usercmd_t* cmd, int runfuncs, double time, unsigned int random_seed);
};

// void HUD_CreateEntities();
struct HUD_CreateEntities_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void HUD_CreateEntities();
};

// void HUD_DrawTransparentTriangles();
struct HUD_DrawTransparentTriangles_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void HUD_DrawTransparentTriangles();
};

// void MakeSkyVec(float s, float t, int axis);
struct MakeSkyVec_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, float, float, int>
{
	bool install();
	static void MakeSkyVec(float s, float t, int axis);
};

// void HUD_Frame();
struct HUD_Frame_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void HUD_Frame();
};

// void R_DrawEntitiesOnList();
struct R_DrawEntitiesOnList_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void R_DrawEntitiesOnList();
};

// void R_StudioSetupLighting(alight_t* plighting);
struct R_StudioSetupLighting_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, hl::alight_t*>
{
	bool install();
	static void R_StudioSetupLighting(hl::alight_t* plighting);
};

// void VGui_ViewportPaintBackground(int* extents);
struct VGui_ViewportPaintBackground_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int*>
{
	bool install();
	static void VGui_ViewportPaintBackground(int* extents);
};

// enable this to detour this function that steamclient.dll uses to log stuff.
//#define INTERCEPT_STEAM_LOGGING

#ifdef INTERCEPT_STEAM_LOGGING
// int __cdecl CLogInstance__log(int a1, int a2, int a3, int a4, int a5, int a6, int *a7, int *a8, int *a9, char* message, int a11)
struct CLogInstance__log_FnDetour_t final : public GenericMemoryFnDetour_cdecl<void, int, int, int, int, int, int, int*, int*, int*, char*, int>
{
	bool install();
	static void CLogInstance__log(int a1, int a2, int a3, int a4, int a5, int a6, int *a7, int *a8, int *a9, char* message, int a11);
};
#endif // /INTERCEPT_STEAM_LOGGING

// int Steam_GSInitiateGameConnection(void* pData, int cbMaxData, uint64 steamID, uint32 unIPServer, uint16 usPortServer, qboolean bSecure);
struct Steam_GSInitiateGameConnection_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, void*, int, hl::uint64, hl::uint32, hl::uint16, hl::qboolean>
{
	bool install();
	static int Steam_GSInitiateGameConnection(void* pData, int cbMaxData, hl::uint64 steamID, hl::uint32 unIPServer, hl::uint16 usPortServer, hl::qboolean bSecure);
};

//---------------------------------------------------------------------------------

class CMemoryFnDetourMgr
{
public:
	DECL_BASIC_CLASS(CMemoryFnDetourMgr)

public:
	bool install_hooks();
	void uninstall_hooks();

	// called inside detoured functions that are high-pace, such as Host_Frame. There can be a synchronization issue where
	// if we unload the dll and we unload Host_Frame detour, we cannot call it after it was unloaded. But if we're in a middle
	// of the Host_Frame detour execution, then we call the original funciton (which is already unloaded), we crash.
	bool exit_if_uninstalling() const { return m_unloading_hooks_mutex ? true : false; }

	// individual hooks
	inline auto& wglSwapBuffers() { static wglSwapBuffers_FnDetour_t fnhook; return fnhook; }
	inline auto& glBegin() { static glBegin_FnDetour_t fnhook; return fnhook; }
	inline auto& glReadPixels() { static glReadPixels_FnDetour_t fnhook; return fnhook; }
	inline auto& glColor4f() { static glColor4f_FnDetour_t fnhook; return fnhook; }
	inline auto& VGui_CallEngineSurfaceAppHandler() { static VGui_CallEngineSurfaceAppHandler_FnDetour_t fnhook; return fnhook; }
	inline auto& VGui_CallEngineSurfaceAppHandler4554() { static VGui_CallEngineSurfaceAppHandler4554_FnDetour_t fnhook; return fnhook; }
	inline auto& Key_Event() { static Key_Event_FnDetour_t fnhook; return fnhook; }
	inline auto& Host_Noclip_f() { static Host_Noclip_f_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_CreateMove() { static CL_CreateMove_FnDetour_t fnhook; return fnhook; }
	inline auto& _Host_Frame() { static _Host_Frame_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_ReallocateDynamicData() { static CL_ReallocateDynamicData_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_DeallocateDynamicData() { static CL_DeallocateDynamicData_FnDetour_t fnhook; return fnhook; }
	inline auto& MYgluPerspective() { static MYgluPerspective_FnDetour_t fnhook; return fnhook; }
	inline auto& R_ForceCVars() { static R_ForceCVars_FnDetour_t fnhook; return fnhook; }
	inline auto& V_CalcRefdef() { static V_CalcRefdef_FnDetour_t fnhook; return fnhook; }
	inline auto& EV_HLDM_FireBullets() { static EV_HLDM_FireBullets_FnDetour_t fnhook; return fnhook; }
	inline auto& HUD_Redraw() { static HUD_Redraw_FnDetour_t fnhook; return fnhook; }
	inline auto& R_GLStudioDrawPoints() { static R_GLStudioDrawPoints_FnDetour_t fnhook; return fnhook; }
	inline auto& V_FadeAlpha() { static V_FadeAlpha_FnDetour_t fnhook; return fnhook; }
	inline auto& V_ApplyShake() { static V_ApplyShake_FnDetour_t fnhook; return fnhook; }
	inline auto& S_StartDynamicSound() { static S_StartDynamicSound_FnDetour_t fnhook; return fnhook; }
	inline auto& R_DrawViewModel() { static R_DrawViewModel_FnDetour_t fnhook; return fnhook; }
	inline auto& CPartSmokeGrenade__Create() { static CPartSmokeGrenade__Create_FnDetour_t fnhook; return fnhook; }
	inline auto& CreateGasSmoke() { static CreateGasSmoke_FnDetour_t fnhook; return fnhook; }
	inline auto& CEngine__Unload() { static CEngine__Unload_FnDetour_t fnhook; return fnhook; }
	inline auto& SCR_CalcRefdef() { static SCR_CalcRefdef_FnDetour_t fnhook; return fnhook; }
	inline auto& SCR_UpdateScreen() { static SCR_UpdateScreen_FnDetour_t fnhook; return fnhook; }
	inline auto& SPR_Set() { static SPR_Set_FnDetour_t fnhook; return fnhook; }
	inline auto& CGame__AppActivate() { static CGame__AppActivate_FnDetour_t fnhook; return fnhook; }
	inline auto& CHudAmmo__DrawCrosshair() { static CHudAmmo__DrawCrosshair_FnDetour_t fnhook; return fnhook; }
	inline auto& R_StudioDrawPlayer() { static R_StudioDrawPlayer_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_SendConsistencyInfo() { static CL_SendConsistencyInfo_FnDetour_t fnhook; return fnhook; }
	inline auto& SCR_DrawFPS() { static SCR_DrawFPS_FnDetour_t fnhook; return fnhook; }
	inline auto& Cmd_AddMallocCommand() { static Cmd_AddMallocCommand_FnDetour_t fnhook; return fnhook; }
	inline auto& hudRegisterVariable() { static hudRegisterVariable_FnDetour_t fnhook; return fnhook; }
	inline auto& MSG_WriteUsercmd() { static MSG_WriteUsercmd_FnDetour_t fnhook; return fnhook; }
	inline auto& CHudSniperScope__Draw() { static CHudSniperScope__Draw_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_IsThirdPerson() { static CL_IsThirdPerson_FnDetour_t fnhook; return fnhook; }
	inline auto& CL_ProcessEntityUpdate() { static CL_ProcessEntityUpdate_FnDetour_t fnhook; return fnhook; }
	inline auto& HUD_PostRunCmd() { static HUD_PostRunCmd_FnDetour_t fnhook; return fnhook; }
	inline auto& HUD_CreateEntities() { static HUD_CreateEntities_FnDetour_t fnhook; return fnhook; }
	inline auto& HUD_DrawTransparentTriangles() { static HUD_DrawTransparentTriangles_FnDetour_t fnhook; return fnhook; }
	inline auto& MakeSkyVec() { static MakeSkyVec_FnDetour_t fnhook; return fnhook; }
	inline auto& HUD_Frame() { static HUD_Frame_FnDetour_t fnhook; return fnhook; }
	inline auto& R_DrawEntitiesOnList() { static R_DrawEntitiesOnList_FnDetour_t fnhook; return fnhook; }
	inline auto& R_StudioSetupLighting() { static R_StudioSetupLighting_FnDetour_t fnhook; return fnhook; }
	inline auto& VGui_ViewportPaintBackground() { static VGui_ViewportPaintBackground_FnDetour_t fnhook; return fnhook; }
#ifdef INTERCEPT_STEAM_LOGGING
	inline auto& CLogInstance__log() { static CLogInstance__log_FnDetour_t fnhook; return fnhook; }
#endif
	inline auto& Steam_GSInitiateGameConnection() { static Steam_GSInitiateGameConnection_FnDetour_t fnhook; return fnhook; }

	void toggle_unloading_from_CEngine__Unload()
	{
		m_unloading_from_CEngine__Unload = true;
	}

private:
	bool m_unloading_from_CEngine__Unload = false;

	bool m_unloading_hooks_mutex = false;
};

#endif // MEMORYFNDETOUR_H
