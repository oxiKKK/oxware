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

template<typename T = void, typename ...Args>
class GenericMemoryFnHook : public GenericMemoryHook<uintptr_t>
{
protected:
	template <typename U>
	inline auto as_func() const { return reinterpret_cast<U>(m_address); };

public:
	//
	// testing
	//

	virtual void add_to_test() override
	{
		CHookTests::the().add_for_testing("MemoryFnHook", this);
	}
};

//---------------------------------------------------------------------------------

template<typename T = void, typename ...Args>
struct GenericMemoryFnHook_cdecl : public GenericMemoryFnHook<uintptr_t>
{
	inline T __cdecl call(Args... args) const { return (as_func<T(__cdecl*)(Args...)>())(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnHook_stdcall : public GenericMemoryFnHook<uintptr_t>
{
	inline T __stdcall call(Args... args) const { return (as_func<T(__stdcall*)(Args...)>())(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnHook_thiscall : public GenericMemoryFnHook<uintptr_t>
{
	inline T __thiscall call(Args... args) const { return (as_func<T(__thiscall*)(Args...)>())(args...); }
};

template<typename T = void, typename ...Args>
struct GenericMemoryFnHook_fastcall : public GenericMemoryFnHook<uintptr_t>
{
	inline T __fastcall call(Args... args) const { return (as_func<T(__fastcall*)(Args...)>())(args...); }
};

//---------------------------------------------------------------------------------

// bool __cdecl VGuiWrap2_IsConsoleVisible()
struct VGuiWrap2_IsConsoleVisible_FnHook_t final : public GenericMemoryFnHook_cdecl<hl::qboolean> { bool install(); };

// void __cdecl VGuiWrap2_ConPrintf(const char *msg)
struct VGuiWrap2_ConPrintf_FnHook_t final : public GenericMemoryFnHook_cdecl<void, const char *> { bool install(); };

// void ClearIOStates()
struct ClearIOStates_FnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install(); };

// int R_GetStudioBounds(const char* filename, float* mins, float* maxs)
struct R_GetStudioBounds_FnHook_t final : public GenericMemoryFnHook_cdecl<int, const char*, float*, float*> { bool install(); };

// int MD5_Hash_File(unsigned char* digest, char* pszFileName, BOOL bUsefopen, BOOL bSeed, unsigned int* seed)
struct MD5_Hash_File_FnHook_t final : public GenericMemoryFnHook_cdecl<int, unsigned char*, char*, BOOL, BOOL, unsigned int*> { bool install(); };

//---------------------------------------------------------------------------------

class CMemoryFnHookMgr
{
public:
	DECL_BASIC_CLASS(CMemoryFnHookMgr)

public:
	bool install_hooks();

	//
	// individual hooks
	//

	inline auto& VGuiWrap2_IsConsoleVisible() { static VGuiWrap2_IsConsoleVisible_FnHook_t hook; return hook; }
	inline auto& VGuiWrap2_ConPrintf() { static VGuiWrap2_ConPrintf_FnHook_t hook; return hook; }
	inline auto& ClearIOStates() { static ClearIOStates_FnHook_t hook; return hook; }
	inline auto& R_GetStudioBounds() { static R_GetStudioBounds_FnHook_t hook; return hook; }
	inline auto& MD5_Hash_File() { static MD5_Hash_File_FnHook_t hook; return hook; }
};

#endif // MEMORYFNHOOK_H