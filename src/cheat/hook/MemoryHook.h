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
// MemoryHook.h -- Resposibility of hooking an address inside memory of another dll.
//

#ifndef MEMORYHOOK_H
#define MEMORYHOOK_H
#pragma once

template<typename T>
class CGenericMemHook
{
public:
	inline bool is_installed() const { return is_initialized() && m_address != nullptr; }
	inline std::string get_name() const { return m_name; }
	inline std::string get_module_name() const { return m_name; }

	virtual bool install() = 0;

	void uninstall()
	{
		return; // this caused some issues, and isn't really needed anywehere so just disable it for now. TODO.
		if (!is_installed())
			return;

		if (m_make_copy && m_address)
		{
			memcpy(m_address , &m_copy, sizeof(T)); // restore original memory
		}
	}

	// store copy of the data we're hooking locally.
	void make_memory_snapshot()
	{
		if (m_make_copy && m_address)
		{
			memcpy(&m_copy, m_address, sizeof(T));
		}
	}
	
protected:
	inline void initialize(const char* name, const wchar_t* module_name, bool make_copy = true)
	{
		assert(!is_installed());
		m_name = name;
		m_module_name = module_name;
		m_make_copy = make_copy;
		CConsole::the().info("Installing {} hook from {}...", m_name, CStringTools::the().unicode_to_utf8(m_module_name));
	}

	// generic handles for finding the hook inside memory
	bool generic_bytepattern_installer(const CBytePattern& pattern, size_t dereference_count = 1, const std::function<void(uintptr_t** address)>& modify_address = NULL);

	inline bool is_initialized() const { return !m_module_name.empty() && !m_name.empty(); }

public:
	// get either base or specific element if this is an array.
	T* get() const { assert(is_installed() && "The hook isn't installed!"); return reinterpret_cast<T*>(m_address); }

private:
	std::wstring m_module_name = L"";
	std::string m_name = "";
	uintptr_t* m_address = nullptr;

	// this holds the copy of the object after its hooked. This is so that if the object data
	// is modified, we can then restore its original values my copying this data to the memory
	// after we're unloading.
	T m_copy;
	bool m_make_copy = false; // this is optional
};

// Searches for a byte pattern of where the memory you wanna hook is used (e.g. a global variable), then dereferences it to get its base address.
template<typename T>
inline bool CGenericMemHook<T>::generic_bytepattern_installer(const CBytePattern& pattern, size_t dereference_count, const std::function<void(uintptr_t** address)>& modify_address)
{
	assert(is_initialized());

	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(m_module_name.c_str(), SPACE_CODE);

	if (!base || !end)
	{
		CConsole::the().error("Can't install hook. Dll probably not loaded in address space.", 
							  CStringTools::the().unicode_to_utf8((m_module_name.c_str())));
		return false;
	}

	m_address = pattern.search_in_loaded_address_space(base, end);
	if (!m_address)
	{
		CConsole::the().error("Failed to install {} hook from {}. Byte pattern is '{}'",
							  m_name, CStringTools::the().unicode_to_utf8(m_module_name), pattern.pattern_as_string());
		return false;
	}

	// dereference it to get the base address of it
	for (size_t i = 0; i < dereference_count; i++)
	{
		m_address = *reinterpret_cast<uintptr_t**>(m_address);
	}

	if (modify_address)
	{
		modify_address(&m_address);
	}

	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_address);

	make_memory_snapshot();

	return true;
}

//-----------------------------------------------------------------------------

class CClDllFuncHook final : public CGenericMemHook<hl::cldll_func_t>
{
public: 
	bool install() override;
};

// cl_enginefunc_t gEngfuncs;
// obtained from ClientDLL_Init()
class cl_enginefuncsHook final : public CGenericMemHook<hl::cl_enginefunc_t>
{
public: 
	bool install() override;
};

// HWND *pmainwindow;
class pmainwindowHook final : public CGenericMemHook<HWND*>
{
public: 
	bool install() override;
};

// qboolean host_initialized
class host_initializedHook final : public CGenericMemHook<hl::qboolean>
{
public: 
	bool install() override;
};

// edict_t* sv_player
// obtained from Host_God_f()
class sv_playerHook final : public CGenericMemHook<hl::edict_t*>
{
public: 
	bool install() override;
};

// client_state_t cl;
// obtained from CL_ClearClientState()
class clHook final : public CGenericMemHook<hl::client_state_t>
{
public: 
	bool install() override;
};

// client_static_t cls;
// obtained from CL_ConnectClient()
class clsHook final : public CGenericMemHook<hl::client_static_t>
{
public: 
	bool install() override;
};

// globalvars_t gGlobalVariables;
// obtained from CL_ConnectClient()
class gGlobalVariablesHook final : public CGenericMemHook<hl::globalvars_t>
{
public: 
	bool install() override;
};

// float scr_fov_value;
// obtained from CL_ConnectClient()
class scr_fov_valueHook final : public CGenericMemHook<float>
{
public: 
	bool install() override;
};

// extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];
// obtained from ...
class g_PlayerExtraInfoHook final : public CGenericMemHook<hl::extra_player_info_t>
{
public: 
	bool install() override;
};

// engine_studio_api_t engine_studio_api
// obtained from ...
class engine_studio_apiHook final : public CGenericMemHook<hl::engine_studio_api_t>
{
public: 
	bool install() override;
};

// svc_func_t cl_parsefuncs[]
// obtained from ...
class cl_parsefuncsHook final : public CGenericMemHook<hl::svc_func_t>
{
public: 
	bool install() override;
};

// playermove_t* pmove;
// obtained from ...
class pmoveHook final : public CGenericMemHook<hl::playermove_t>
{
public: 
	bool install() override;
};

// UserMsg* gClientUserMsgs;
// obtained from ...
class gClientUserMsgsHook final : public CGenericMemHook<hl::UserMsg*>
{
public: 
	bool install() override;
};

//-----------------------------------------------------------------------------

class CMemoryHookMgr
{
public:
	DECL_BASIC_CLASS(CMemoryHookMgr);

public:
	bool install_hooks();
	void uninstall_hooks();

	// individual hooks

	inline static auto& cldllfunc() { static CClDllFuncHook hook; return hook; };
	inline static auto& cl_enginefuncs() { static cl_enginefuncsHook hook; return hook; };
	inline static auto& pmainwindow() { static pmainwindowHook hook; return hook; };
	inline static auto& host_initialized() { static host_initializedHook hook; return hook; };
	inline static auto& sv_player() { static sv_playerHook hook; return hook; };
	inline static auto& cl() { static clHook hook; return hook; };
	inline static auto& cls() { static clsHook hook; return hook; };
	inline static auto& gGlobalVariables() { static gGlobalVariablesHook hook; return hook; };
	inline static auto& scr_fov_value() { static scr_fov_valueHook hook; return hook; };
	inline static auto& g_PlayerExtraInfo() { static g_PlayerExtraInfoHook hook; return hook; };
	inline static auto& engine_studio_api() { static engine_studio_apiHook hook; return hook; };
	inline static auto& cl_parsefuncs() { static cl_parsefuncsHook hook; return hook; };
	inline static auto& pmove() { static pmoveHook hook; return hook; };
	inline static auto& gClientUserMsgs() { static gClientUserMsgsHook hook; return hook; };
};

#endif // MEMORYHOOK_H
