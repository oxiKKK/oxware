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
// VirtualMemorymanager.h -- Basic class for VMM (Virtual Memory Management)
//

#ifndef VIRTUALMEMORYMANAGER_H
#define VIRTUALMEMORYMANAGER_H
#pragma once

#include "tier/GenericUtil.h"

class CVirtualMemoryManager
{
public:
	DECL_BASIC_CLASS(CVirtualMemoryManager)

public:
	void deallocate(LPVOID at);
	void deallocate(HANDLE target_process, LPVOID at);

	template<typename T> T* allocate(LPVOID at, size_t amount, uint32_t protection)
	{
		return reinterpret_cast<T*>(alloc_internal(at, amount, protection));
	}

	template<typename T> T* allocate(HANDLE target_process, LPVOID at, size_t amount, uint32_t protection)
	{
		if (target_process == CGenericUtil::the().get_current_process_handle())
		{
			return reinterpret_cast<T*>(alloc_internal(at, amount, protection));
		}
		else
		{
			return reinterpret_cast<T*>(alloc_internal(target_process, at, amount, protection));
		}
	}

	bool write(PVOID at, PVOID what, size_t size);
	bool write(HANDLE process_handle, PVOID at, PVOID what, size_t size);

	template<typename T> bool read(PVOID at, T* what)
	{
		return read_internal(CGenericUtil::the().get_current_process_handle(), at, (PVOID)what, sizeof(T));
	}

	template<typename T> bool read(HANDLE process_handle, PVOID at, T* what)
	{
		return read_internal(process_handle, at, (PVOID)what, sizeof(T));
	}

private:
	LPVOID alloc_internal(LPVOID at, size_t amount, uint32_t protection);
	LPVOID alloc_internal(HANDLE target_process, LPVOID at, size_t amount, uint32_t protection);

	bool read_internal(HANDLE process_handle, PVOID at, PVOID what, size_t size);
};

#endif // VIRTUALMEMORYMANAGER_H