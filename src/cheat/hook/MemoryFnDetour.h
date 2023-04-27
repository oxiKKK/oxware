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

#define DECL_GENERIC_MEMORY_FN_DETOUR(call_conv, call_conv_raw)																					\
template<typename T = void, typename ...Args>																									\
class CGenericMemoryFnDetour##call_conv : public CGenericMemoryFnHook##call_conv<T, Args...>													\
{																																				\
	typedef CGenericMemoryFnHook##call_conv<T, Args...> BaseClass;																				\
																																				\
	/* a bunch of convenience typedefs in order to acess base class members (dependence stuff) */												\
	typedef BaseClass::pfnRoutineType_t pfnRoutineType_t;																						\
	using BaseClass::m_module_name;																												\
	using BaseClass::m_name;																													\
	using BaseClass::m_fn_address;																												\
																																				\
public:																																			\
	inline T call_conv_raw call_detoured(Args... args) const { return m_detour_func(args...); }													\
																																				\
	virtual bool install() = 0;																													\
																																				\
	/* if the restore fails, a message is printed to the console. this isn't fatal, */															\
	/* don't exit the application if we fail restoring the detour..					*/															\
	void uninstall()																															\
	{																																			\
		if (!BaseClass::is_installed())																											\
		{																																		\
			CConsole::the().warning("Trying to uninstall not installed detour!");																\
			return;																																\
		}																																		\
																																				\
		CConsole::the().info("Removing detour from function {} from {}...",																		\
							 m_name, CStringTools::the().unicode_to_utf8(m_module_name));														\
																																				\
		restore_internal();																														\
	}																																			\
																																				\
protected:																																		\
	virtual void init_msg() const																												\
	{																																			\
		CConsole::the().info("Detouring {} function from {}...",																				\
							 m_name, CStringTools::the().unicode_to_utf8(m_module_name));														\
	}																																			\
																																				\
	bool detour_internal()																														\
	{																																			\
		DetourTransactionBegin();																												\
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());																	\
		DetourAttach(&reinterpret_cast<PVOID&>(m_fn_address), m_detour_func);																	\
																																				\
		uint32_t error = DetourTransactionCommit();																								\
		if (error != NO_ERROR)																													\
		{																																		\
			CConsole::the().error("Failed to apply detour to a function '{}'. (code={})", m_name, error);										\
			return false;																														\
		}																																		\
																																				\
		return true;																															\
	}																																			\
																																				\
	bool restore_internal()																														\
	{																																			\
		DetourTransactionBegin();																												\
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());																	\
		DetourDetach(&reinterpret_cast<PVOID&>(m_fn_address), m_detour_func);																	\
																																				\
		m_fn_address = nullptr;																													\
		m_detour_func = nullptr;																												\
																																				\
		uint32_t error = DetourTransactionCommit();																								\
		if (error != NO_ERROR)																													\
		{																																		\
			CConsole::the().error("Failed to restore detour on function '{}'. (code={})", m_name, error);										\
			return false;																														\
		}																																		\
																																				\
		return true;																															\
	}																																			\
																																				\
	bool generic_bytepattern_detour(pfnRoutineType_t detour_fn, const CBytePattern& pattern);													\
	bool generic_exported_procname_detour(pfnRoutineType_t detour_fn, const char* procname);													\
	bool generic_functionaddr_detour(pfnRoutineType_t detour_fn, uintptr_t* function_pointer);													\
																																				\
protected:																																		\
	/* our local function that will detour the code flow of the original function, when called */												\
	pfnRoutineType_t m_detour_func = nullptr;																									\
};																																				\
																																				\
template<typename T, typename ...Args>																											\
inline bool CGenericMemoryFnDetour##call_conv<T, Args...>::generic_bytepattern_detour(pfnRoutineType_t detour_fn, const CBytePattern& pattern)	\
{																																				\
	m_detour_func = detour_fn;																													\
																																				\
	/* find the function via byte pattern */																									\
	if (!BaseClass::generic_bytepattern_installer(pattern))																						\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	/* detour it */																																\
	if (!detour_internal())																														\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	return true;																																\
}																																				\
																																				\
template<typename T, typename ...Args>																											\
inline bool CGenericMemoryFnDetour##call_conv<T, Args...>::generic_exported_procname_detour(pfnRoutineType_t detour_fn, const char* procname)	\
{																																				\
	m_detour_func = detour_fn;																													\
																																				\
	/* find exported function */																												\
	if (!BaseClass::generic_exported_procname_installer(procname))																				\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	/* detour it */																																\
	if (!detour_internal())																														\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	return true;																																\
}																																				\
																																				\
template<typename T, typename ...Args>																											\
inline bool CGenericMemoryFnDetour##call_conv<T, Args...>::generic_functionaddr_detour(pfnRoutineType_t detour_fn, uintptr_t* function_pointer)	\
{																																				\
	m_detour_func = detour_fn;																													\
																																				\
	/* find the function via byte pattern */																									\
	if (!BaseClass::generic_functionaddr_installer(function_pointer))																			\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	/* detour it */																																\
	if (!detour_internal())																														\
	{																																			\
		return false;																															\
	}																																			\
																																				\
	return true;																																\
}																																				\

DECL_GENERIC_MEMORY_FN_DETOUR(Cdecl, __cdecl);
DECL_GENERIC_MEMORY_FN_DETOUR(Stdcall, __stdcall);
DECL_GENERIC_MEMORY_FN_DETOUR(Thiscall, __thiscall);
DECL_GENERIC_MEMORY_FN_DETOUR(Fastcall, __fastcall);

//---------------------------------------------------------------------------------

class wglSwapBuffersFnHook_t final : public CGenericMemoryFnDetourStdcall<BOOL, HDC>
{
public:
	bool install();

	static BOOL APIENTRY wglSwapBuffers(HDC hdc);
};

// void __cdecl VGui_CallEngineSurfaceAppHandler(void *event, void *userData)
class VGui_CallEngineSurfaceAppHandlerFnHook_t final : public CGenericMemoryFnDetourCdecl<void, void *, void *>
{
public:
	bool install();

	static void VGui_CallEngineSurfaceAppHandler(void *event, void *userData);
};

// void __cdecl Key_Event(int key, bool down)
class Key_EventFnHook_t final : public CGenericMemoryFnDetourCdecl<void, int, hl::qboolean>
{
public:
	bool install();

	static void Key_Event(int key, hl::qboolean down);
};

// void Host_Noclip_f(void)
class Host_Noclip_fFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void Host_Noclip_f();
};

// void __cdecl ClientDLL_CreateMove(float frametime, usercmd_t *cmd, int active)
class ClientDLL_CreateMoveFnHook_t final : public CGenericMemoryFnDetourCdecl<void, float, hl::usercmd_t*, int>
{
public:
	bool install();

	static void ClientDLL_CreateMove(float frametime, hl::usercmd_t *cmd, int active);
};

// void _Host_Frame(float time)
class _Host_FrameFnHook_t final : public CGenericMemoryFnDetourCdecl<void, float>
{
public:
	bool install();

	static void _Host_Frame(float time);
};

// void __cdecl CL_ReallocateDynamicData(int nMaxClients)
class CL_ReallocateDynamicDataFnHook_t final : public CGenericMemoryFnDetourCdecl<void, int>
{
public:
	bool install();

	static void CL_ReallocateDynamicData(int nMaxClients);
};

// void __cdecl CL_DeallocateDynamicData()
class CL_DeallocateDynamicDataFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void CL_DeallocateDynamicData();
};

// void __cdecl MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
class MYgluPerspectiveFnHook_t final : public CGenericMemoryFnDetourCdecl<void, GLdouble, GLdouble, GLdouble, GLdouble>
{
public:
	bool install();

	static void MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
};

// void __cdecl R_ForceCVars(qboolean mp)
class R_ForceCVarsFnHook_t final : public CGenericMemoryFnDetourCdecl<void, hl::qboolean>
{
public:
	bool install();

	static void R_ForceCVars(hl::qboolean mp);
};

// void __cdecl V_CalcRefdef(ref_params_s *pparams)
class V_CalcRefdefFnHook_t final : public CGenericMemoryFnDetourCdecl<void, hl::ref_params_t*>
{
public:
	bool install();

	static void V_CalcRefdef(hl::ref_params_t *pparams);
};

// void __cdecl EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration)
class EV_HLDM_FireBulletsFnHook_t final : public CGenericMemoryFnDetourCdecl<void, int , float *, float *, float *, int , float *, float *, float*, float, int, int, int*, int>
{
public:
	bool install();

	static void EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration);
};

// int HUD_Redraw(float time, int intermission)
class HUD_RedrawFnHook_t final : public CGenericMemoryFnDetourCdecl<int, float, int>
{
public:
	bool install();

	static int HUD_Redraw(float time, int intermission);
};

// void R_StudioDrawPoints()
class R_StudioDrawPointsFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void R_StudioDrawPoints();
};

// void __cdecl R_LightLambert(float (*light)[4], float *normal, float *src, float *lambert)
class R_LightLambertFnHook_t final : public CGenericMemoryFnDetourCdecl<void, float**, float*, float*, float*>
{
public:
	bool install();

	static void R_LightLambert(float** light, float *normal, float *src, float *lambert);
};

// int V_FadeAlpha()
class V_FadeAlphaFnHook_t final : public CGenericMemoryFnDetourCdecl<int>
{
public:
	bool install();

	static int V_FadeAlpha();
};

// void __cdecl V_ApplyShake(float *origin, float *angles, float factor);
class V_ApplyShakeFnHook_t final : public CGenericMemoryFnDetourCdecl<void, float*, float*, float>
{
public:
	bool install();

	static void V_ApplyShake(float *origin, float *angles, float factor);
};

// void S_StartStaticSound(int entnum, int entchannel, sfx_t* sfx, vec_t* origin, float fvol, float attenuation, int flags, int pitch)
class S_StartStaticSoundFnHook_t final : public CGenericMemoryFnDetourCdecl<void, int, int, hl::sfx_t*, hl::vec_t*, float, float, int, int>
{
public:
	bool install();

	static void S_StartStaticSound(int entnum, int entchannel, hl::sfx_t* sfx, hl::vec_t* origin, float fvol, float attenuation, int flags, int pitch);
};

// void R_DrawViewModel(h)
class R_DrawViewModelFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void R_DrawViewModel();
};

// static CPartSmokeGrenade *__cdeclcall CPartSmokeGrenade::Create(CPartSmokeGrenade* this, Vector org, Vector normal, model_s *sprite, float size, float brightness, const char *classname)
class CPartSmokeGrenade__CreateFnHook_t final : public CGenericMemoryFnDetourCdecl<hl::CPartSmokeGrenade*, hl::CPartSmokeGrenade*, Vector, Vector, hl::model_t*, float, float, const char*>
{
public:
	bool install();

	static hl::CPartSmokeGrenade* __cdecl CPartSmokeGrenade__Create(hl::CPartSmokeGrenade* _this, Vector org, Vector normal, hl::model_t* sprite, float size, float brightness, const char* classname);
};

// void __cdecl CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable)
class CreateGasSmokeFnHook_t final : public CGenericMemoryFnDetourCdecl<void, Vector, Vector, bool, bool, bool>
{
public:
	bool install();

	static void CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable);
};

// void __thiscall CEngine::Unload(CEngine* this)
class CEngine__UnloadFnHook_t final : public CGenericMemoryFnDetourThiscall<void, hl::CEngine*>
{
public:
	bool install();

	static void __thiscall CEngine__Unload(hl::CEngine* ecx);
};

// void __cdecl SCR_CalcRefdef()
class SCR_CalcRefdefFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void SCR_CalcRefdef();
};

// void __cdecl SCR_UpdateScreen()
class SCR_UpdateScreenFnHook_t final : public CGenericMemoryFnDetourCdecl<void>
{
public:
	bool install();

	static void SCR_UpdateScreen();
};

// void SPR_Set(HSPRITE_t hSprite, int r, int g, int b)
class SPR_SetFnHook_t final : public CGenericMemoryFnDetourCdecl<void, hl::HSPRITE_t, int, int, int>
{
public:
	bool install();

	static void SPR_Set(hl::HSPRITE_t hSprite, int r, int g, int b);
};

// void __thiscall CGame__AppActivate(CGame *this, bool fActive)
class CGame__AppActivateFnHook_t final : public CGenericMemoryFnDetourThiscall<void, void*, bool>
{
public:
	bool install();

	static void __thiscall CGame__AppActivate(void* ecx, bool fActive);
};

// int __cdecl CHudAmmo__DrawCrosshair(CHudAmmo *const this, float flTime, int weaponid)
class CHudAmmo__DrawCrosshairFnHook_t final : public CGenericMemoryFnDetourThiscall<int, void*, float, int>
{
public:
	bool install();

	static int __thiscall CHudAmmo__DrawCrosshair(void* ecx, float flTime, int weaponid);
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
	inline auto& wglSwapBuffers() { static wglSwapBuffersFnHook_t fnhook; return fnhook; }
	inline auto& VGui_CallEngineSurfaceAppHandler() { static VGui_CallEngineSurfaceAppHandlerFnHook_t fnhook; return fnhook; }
	inline auto& Key_Event() { static Key_EventFnHook_t fnhook; return fnhook; }
	inline auto& Host_Noclip_f() { static Host_Noclip_fFnHook_t fnhook; return fnhook; }
	inline auto& ClientDLL_CreateMove() { static ClientDLL_CreateMoveFnHook_t fnhook; return fnhook; }
	inline auto& _Host_Frame() { static _Host_FrameFnHook_t fnhook; return fnhook; }
	inline auto& CL_ReallocateDynamicData() { static CL_ReallocateDynamicDataFnHook_t fnhook; return fnhook; }
	inline auto& CL_DeallocateDynamicData() { static CL_DeallocateDynamicDataFnHook_t fnhook; return fnhook; }
	inline auto& MYgluPerspective() { static MYgluPerspectiveFnHook_t fnhook; return fnhook; }
	inline auto& R_ForceCVars() { static R_ForceCVarsFnHook_t fnhook; return fnhook; }
	inline auto& V_CalcRefdef() { static V_CalcRefdefFnHook_t fnhook; return fnhook; }
	inline auto& EV_HLDM_FireBullets() { static EV_HLDM_FireBulletsFnHook_t fnhook; return fnhook; }
	inline auto& HUD_Redraw() { static HUD_RedrawFnHook_t fnhook; return fnhook; }
	inline auto& R_StudioDrawPoints() { static R_StudioDrawPointsFnHook_t fnhook; return fnhook; }
	inline auto& R_LightLambert() { static R_LightLambertFnHook_t fnhook; return fnhook; }
	inline auto& V_FadeAlpha() { static V_FadeAlphaFnHook_t fnhook; return fnhook; }
	inline auto& V_ApplyShake() { static V_ApplyShakeFnHook_t fnhook; return fnhook; }
	inline auto& S_StartStaticSound() { static S_StartStaticSoundFnHook_t fnhook; return fnhook; }
	inline auto& R_DrawViewModel() { static R_DrawViewModelFnHook_t fnhook; return fnhook; }
	inline auto& CPartSmokeGrenade__Create() { static CPartSmokeGrenade__CreateFnHook_t fnhook; return fnhook; }
	inline auto& CreateGasSmoke() { static CreateGasSmokeFnHook_t fnhook; return fnhook; }
	inline auto& CEngine__Unload() { static CEngine__UnloadFnHook_t fnhook; return fnhook; }
	inline auto& SCR_CalcRefdef() { static SCR_CalcRefdefFnHook_t fnhook; return fnhook; }
	inline auto& SCR_UpdateScreen() { static SCR_UpdateScreenFnHook_t fnhook; return fnhook; }
	inline auto& SPR_Set() { static SPR_SetFnHook_t fnhook; return fnhook; }
	inline auto& CGame__AppActivate() { static CGame__AppActivateFnHook_t fnhook; return fnhook; }
	inline auto& CHudAmmo__DrawCrosshair() { static CHudAmmo__DrawCrosshairFnHook_t fnhook; return fnhook; }

	void toggle_unloading_from_CEngine__Unload()
	{
		m_unloading_from_CEngine__Unload = true;
	}

private:
	bool m_unloading_from_CEngine__Unload = false;

	bool m_unloading_hooks_mutex = false;
};

#endif // MEMORYFNDETOUR_H
