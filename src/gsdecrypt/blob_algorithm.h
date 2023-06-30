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
#ifndef BLOB_ALHORITHM
#define BLOB_ALHORITHM
#pragma once

#include <vector>

class CBlobAlgorithm
{
public:
	DECL_BASIC_CLASS(CBlobAlgorithm)

public:
	bool decrypt_file_buffer(uint8_t* filebuffer, uint32_t length, reconstructed_blob_module_info_t& info);

	// Write raw data for each section into the file
	void write_section_data(uint8_t* filebuffer, uint32_t file_alignment, std::ostringstream& ofs);

	void decrypt_header(blob_hdr_t* header);

	// Xor the entire file using 'W'
	void xor_buffer(uint8_t* filebuffer, uint32_t length);

private:
	// Checks for magic number
	bool valid_info_header();

	// Checks for blob header data
	bool valid_blob_data_header();

private:
	blob_info_t*			m_info;
	blob_hdr_t*				m_header;
	blob_section_t*			m_sectionbase;
};

#endif