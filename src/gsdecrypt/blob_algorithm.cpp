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

bool CBlobAlgorithm::decrypt_file_buffer(uint8_t* filebuffer, uint32_t length, reconstructed_blob_module_info_t& info)
{
	CConsole::the().info("Starting to decrypt the file...");

	m_info = reinterpret_cast<blob_info_t*>(filebuffer);

	// Validate magic number
	if (!valid_info_header())
	{
		CConsole::the().error("Bad blob info header!");
		return false;
	}

	CConsole::the().info("--- Blob info header ---");
	CConsole::the().info("  Path    : {}", m_info->m_szPath[0] != '\0' ? m_info->m_szPath : "none");
	CConsole::the().info("  Describe: {}", m_info->m_szDescribe[0] != '\0' ? m_info->m_szDescribe : "none");
	CConsole::the().info("  Company : {}", m_info->m_szCompany[0] != '\0' ? m_info->m_szCompany : "none");
	CConsole::the().info("  Magic   : 0x{:08X}", m_info->m_dwMagic);

	// Xor the entire file buffer with 'W'
	xor_buffer(filebuffer, length);

	// Get the blob header
	m_header = reinterpret_cast<blob_hdr_t*>(filebuffer + sizeof(blob_info_t));
	decrypt_header(m_header);

	// Validate addresses exposed by the blob header
	if (!valid_blob_data_header())
	{
		CConsole::the().error("Bad blob data header!");
		return false;
	}

	// In blob files, there's always one section+, so we increment this now
	// so we don't have to do + 1 every time we use this afterwards.
	m_header->m_wSectionCount++;
	
	CConsole::the().info("--- Blob data header ---");
	CConsole::the().info("                {:>10} {}", "VA", "RVA");
	CConsole::the().info("  Image base  : 0x{:08X}", m_header->m_dwImageBase);
	CConsole::the().info("  Entry point : 0x{:08X} 0x{:08X}", m_header->m_dwEntryPoint, m_header->m_dwEntryPoint - m_header->m_dwImageBase);
	CConsole::the().info("  Import table: 0x{:08X} 0x{:08X}", m_header->m_dwImportTable, m_header->m_dwImportTable - m_header->m_dwImageBase);
	CConsole::the().info("  Export      : 0x{:08X} 0x{:08X}", m_header->m_dwExportPoint, m_header->m_dwExportPoint - m_header->m_dwImageBase);
	CConsole::the().info("  Checksum    : 0x{:08X}", m_header->m_dwCheckSum);
	CConsole::the().info("  Sections    : {}", m_header->m_wSectionCount);

	m_sectionbase = reinterpret_cast<blob_section_t*>(filebuffer + sizeof(blob_info_t) + sizeof(blob_hdr_t));

	for (uint16_t i = 0; i < m_header->m_wSectionCount; i++)
	{
		blob_section_t* sec = &m_sectionbase[i]; 

		CConsole::the().info("--- Section {} ---", i);
		CConsole::the().info("  VA             : 0x{:08X}", sec->m_dwVirtualAddress);
		CConsole::the().info("  RVA            : 0x{:08X}", sec->m_dwVirtualAddress - m_header->m_dwImageBase);
		CConsole::the().info("  Virtual size   : {} bytes (0x{:08X})", sec->m_dwVirtualSize, sec->m_dwVirtualSize);
		CConsole::the().info("  Data RA        : 0x{:08X}", sec->m_dwDataAddress);
		CConsole::the().info("  Data size      : {} bytes (0x{:08X})", sec->m_dwDataSize, sec->m_dwDataSize);
		CConsole::the().info("  Is special     : {}", sec->m_bIsSpecial == TRUE ? "yes" : "no");
	}

	// Build pe header for the new image
	info = CPEBuilder::the().build_pe_header(filebuffer, m_header, m_sectionbase);

	// set addional data
	info.blob.info = *m_info;
	info.blob.header = *m_header;

	return true;
}

bool CBlobAlgorithm::valid_info_header()
{
	if (m_info->m_dwMagic != BLOB_ALGORITHM_MAGIC)
	{
		CConsole::the().error("Invalid blob algorithm magic number: 0x{:08X}", m_info->m_dwMagic);
		return false;
	}

	CConsole::the().info("Blob info header is fine.");

	return true;
}

bool CBlobAlgorithm::valid_blob_data_header()
{
	if (!m_header->m_dwImageBase)
	{
		CConsole::the().error("Blob header exposed invalid image base!");
		return false;
	}

	if (!m_header->m_dwExportPoint)
	{
		CConsole::the().error("Error: Blob header exposed invalid entry point!");
		return false;
	}

	if (!m_header->m_dwImportTable)
	{
		CConsole::the().error("Error: Blob header exposed invalid import table!");
		return false;
	}

	if (!m_header->m_wSectionCount)
	{
		CConsole::the().error("Error: Blob header exposed invalid section count!");
		return false;
	}

	CConsole::the().info("Blob data header is fine.");

	return true;
}

void CBlobAlgorithm::xor_buffer(uint8_t* filebuffer, uint32_t length)
{
	uint8_t xor_char = 'W';

	// Start at blob_info header and continue xoring till the end
	for (uint32_t i = sizeof(blob_info_t); i < length; i++)
	{
		filebuffer[i] ^= xor_char;
		xor_char += filebuffer[i] + 'W';
	}

	CConsole::the().info("Xorred file buffer with Valve's magic xor number: W (0x{:02X})", 'W');
}

void CBlobAlgorithm::write_section_data(uint8_t* filebuffer, uint32_t file_alignment, std::ostringstream& ofs)
{
	CConsole::the().info("0x{:08X} Writing section contents", (uint32_t)ofs.tellp());

	for (uint16_t i = 0; i < m_header->m_wSectionCount; i++)
	{
		auto sec = &m_sectionbase[i];

		ofs.write((const char*)&filebuffer[sec->m_dwDataAddress], sec->m_dwDataSize);

		CConsole::the().info("0x{:08X} Wrote {} bytes for section {}", (uint32_t)ofs.tellp(), sec->m_dwDataSize, i);

		// Sections are aligned to the FileAlignment value stored inside 
		// Optional Header.
		uint32_t mod = file_alignment - (sec->m_dwDataSize % file_alignment);

		if (mod != file_alignment)
		{
			uint8_t* zero_buffer = new uint8_t[file_alignment];
			memset(zero_buffer, NULL, file_alignment);

			ofs.write((const char*)zero_buffer, mod);

			CConsole::the().info("0x{:08X} Wrote {} bytes of alignment", (uint32_t)ofs.tellp(), mod);
			delete[] zero_buffer;
		}
	}
}

void CBlobAlgorithm::decrypt_header(blob_hdr_t* header)
{
	// decrypt header with magic XORs
	header->m_dwExportPoint ^= 0x7A32BC85;
	header->m_dwImageBase ^= 0x49C042D1;
	header->m_dwEntryPoint -= 0x0000000C;
	header->m_dwImportTable ^= 0x872C3D47;
}
