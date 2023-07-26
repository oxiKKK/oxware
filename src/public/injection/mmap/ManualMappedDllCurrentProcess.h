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
// ManualMappedDllCurrentProcess.h -- Dll that is injected to current process through manual mapping injection technique.
//

#ifndef MANUALMAPPEDDLLCURRENTPROCESS_H
#define MANUALMAPPEDDLLCURRENTPROCESS_H
#pragma once

#include "Console.h"

class CManualMappedDllCurrentProcess : public IInjectableModuleObject
{
public:	
	CManualMappedDllCurrentProcess(const std::filesystem::path& dll_path)
	{
		m_dll_filepath = dll_path;
		m_exe_name = {};
		m_inject_to_current_process = true;
	}

	bool start_injection() override;
	void unload(bool tell_client_dll);

	bool communicate_with_injected_dll(EClient2Injector& code);

	uintptr_t get_base_address() const { assert(m_successfully_injected); return (uintptr_t)m_allocation_block_ptr; }

	inline ExposeModuleFn get_ExposeModule_fn() const { assert(m_successfully_injected); return m_pfnExposeModuleFn; };
	inline GetInterfaceInstanceFn get_GetInterfaceInstance_fn() const { assert(m_successfully_injected); return m_pfnGetInterfaceInstanceFn; };

	void on_successfull_initialize();

private:
	bool map_dll_to();

	bool read_execution_context_back(manualmap_shellcode_execution_context_t* sec);

	// memory
	bool allocate_memory();
	void deallocate_all_memory();

	uint8_t* m_allocation_block_ptr;	// locally allocated DLL image in our process's VM + the communication block.

private:
	// operations on dll image
	bool copy_dll_image_data();
	bool relocate_dll_image_data();
	bool execute_shellcode();			// we can execute shellcode locally, since we're not injecting into target process

	ExposeModuleFn m_pfnExposeModuleFn;
	GetInterfaceInstanceFn m_pfnGetInterfaceInstanceFn;

	uintptr_t get_target_process_shellcode_execution_context_addr() const
	{
		assert(m_allocation_block_ptr);
		return (uintptr_t)(m_allocation_block_ptr + m_dll_to_be_injected.get_size_of_image());
	};

	uintptr_t get_target_process_ipc_block_addr() const
	{
		if (is_communicative_dll())
		{
			return get_target_process_shellcode_execution_context_addr() + sizeof(manualmap_shellcode_execution_context_t);
		}
		else
		{
			assert(0);
			return NULL;
		}
	}

	// Shellcode -- this can be here just fine, since we aren't executing this remotely
	HINSTANCE shellcode_routine();
};

#endif // MANUALMAPPEDDLLCURRENTPROCESS_H