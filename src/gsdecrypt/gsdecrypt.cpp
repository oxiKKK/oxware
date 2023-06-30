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

bool CGSDecrypt::decrypt_dll(const std::filesystem::path& encrypted_file, reconstructed_blob_module_info_t& info)
{
	CConsole::the().info("Starting DLL decryption...");

	info = {};

	auto path_in = encrypted_file;

	CConsole::the().info("Input file: \"{}\"", path_in.string().c_str());

	// If the file is not found, try to search locally
	if (!std::filesystem::exists(path_in))
	{
		CConsole::the().error("Input file doesn't exist!");
		return false;
	}

	const auto in_filesize = std::filesystem::file_size(path_in);

	if (!CDecryptProcessor::the().process_file(path_in, info, in_filesize))
	{
		CConsole::the().error("Failed to decrypt the file!");
		return false;
	}

	CConsole::the().info("Success!");
	return true;
}

bool CGSDecrypt::is_dll_encrypted(const std::filesystem::path& encrypted_file)
{
	std::ifstream ifs(encrypted_file, std::ios_base::in | std::ios_base::binary);

	if (ifs.bad())
	{
		CConsole::the().error("Failed to open the input file! ({})", encrypted_file);
		return false;
	}

	const auto filesize = std::filesystem::file_size(encrypted_file);
	if (!filesize)
	{
		CConsole::the().error("Invalid size on input file! ({})", encrypted_file);
		return false;
	}

	auto filebuffer = new uint8_t[filesize];

	ifs.read((char*)filebuffer, filesize);

	auto info = reinterpret_cast<blob_info_t*>(filebuffer);
	bool is_encrypted = info->m_dwMagic == BLOB_ALGORITHM_MAGIC;

	delete[] filebuffer;
	ifs.close();

	return is_encrypted;
}
