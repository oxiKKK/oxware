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

#ifndef SVCFUNCDETOUR_H
#define SVCFUNCDETOUR_H
#pragma once

using pfnSVCRoutine_t = void(__cdecl*)();

class CGenericSVCFnDetour
{
public:
	inline void __cdecl call() const { reinterpret_cast<pfnSVCRoutine_t>(m_fn_address)(); }

	inline bool is_initialized() const { return !m_name.empty(); }
	inline bool is_installed() const { return is_initialized() && m_fn_address != nullptr; }

	bool install(hl::svc_parsefn_t func, const char* svc_name, int idx)
	{
		assert(idx > svc_bad && idx < SVC_LASTMSG);

		m_name = svc_name;

		auto svc_func = &CMemoryHookMgr::the().cl_parsefuncs().get()[idx];

		return generic_functionaddr_detour(func, (uintptr_t*)svc_func->pfnParse);
	}

	/* if the restore fails, a message is printed to the console. this isn't fatal, */
	/* don't exit the application if we fail restoring the detour..					*/
	void uninstall()
	{
		if (!is_installed())
			return;

		CConsole::the().info("Removing detoured SVC {}...", m_name);

		restore_internal();
	}

protected:
	void init_msg() const
	{
		CConsole::the().info("Detouring {} SVC function...", m_name);
	}

	bool detour_internal()
	{
		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourAttach(&reinterpret_cast<PVOID&>(m_fn_address), m_detoured_svc);

		uint32_t error = DetourTransactionCommit();
		if (error != NO_ERROR)
		{
			CConsole::the().error("Failed to apply detour to SVC '{}'. (code={})", m_name, error);
			return false;
		}

		return true;
	}

	bool restore_internal()
	{
		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourDetach(&reinterpret_cast<PVOID&>(m_fn_address), m_detoured_svc);

		m_fn_address = nullptr;
		m_detoured_svc = nullptr;

		uint32_t error = DetourTransactionCommit();
		if (error != NO_ERROR)
		{
			CConsole::the().error("Failed to restore detour on SVC '{}'. (code={})", m_name, error);
			return false;
		}

		return true;
	}

	bool generic_functionaddr_detour(pfnSVCRoutine_t detour_fn, uintptr_t* function_pointer);
	bool generic_functionaddr_installer(uintptr_t* function_pointer);

protected:
	pfnSVCRoutine_t m_detoured_svc = nullptr;

	uintptr_t* m_fn_address = nullptr;

	std::string m_name = "empty";
};

inline bool CGenericSVCFnDetour::generic_functionaddr_detour(pfnSVCRoutine_t detour_fn, uintptr_t* function_pointer)
{
	m_detoured_svc = detour_fn;

	/* find the function via byte pattern */
	if (!generic_functionaddr_installer(function_pointer))
	{
		return false;
	}

	/* detour it */
	if (!detour_internal())
	{
		return false;
	}

	return true;
}

inline bool CGenericSVCFnDetour::generic_functionaddr_installer(uintptr_t* function_pointer)
{
	m_fn_address = function_pointer;
	if (!m_fn_address)
	{
		CConsole::the().error("Failed to install {} SVC hook. The function pointer specified is NULL.", m_name);
		return false;
	}

	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_fn_address);

	return true;
}

//-------------------------------------------------------------------------------------------

class CSVCFuncDetourMgr
{
public:
	DECL_BASIC_CLASS(CSVCFuncDetourMgr);

public:
	bool install_hooks();
	void uninstall_hooks();

	// detoured SVC hooks
	inline auto& svc_sound_fn() { static CGenericSVCFnDetour fnhook; return fnhook; }
	inline auto& svc_time_fn() { static CGenericSVCFnDetour fnhook; return fnhook; }
	inline auto& svc_sendcvarvalue_fn() { static CGenericSVCFnDetour fnhook; return fnhook; }
	inline auto& svc_sendcvarvalue2_fn() { static CGenericSVCFnDetour fnhook; return fnhook; }
	inline auto& svc_stufftext_fn() { static CGenericSVCFnDetour fnhook; return fnhook; }

private:
	static void svc_sound_f();
	static void svc_time_f();
	static void svc_sendcvarvalue_f();
	static void svc_sendcvarvalue2_f();
	static void svc_stufftext_f();
};

#endif // SVCFUNCDETOUR_H
