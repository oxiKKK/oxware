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
// PortableExecutableObject.cpp -- Handles DLL parsing.
//

#include "precompiled.h"

bool CPortableExecutableObject::create(const std::filesystem::path& dll_path)
{
	m_dll_path = dll_path;
	
	if (!m_dll_contents.create(dll_path, (uintptr_t)std::filesystem::file_size(dll_path)))
	{
		CConsole::the().error("Failed to create ByteBuffer object on DLL.");
		return false;
	}

	CConsole::the().info("Read {} bytes of DLL data", m_dll_contents.get_size());

	if (!parse_dll())
	{
		CConsole::the().error("Failed to parse DLL contents.");
		return false;
	}

	CConsole::the().info("Successfuly parsed DLL contents.");

	return true;
}

void CPortableExecutableObject::destroy()
{
	m_dll_contents.destroy();
}

bool CPortableExecutableObject::parse_dll()
{
	m_dos = m_dll_contents.get_at<IMAGE_DOS_HEADER>(0);
	m_nt_hdrs = m_dll_contents.get_at<IMAGE_NT_HEADERS>(m_dos->e_lfanew);
	m_opt_hdr = &m_nt_hdrs->OptionalHeader;
	m_file_hdr = &m_nt_hdrs->FileHeader;

	return true;
}

bool CPortableExecutableObject::find_export(const char* name)
{
	assert(m_dos);

	// Get the data directory for exports from optional header located inside NT header.
	auto exports_data_directory = &m_opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	if (!exports_data_directory->Size || !exports_data_directory->VirtualAddress)
		return false;

	// Individual export addresses we need are located in this data structure.
	auto exports_directory = m_dll_contents.get_at<IMAGE_EXPORT_DIRECTORY>(offset_relative_to_a_section(exports_data_directory->VirtualAddress));
	if (!exports_directory)
		return false;

	auto function_table_base = m_dll_contents.get_at<uintptr_t>(offset_relative_to_a_section(exports_directory->AddressOfFunctions));
	auto name_table_base = m_dll_contents.get_at<uintptr_t>(offset_relative_to_a_section(exports_directory->AddressOfNames));
	auto ordinal_table_base = m_dll_contents.get_at<uint16_t>(offset_relative_to_a_section(exports_directory->AddressOfNameOrdinals));

	for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
	{
		auto export_procname = m_dll_contents.get_at<const char>(offset_relative_to_a_section(name_table_base[i]));
		if (!stricmp(export_procname, name))
		{
			return true;
		}
	}

	return false;
}

uintptr_t CPortableExecutableObject::offset_relative_to_a_section(uintptr_t rva)
{
	assert(m_dos);
	
	auto section = IMAGE_FIRST_SECTION(m_nt_hdrs);

	for (size_t i = 0; i < m_file_hdr->NumberOfSections; i++)
	{
		// Check if we're in-bounds
		if (rva >= section->VirtualAddress && rva <= (section->VirtualAddress + section->Misc.VirtualSize - 1))
		{
			return (rva - section->VirtualAddress) + section->PointerToRawData;
		}

		section++;
	}

	assert(0);
	return 0;
}