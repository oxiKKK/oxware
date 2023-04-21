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

#include "precompiled.h"

void CVirtualMemoryManager::deallocate(LPVOID at)
{
	BOOL success = VirtualFree(at, NULL, MEM_RELEASE);
	if (!success)
	{
		CConsole::the().error("VMM: Failed to free memory at 0x{:08X} (last error={})", (uintptr_t)at, CGenericUtil::the().get_last_win_error());
		return;
	}

	CConsole::the().info("VM: Deallocated memory at 0x{:08X}", (uintptr_t)at);
}

void CVirtualMemoryManager::deallocate(HANDLE target_process, LPVOID at)
{	
	BOOL success = FALSE;
	if (target_process == GET_CURRENT_PROCESS_HANDLE())
	{
		success = VirtualFree(at, NULL, MEM_RELEASE);
	}
	else
	{
		success = VirtualFreeEx(target_process, at, NULL, MEM_RELEASE);
	}

	if (!success)
	{
		CConsole::the().error("VMM: Failed to free memory at 0x{:08X} (last error={})", (uintptr_t)at, CGenericUtil::the().get_last_win_error());
		return;
	}

	CConsole::the().info("VM: Deallocated memory in target process at 0x{:08X}", (uintptr_t)at);
}

LPVOID CVirtualMemoryManager::alloc_internal(LPVOID at, size_t amount, uint32_t protection)
{
	assert(protection != NULL);

	LPVOID addr = VirtualAlloc(at, amount, MEM_COMMIT | MEM_RESERVE, protection);
	if (!addr)
	{
		CConsole::the().error("VMM: Failed to allocate {} bytes of memory at 0x{:08X} (last error={})", amount, (uintptr_t)at, CGenericUtil::the().get_last_win_error());
		return nullptr;
	}

	CConsole::the().info("VM: Allocated 0x{:08X} bytes at 0x{:08X}", amount, (uintptr_t)addr);

	return addr;
}

LPVOID CVirtualMemoryManager::alloc_internal(HANDLE target_process, LPVOID at, size_t amount, uint32_t protection)
{
	assert(protection != NULL);

	LPVOID addr = NULL;
	if (target_process == GET_CURRENT_PROCESS_HANDLE())
	{
		addr = alloc_internal(at, amount, protection);
	}
	else
	{
		addr = VirtualAllocEx(target_process, at, amount, MEM_COMMIT | MEM_RESERVE, protection);
		if (!addr)
		{
			CConsole::the().error("VMM: Failed to allocate {} bytes of memory at 0x{:08X} (last error={})", amount, (uintptr_t)at, CGenericUtil::the().get_last_win_error());
			return nullptr;
		}
	}

	CConsole::the().info("VM: Allocated 0x{:08X} bytes in target process at 0x{:08X}", amount, (uintptr_t)addr);

	return addr;
}

bool CVirtualMemoryManager::write(PVOID at, PVOID what, size_t size)
{
	return write(CGenericUtil::the().get_current_process_handle(), at, what, size);
}

bool CVirtualMemoryManager::write(HANDLE target_process, PVOID at, PVOID what, size_t size)
{
	if (target_process == GET_CURRENT_PROCESS_HANDLE())
	{
		memcpy(at, what, size);
	}
	else
	{
		SIZE_T num_bytes_written = 0;
		NTSTATUS st;
		if (!NT_SUCCESS(st = NtWriteVirtualMemory(target_process, at,
												  what, size,
												  &num_bytes_written)) || num_bytes_written == 0)
		{
			CConsole::the().error("VMM: Couldn't write to the target process. (status=0x{:08X})", (uint32_t)st);
			CConsole::the().error("target_process: 0x{:08X}, at: 0x{:08X}, size: 0x{:08X}", (uintptr_t)target_process, (uintptr_t)at, (size_t)size);
			return false;
		}
	}

	return true;
}

bool CVirtualMemoryManager::read_internal(HANDLE target_process, PVOID at, PVOID what, size_t size)
{
	if (target_process == GET_CURRENT_PROCESS_HANDLE())
	{
		memcpy(what, at, size);
	}
	else
	{
		SIZE_T num_bytes_read = 0;
		NTSTATUS st;
		if (!NT_SUCCESS(st = NtReadVirtualMemory(target_process, at,
												 what, size,
												 &num_bytes_read)) || num_bytes_read == 0)
		{
			CConsole::the().error("VMM: Couldn't read data from the target process. (status=0x{:08X})", (uint32_t)st);
			return false;
		}
	}

	return true;
}