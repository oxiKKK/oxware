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
// LoadLibrareredDll.h -- Dll that is injected to a target process through LoadLibrary injection technique.
//
//	PS: I know LoadLibrareredDll sounds really bad :D but whatever
//

#ifndef LOADLIBRAREREDDLL_H
#define LOADLIBRAREREDDLL_H
#pragma once

class CLoadLibrareredDll : public IInjectableModuleObject
{
public:
	CLoadLibrareredDll(const char* execuatable_name, const std::filesystem::path& dll_path)
	{
		m_dll_filepath = dll_path;
		m_exe_name = execuatable_name;
	}

	bool start_injection();

	void unload(bool tell_client_dll);

	bool communicate_with_injected_dll(EClient2Injector& code);

	uintptr_t get_base_address() const { assert(m_successfully_injected); return (uintptr_t)m_target_dll_image_ptr; }

	// functions exported by the injected dll we need
	inline ExposeModuleFn get_ExposeModule_fn() const { assert(m_successfully_injected); return m_shellcode_execution_context.pfnExposeModule; };
	inline GetInterfaceInstanceFn get_GetInterfaceInstance_fn() const { assert(m_successfully_injected); return m_shellcode_execution_context.pfnGetInterfaceInstance; };

	void on_successfull_initialize();

private:
	bool inject_dll_to();

	bool free_injected_dll();

	bool read_execution_context_back(loadlibrary_shellcode_execution_context_t* sec);

	// memory
	bool allocate_memory();
	bool allocate_target_process_memory();

	void deallocate_target_process_memory();
	void deallocate_all_memory()
	{
		deallocate_target_process_memory();
	}

	uint8_t* m_target_dll_image_ptr;	// memory for the data allocated in target process

	// this includes the size of the shellcode routine + the context etc..
	size_t m_target_process_total_allocated_mem;

private:
	// operations on the dll image
	bool write_data_into_target_process();
	bool execute_shellcode_in_target_process();

	void update_shellcode_execution_context_data();
	loadlibrary_shellcode_execution_context_t m_shellcode_execution_context;

	uintptr_t get_target_process_shellcode_execution_context_addr() const
	{
		assert(m_target_dll_image_ptr);
		return (uintptr_t)(m_target_dll_image_ptr + m_dll_to_be_injected.get_size_of_image());
	};

	uintptr_t get_target_process_ipc_block_addr() const
	{
		if (is_communicative_dll())
		{
			return get_target_process_shellcode_execution_context_addr() + sizeof(loadlibrary_shellcode_execution_context_t);
		}
		else
		{
			assert(0);
			return NULL;
		}
	}

	uintptr_t get_target_process_shellcode_addr() const
	{
		if (is_communicative_dll())
		{
			return get_target_process_ipc_block_addr() + sizeof(IPCInterface_t);
		}
		else
		{
			return get_target_process_shellcode_execution_context_addr() + sizeof(loadlibrary_shellcode_execution_context_t);
		}
	};

	// Shellcode
	static HINSTANCE __stdcall shellcode_routine(loadlibrary_shellcode_execution_context_t* context);
	static DWORD calc_shellcode_routine_size_in_bytes();
};

#endif // LOADLIBRAREREDDLL_H