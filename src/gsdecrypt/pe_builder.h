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
#ifndef PE_BUILDER_H
#define PE_BUILDER_H
#pragma once

struct ordinary_section_data_t
{
	const char* m_szName;
	DWORD		m_dwCharacteristics;
};

class CPEBuilder
{
public:
	DECL_BASIC_CLASS(CPEBuilder);

public:
	// Reconstructs the PE header for new image from the blob data that we have
	reconstructed_blob_module_info_t build_pe_header(uint8_t* filebuffer, blob_hdr_t* blob_hdr, blob_section_t* blob_sections);
	
private:
	// Building data directories from OptionalHeader
	void build_data_directories(uint8_t* filebuffer, blob_hdr_t* blob_hdr, blob_section_t* blob_sections);
	void print_data_directory(const char* name, PIMAGE_DATA_DIRECTORY idd);

	void process_imports(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* idd, IMAGE_DATA_DIRECTORY* iatdd, blob_hdr_t* blob_hdr, blob_section_t* blob_sections, uint32_t* export_last_thunk_func);
	void process_exports(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* edd, blob_hdr_t* blob_hdr, blob_section_t* blob_sections, uint32_t export_last_thunk_func);
	void process_resources(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* rdd, blob_hdr_t* blob_hdr, blob_section_t* blob_sections);

	uint32_t rva_to_u32_offset(blob_section_t* sectionbase, uint16_t num_sections, uint32_t rva);

	// Little utility function to convert FileHeader's timestamp into actual readable string
	std::string timestamp_as_string(std::time_t timestamp);

private:
	reconstructed_blob_module_info_t info; // current info
};

#endif