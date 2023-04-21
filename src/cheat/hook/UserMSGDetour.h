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

#ifndef USERMSGDETOUR_H
#define USERMSGDETOUR_H
#pragma once

using pfnUserMSGRoutine_t = hl::pfnUserMsgHook;

class CGenericUserMSGFnDetour
{
public:
	inline int __cdecl call(int iSize, void* pbuf) const { return reinterpret_cast<pfnUserMSGRoutine_t>(m_fn_address)(m_name.c_str(), iSize, pbuf); }

	inline bool is_initialized() const { return !m_name.empty(); }
	inline bool is_installed() const { return is_initialized() && m_fn_address != nullptr; }

	bool install(pfnUserMSGRoutine_t func, const char* usermsg_name)
	{
		m_name = usermsg_name;

		// find the right entry
		hl::UserMsg* entry = *CMemoryHookMgr::the().gClientUserMsgs().get();
		while (entry)
		{
			if (!stricmp(entry->szName, usermsg_name))
			{
				return generic_functionaddr_detour(func, (uintptr_t*)entry->pfn);
			}

			entry = entry->next;
		}

		CConsole::the().error("Couldn't find usermsg {}", usermsg_name);
		return false;
	}

	/* if the restore fails, a message is printed to the console. this isn't fatal, */
	/* don't exit the application if we fail restoring the detour..					*/
	void uninstall()
	{
		if (!is_installed())
			return;

		CConsole::the().info("Removing detoured UserMSG {}...", m_name);

		restore_internal();
	}

protected:
	void init_msg() const
	{
		CConsole::the().info("Detouring {} UserMSG function...", m_name);
	}

	bool detour_internal()
	{
		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourAttach(&reinterpret_cast<PVOID&>(m_fn_address), m_detoured_umsg);

		uint32_t error = DetourTransactionCommit();
		if (error != NO_ERROR)
		{
			CConsole::the().error("Failed to apply detour to UserMSG '{}'. (code={})", m_name, error);
			return false;
		}

		return true;
	}

	bool restore_internal()
	{
		DetourTransactionBegin();
		DetourUpdateThread(CGenericUtil::the().get_current_thread_handle());
		DetourDetach(&reinterpret_cast<PVOID&>(m_fn_address), m_detoured_umsg);

		m_fn_address = nullptr;
		m_detoured_umsg = nullptr;

		uint32_t error = DetourTransactionCommit();
		if (error != NO_ERROR)
		{
			CConsole::the().error("Failed to restore detour on UserMSG '{}'. (code={})", m_name, error);
			return false;
		}

		return true;
	}

	bool generic_functionaddr_detour(pfnUserMSGRoutine_t detour_fn, uintptr_t* function_pointer);
	bool generic_functionaddr_installer(uintptr_t* function_pointer);

protected:
	pfnUserMSGRoutine_t m_detoured_umsg = nullptr;

	uintptr_t* m_fn_address = nullptr;

	std::string m_name = "empty";
};

inline bool CGenericUserMSGFnDetour::generic_functionaddr_detour(pfnUserMSGRoutine_t detour_fn, uintptr_t* function_pointer)
{
	m_detoured_umsg = detour_fn;

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

inline bool CGenericUserMSGFnDetour::generic_functionaddr_installer(uintptr_t* function_pointer)
{
	m_fn_address = function_pointer;
	if (!m_fn_address)
	{
		CConsole::the().error("Failed to install {} UserMSG hook. The function pointer specified is NULL.", m_name);
		return false;
	}

	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_fn_address);

	return true;
}

//-------------------------------------------------------------------------------------------

class CUserMSGDetourMgr
{
public:
	DECL_BASIC_CLASS(CUserMSGDetourMgr);

public:
	bool install_hooks();
	void uninstall_hooks();

	// detoured UserMSG hooks
	inline auto& HideWeapon_fn() { static CGenericUserMSGFnDetour fnhook; return fnhook; }

private:
	static int HideWeapon_f(const char* pszName, int iSize, void* pbuf);
};

#endif // USERMSGDETOUR_H
