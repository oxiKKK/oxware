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

// objects that reperesents a generic hookable memory address.
template<typename T>
class GenericMemoryHook : public TestableHook
{
public:
	inline bool is_initialized() const { return !m_module_name.empty() && !m_name.empty(); }
	inline bool is_installed() const { return is_initialized() && m_address != nullptr; }

	// getters
	inline std::string get_name() const { return m_name; }
	inline std::string get_module_name() const { return m_name; }

	virtual bool install() = 0;

protected:
	inline void initialize(const char* name, const wchar_t* module_name)
	{
		m_module_name = module_name;
		m_name = name;

#ifdef OX_ENABLE_HOOK_TESTING
		add_to_test();
#endif
	}

	// generic handlers for finding the hook inside memory
	virtual bool install_using_bytepattern(size_t dereference_count, const std::function<void(uintptr_t** address)>& modify_address = NULL);
	virtual bool install_using_exported_name(const char* export_name);
	virtual bool install_using_memory_address(uintptr_t* memory_address, size_t dereference_count = 0, const std::function<void(uintptr_t** address)>& modify_address = NULL);

private:
	//
	// convenience code
	//

	void on_hook_install_begin(const char* install_method)
	{
		assert(is_initialized() && "hook must be initialized first before installed!");

		CConsole::the().info("Installing hook '{}' ({}) using '{}'...", m_name, CStringTools::the().utf16_to_utf8(m_module_name), install_method);
	}

	void on_hook_install_success()
	{
		CConsole::the().info("Found at 0x{:08X}", (uintptr_t)m_address);
	}

	template <class... _Types>
	void on_hook_install_fail(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		std::string formatted_error = std::vformat(_Fmt.get(), std::make_format_args(_Args...));

		CMessageBox::display_error("Failed to install hook '{}' from '{}' !!!\n\nDetails: {}",
								   m_name, CStringTools::the().utf16_to_utf8(m_module_name),
								   formatted_error);
	}

public:
	//
	// testing
	//

	virtual void add_to_test()
	{
		CHookTests::the().add_for_testing("MemoryHook", this);
	}

	// default testing for this hook
	virtual void default_test_hook()
	{
		// see if the address isn't invalid
		CHookTests::the().run_seh_protected_block(
			m_name,
			[&]()
			{
				return is_within_address_space();
			}, 
			true);
	}

protected:
	bool is_within_address_space()
	{
		auto [base, end] = g_libloader_i->get_loaded_dll_address_space(m_module_name.c_str(), SPACE_FULL);

		if (!base || !end)
		{
			return false;
		}

		uintptr_t addr = (uintptr_t)m_address;

		// see if there's a detour installed
		uint8_t op = *((uint8_t*)m_address);
		bool is_detoured = op == (uint8_t)'\xE9';

		// see if in address space of presumed module.
		bool out_of_range = (addr >= base && addr < ((end - 1) - sizeof(uintptr_t)));
		if (!out_of_range)
		{
			// if it's detoured, then it's completely outside of the dll address space
			if (!is_detoured)
			{
				CConsole::the().error("Hook '{}' out of range: is 0x{:08X} but the range is [0x{:08X}; 0x{:08X}] (module: {})",
									 m_name, addr, base, ((end - 1) - sizeof(uintptr_t)), CStringTools::the().utf16_to_utf8(m_module_name));
				return false;
			}
		}

		return true; // ok
	}

public:
	// get either base or specific element if this is an array.
	T* get() const
	{
		assert(is_installed() && "The hook isn't installed!");
		assert(m_address && "Accessing nullptr hook pointer!");

		return reinterpret_cast<T*>(m_address);
	}

protected:
	std::wstring m_module_name = L"";
	std::string m_name = "";
	uintptr_t* m_address = nullptr;
};

// Searches for a byte pattern of where the memory you wanna hook is used (e.g. a global variable), then dereferences it to get its base address.
template<typename T>
inline bool GenericMemoryHook<T>::install_using_bytepattern(size_t dereference_count, const std::function<void(uintptr_t** address)>& modify_address)
{
	on_hook_install_begin("byte pattern");

	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(m_module_name.c_str(), SPACE_CODE);
	if (!base || !end)
	{
		on_hook_install_fail("Can't install hook. Dll probably not loaded in address space.");
		return false;
	}

	CBytePattern p = g_bytepattern_bank_i->get_pattern(m_name);
//	CConsole::the().dinfo("{}", p.pattern_as_string()); // print pattern
	if (p.empty())
	{
		on_hook_install_fail("Couldn't find byte pattern for current hook inside the bank!!!");
		return false;
	}

	m_address = p.search_in_loaded_address_space(base, end);
	if (!m_address) 
	{
		on_hook_install_fail("Byte pattern inside the module couldn't be found!");
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

	on_hook_install_success();
	return true;
}

template<typename T>
inline bool GenericMemoryHook<T>::install_using_exported_name(const char* export_name)
{
	on_hook_install_begin("exported name");

	uintptr_t module_base = (uintptr_t)g_libloader_i->load_library(NULL, m_module_name.c_str());
	if (!module_base)
	{
		on_hook_install_fail("Can't install hook. Dll couldn't be found inside process memory.");
		return false;
	}

	m_address = (uintptr_t*)g_libloader_i->find_proc_in_target_library(module_base, export_name);
	if (!m_address)
	{
		on_hook_install_fail("Exported name '{}' couldn't be found inside the module.", export_name);
		return false;
	}

	on_hook_install_success();
	return true;
}

template<typename T>
inline bool GenericMemoryHook<T>::install_using_memory_address(uintptr_t* memory_address, size_t dereference_count, const std::function<void(uintptr_t** address)>& modify_address)
{
	on_hook_install_begin("memory address");

	m_address = memory_address;
	if (!m_address)
	{
		on_hook_install_fail("Can't install hook, the memory address provided for the hook is nullptr!");
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

	on_hook_install_success();
	return true;
}

//-----------------------------------------------------------------------------

template <typename T>
class RestorableGenericMemoryHook : public GenericMemoryHook<T>
{
public:
	void restore()
	{
		if (GenericMemoryHook<T>::m_address)
		{
			memcpy(GenericMemoryHook<T>::m_address, &m_saved, sizeof(T));
		}
	}

private:
	T m_saved;
};

//-----------------------------------------------------------------------------

// HWND *pmainwindow;
struct pmainwindow_MemoryHook final : public GenericMemoryHook<HWND*>
{
	bool install() override;
	void test_hook() override;
};

// qboolean host_initialized
struct host_initialized_MemoryHook final : public GenericMemoryHook<hl::qboolean>
{
	bool install() override;
	void test_hook() override;
};

// edict_t* sv_player
struct sv_player_MemoryHook final : public GenericMemoryHook<hl::edict_t*>
{
	bool install() override;
};

// client_state_t cl;
struct cl_MemoryHook final : public GenericMemoryHook<hl::client_state_t>
{
	bool install() override;
	void test_hook() override;
};

// globalvars_t gGlobalVariables;
struct gGlobalVariables_MemoryHook final : public GenericMemoryHook<hl::globalvars_t>
{
	bool install() override;
	void test_hook() override;
};

// float scr_fov_value;
struct scr_fov_value_MemoryHook final : public GenericMemoryHook<float>
{
	bool install() override; 
	void test_hook() override;
};

// extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];
struct g_PlayerExtraInfo_MemoryHook final : public GenericMemoryHook<hl::extra_player_info_t> 
{
	bool install() override; 
	void test_hook() override;
};

// extra_player_info_old_t g_PlayerExtraInfo[MAX_PLAYERS + 1];
// this version forbids some members that were added in 8684
struct g_PlayerExtraInfoOld_MemoryHook final : public GenericMemoryHook<hl::extra_player_info_old_t>
{
	bool install() override; 
	void test_hook() override;
};

// engine_studio_api_t engine_studio_api
struct engine_studio_api_MemoryHook final : public GenericMemoryHook<hl::engine_studio_api_t> 
{
	bool install() override;
	void test_hook() override;
};

// svc_func_t cl_parsefuncs[]
struct cl_parsefuncs_MemoryHook final : public GenericMemoryHook<hl::svc_func_t> 
{ 
	bool install() override;
	void test_hook() override;
};

// playermove_t* pmove;
struct pmove_MemoryHook final : public GenericMemoryHook<hl::playermove_t*>
{ 
	bool install() override;
	void test_hook() override;
};

// UserMsg* gClientUserMsgs;
struct gClientUserMsgs_MemoryHook final : public GenericMemoryHook<hl::UserMsg*> 
{ 
	bool install() override; 
	void test_hook() override;
};

// int g_iShotsFired;
struct g_iShotsFired_MemoryHook final : public GenericMemoryHook<int> 
{ 
	bool install() override; 
};

// model_t* r_model;
// currently rendered model inside the studio code.
struct r_model_MemoryHook final : public GenericMemoryHook<hl::model_t*> 
{ 
	bool install() override; 
};

// studiohdr_t* pstudiohdr;
// header of currently rendered model inside the studio code.
struct pstudiohdr_MemoryHook final : public GenericMemoryHook<hl::studiohdr_t*> 
{
	bool install() override; 
};

// r_studio_interface_t* pStudioAPI;
struct pStudioAPI_MemoryHook final : public GenericMemoryHook<hl::r_studio_interface_t*> 
{ 
	bool install() override; 
	void test_hook() override;
};

// cl_entity_t* currententity;
struct currententity_MemoryHook final : public GenericMemoryHook<hl::cl_entity_t*>
{ 
	bool install() override; 
};

// int host_framecount;
struct host_framecount_MemoryHook final : public GenericMemoryHook<int>
{ 
	bool install() override; 
	void test_hook() override;
};

// double realtime;
struct realtime_MemoryHook final : public GenericMemoryHook<double>
{ 
	bool install() override; 
	void test_hook() override;
};

// IVideoMode* videomode;
struct IVideoMode_MemoryHook final : public GenericMemoryHook<hl::IVideoMode*>
{ 
	bool install() override; 
	void test_hook() override;
};

// double 1000.0;
//
// This is a strange one. This hook hooks a global variable generated by the compiler when the binary is built.
// It is a constant (1000.0) that is frequently used within the hw.dll code in functions like CL_Move, CL_AdjustClock
// or CL_RunUsercmd. This is used by some client-side movement-related functions in formulas that affects the overall
// frametimes when dealing with the movement. I have no fucking idea who discovered this, but it's the most random
// thing I have ever seen. Hooking random compiler-generated global variable, that is just a "1000.0" constant defined
// inside code, that is randomly distributed all over the entire code as a compiler optimization... Heh, I have just no words...
// Well, as stupid as it may seem, it really works...
//
// see the commentary inside classic_cs16_cheating_scene_speedhack() more more details.
//
struct random_1k_speedhack_modifier_constant_MemoryHook final : public GenericMemoryHook<double>
{ 
	bool install() override; 
	void test_hook() override;
};

// keydest_t key_dest;
struct key_dest_MemoryHook final : public GenericMemoryHook<hl::keydest_t>
{ 
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

	//
	// individual hooks
	//

	inline static auto& pmainwindow() { static pmainwindow_MemoryHook hook; return hook; };
	inline static auto& host_initialized() { static host_initialized_MemoryHook hook; return hook; };
	inline static auto& sv_player() { static sv_player_MemoryHook hook; return hook; };
	inline static auto& cl() { static cl_MemoryHook hook; return hook; };
	inline static auto& gGlobalVariables() { static gGlobalVariables_MemoryHook hook; return hook; };
	inline static auto& scr_fov_value() { static scr_fov_value_MemoryHook hook; return hook; };
	inline static auto& g_PlayerExtraInfo() { static g_PlayerExtraInfo_MemoryHook hook; return hook; };
	inline static auto& g_PlayerExtraInfoOld() { static g_PlayerExtraInfoOld_MemoryHook hook; return hook; };
	inline static auto& engine_studio_api() { static engine_studio_api_MemoryHook hook; return hook; };
	inline static auto& cl_parsefuncs() { static cl_parsefuncs_MemoryHook hook; return hook; };
	inline static auto& pmove() { static pmove_MemoryHook hook; return hook; };
	inline static auto& gClientUserMsgs() { static gClientUserMsgs_MemoryHook hook; return hook; };
	inline static auto& g_iShotsFired() { static g_iShotsFired_MemoryHook hook; return hook; };
	inline static auto& r_model() { static r_model_MemoryHook hook; return hook; };
	inline static auto& pstudiohdr() { static pstudiohdr_MemoryHook hook; return hook; };
	inline static auto& pStudioAPI() { static pStudioAPI_MemoryHook hook; return hook; };
	inline static auto& currententity() { static currententity_MemoryHook hook; return hook; };
	inline static auto& host_framecount() { static host_framecount_MemoryHook hook; return hook; };
	inline static auto& realtime() { static realtime_MemoryHook hook; return hook; };
	inline static auto& IVideoMode() { static IVideoMode_MemoryHook hook; return hook; };
	inline static auto& random_1k_speedhack_modifier_constant() { static random_1k_speedhack_modifier_constant_MemoryHook hook; return hook; };
	inline static auto& key_dest() { static key_dest_MemoryHook hook; return hook; };

	//
	// we can get these from elsewhere
	//

	static hl::cl_enginefunc_t* cl_enginefuncs();
	static hl::cldll_func_t* cl_funcs();
	static hl::client_static_t* cls();
	static char** keybindings();
};

#endif // MEMORYHOOK_H
