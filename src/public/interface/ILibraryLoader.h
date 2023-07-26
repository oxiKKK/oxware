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

#ifndef ILIBRARYLOADER_H
#define ILIBRARYLOADER_H
#pragma once

#include "interface.h"
#include "commondefs.h"

#include "ifilesystem.h"

#include <gsdecrypt/gsdecrypt.h>

enum EAddressSpaceSearch
{
	SPACE_FULL,
	SPACE_CODE,
};

class ILibraryLoader : public IBaseInterface
{
public:
	virtual void initialize() = 0;

	virtual bool register_manualmapped_module(const wchar_t* module_name, uintptr_t base_address) = 0;
	virtual void remove_manualmapped_module(const wchar_t* module_name) = 0;

	virtual bool register_encrypted_module(const wchar_t* module_name, reconstructed_blob_module_info_t blob_info) = 0;
	virtual void remove_encrypted_module(const wchar_t* module_name) = 0;

	virtual void* find_proc_in_target_library(const wchar_t* library_name, const char* proc_name) = 0;
	virtual void* find_proc_in_target_library(uintptr_t library_base_addr, const char* proc_name) = 0;

	virtual uintptr_t get_target_loaded_dll_base_address(const wchar_t* library_name) = 0;
	virtual PLDR_DATA_TABLE_ENTRY get_target_loaded_dll_data_table_entry(const wchar_t* library_name) = 0;

	virtual std::pair<uintptr_t, uintptr_t> get_loaded_dll_address_space(const wchar_t* library_name, EAddressSpaceSearch settings = SPACE_FULL) = 0;

	virtual HMODULE load_library(const wchar_t* path_to, const wchar_t* name) = 0;
	virtual void unload_library(HMODULE module) = 0;
	
	virtual std::filesystem::path get_windows_directory(const std::filesystem::path& subdir = "") = 0;

	virtual bool is_dll_loaded(const wchar_t* library_name) = 0;
};

extern ILibraryLoader* g_libloader_i;

#define ILIBRARYLOADER_INTERFACEID "ILibraryLoader"

#endif // ILIBRARYLOADER_H