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
#ifndef GSDECRYPT_H
#define GSDECRYPT_H
#pragma once

#include "blob.h"

struct reconstructed_import_entry_t
{
	std::string fn_name;
	uint16_t	hint;
	uint16_t	ordinal;
	uint32_t	va;
};

struct reconstructed_export_entry_t
{
	std::string fn_name;
	uint16_t	ordinal;
	uint32_t	va;
};

// information you retreive after decrypting the file
struct reconstructed_blob_module_info_t
{
	// blob data in case you need it
	struct 
	{
		blob_info_t info;
		blob_hdr_t  header;
	} blob;

	// PE format data
	IMAGE_DOS_HEADER dos_hdr;
	IMAGE_NT_HEADERS nt_headers;
	std::vector<IMAGE_SECTION_HEADER> pe_section_table;

	std::vector<reconstructed_import_entry_t> m_imports;
	std::vector<reconstructed_export_entry_t> m_exports;

	// helper functions
	inline auto& dos() const { return dos_hdr; }
	inline auto& opt() const { return nt_headers.OptionalHeader; }
	inline auto& file() const { return nt_headers.FileHeader; }
};

// GoldSource DLL decryptor for older builds
// https://github.com/oxiKKK/goldsource-dll-decryptor
class CGSDecrypt
{
public:
	DECL_BASIC_CLASS(CGSDecrypt);

public:
	// decrypts the input file and writes the result into a new byte stream.
	bool decrypt_dll(const std::filesystem::path& encrypted_file, reconstructed_blob_module_info_t& info);

	bool is_dll_encrypted(const std::filesystem::path& encrypted_file);
};

#endif // GSDECRYPT_H