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

#include <commondefs.h>

#include <interface/IBaseModule.h>

#include <injection/IBaseInjector.h>
#include <injection/BaseInjector.h>

//
// shellcodes.cpp
// 
//	Because of Function-level linking compiler option, we have to create separate source files for
//	shellcodes, because this feature, when enabled, makes it so that the linker doesn't put functions
//	in the same order as they're defined.. Oh well..
// 
//	More on this here:
//		Function-level linking	- https://learn.microsoft.com/en-us/cpp/build/reference/gy-enable-function-level-linking?view=msvc-170
//		COMDAT folding			- https://stackoverflow.com/questions/1834597/what-is-the-comdat-section-used-for
//

//
//	Policy of the shellcode routine code:
// 
//	Cannot have:
//		Please See building.md
//
DISABLE_SAFEBUFFERS HINSTANCE __stdcall CLoadLibrareredDll::shellcode_routine(loadlibrary_shellcode_execution_context_t* context)
{
	context->pfnOutputDebugStringA(context->debug_messages[0]);

	//
	// Load the dll using native API so it is added to the DLL load list.
	// 
	// This calls the DllMain routine automatically.
	//

	context->pfnOutputDebugStringA(context->debug_messages[1]);

	HMODULE module = NULL;

	// first try to load the dll in common dirs (e.g. PATH or SysWOW64 etc..)
	module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!module)
	{
		// altered path tries to search for dependencies inside the loaded dll path. The path must be absolute.
		module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!module)
		{
			// the directory where the loaded dll is temporarily added to the directory list when loading dll dependencies
			module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

			// if this fails, fuck you, windows
		}
	}

	if (!module)
	{
		return NULL;
	}

	context->m_loaded_module = module;

	//
	// Get information we need from the dll after it is loaded into memory.
	//

	uint8_t* base = (uint8_t*)module;
	auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
	auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
	auto op = &nt->OptionalHeader;

	auto teb = GET_CURRENT_TEB();
	auto peb = teb->ProcessEnvironmentBlock;

	pfnCommunicativeDllEntryPoint_t pfnCommunicativeDllEntryPoint = nullptr;

	//
	// watch for dll exports we want
	//
	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		auto exports_directory = (PIMAGE_EXPORT_DIRECTORY)((uint8_t*)base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		auto function_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)base + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)base + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)base + exports_directory->AddressOfNameOrdinals);

		// search for some procs we want to have
		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)base + name_table_base[i]);
			//context->pfnOutputDebugStringA(export_procname);
			if (!context->pfn_stricmp(export_procname, context->export_names[0]))
			{
				context->pfnExposeModule = reinterpret_cast<ExposeModuleFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[1]))
			{
				context->pfnGetInterfaceInstance = reinterpret_cast<GetInterfaceInstanceFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[2]))
			{
				pfnCommunicativeDllEntryPoint = reinterpret_cast<pfnCommunicativeDllEntryPoint_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[2]);

	//
	// if this module we're loading is the communicative one, we have to listen to it.
	//
	if (pfnCommunicativeDllEntryPoint != nullptr)
	{
		context->m_information_package.m_ipc_block_ptr->m_technique = INJECT_LOADLIBRARY;

		if (!pfnCommunicativeDllEntryPoint(&context->m_information_package))
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[3]);

	return (HINSTANCE)module;
}

DWORD CLoadLibrareredDll::shellcode_routine_end_marker()
{
	return 123456; // return value doesn't matter. Return something, so that the compiler doesn't inline this or smth
}

//------------------------------------------------------------------------------------------------------------------------


//
//	Policy of the shellcode routine code:
// 
//	Cannot have:
//		Please See building.md
//
DISABLE_SAFEBUFFERS HINSTANCE __stdcall CManualMappedDll::shellcode_routine(manualmap_shellcode_execution_context_t* context)
{
	uint8_t* base = context->module_base;
	auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
	auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
	auto op = &nt->OptionalHeader;

	context->pfnOutputDebugStringA(context->debug_messages[0]);

	auto teb = GET_CURRENT_TEB();
	auto peb = teb->ProcessEnvironmentBlock;

	auto pfnDllMain = reinterpret_cast<pfnDllMain_t>(base + op->AddressOfEntryPoint);

	pfnCommunicativeDllEntryPoint_t pfnCommunicativeDllEntryPoint = nullptr;

	//
	// manually re-set image imports corresponding to target process's address space addresses.
	//
	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		auto import_desc = (IMAGE_IMPORT_DESCRIPTOR*)(base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		//context->pfnOutputDebugStringA(context->debug_messages[3]);
		while (import_desc->Name)
		{
			char* module_name = (char*)(base + import_desc->Name);

			//context->pfnOutputDebugStringA(module_name);

			HINSTANCE module_handle = (HINSTANCE)context->pfnLoadLibraryA(module_name);

			auto thunk_ref = (ULONG_PTR*)(base + import_desc->OriginalFirstThunk);
			auto func_ref = (ULONG_PTR*)(base + import_desc->FirstThunk);

			if (!import_desc->OriginalFirstThunk)
			{
				thunk_ref = func_ref;
			}

			for (; *thunk_ref; ++thunk_ref, ++func_ref)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*thunk_ref))
				{
					*func_ref = (ULONG_PTR)context->pfnGetProcAddress(module_handle, (char*)IMAGE_ORDINAL32(*thunk_ref));
				}
				else
				{
					auto import = (IMAGE_IMPORT_BY_NAME*)(base + (*thunk_ref));
					*func_ref = (ULONG_PTR)context->pfnGetProcAddress(module_handle, import->Name);
					//context->pfnOutputDebugStringA(import->Name);
				}
			}

			import_desc++;
		}
	}

	//
	// watch for dll exports we want
	//
	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		auto exports_directory = (PIMAGE_EXPORT_DIRECTORY)((uint8_t*)base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		auto function_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)base + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)base + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)base + exports_directory->AddressOfNameOrdinals);

		// search for some procs we want to have
		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)base + name_table_base[i]);
			//context->pfnOutputDebugStringA(export_procname);
			if (!context->pfn_stricmp(export_procname, context->export_names[0]))
			{
				context->pfnExposeModule = reinterpret_cast<ExposeModuleFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[1]))
			{
				context->pfnGetInterfaceInstance = reinterpret_cast<GetInterfaceInstanceFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[2]))
			{
				pfnCommunicativeDllEntryPoint = reinterpret_cast<pfnCommunicativeDllEntryPoint_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
		}
	}

	//
	// Make a memory snapshot of the first 4064 bytes of the DLL file, if needed for later
	//
	context->pfnmemcpy(context->m_nt_headers_snapshot, base, sizeof(context->m_nt_headers_snapshot));
	context->pfnmemset(base, 0x0, sizeof(context->m_nt_headers_snapshot));

	//
	// clear some things we don't want to have in our memory snapshot
	//
	auto snap_dos = (PIMAGE_DOS_HEADER)context->m_nt_headers_snapshot;
	auto snap_nt = (PIMAGE_NT_HEADERS)(snap_dos + dos->e_lfanew);
	snap_dos->e_magic = 0;
	snap_nt->OptionalHeader.Magic = 0;

	context->pfnOutputDebugStringA(context->debug_messages[1]);

	//
	// call the generic initialization routine that gets called normally. (_DllMainCRTStartup(), usually)
	//
	if (!pfnDllMain((HINSTANCE)base, DLL_PROCESS_ATTACH, NULL))
	{
		return NULL;
	}

	//
	// if this module we're loading is the communicative one, we have to listen to it.
	//
	if (pfnCommunicativeDllEntryPoint != nullptr)
	{
		context->m_information_package.m_ipc_block_ptr->m_technique = INJECT_MANUALMAP;

		if (!pfnCommunicativeDllEntryPoint(&context->m_information_package))
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[2]);

	//__debugbreak();

	return (HINSTANCE)base;
}

DWORD CManualMappedDll::shellcode_routine_end_marker()
{
	return 123456; // return value doesn't matter. Return something, so that the compiler doesn't inline this or smth
}
