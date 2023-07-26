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
// LibraryLoader.cpp -- Implements independent API to load libararies, their exports, etc
//

#include "interface/ILibraryLoader.h"

#include "tier/GenericUtil.h"
#include "tier/StringTools.h"
#include "tier/MessageBox.h"

#include "Console.h"

#include <cstdint>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <functional>

ILibraryLoader* g_libloader_i = nullptr;

class CLibraryLoader : public ILibraryLoader
{
public:
	CLibraryLoader();
	~CLibraryLoader();

	void initialize(); // this gets called only once.

	bool register_manualmapped_module(const wchar_t* module_name, uintptr_t base_address);
	void remove_manualmapped_module(const wchar_t* module_name);

	bool register_encrypted_module(const wchar_t* module_name, reconstructed_blob_module_info_t blob_info);
	void remove_encrypted_module(const wchar_t* module_name);

	void* find_proc_in_target_library(const wchar_t* library_name, const char* proc_name);
	void* find_proc_in_target_library(uintptr_t library_base_addr, const char* proc_name);
	void* find_proc_in_target_library(uintptr_t library_base_addr, uint16_t ordinal);

	uintptr_t get_target_loaded_dll_base_address(const wchar_t* library_name);
	PLDR_DATA_TABLE_ENTRY get_target_loaded_dll_data_table_entry(const wchar_t* library_name);

	std::pair<uintptr_t, uintptr_t> get_loaded_dll_address_space(const wchar_t* library_name, EAddressSpaceSearch settings = SPACE_FULL);

	HMODULE load_library(const wchar_t* path_to, const wchar_t* name);
	void unload_library(HMODULE module);

	bool add_directory_to_search_list(const wchar_t* absolute_directory);

	std::filesystem::path get_windows_directory(const std::filesystem::path& subdir);

	bool is_dll_loaded(const wchar_t* library_name);

	template<typename T>
	T initialize_procedure_function_pointer(HMODULE module, const char* proc_name)
	{
		uintptr_t address_to_procedure = (uintptr_t)find_proc_in_target_library((uintptr_t)module, proc_name);
		if (!address_to_procedure)
		{
			CConsole::the().error("LibraryLoader: Could not locate {} function.", proc_name);
			return nullptr;
		}
		return reinterpret_cast<T>(address_to_procedure);
	}

private:
	void* find_proc_in_target_manualmapped_library(const wchar_t* library_name, const char* proc_name);
	void* find_proc_in_target_manualmapped_library(uintptr_t library_base_addr, const char* proc_name);
	void* find_proc_in_target_manualmapped_library(uintptr_t library_base_addr, uint16_t ordinal);

	void for_each_exported_name(uintptr_t library_base_addr, const std::function<void(uintptr_t rva, const std::string& name, uint16_t ordinal, PIMAGE_DATA_DIRECTORY idd)>& callback);

	void* handle_forwarded_export_entry(const std::string& forwarded_name);

	template<typename T>
	bool return_if_absent(T func, const char* function_name)
	{
		if (!func)
		{
			CConsole::the().error("LibraryLoader: The function {} is not available.", function_name);
			return false;
		}

		return true;
	}

	// this are needed in order to actually load the dlls.
	pfnLoadLibraryExW_t pfnLoadLibraryExW;
	pfnFreeLibrary_t pfnFreeLibrary;
	pfnSetDefaultDllDirectories_t pfnSetDefaultDllDirectories;
	pfnLdrAddDllDirectory_t pfnLdrAddDllDirectory;
	pfnRtlInitUnicodeString_t pfnRtlInitUnicodeString;

	// since manual mapped modules don't provide loader entry inside PEB, there's now way of searching for exports, for example.
	// when manualmapped module is loaded, this structure will hold information about its exports, base address, and more.
	struct manualmapped_module_information_t
	{
		std::wstring	m_module_name = {};
		uintptr_t		m_base_address = 0;

		struct module_export_t
		{
			std::string m_procname = {};
			uint16_t	m_ordinal = 0;
			uintptr_t	m_address = 0;
		};
		std::vector<module_export_t> m_exports;

		inline module_export_t* find_export(const char* proc_name)
		{
			for (auto& e : m_exports)
			{
				if (e.m_procname == proc_name)
					return &e;
			}

			assert(0);
			return nullptr;
		}
		inline module_export_t* find_export(uint16_t ord)
		{
			for (auto& e : m_exports)
			{
				if (e.m_ordinal == ord)
					return &e;
			}

			assert(0);
			return nullptr;
		}

		inline bool operator==(const wchar_t* name) const { return !wcsicmp(m_module_name.c_str(), name); };
		inline bool operator!=(const wchar_t* name) const { return wcsicmp(m_module_name.c_str(), name); };
	};
	std::vector<manualmapped_module_information_t> m_mapped_info;

	inline manualmapped_module_information_t* find_manualmapped_module(const wchar_t* library_name)
	{
		for (auto& m : m_mapped_info)
		{
			if (m.m_module_name == library_name)
				return &m;
		}
		return nullptr;
	}
	
	inline manualmapped_module_information_t* find_manualmapped_module(uintptr_t library_base_address)
	{
		for (auto& m : m_mapped_info)
		{
			if (m.m_base_address == library_base_address)
				return &m;
		}
		return nullptr;
	}

	//
	// encrypted modules
	//

	// since encrypted modules aren't normally visible (e.g. they're not in PEB), we
	// have to detect if the module is encrypted and then decrypt it and load into our memory.
	struct encrypted_module_information_t
	{
		std::wstring					 m_module_name = {};
		reconstructed_blob_module_info_t m_reconstructed;

		inline bool operator==(const wchar_t* name) const { return !wcsicmp(m_module_name.c_str(), name); };
		inline bool operator!=(const wchar_t* name) const { return wcsicmp(m_module_name.c_str(), name); };
	};
	std::vector<encrypted_module_information_t> m_encrypted_modules_info;

	inline bool is_module_encrypted(const wchar_t* library_name)
	{
		return find_encrypted_module(library_name) != nullptr;
	}

	inline bool is_module_encrypted(uintptr_t library_base_address)
	{
		return find_encrypted_module(library_base_address) != nullptr;
	}

	inline encrypted_module_information_t* find_encrypted_module(const wchar_t* library_name)
	{
		for (auto& m : m_encrypted_modules_info)
		{
			if (m.m_module_name == library_name)
				return &m;
		}
		return nullptr;
	}

	inline encrypted_module_information_t* find_encrypted_module(uintptr_t library_base_address)
	{
		for (auto& m : m_encrypted_modules_info)
		{
			if (m.m_reconstructed.opt().ImageBase == library_base_address)
				return &m;
		}
		return nullptr;
	}

	bool m_initialized = false;
};

CLibraryLoader g_libloader;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CLibraryLoader, ILibraryLoader, ILIBRARYLOADER_INTERFACEID, g_libloader);

//-------------------------------------------------------------------------------
//
// CLibraryLoader implementation
//
//-------------------------------------------------------------------------------

CLibraryLoader::CLibraryLoader()
{
	g_libloader_i = this;

	initialize();
}

CLibraryLoader::~CLibraryLoader()
{
	g_libloader_i = nullptr;
}

void CLibraryLoader::initialize()
{
	CConsole::the().info("Initializing LibraryLoader...");

	if (m_initialized)
	{
		CConsole::the().warning("Recursive call to LibraryLoader initialization routine.");
		return;
	}

	// ntdll.dll and kernel32.dll are loaded by default, always, so we don't have to worry about loading those.
	auto ke32 = (HMODULE)get_target_loaded_dll_base_address(L"kernel32.dll");
	pfnLoadLibraryExW = initialize_procedure_function_pointer<pfnLoadLibraryExW_t>(ke32, "LoadLibraryExW");
	pfnFreeLibrary = initialize_procedure_function_pointer<pfnFreeLibrary_t>(ke32, "FreeLibrary");
	pfnSetDefaultDllDirectories = initialize_procedure_function_pointer<pfnSetDefaultDllDirectories_t>(ke32, "SetDefaultDllDirectories");

	auto ntdll = (HMODULE)get_target_loaded_dll_base_address(L"ntdll.dll");
	pfnRtlInitUnicodeString = initialize_procedure_function_pointer<pfnRtlInitUnicodeString_t>(ntdll, "RtlInitUnicodeString");
	pfnLdrAddDllDirectory = initialize_procedure_function_pointer<pfnLdrAddDllDirectory_t>(ntdll, "LdrAddDllDirectory");

	if (pfnSetDefaultDllDirectories)
	{
		// specify a set of directories to search when LoadLibraryExW will be called by the process without any flags.
		// we have control in our code over dll loading, by using load_library function. However, one
		// of the game modules (hw.dll or client dlls) do load module using LoadLibrary function without any flags.
		// and since they're in a distant directory (not within our executable's directory), we have to specify
		// search flags in order to load it properly together with its dependencies.
		BOOL did_set = pfnSetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

		if (!did_set)
		{
			CConsole::the().warning("LoadLibrary: Couldn't set default dll directories. This may cause errors when loading a module. (last err: {})", CGenericUtil::the().get_last_win_error());
		}
	}
	else
	{
		// This feature of 'adding DLL load search directories' doesn't exist in windows 7 and older versions at all.
		CConsole::the().warning("LoadLibrary: The 'SetDefaultDllDirectories' could not be found! You may have trouble loading dlls!");
	}	

	CConsole::the().info("LoadLibrary initialized.");

	m_initialized = true;
}

bool CLibraryLoader::register_manualmapped_module(const wchar_t* module_name, uintptr_t base_address)
{
	assert(base_address);

	manualmapped_module_information_t m;
	m.m_module_name = module_name;
	m.m_base_address = base_address;

	for_each_exported_name(m.m_base_address, [&](uintptr_t rva, const std::string& name, uint16_t ordinal, PIMAGE_DATA_DIRECTORY idd)
	{
		manualmapped_module_information_t::module_export_t e;

		e.m_procname = name;
		e.m_ordinal = ordinal;
		e.m_address = base_address + rva;
		m.m_exports.emplace_back(e);
	});

	m_mapped_info.emplace_back(m);

	return true;
}

void CLibraryLoader::remove_manualmapped_module(const wchar_t* module_name)
{
	m_mapped_info.erase(std::remove(m_mapped_info.begin(), m_mapped_info.end(), module_name), m_mapped_info.end());
}

bool CLibraryLoader::register_encrypted_module(const wchar_t* module_name, reconstructed_blob_module_info_t blob_info)
{
	encrypted_module_information_t m;
	m.m_module_name = module_name;
	m.m_reconstructed = blob_info;

	m_encrypted_modules_info.emplace_back(m);

	return true;
}

void CLibraryLoader::remove_encrypted_module(const wchar_t * module_name)
{
	m_encrypted_modules_info.erase(std::remove(m_encrypted_modules_info.begin(), m_encrypted_modules_info.end(), module_name), m_encrypted_modules_info.end());
}

void* CLibraryLoader::find_proc_in_target_library(const wchar_t* library_name, const char* proc_name)
{
	// First we obtain the base address of the library by it's name.
	auto base = get_target_loaded_dll_base_address(library_name);
	if (!base)
	{
		// let's try manualmapped modules, if any
		return find_proc_in_target_manualmapped_library(library_name, proc_name);
	}

	return find_proc_in_target_library(base, proc_name);
}

void* CLibraryLoader::find_proc_in_target_library(uintptr_t library_base_addr, const char* proc_name)
{
	void* pfn_routine = nullptr;
	for_each_exported_name(library_base_addr, [&](uintptr_t rva, const std::string& name, uint16_t ordinal, PIMAGE_DATA_DIRECTORY idd)
	{
		if (!stricmp(name.c_str(), proc_name))
		{
			//
			// We access the function address by its ordinal. Ordinal is like an index into the export table and
			// each exported function has its own ordinal, which we can access it through.
			//

			bool is_forwarded = (rva >= idd->VirtualAddress) &&
				(rva < idd->VirtualAddress + idd->Size);

			if (!is_forwarded)
			{
				pfn_routine = reinterpret_cast<uintptr_t*>((uint8_t*)library_base_addr + rva); // Note: Without the ordinal base!
				return;
			}
			else
			{
				std::string forwarded_name = reinterpret_cast<const char*>((uint8_t*)library_base_addr + rva);
				pfn_routine = handle_forwarded_export_entry(forwarded_name);
				return;
			}
		}
	});

	if (!pfn_routine)
	{
		// let's try manualmapped modules, if any
		return find_proc_in_target_manualmapped_library(library_base_addr, proc_name);
	}

	return pfn_routine;
}

void* CLibraryLoader::find_proc_in_target_library(uintptr_t library_base_addr, uint16_t ordinal)
{
	void* pfn_routine = nullptr;
	for_each_exported_name(library_base_addr, [&](uintptr_t rva, const std::string& name, uint16_t ord, PIMAGE_DATA_DIRECTORY idd)
	{
		if (ord == ordinal)
		{
			//
			// We access the function address by its ordinal. Ordinal is like an index into the export table and
			// each exported function has its own ordinal, which we can access it through.
			//

			bool is_forwarded = (rva >= idd->VirtualAddress) &&
				(rva < idd->VirtualAddress + idd->Size);

			if (!is_forwarded)
			{
				pfn_routine = reinterpret_cast<uintptr_t*>((uint8_t*)library_base_addr + rva); // Note: Without the ordinal base!
				return;
			}
			else
			{
				std::string forwarded_name = reinterpret_cast<const char*>((uint8_t*)library_base_addr + rva);
				pfn_routine = handle_forwarded_export_entry(forwarded_name);
				return;
			}
		}
	});

	if (!pfn_routine)
	{
		// let's try manualmapped modules, if any
		return find_proc_in_target_manualmapped_library(library_base_addr, ordinal);
	}

	return pfn_routine;
}

uintptr_t CLibraryLoader::get_target_loaded_dll_base_address(const wchar_t* library_name)
{
	auto returned_entry = get_target_loaded_dll_data_table_entry(library_name);

	if (returned_entry != NULL)
	{
		return (uintptr_t)returned_entry->DllBase;
	}

	// let's see our manual mapped modules, too
	auto mmapped = find_manualmapped_module(library_name);
	if (mmapped)
	{
		return mmapped->m_base_address;
	}

	// not manual mapped? let's see if encrypted
	auto encrypted = find_encrypted_module(library_name);
	if (encrypted)
	{
		return encrypted->m_reconstructed.opt().ImageBase;
	}

	return NULL; // :/
}

PLDR_DATA_TABLE_ENTRY CLibraryLoader::get_target_loaded_dll_data_table_entry(const wchar_t * library_name)
{
	auto peb = NtCurrentTeb()->ProcessEnvironmentBlock;

	//
	// PEB contains information about current process. This includes the list of loaded
	// modules into process memory. We can iterate through each module and get the data 
	// table entry which contains the module base address.
	//

	auto topmost_entry = &peb->Ldr->InMemoryOrderModuleList;
	auto front_link = topmost_entry->Flink;

	PLDR_DATA_TABLE_ENTRY returned_entry = nullptr;

	do
	{
		auto module_entry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>((uint8_t*)front_link - (sizeof(LIST_ENTRY)));

		if (!_wcsicmp(module_entry->BaseDllName.Buffer, library_name))
		{
			returned_entry = module_entry;
			break;
		}

		front_link = front_link->Flink;
	} while (front_link != topmost_entry);

	return returned_entry;
}

std::pair<uintptr_t, uintptr_t> CLibraryLoader::get_loaded_dll_address_space(const wchar_t* library_name, EAddressSpaceSearch settings)
{
	auto encrypted = find_encrypted_module(library_name);
	if (encrypted != nullptr)
	{
		uintptr_t start = encrypted->m_reconstructed.opt().ImageBase;

		switch (settings)
		{
			default:
			case SPACE_FULL:
			{
				uintptr_t end = start + encrypted->m_reconstructed.opt().SizeOfImage;
				return std::make_pair(start, end);
			}
			case SPACE_CODE:
			{
				start = start + encrypted->m_reconstructed.opt().BaseOfCode;
				uintptr_t end = start + encrypted->m_reconstructed.opt().SizeOfCode;
				return std::make_pair(start, end);
			}
		}
	}
	else
	{
		uintptr_t start = get_target_loaded_dll_base_address(library_name);
		if (!start)
		{
			return {};
		}

		// Every module has a DOS header at the beginning of its address space.
		auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(start);
		if (!dos)
			return {};

		// NT header is located via the e_lfanew member inside DOS.
		auto nt_hdrs = reinterpret_cast<PIMAGE_NT_HEADERS>((uint8_t*)start + dos->e_lfanew);
		if (!nt_hdrs)
			return {};

		switch (settings)
		{
			default:
			case SPACE_FULL:
			{
				uintptr_t end = start + nt_hdrs->OptionalHeader.SizeOfImage;
				return std::make_pair(start, end);
			}
			case SPACE_CODE:
			{
				start = start + nt_hdrs->OptionalHeader.BaseOfCode;
				uintptr_t end = start + nt_hdrs->OptionalHeader.SizeOfCode;
				return std::make_pair(start, end);
			}
		}
	}
}

HMODULE CLibraryLoader::load_library(const wchar_t* path_to, const wchar_t* name)
{
	// hell, this was fucking pain to figure out...

	HMODULE module = NULL;

	// compute the full path for the searched dll
	std::wstring full_path;
	if (path_to)
	{
		full_path = path_to;
		if (full_path.back() != '/' && full_path.back() != '\\')
			full_path.push_back('\\');
		full_path += name;
	}
	else
	{
		full_path = name;
	}

	full_path = CStringTools::the().fix_trailing_slashes(full_path);

	CConsole::the().info("LoadLibrary: Trying to load module '{}'...", CStringTools::the().utf16_to_utf8(full_path));

	module = pfnLoadLibraryExW((LPWSTR)full_path.c_str(), NULL, NULL);

	if (!module)
	{
		CConsole::the().warning("LoadLibrary: Loading module using without using any flags failed... (last err: {})", CGenericUtil::the().get_last_win_error());

		// first try to load the dll in common dirs (e.g. PATH or SysWOW64 etc..)
		module = pfnLoadLibraryExW((LPWSTR)full_path.c_str(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

		if (!module)
		{
			CConsole::the().warning("LoadLibrary: Loading module using default dirs failed... (last err: {})", CGenericUtil::the().get_last_win_error());

			// try to load the dll using our custom path (has to be absolute)
			if (path_to)
			{
				// add the path to the load list
				BOOL did_add = add_directory_to_search_list(path_to);
				if (!did_add)
				{
					CConsole::the().warning("LoadLibrary: Failed to add dll directory '{}' to the search list. (last err: {})",
											CStringTools::the().utf16_to_utf8(path_to),
											CGenericUtil::the().get_last_win_error());
				}

				// don't need to specify the full path, since we've added it into the search list
				module = pfnLoadLibraryExW((LPWSTR)name, NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);
			}

			if (!module)
			{
				// altered path tries to search for dependencies inside the loaded dll path. The path must be absolute.
				if (path_to)
				{
					module = pfnLoadLibraryExW((LPWSTR)full_path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (!module)
					{
						CConsole::the().warning("LoadLibrary: Loading module using altered search path failed... (last err: {})", CGenericUtil::the().get_last_win_error());
					}
				}

				if (!module)
				{
					// the directory where the loaded dll is temporarily added to the directory list when loading dll dependencies
					module = pfnLoadLibraryExW((LPWSTR)full_path.c_str(), NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
					if (!module)
					{
						CConsole::the().warning("LoadLibrary: Loading module using search dll load dir failed... (last err: {})", CGenericUtil::the().get_last_win_error());
					}

					// if this fails, fuck you, windows
				}
			}
		}
	}

	if (!module)
	{
		CConsole::the().error("LoadLibrary: Completely failed to load the module. (last err: {})", CGenericUtil::the().get_last_win_error());
		return NULL;
	}

	return module;
}

void CLibraryLoader::unload_library(HMODULE module)
{
	pfnFreeLibrary(module);
}

bool CLibraryLoader::add_directory_to_search_list(const wchar_t* absolute_directory)
{
	if (return_if_absent(pfnLdrAddDllDirectory, "LdrAddDllDirectory"))
	{
		CConsole::the().error("LoadLibrary: Failed to add directory to the search list because LdrAddDllDirectory is not available.");
		return false;
	}

	DLL_DIRECTORY_COOKIE cookie = NULL;
	UNICODE_STRING uni;
	pfnRtlInitUnicodeString(&uni, absolute_directory);

	NTSTATUS st = pfnLdrAddDllDirectory(&uni, &cookie);

	bool success = st == STATUS_SUCCESS && cookie;

	if (success)
	{
		CConsole::the().info("LoadLibrary: Successfully added directory {} to the DLL search list.", 
							 CStringTools::the().utf16_to_utf8(absolute_directory));
	}
	else
	{
		CConsole::the().error("LoadLibrary: Failed to add directory to the search list: '{}'", CStringTools::the().utf16_to_utf8(absolute_directory));
	}

	return success;
}

std::filesystem::path CLibraryLoader::get_windows_directory(const std::filesystem::path& subdir)
{
	uintptr_t* volatile pNtSystemRoot = (uintptr_t*)0x7FFE0030;
	return std::filesystem::path((const wchar_t*)pNtSystemRoot) / subdir; // cool, isn't it?
}

bool CLibraryLoader::is_dll_loaded(const wchar_t* library_name)
{
	return get_target_loaded_dll_base_address(library_name) != NULL;
}

void* CLibraryLoader::find_proc_in_target_manualmapped_library(const wchar_t* library_name, const char* proc_name)
{
	auto module = find_manualmapped_module(library_name);
	if (!module)
	{
		return nullptr; // no luck here
	}

	auto e = module->find_export(proc_name);
	if (!e)
	{
		return nullptr;
	}

	return (void*)e->m_address;
}

void* CLibraryLoader::find_proc_in_target_manualmapped_library(uintptr_t library_base_addr, const char* proc_name)
{
	auto module = find_manualmapped_module(library_base_addr);
	if (!module)
	{
		return nullptr; // no luck here
	}

	auto e = module->find_export(proc_name);
	return (void*)e->m_address;
}

void* CLibraryLoader::find_proc_in_target_manualmapped_library(uintptr_t library_base_addr, uint16_t ordinal)
{
	auto module = find_manualmapped_module(library_base_addr);
	if (!module)
	{
		return nullptr; // no luck here
	}

	auto e = module->find_export(ordinal);
	return (void*)e->m_address;
}

void CLibraryLoader::for_each_exported_name(uintptr_t library_base_addr,
											const std::function<void(uintptr_t rva, const std::string& name, uint16_t ordinal, PIMAGE_DATA_DIRECTORY idd)>& callback)
{
	auto encrypted = find_encrypted_module(library_base_addr);
	if (encrypted != nullptr)
	{
		auto idd = &encrypted->m_reconstructed.opt().DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		uintptr_t base = encrypted->m_reconstructed.opt().ImageBase;
		for (auto& ex : encrypted->m_reconstructed.m_exports)
		{
			callback(ex.va - base, ex.fn_name, ex.ordinal, (PIMAGE_DATA_DIRECTORY)idd);
		}
	}
	else
	{
		// Every module has a DOS header at the beginning of its address space.
		auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(library_base_addr);
		if (!dos)
			return;

		// NT header is located via the e_lfanew member inside DOS.
		auto nt_hdrs = reinterpret_cast<PIMAGE_NT_HEADERS>((uint8_t*)library_base_addr + dos->e_lfanew);
		if (!nt_hdrs)
			return;

		// Get the data directory for exports from optional header located inside NT header.
		auto exports_data_directory = &nt_hdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		if (!exports_data_directory->Size || !exports_data_directory->VirtualAddress)
			return;

		// Individual export addresses we need are located in this data structure.
		auto exports_directory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>((uint8_t*)library_base_addr + exports_data_directory->VirtualAddress);
		if (!exports_directory)
			return;

		auto function_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)library_base_addr + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uintptr_t*>((uint8_t*)library_base_addr + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)library_base_addr + exports_directory->AddressOfNameOrdinals);

		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)library_base_addr + name_table_base[i]);
			callback(function_table_base[ordinal_table_base[i]], export_procname, ordinal_table_base[i], exports_data_directory);
		}
	}
}

void* CLibraryLoader::handle_forwarded_export_entry(const std::string& forwarded_name)
{
	std::string dll_name = forwarded_name.substr(0, forwarded_name.find_last_of('.')) + ".dll";

	// try to load forwarded dll
	HMODULE forwarded_module;
	forwarded_module = load_library(nullptr, CStringTools::the().utf8_to_utf16(dll_name).c_str());
	if (!forwarded_module)
	{
		assert(0);
		return nullptr;
	}

	// can be one of those two
	size_t pos = forwarded_name.find_last_of('.');
	std::string routine_name_or_ordinal = forwarded_name.substr(pos + 1);

	if (routine_name_or_ordinal.find('#') != std::string::npos) // ordinal
	{
		std::string ordinal_str = routine_name_or_ordinal.substr(routine_name_or_ordinal.find('#') + 1);

		return find_proc_in_target_library((uintptr_t)forwarded_module, (uint16_t)std::stol(ordinal_str));
	}
	else // function
	{
		return find_proc_in_target_library((uintptr_t)forwarded_module, routine_name_or_ordinal.c_str());
	}
}
