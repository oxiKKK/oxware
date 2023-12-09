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
// LoadLibrareredDll.cpp -- Defines a LoadLibrary injector interface.
// 
// STEPS OF LOAD LIBRARY INJECTION METHOD
//
//	1) MEMORY ALLOCATION
//			a)	Allocate memory inside the TARGET process for the shellcode execution context as well as
//				for the shellcode itself.
// 
//	2) WRITING TO TARGET PROCESS'S MEMORY
//			a)	Write the shellcode execution context as well as the shellcode itself to the allocated
//				memory region inside target process.
// 
//	3) CREATING A NEW THREAD IN THE TARGET PROCESS
//			-	A new thread will be created which will execute the shellcode. The thread is
//				located at the shellcode section base address, and as a parameter it is being
//				passed the shellcode data execution context address.
//			-	The dll entry point will be called.
//
//	4) WAITING FOR THE DLL TO RESPOND, IF IT IS A COMMUNICATIVE DLL.
//

//
//
//
//	READ OR YOU WILL HAVE TO DEAL WITH ANNOYING BUGS THAT WILL TAKE A LOG OF TIME TO FIX !!!!
// 
//		DON'T build with INCREMENTAL LINKING on!!! Otherwise the shellcode part of the injection will not work properly!
//		Incremental linking creates a function jump table inside the binary, therefore, when e.g. calculating the size
//		of the shellcode it will fail! because the function pointer for the shellcode routine will point to an entry inside
//		a jump table! Same with writing the shellcode to the target proces!
//
//
//

#include "precompiled.h"

bool CLoadLibrareredDll::start_injection()
{
	m_successfully_injected = false;
	m_start_time = std::chrono::high_resolution_clock::now();

	if (!inject_dll_to())
	{
		if (is_injecting_to_current_process())
		{
			CConsole::the().error("Failed to inject '{}' into current process.", m_dll_filepath.string());
		}
		else
		{
			CConsole::the().error("Failed to inject '{}' into a target process '{}'.", m_dll_filepath.string(), m_exe_name);
		}
		return false;
	}

	m_successfully_injected = true;

	return true;
}

void CLoadLibrareredDll::unload(bool tell_client_dll)
{
	// only parent client modules provides communication interface
	if ((tell_client_dll && !m_requested_injected_dll_unload) && is_communicative_dll())
	{
		// the client module doesn't know about this, we'll have to tell them.
		m_requested_injected_dll_unload = true;
		m_unload_request_time = std::chrono::high_resolution_clock::now();

		CConsole::the().info("Unloading {}... Waiting for the mdoule to acknowledge that and respond back to us...", m_dll_filepath.string());

		if (!tell_to(I2C_RequestUnload))
		{
			CConsole::the().error("Failed to tell to the module, unloading...");
			m_requested_injected_dll_unload = false;
			unload(false);
		}
	}
	else
	{
		// mostlikely requested by the client module itself
		CConsole::the().info("Unloading... {}", m_dll_filepath.string());

		if (m_shellcode_execution_context.m_loaded_module)
		{
			free_injected_dll();
		}
		else
		{
			CConsole::the().warning("Couldn't free the target library! It is most likely still loaded inside the executable!");
		}

		deallocate_target_process_memory();

		m_dll_to_be_injected.destroy();
		close_process_handle();

		CConsole::the().info("{} unloaded successfully.", m_dll_filepath.string());
		m_unloaded = true;
	}
}

bool CLoadLibrareredDll::communicate_with_injected_dll(EClient2Injector & code)
{
	EClient2Injector c2m;
	if (!listen_to_injected_dll(c2m))
	{
		return false;
	}

	// keep in sync between injector and client dll
	loadlibrary_shellcode_execution_context_t sec;
	if (!read_execution_context_back(&sec))
	{
		return false;
	}

	m_shellcode_execution_context = sec;

	// see if the module has responded within a defined time. if not, it failed to sucessfully initialize.
	if (c2m == C2I_None)
	{
		if (calc_current_load_and_init_threshold_seconds() > k_load_and_init_threshold_seconds)
		{
			CConsole::the().error("Client dll didn't respond in {} seconds. Hence failed to load.", (int)k_load_and_init_threshold_seconds);
			return false;
		}
	}

	// we told the module that we're unloading and now we're waiting for it to respond..
	if (m_requested_injected_dll_unload)
	{
		if (calc_time_elapsed_since_unload_request_seconds() > k_seconds_for_the_client_dll_to_unload)
		{
			m_requested_injected_dll_unload = false;

			CConsole::the().error("Client dll failed to acknowledge its unload within {} seconds. Unloading...", (int)k_seconds_for_the_client_dll_to_unload);
			code = C2I_Unloading; // manually force DLL unload
			return true;
		}
	}

	code = c2m;
	return true;
}

void CLoadLibrareredDll::on_successfull_initialize()
{
	if (!m_injected_module_successfully_initialized)
	{
		m_initialization_time = std::chrono::high_resolution_clock::now();

		CConsole::the().info("Injected module successfully initialized. Took {} seconds to fully load and initialize.",
							 std::chrono::duration<float, std::ratio<1, 1>>(m_initialization_time - m_start_time).count());

		m_injected_module_successfully_initialized = true;
	}
}

bool CLoadLibrareredDll::inject_dll_to()
{
	if (is_injecting_to_current_process())
	{
		CConsole::the().info("Trying to inject '{}' into current process...", m_dll_filepath.string());
	}
	else
	{
		CConsole::the().info("Trying to inject '{}' into a target process '{}'...", m_dll_filepath.string(), m_exe_name);
	}

	if (!std::filesystem::exists(m_dll_filepath))
	{
		CConsole::the().error("The DLL specified couldn't be found.");
		return false;
	}

	if (is_injecting_to_current_process())
	{
		m_target_process_handle = CGenericUtil::the().get_current_process_handle();
	}
	else
	{
		m_target_process_handle = CInjectorUtils::the().open_process(m_exe_name.c_str());
	}

	// when injecting to current process, the handle is actually -1.
	if (!is_injecting_to_current_process() && m_target_process_handle == INVALID_HANDLE_VALUE)
	{
		CConsole::the().error("The target process specified couldn't be found.");
		return false;
	}

	if (!is_injecting_to_current_process())
	{
		CConsole::the().info("Found the target process (HANDLE={})", (uintptr_t)m_target_process_handle);
	}

	if (!m_dll_to_be_injected.create(m_dll_filepath))
	{
		return false;
	}

	bool is_communicative = m_dll_to_be_injected.find_export(COMMUNICATIVEDLLENTRYPOINT_PROCNAME);
	if (is_injecting_to_current_process())
	{
		initialize_communication_interface(is_communicative);
	}
	else
	{
		initialize_communication_interface(m_target_process_handle, is_communicative);
	}

	if (!allocate_memory())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!write_data_into_target_process())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	CConsole::the().info("Executing shellcode in target process...");

	if (!execute_shellcode_in_target_process())
	{
		deallocate_target_process_memory();
		close_process_handle();
		return false;
	}

	// now the dll is injected and we have to wait for its response, if there'll be any.

	return true;
}

bool CLoadLibrareredDll::free_injected_dll()
{
	//
	// create shellcode for dll unload.
	//

	CConsole::the().info("Executing FreeLibrary in target process...");

	HANDLE hThread;
	NTSTATUS st;
	if (!NT_SUCCESS(st = NtCreateThreadEx(
		&hThread, THREAD_ALL_ACCESS, nullptr,
		m_target_process_handle, (LPTHREAD_START_ROUTINE)FreeLibrary,
		m_shellcode_execution_context.m_loaded_module,
		NULL, 0, 0, 0, nullptr)) || !hThread)
	{
		CConsole::the().error("Couldn't create thread in target process. (status=0x{:08X})", (uint32_t)st);
		return false;
	}

	NtClose(hThread);

	CConsole::the().info("Successfully called FreeLibrary remotely inside the target process.");
	return true;
}

bool CLoadLibrareredDll::read_execution_context_back(loadlibrary_shellcode_execution_context_t* sec)
{
	if (!CVirtualMemoryManager::the().read<loadlibrary_shellcode_execution_context_t>(
		m_target_process_handle,
		(PVOID)get_target_process_shellcode_execution_context_addr(), sec))
	{
		CConsole::the().error("Couldn't read target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

	return true;

}

bool CLoadLibrareredDll::allocate_memory()
{
	if (!allocate_target_process_memory())
	{
		CConsole::the().error("Failed to allocate target process memory.");
		return false;
	}

	return true;
}

bool CLoadLibrareredDll::allocate_target_process_memory()
{
	m_target_process_total_allocated_mem =
		m_dll_to_be_injected.get_size_of_image()
		+ sizeof(loadlibrary_shellcode_execution_context_t)
		+ (is_communicative_dll() ? sizeof(IPCInterface_t) : 0)
		+ calc_shellcode_routine_size_in_bytes();

	CConsole::the().info("Shellcode routine size: {} bytes", calc_shellcode_routine_size_in_bytes());

	CConsole::the().info("Total target process allocation size: {} bytes", m_target_process_total_allocated_mem);

	m_target_dll_image_ptr = CVirtualMemoryManager::the().allocate<uint8_t>(
		m_target_process_handle, nullptr, m_target_process_total_allocated_mem, PAGE_EXECUTE_READWRITE);
	if (!m_target_dll_image_ptr)
	{
		return false;
	}

	CConsole::the().info("DLL's base address is 0x{:08X}, in target process's memory", (uintptr_t)m_target_dll_image_ptr);

	CConsole::the().info("Allocated target process memory.");
	return true;
}

void CLoadLibrareredDll::deallocate_target_process_memory()
{
	if (m_target_dll_image_ptr)
	{
		CVirtualMemoryManager::the().deallocate(m_target_process_handle, m_target_dll_image_ptr);
		m_target_dll_image_ptr = nullptr;
	}

	CConsole::the().info("Deallocated target process memory.");
}

bool CLoadLibrareredDll::write_data_into_target_process()
{
	update_shellcode_execution_context_data();

	// write the context, first
	if (!CVirtualMemoryManager::the().write(
		m_target_process_handle, (LPVOID)get_target_process_shellcode_execution_context_addr(),
		&m_shellcode_execution_context, sizeof(m_shellcode_execution_context)))
	{
		return false;
	}

	CConsole::the().info("Shellcode context at 0x{:08X}", get_target_process_shellcode_execution_context_addr());

	// then write the communication interface block
	if (is_communicative_dll())
	{
		if (!write_ipc_block((LPVOID)get_target_process_ipc_block_addr()))
		{
			return false;
		}

		CConsole::the().info("Communication interface block at 0x{:08X}", get_target_process_ipc_block_addr());
	}

	//
	// NOTE: DON'T build with INCREMENTAL LINKING on!!! Otherwise 'shellcode_routine' will point to a jump table entry
	//		 generated by the compiler and not to the actual address. Now, I'm not entirely sure whether this is because
	//		 the function is a static member function, nonetheless, if you don't disable it, this will fail.
	//

	// now write the actual shellcode
	if (!CVirtualMemoryManager::the().write(
		m_target_process_handle, (LPVOID)get_target_process_shellcode_addr(),
		shellcode_routine, calc_shellcode_routine_size_in_bytes()))
	{
		CConsole::the().error("FATAL ERROR! Couldn't write the shellcode routine to the target process!"
							  " CHECK IF YOU'RE BUILDING WITHOUT INCREMENTAL LINKING!!! THIS IS A COMMON ISSUE.");
		return false;
	}

	CConsole::the().info("Successfuly wrote the DLL image into target process.");
	return true;
}

bool CLoadLibrareredDll::execute_shellcode_in_target_process()
{
	HANDLE hThread;
	NTSTATUS st;
	if (!NT_SUCCESS(st = NtCreateThreadEx(
		&hThread, THREAD_ALL_ACCESS, nullptr,
		m_target_process_handle, (LPTHREAD_START_ROUTINE)get_target_process_shellcode_addr(),
		(PVOID)get_target_process_shellcode_execution_context_addr(),
		NULL, 0, 0, 0, nullptr)) || !hThread)
	{
		CConsole::the().error("Couldn't create thread in target process. (status=0x{:08X})", (uint32_t)st);
		return false;
	}

	if (is_communicative_dll())
	{
		CConsole::the().info("Shellcode is executing... waiting for the dll to respond...");
	}
	else
	{
		// if not communicative, there's no way of us knowing that it initialized.. just wait here
		// if it hangs, we're fucked.. TODO
		NtWaitForSingleObject(hThread, FALSE, NULL);
	}

	// "Closing a thread handle does not terminate the associated thread or remove the thread object"
	// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	NtClose(hThread);

	return true;
}

void CLoadLibrareredDll::update_shellcode_execution_context_data()
{
	auto full_path = std::filesystem::current_path() / m_dll_filepath;
	if (!std::filesystem::exists(full_path))
	{
		CConsole::the().error("Dll path not found, Dll cannot be injected. '{}'", full_path.string());
		return;
	}

	strcpy_s(m_shellcode_execution_context.m_injected_dll_name, full_path.string().c_str());

	// routines
	m_shellcode_execution_context.pfnLoadLibraryExA = (pfnLoadLibraryExA_t)LoadLibraryExA;
	m_shellcode_execution_context.pfnOutputDebugStringA = (pfnOutputDebugStringA_t)OutputDebugStringA;
	// this needs to be from ntdll.dll! we're lucky that it exports such functions anyways.. heh
	m_shellcode_execution_context.pfn_stricmp = (pfn_stricmp_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "_stricmp");
	assert(m_shellcode_execution_context.pfnLoadLibraryExA);
	assert(m_shellcode_execution_context.pfnOutputDebugStringA);
	assert(m_shellcode_execution_context.pfn_stricmp);

	// debug strings
	strcpy_s(m_shellcode_execution_context.debug_messages[0], "Hello from shellcode.\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[1], "Calling LoadLibraryExA...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[2], "Calling DllPreLoad..\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[3], "Calling DLL communication entry point...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[4], "Shellcode finish.\n");

	// exported procnames
	strcpy_s(m_shellcode_execution_context.export_names[0], EXPOSEMODULE_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[1], INTERFACEINSTANCEGETTER_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[2], COMMUNICATIVEDLLENTRYPOINT_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[3], PRE_DLL_LOAD_PROCNAME);

	if (is_communicative_dll())
	{
		m_shellcode_execution_context.m_information_package.m_ipc_block_ptr = (IPCInterface_t*)get_target_process_ipc_block_addr();
		m_shellcode_execution_context.m_information_package.m_current_session_id = generate_new_session_id();
	}
	else
	{
		m_shellcode_execution_context.m_information_package.m_ipc_block_ptr = nullptr;
		m_shellcode_execution_context.m_information_package.m_current_session_id = 0;
	}

	auto current_path = std::filesystem::current_path().wstring();
	wcscpy_s(m_shellcode_execution_context.m_information_package.m_loader_path, current_path.c_str());
}

DWORD CLoadLibrareredDll::calc_shellcode_routine_size_in_bytes()
{
	return CInjectorUtils::the().calc_func_size((uintptr_t)shellcode_routine);
}