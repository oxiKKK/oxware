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
// ManualMappedDll.cpp -- Defines a ManualMap injector interface.
// 
// STEPS OF MANUAL MAPPING
// 
//	1) MEMORY ALLOCATION
//			a)	Allocate enough memory in CURRENT PROCESS for the dll image which
//				will be then copied into this memory. 
//
//			b)	Allocate enough memory in TARGET PROCESS for the dll image, as well
//				as the shellcode execution data, and the shellcode itself.
//
//	2) DLL HEADER COPYING AND WRITING MMAP INFORMATION INTO TARGET PROCESS
//			a)	Copying all individual headers of the loaded dll file into current process 
//				allocated memory via the PointerToRawData offset from the raw dll data base 
//				address.
//
//			b)	The manual map information including the manual mapping data and shellcode
//				are both written into the target process, to previously allocated memory.
//
//	3) RELOCATE ALL BASERELOCATIONS IF NEEDED
//
//	4) FIX DLL IMPORTS & LOAD ADDITIONAL LIBRARIES THAT THE DLL IS USING
//
//	5) WRITE DLL IMAGE INTO TARGET PROCESS
//
//	6) DEALLOCATING ALLOCATED MEMORY INSIDE CURRENT PROCESS
//
//	8) CREATING A NEW THREAD IN THE TARGET PROCESS
//			-	A new thread will be created which will execute the shellcode. The thread is
//				located at the shellcode section base address, and as a parameter it is being
//				passed the shellcode data execution context address.
//			-	The dll entry point will be called.
//
//	9) WAITING FOR THE DLL TO RESPOND, IF IT IS A COMMUNICATIVE DLL.
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

bool CManualMappedDll::start_injection()
{
	m_successfully_injected = false;
	m_start_time = std::chrono::high_resolution_clock::now();

	if (!map_dll_to())
	{
		CConsole::the().error("Failed to inject '{}' into a target process '{}'.", m_dll_filepath.string(), m_exe_name);
		return false;
	}

	m_successfully_injected = true;

	return true;
}

void CManualMappedDll::unload(bool tell_client_dll)
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

		deallocate_target_process_memory();
		
		m_dll_to_be_injected.destroy();
		close_process_handle();

		CConsole::the().info("{} unloaded successfully.", m_dll_filepath.string());
		m_unloaded = true;
	}
}

bool CManualMappedDll::communicate_with_injected_dll(EClient2Injector& code)
{
	EClient2Injector c2m;
	if (!listen_to_injected_dll(c2m))
	{
		return false;
	}

	// keep in sync between injector and client dll
	manualmap_shellcode_execution_context_t sec;
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

void CManualMappedDll::on_successfull_initialize()
{
	if (!m_injected_module_successfully_initialized)
	{
		m_initialization_time = std::chrono::high_resolution_clock::now();

		CConsole::the().info("Manual-mapped module successfully initialized. Took {} seconds to fully load and initialize.", 
							 std::chrono::duration<float, std::ratio<1, 1>>(m_initialization_time - m_start_time).count());

		m_injected_module_successfully_initialized = true;
	}
}

bool CManualMappedDll::map_dll_to()
{
	CConsole::the().info("Trying to inject '{}' into a target process '{}'...", m_dll_filepath.string(), m_exe_name);

	if (!std::filesystem::exists(m_dll_filepath))
	{
		CConsole::the().error("The DLL specified couldn't be found.");
		return false;
	}

	m_target_process_handle = CInjectorUtils::the().open_process(m_exe_name.c_str());
	if (m_target_process_handle == INVALID_HANDLE_VALUE)
	{
		CConsole::the().error("The target process specified couldn't be found.");
		return false;
	}

	CConsole::the().info("Found the target process (HANDLE={})", (uintptr_t)m_target_process_handle);

	if (!m_dll_to_be_injected.create(m_dll_filepath))
	{
		return false;
	}

	bool is_communicative = m_dll_to_be_injected.find_export(COMMUNICATIVEDLLENTRYPOINT_PROCNAME);
	initialize_communication_interface(m_target_process_handle, is_communicative);

	if (!allocate_memory())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!copy_dll_image_data())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!relocate_dll_image_data())
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

	// we don't need this anymore.
	deallocate_current_process_memory();

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

bool CManualMappedDll::read_execution_context_back(manualmap_shellcode_execution_context_t* sec)
{
	if (!CVirtualMemoryManager::the().read<manualmap_shellcode_execution_context_t>(
		m_target_process_handle,
		(PVOID)get_target_process_shellcode_execution_context_addr(), sec))
	{
		CConsole::the().error("Couldn't read target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

	return true;
}

bool CManualMappedDll::allocate_memory()
{
	if (!allocate_current_process_memory())
	{
		CConsole::the().error("Failed to allocate current process memory.");
		return false;
	}

	if (!allocate_target_process_memory())
	{
		CConsole::the().error("Failed to allocate target process memory.");
		return false;
	}

	return true;
}

bool CManualMappedDll::allocate_current_process_memory()
{
	// Allocate space for the DLL image inside our process.
	m_local_dll_image_ptr = CVirtualMemoryManager::the().allocate<uint8_t>(
		nullptr, m_dll_to_be_injected.get_size_of_image(), PAGE_EXECUTE_READWRITE);
	if (!m_local_dll_image_ptr)
	{
		return false;
	}

	CConsole::the().info("Allocated current process memory.");
	return true;
}

bool CManualMappedDll::allocate_target_process_memory()
{
	m_target_process_total_allocated_mem = 
		m_dll_to_be_injected.get_size_of_image()
		+ sizeof(manualmap_shellcode_execution_context_t)
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

void CManualMappedDll::deallocate_current_process_memory()
{
	if (m_local_dll_image_ptr)
	{
		CVirtualMemoryManager::the().deallocate(m_local_dll_image_ptr);
		m_local_dll_image_ptr = nullptr;
	}

	CConsole::the().info("Deallocated current process memory.");
}

void CManualMappedDll::deallocate_target_process_memory()
{
	if (m_target_dll_image_ptr)
	{
		CVirtualMemoryManager::the().deallocate(m_target_process_handle, m_target_dll_image_ptr);
		m_target_dll_image_ptr = nullptr;
	}

	CConsole::the().info("Deallocated target process memory.");
}

bool CManualMappedDll::copy_dll_image_data()
{
	// Copy NT & DOS headers
	memcpy(m_local_dll_image_ptr, m_dll_to_be_injected.get_byte_buffer().get_raw(), m_dll_to_be_injected.get_size_of_headers());

	// Copy image sections
	auto first_section = m_dll_to_be_injected.get_image_first_section();
	for (uint16_t i = 0; i < m_dll_to_be_injected.get_num_of_sections(); i++)
	{
		auto current_section = &first_section[i];

		if (current_section->SizeOfRawData == 0)
		{
			CConsole::the().warning("section '{}' without raw data... skipping", (const char*)current_section->Name);
			continue;
		}

		uintptr_t from = (uintptr_t)(m_local_dll_image_ptr + current_section->VirtualAddress);
		uintptr_t to = (uintptr_t)(m_dll_to_be_injected.get_byte_buffer().get_raw() + current_section->PointerToRawData);

		memcpy((void*)from, (void*)to, current_section->SizeOfRawData);

		CConsole::the().info("[{:02}] Created {:<24} at 0x{:08X}", i, (const char*)current_section->Name, to);
	}

	CConsole::the().info("Created write-able DLL memory image.");

	return true;
}

bool CManualMappedDll::relocate_dll_image_data()
{
	// relocate only if the base address specified by the DLL doesn't corespond to the real base address. Then everything needs to be relocated.
	uintptr_t* amount_to_relocate = (uintptr_t*)(m_target_dll_image_ptr - m_dll_to_be_injected.get_image_base());
	if (amount_to_relocate == 0)
	{
		// No need for relocation0x000aaa00
		return true;
	}

	CConsole::the().info("Image relocation is needed.");

	auto& ddir = m_dll_to_be_injected.get_data_directory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
	assert(ddir.Size != 0);

	auto ibr = (PIMAGE_BASE_RELOCATION)(m_local_dll_image_ptr + ddir.VirtualAddress);
	size_t total_size = 0;
	while (ddir.Size > total_size && ibr->SizeOfBlock > 0)
	{
		typedef uint16_t relocation_entry;

		auto relative_info = (relocation_entry*)(ibr + sizeof(uint8_t));

		// size of a complete block without the header divided by the entry size.
		size_t num_of_relocations = (ibr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(relocation_entry);
		for (size_t i = 0; i < num_of_relocations; i++)
		{
			uint16_t type = (relative_info[i] & 0xF000) >> 12;
			uint16_t offs = (relative_info[i] & 0x0FFF);

			//CConsole::the().info("reloc type: {}", type);

			switch (type)
			{
				case IMAGE_REL_BASED_HIGHLOW:
				{
					uintptr_t* address_to_patch = (uintptr_t*)(m_local_dll_image_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += (uintptr_t)amount_to_relocate;
					break;
				}
				case IMAGE_REL_BASED_HIGH:
				{
					uint16_t* address_to_patch = (uint16_t*)(m_local_dll_image_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += HIWORD((uintptr_t)amount_to_relocate);
					break;
				}
				case IMAGE_REL_BASED_LOW:
				{
					uint16_t* address_to_patch = (uint16_t*)(m_local_dll_image_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += LOWORD((uintptr_t)amount_to_relocate);
					break;
				}
				case IMAGE_REL_BASED_ABSOLUTE:
				{
					// This is skipped
					break;
				}
				default:
				{
					CConsole::the().warning("Unhandled relocation type: {}", type);
					break;
				}
			}
		}

		total_size += ibr->SizeOfBlock;
		ibr = (PIMAGE_BASE_RELOCATION)((uint8_t*)ibr + ibr->SizeOfBlock);
	}

	CConsole::the().info("Successfuly relocated the DLL image.");

	return true;
}

bool CManualMappedDll::write_data_into_target_process()
{
	// write the dll code itself
	if (!CVirtualMemoryManager::the().write(
		m_target_process_handle, m_target_dll_image_ptr,
		m_local_dll_image_ptr, m_dll_to_be_injected.get_size_of_image()))
	{
		return false;
	}

	if (!update_shellcode_execution_context_data())
	{
		return false;
	}

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
	
	CConsole::the().info("Trying to write shellcode_routine at 0x{:08X}", (uintptr_t)get_target_process_shellcode_addr());

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

bool CManualMappedDll::execute_shellcode_in_target_process()
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

bool CManualMappedDll::update_shellcode_execution_context_data()
{
	m_shellcode_execution_context.module_base = m_target_dll_image_ptr;
	m_shellcode_execution_context.pfnGetProcAddress = (pfnGetProcAddress_t)GetProcAddress;
	m_shellcode_execution_context.pfnLoadLibraryA = (pfnLoadLibraryA_t)LoadLibraryA;
	m_shellcode_execution_context.pfnOutputDebugStringA = (pfnOutputDebugStringA_t)OutputDebugStringA;
	// this needs to be from ntdll.dll! we're lucky that it exports such functions anyways.. heh
	m_shellcode_execution_context.pfn_stricmp = (pfn_stricmp_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "_stricmp");
	m_shellcode_execution_context.pfn_wcsicmp = (pfn_wcsicmp_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "_wcsicmp");
	m_shellcode_execution_context.pfnmemset = (pfnmemset_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "memset");
	m_shellcode_execution_context.pfnmemcpy = (pfnmemcpy_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "memcpy");

	assert(m_shellcode_execution_context.pfnGetProcAddress);
	assert(m_shellcode_execution_context.pfnLoadLibraryA);
	assert(m_shellcode_execution_context.pfnOutputDebugStringA);
	assert(m_shellcode_execution_context.pfn_stricmp);
	assert(m_shellcode_execution_context.pfn_wcsicmp);
	assert(m_shellcode_execution_context.pfnmemset);
	assert(m_shellcode_execution_context.pfnmemcpy);

	// debug strings
	strcpy_s(m_shellcode_execution_context.debug_messages[0], "Hello from shellcode.\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[1], "Resolving imports...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[2], "Resolving exports...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[3], "Finding ntdll.dll base...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[4], "Resolving C++ exceptions...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[5], "Searching for RtlInsertInvertedFunctionTable...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[6], "Executing RtlInsertInvertedFunctionTable...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[7], "Calling PreDllLoad...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[8], "Calling DllMain...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[9], "Calling Communicative Entry Point...\n");
	strcpy_s(m_shellcode_execution_context.debug_messages[10], "Shellcode end.\n");

	// exported procnames
	strcpy_s(m_shellcode_execution_context.export_names[0], EXPOSEMODULE_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[1], INTERFACEINSTANCEGETTER_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[2], COMMUNICATIVEDLLENTRYPOINT_PROCNAME);
	strcpy_s(m_shellcode_execution_context.export_names[3], PRE_DLL_LOAD_PROCNAME);

	// dll names
	wcscpy_s(m_shellcode_execution_context.dll_names[0], L"ntdll.dll");

	// byte patterns
	if (!RtlIIFT_BytePattern_Search::the().resolve_bytepatterns())
	{
		return false;
	}

	strcpy_s(m_shellcode_execution_context.byte_patterns[BPattern_RtlIIFT_Idx].bytepattern, RtlIIFT_BytePattern_Search::the().m_RtlIIFT_bytepattern.c_str());
	strcpy_s(m_shellcode_execution_context.byte_patterns[BPattern_RtlIIFT_Idx].mask, RtlIIFT_BytePattern_Search::the().m_RtlIIFT_bytepattern_mask.c_str());
	m_shellcode_execution_context.byte_patterns[BPattern_RtlIIFT_Idx].length = RtlIIFT_BytePattern_Search::the().m_RtlIIFT_bytepattern.length();

	if (is_communicative_dll())
	{
		m_shellcode_execution_context.m_information_package.m_ipc_block_ptr = (IPCInterface_t*)get_target_process_ipc_block_addr();
		m_shellcode_execution_context.m_information_package.m_current_session_id = generate_new_session_id();

		// c++ exceptions
		strcpy_s(m_shellcode_execution_context.m_information_package.m_RtlRIFT_pattern.bytepattern, RtlIIFT_BytePattern_Search::the().m_RtlRIFT_bytepattern.c_str());
		strcpy_s(m_shellcode_execution_context.m_information_package.m_RtlRIFT_pattern.mask, RtlIIFT_BytePattern_Search::the().m_RtlRIFT_bytepattern_mask.c_str());
		m_shellcode_execution_context.m_information_package.m_RtlRIFT_pattern.length = RtlIIFT_BytePattern_Search::the().m_RtlRIFT_bytepattern.length();
	}
	else
	{
		m_shellcode_execution_context.m_information_package.m_ipc_block_ptr = nullptr;
		m_shellcode_execution_context.m_information_package.m_current_session_id = 0;
		m_shellcode_execution_context.m_information_package.m_RtlRIFT_pattern = {};
	}

	auto current_path = std::filesystem::current_path().wstring();
	wcscpy_s(m_shellcode_execution_context.m_information_package.m_loader_path, current_path.c_str());

	return true;
}

DWORD CManualMappedDll::calc_shellcode_routine_size_in_bytes()
{
	return CInjectorUtils::the().calc_func_size((uintptr_t)shellcode_routine);
}