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
// MemoryHook.h -- Resposibility of hooking a function on an address inside memory of another dll.
//

#ifndef MEMORYFNHOOK_H
#define MEMORYFNHOOK_H
#pragma once

#define DECL_GENERIC_MEMORY_FN_HOOK(call_conv, call_conv_raw)																				\
template<typename T = void, typename ...Args>																								\
class CGenericMemoryFnHook##call_conv																										\
{																																			\
public:																																		\
	inline T call_conv_raw call(Args... args) const { return as_func()(args...); }															\
																																			\
	virtual bool install() { return false; }																								\
																																			\
	inline bool is_installed() const { return is_initialized() && m_fn_address != nullptr; }												\
	inline std::string get_name() const { return m_name; }																					\
	inline std::string get_module_name() const { return m_name; }																			\
																																			\
protected:																																	\
	virtual inline void initialize(const char* name, const wchar_t* module_name)															\
	{																																		\
		assert(!is_installed() && "hook install recursion occured!");																		\
		m_name = name;																														\
		m_module_name = module_name;																										\
		init_msg();																															\
	}																																		\
																																			\
	virtual void init_msg() const																											\
	{																																		\
		CConsole::the().info("Installing {} function hook from {}...",																		\
										 m_name, CStringTools::the().unicode_to_utf8(m_module_name));										\
	}																																		\
																																			\
	/* generic handles for finding the hook inside memory */																				\
	bool generic_bytepattern_installer(const CBytePattern& pattern);																		\
	bool generic_exported_procname_installer(const char* procname);																			\
	bool generic_functionaddr_installer(uintptr_t* function_pointer);																		\
																																			\
	inline bool is_initialized() const { return !m_module_name.empty() && !m_name.empty(); }												\
																																			\
	typedef T(call_conv_raw*pfnRoutineType_t)(Args... args);																				\
	inline auto as_func() const { return reinterpret_cast<pfnRoutineType_t>(m_fn_address); };												\
																																			\
protected:																																	\
	uintptr_t* m_fn_address = nullptr; /* address to the first byte of the function */														\
																																			\
	std::string m_name = "";																												\
	std::wstring m_module_name = L"";																										\
};																																			\
																																			\
template<typename T, typename ...Args>																										\
inline bool CGenericMemoryFnHook##call_conv<T, Args...>::generic_bytepattern_installer(const CBytePattern& pattern)							\
{																																			\
	assert(is_initialized() && "hook must be initialized first to be then used for " __FUNCTION__ ".");										\
																																			\
	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(m_module_name.c_str(), SPACE_CODE);										\
																																			\
	if (!base || !end)																														\
	{																																		\
		CConsole::the().error("Can't install function hook. Dll probably not loaded in address space.",										\
										  CStringTools::the().unicode_to_utf8((m_module_name.c_str())));									\
		return false;																														\
	}																																		\
																																			\
	m_fn_address = pattern.search_in_loaded_address_space(base, end);																		\
	if (!m_fn_address)																														\
	{																																		\
		CConsole::the().error("Failed to install {} function hook from {}. Byte pattern is '{}'",											\
							  m_name, CStringTools::the().unicode_to_utf8(m_module_name), pattern.pattern_as_string());						\
		return false;																														\
	}																																		\
																																			\
	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_fn_address);															\
																																			\
	return true;																															\
}																																			\
																																			\
template<typename T, typename ...Args>																										\
inline bool CGenericMemoryFnHook##call_conv<T, Args...>::generic_exported_procname_installer(const char* procname)							\
{																																			\
	assert(is_initialized() && "hook must be initialized first to be then used for " __FUNCTION__ ".");										\
																																			\
	uintptr_t module_base = (uintptr_t)g_libloader_i->load_library(NULL, m_module_name.c_str());											\
																																			\
	if (!module_base)																														\
	{																																		\
		CConsole::the().error("Can't install function hook. Dll probably not loaded in address space.",										\
							  CStringTools::the().unicode_to_utf8(m_module_name.c_str()));													\
		return false;																														\
	}																																		\
																																			\
	m_fn_address = (uintptr_t*)g_libloader_i->find_proc_in_target_library(module_base, procname);											\
	if (!m_fn_address)																														\
	{																																		\
		CConsole::the().error("Failed to install {} function hook from {}. The exported function couldn't be found.",						\
							  m_name, CStringTools::the().unicode_to_utf8(m_module_name));													\
		return false;																														\
	}																																		\
																																			\
	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_fn_address);															\
																																			\
	return true;																															\
}																																			\
																																			\
template<typename T, typename ...Args>																										\
inline bool CGenericMemoryFnHook##call_conv<T, Args...>::generic_functionaddr_installer(uintptr_t* function_pointer)						\
{																																			\
	assert(is_initialized() && "hook must be initialized first to be then used for " __FUNCTION__ ".");										\
																																			\
	m_fn_address = function_pointer;																										\
	if (!m_fn_address)																														\
	{																																		\
		CConsole::the().error("Failed to install {} function hook from {}. The function pointer specified is NULL.",						\
							  m_name, CStringTools::the().unicode_to_utf8(m_module_name));													\
		return false;																														\
	}																																		\
																																			\
	CConsole::the().info("Found {} at 0x{:08X}", m_name, (uintptr_t)m_fn_address);															\
																																			\
	return true;																															\
}

DECL_GENERIC_MEMORY_FN_HOOK(Cdecl, __cdecl);
DECL_GENERIC_MEMORY_FN_HOOK(Stdcall, __stdcall);
DECL_GENERIC_MEMORY_FN_HOOK(Thiscall, __thiscall);
DECL_GENERIC_MEMORY_FN_HOOK(Fastcall, __fastcall);

//---------------------------------------------------------------------------------

// bool __cdecl VGuiWrap2_IsConsoleVisible()
class VGuiWrap2_IsConsoleVisibleFnHook_t final : public CGenericMemoryFnHookCdecl<hl::qboolean>
{
public:
	bool install();
};

// void __cdecl VGuiWrap2_ConPrintf(const char *msg)
class VGuiWrap2_ConPrintfFnHook_t final : public CGenericMemoryFnHookCdecl<void, const char *>
{
public:
	bool install();
};

// void ClearIOStates()
class ClearIOStatesFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

//---------------------------------------------------------------------------------

class CMemoryFnHookMgr
{
public:
	DECL_BASIC_CLASS(CMemoryFnHookMgr)

public:
	bool install_hooks();

	// individual hooks

	inline auto& VGuiWrap2_IsConsoleVisible() { static VGuiWrap2_IsConsoleVisibleFnHook_t hook; return hook; }
	inline auto& VGuiWrap2_ConPrintf() { static VGuiWrap2_ConPrintfFnHook_t hook; return hook; }
	inline auto& ClearIOStates() { static ClearIOStatesFnHook_t hook; return hook; }
};

#endif // MEMORYFNHOOK_H