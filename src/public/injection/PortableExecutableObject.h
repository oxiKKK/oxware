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

#ifndef PORTABLEEXECUTABLEOBEJCT_H
#define PORTABLEEXECUTABLEOBEJCT_H
#pragma once

#include <iostream>

#include "ByteBuffer.h"

// An object to carry around information about a dll
class CPortableExecutableObject
{
public:
	bool create(const std::filesystem::path& dll_path);
	void destroy();

	// ------- API ---------

	inline size_t get_size_of_image() const { return assert_get(m_opt_hdr)->SizeOfImage; }
	inline size_t get_size_of_headers() const { return assert_get(m_opt_hdr)->SizeOfHeaders; }
	inline uintptr_t get_image_base() const { return assert_get(m_opt_hdr)->ImageBase; }
	inline auto& get_data_directory(uint16_t ddir) const { assert(ddir >= 0 && ddir < IMAGE_NUMBEROF_DIRECTORY_ENTRIES); return assert_get(m_opt_hdr)->DataDirectory[ddir]; }
	inline auto& get_byte_buffer() const { return m_dll_contents; }

	inline auto get_image_first_section() const { return IMAGE_FIRST_SECTION(assert_get(m_nt_hdrs)); }
	inline uint16_t get_num_of_sections() const { return assert_get(m_file_hdr)->NumberOfSections; }

	inline uint32_t va_as_rva(uint32_t va) const { return va - assert_get(m_opt_hdr)->ImageBase; }
	inline uint32_t rva_as_va(uint32_t rva) const { return rva + assert_get(m_opt_hdr)->ImageBase; }

	// returns true if exported name is within this executable
	bool find_export(const char* name);

	// Accepts rva address and returns file pointer to the address 
	// relative to the start of appropriate section. If the rva is
	// out of bounds of all sections, 0 is returned.
	uintptr_t offset_relative_to_a_section(uintptr_t rva);

private:
	bool parse_dll();

	// utility function to always assure us that the pointer we're getting is alright.
	template<typename T>
	inline T* assert_get(T* what) const { assert(what); return what; }

private:
	std::filesystem::path m_dll_path;
	ByteBuffer<uintptr_t> m_dll_contents;

	PIMAGE_DOS_HEADER m_dos;
	PIMAGE_NT_HEADERS m_nt_hdrs;
	PIMAGE_OPTIONAL_HEADER m_opt_hdr;
	PIMAGE_FILE_HEADER m_file_hdr;
};

#endif // PORTABLEEXECUTABLEOBEJCT_H