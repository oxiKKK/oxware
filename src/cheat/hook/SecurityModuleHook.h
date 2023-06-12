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

#ifndef SECURITYMODULEHOOK_H
#define SECURITYMODULEHOOK_H
#pragma once

//-----------------------------------------------------------------------------

// void __cdecl CommonLoadSecurityModule(void *nAddrModule)
struct CommonLoadSecurityModule_FnHook_t final : public GenericMemoryFnHook_cdecl<void, void*> { bool install(); };

// void CloseSecurityModule(void);
struct CloseSecurityModule_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void CloseSecurityModule();
};

// modfuncs_t g_modfuncs
struct g_modfuncs_MemoryHook final : public GenericMemoryHook<hl::modfuncs_t>
{
	bool install() override;
	void test_hook() override;
};

//-----------------------------------------------------------------------------

template<class T>
struct callback_table_hook_t
{
	// store the pointer as well as the original one, so that we can restore pointers on unload later.
	T* ptr, original;

	void install(const char* name, T* _ptr, const T& original);
	void restore()
	{
		*ptr = original;
	}
};

class CSecurityModuleHook
{
public:
	DECL_BASIC_CLASS(CSecurityModuleHook);

public:
	bool install_hooks();
	void uninstall_hooks();

private:
	void setup();

	void fill_modfuncs_callback_table();
	void restore_modfuncs_callback_table();

	void fill_engdst_callback_table();

public:
	inline auto& CommonLoadSecurityModule() { static CommonLoadSecurityModule_FnHook_t fnhook; return fnhook; }
	inline auto& CloseSecurityModule() { static CloseSecurityModule_FnDetour_t fnhook; return fnhook; }

	inline static auto& g_modfuncs() { static g_modfuncs_MemoryHook hook; return hook; };

	//
	// things we set at setup
	//

	inline static auto& engdata() { return s_engdata; }
	inline static auto cl_enginefuncs() { return s_enginefunc_hook.ptr; }
	inline static auto cl_enginefuncs_dst() { return s_enginefunc_dst_hook.ptr; }
	inline static auto cl_funcs() { return s_cl_funcs_hook.ptr; }
	inline static auto keybindings() { return s_keybindings; }
	inline static auto cls() { return s_cls; }

	//
	// setters, don't call this code.
	//
	void set_engdata(hl::engdata_t* engdata_ptr)
	{
		s_engdata = *engdata_ptr;
	}

private:
	inline static hl::engdata_t s_engdata;
	
	// callback tables
	inline static callback_table_hook_t<hl::cl_enginefunc_t> s_enginefunc_hook;
	inline static callback_table_hook_t<hl::cl_enginefunc_dst_t> s_enginefunc_dst_hook;
	inline static callback_table_hook_t<hl::cldll_func_t> s_cl_funcs_hook;

	// regular pointer to pointer hooks
	inline static char** s_keybindings;
	inline static hl::client_static_t* s_cls;

	bool m_initialized = false;
};

#endif // SECURITYMODULEHOOK_H
