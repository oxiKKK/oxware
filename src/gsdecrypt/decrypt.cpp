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
#include "precompiled.h"

// The encrypted file always has @ character at this offset inside th file
#define AT_SIGN_CHARACTER_OFFSET 60

bool CDecryptProcessor::process_file(const std::filesystem::path& file_in, reconstructed_blob_module_info_t& info, uint32_t filesize)
{
	// Read and allocate data
	std::ifstream ifs(file_in, std::ios_base::in | std::ios_base::binary);

	if (ifs.bad())
	{
		CConsole::the().error("Failed to open the input file!");
		return false;
	}

	// Files smaller than this are just not gonna work
	if (filesize < 4096)
	{
		CConsole::the().error("File has invalid length ({} bytes)", filesize);
		return false;
	}

	if (!allocate_buffer(filesize))
	{
		CConsole::the().error("Failed to allocate buffer!");
		return false;
	}

	// Read the whole file contents
	ifs.read((char*)m_filebuffer, filesize);
	ifs.close();

	// Decrypt the buffer, gain blob information, build pe header
	if (!CBlobAlgorithm::the().decrypt_file_buffer(m_filebuffer, m_buffer_size, info))
	{
		CConsole::the().error("Failed to decrypt the file using original Valve blob algorithm!");
		return false;
	}

	// We don't need this buffer anymore
	deallocate_buffer();

	return true;
}

bool CDecryptProcessor::allocate_buffer(uint32_t size)
{
	m_filebuffer = reinterpret_cast<uint8_t*>(malloc(size));

	if (!m_filebuffer)
	{
		CConsole::the().error("Failed to allocate file buffer on heap!");
		return false;
	}

	m_buffer_size = size;

	CConsole::the().info("Allocated {} file buffer bytes", m_buffer_size);

	return true;
}

void CDecryptProcessor::deallocate_buffer()
{
	if (!m_filebuffer)
	{
		CConsole::the().warning("Failed to deallocate null file buffer!");
		return;
	}

	free(m_filebuffer);
	m_filebuffer = nullptr;
}
