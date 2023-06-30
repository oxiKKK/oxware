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

#define ALIGN_AS(num, align) (((num) + ((align) - 1)) & (~((align) - 1)))

// Pre-defined ordinary sections that are static for all Dlls
static const ordinary_section_data_t ordinary_sections[] = {
	{ ".text",  IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ },
	{ ".rdata", IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ },
	{ ".data",  IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE },
	{ ".rsrc",  IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_DISCARDABLE },
	{ NULL, NULL },
};

#define ORD_SIZE (ARRAYSIZE(ordinary_sections) - 1)

// The order of sections inside a pe file should be same across
// all encrypted Valve dlls.
#define ORD_SEC_TEXT	0
#define ORD_SEC_RDATA	1
#define ORD_SEC_DATA	2
#define ORD_SEC_RSRC	3

// Section with rw & uninitialized data characteristic we count as an ordinary one
#define ORDINARY_SCN_CHARACTERISTICS (IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_WRITE)

// This routine is placed right after the cos header and contains
// "This program cannot be run in DOS mode." text
static const char dos_stub[] =
	"\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21\x54\x68"
	"\x69\x73\x20\x70\x72\x6F\x67\x72\x61\x6D\x20\x63\x61\x6E\x6E\x6F"
	"\x74\x20\x62\x65\x20\x72\x75\x6E\x20\x69\x6E\x20\x44\x4F\x53\x20"
	"\x6D\x6F\x64\x65\x2E\x0D\x0D\x0A\x24\x00\x00\x00\x00\x00\x00\x00"
	"\xDB\xD6\xCC\x61\x9F\xB7\xA2\x32\x9F\xB7\xA2\x32\x9F\xB7\xA2\x32"
	"\xE4\xAB\xAE\x32\x97\xB7\xA2\x32\xF0\xA8\xA9\x32\x90\xB7\xA2\x32"
	"\x1C\xAB\xAC\x32\xAE\xB7\xA2\x32\xF0\xA8\xA8\x32\x31\xB7\xA2\x32"
	"\xC0\x95\xA8\x32\x9E\xB7\xA2\x32\x65\x93\xBB\x32\x9D\xB7\xA2\x32"
	"\xC0\x95\xA9\x32\xB1\xB7\xA2\x32\x18\xAB\xA0\x32\xB9\xB7\xA2\x32"
	"\x70\x95\x92\x32\x9E\xB7\xA2\x32\x9F\xB7\xA3\x32\x6C\xB7\xA2\x32"
	"\xFD\xA8\xB1\x32\x8E\xB7\xA2\x32\xE1\x95\xBE\x32\x9C\xB7\xA2\x32"
	"\xAC\x95\x87\x32\x9B\xB7\xA2\x32\xCB\x94\x93\x32\xAB\xB7\xA2\x32"
	"\xCB\x94\x92\x32\xF2\xB7\xA2\x32\x58\xB1\xA4\x32\x9E\xB7\xA2\x32"
	"\x9F\xB7\xA2\x32\x80\xB7\xA2\x32\x60\x97\xA6\x32\x8C\xB7\xA2\x32"
	"\x52\x69\x63\x68\x9F\xB7\xA2\x32\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

// This stub is Valve's one and it is in most of Valve's modules.
static const char valve_stub[] =
	"\x56\x4C\x56\x00\x01\x00\x00\x00\x59\xF0\x18\x00\xA7\x8D\x3D\x5F"
	"\x50\xFD\x83\x01\xE0\xD5\xF7\x8B\x91\xEE\x1C\x6A\xC5\xE2\x3C\x77"
	"\xF7\x6F\xE9\x3D\xE8\x45\x9F\xB1\xCE\x9C\xC1\x99\x6F\x3B\x23\xCE"
	"\x4D\x75\xA2\xBA\x50\xC0\x8F\x6B\x6B\xC6\x04\xA0\xCB\x83\xAB\x9C"
	"\x6A\x77\x43\x55\x95\xFE\x60\x40\xA0\x4C\xAB\x59\xCA\x29\xE0\x35"
	"\xA7\xBD\x2A\x22\xCD\x4B\x33\xA7\xAF\xA3\x1B\x64\xEE\xD5\x6E\x82"
	"\x5E\xE9\x0D\x8D\x74\xB7\x30\x56\xF2\x82\x6B\x9E\xE5\xD7\x18\xA0"
	"\xFF\x0E\x88\x3C\x97\x50\xD3\x30\x19\x10\xE1\xCF\x26\xFC\xFE\xFE"
	"\xE2\xF7\xFF\x12\x22\x93\x78\x0A\x98\x2D\x29\x3E\xF2\x7F\xD6\x99"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00";

// Get offset relative to the base address of a section
uint32_t CPEBuilder::rva_to_u32_offset(blob_section_t* sectionbase, uint16_t num_sections, uint32_t rva)
{
	for (uint16_t i = 0; i < num_sections; i++)
	{
		const auto sec = &sectionbase[i];

		// Check if the address is inside boundary of this section, if
		// yes, proceed further
		if (sec->m_dwVirtualAddress > rva || (sec->m_dwVirtualAddress + sec->m_dwVirtualSize) <= rva)
			continue;

		// Get the relative address starting by this section
		return (rva - sec->m_dwVirtualAddress) + sec->m_dwDataAddress;
	}

	return NULL;
}

reconstructed_blob_module_info_t CPEBuilder::build_pe_header(uint8_t* filebuffer, blob_hdr_t* blob_hdr, blob_section_t* blob_sections)
{
	CConsole::the().info("Building PE header...");

	// reset info
	info = {};

	// This data seems to be constant stable
	info.dos_hdr.e_magic = IMAGE_DOS_SIGNATURE;
	info.dos_hdr.e_cblp = 144;
	info.dos_hdr.e_cp = 3;
	info.dos_hdr.e_crlc = 0;
	info.dos_hdr.e_cparhdr = 4;
	info.dos_hdr.e_minalloc = 0;
	info.dos_hdr.e_maxalloc = 0xffff;
	info.dos_hdr.e_ss = 0;
	info.dos_hdr.e_sp = 184;
	info.dos_hdr.e_csum = 0;
	info.dos_hdr.e_ip = 0;
	info.dos_hdr.e_cs = 0;
	info.dos_hdr.e_lfarlc = 64;
	info.dos_hdr.e_ovno = 0;
	info.dos_hdr.e_oemid = 0;
	info.dos_hdr.e_oeminfo = 0;
	info.dos_hdr.e_lfanew = sizeof(IMAGE_DOS_HEADER) + sizeof(valve_stub);

	CConsole::the().info("  Size of Valve stub program: 0x{:08X}", sizeof(valve_stub));
	CConsole::the().info("  File address to NT headers: 0x{:08X}", info.dos_hdr.e_lfanew);

	CConsole::the().info("DOS header done...");

	// Build NT headers
	info.nt_headers.Signature = IMAGE_NT_SIGNATURE;

	// Build file header
	info.nt_headers.FileHeader.Machine = IMAGE_FILE_MACHINE_I386;
	info.nt_headers.FileHeader.NumberOfSections = blob_hdr->m_wSectionCount;
	info.nt_headers.FileHeader.TimeDateStamp = time(nullptr);
	info.nt_headers.FileHeader.PointerToSymbolTable = 0;
	info.nt_headers.FileHeader.NumberOfSymbols = 0;
	info.nt_headers.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER);
	info.nt_headers.FileHeader.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_LOCAL_SYMS_STRIPPED | IMAGE_FILE_DEBUG_STRIPPED | IMAGE_FILE_DLL;

	CConsole::the().info("  Number of PE sections: {}", info.nt_headers.FileHeader.NumberOfSections);
	CConsole::the().info("  Set FileHeader timestamp: {}", timestamp_as_string(info.nt_headers.FileHeader.TimeDateStamp).c_str());
	CConsole::the().info("  Size of optional header: {}/0x{:08X} bytes", info.nt_headers.FileHeader.SizeOfOptionalHeader, info.nt_headers.FileHeader.SizeOfOptionalHeader);
	CConsole::the().info("  File header characteristics flags: 0x{:08X}", info.nt_headers.FileHeader.Characteristics);

	CConsole::the().info("NT file header done...");

	// Build optional header
	info.nt_headers.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR_MAGIC;
	info.nt_headers.OptionalHeader.MajorLinkerVersion = 6;
	info.nt_headers.OptionalHeader.MinorLinkerVersion = 0;
	info.nt_headers.OptionalHeader.AddressOfEntryPoint = blob_hdr->m_dwEntryPoint - blob_hdr->m_dwImageBase; // VA to image's entry point
	info.nt_headers.OptionalHeader.BaseOfCode = blob_sections[ORD_SEC_TEXT].m_dwVirtualAddress - blob_hdr->m_dwImageBase; // First is always .text
	info.nt_headers.OptionalHeader.BaseOfData = blob_sections[ORD_SEC_RDATA].m_dwVirtualAddress - blob_hdr->m_dwImageBase; // Second is always .rdata
	info.nt_headers.OptionalHeader.ImageBase = blob_hdr->m_dwImageBase;
	info.nt_headers.OptionalHeader.SectionAlignment = 4096; // The alignment (in bytes) of sections when they are loaded into memory.
	info.nt_headers.OptionalHeader.FileAlignment = 4096; // The alignment factor (in bytes) that is used to align the raw data of sections in the image file
	info.nt_headers.OptionalHeader.MajorOperatingSystemVersion = 4;
	info.nt_headers.OptionalHeader.MinorOperatingSystemVersion = 0;
	info.nt_headers.OptionalHeader.MajorImageVersion = 0;
	info.nt_headers.OptionalHeader.MinorImageVersion = 0;
	info.nt_headers.OptionalHeader.MajorSubsystemVersion = 4;
	info.nt_headers.OptionalHeader.MinorSubsystemVersion = 0;
	info.nt_headers.OptionalHeader.Win32VersionValue = NULL;
	info.nt_headers.OptionalHeader.SizeOfHeaders = 4096; // Usually this
	info.nt_headers.OptionalHeader.CheckSum = blob_hdr->m_dwCheckSum; // Not sure about this?
	info.nt_headers.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
	info.nt_headers.OptionalHeader.DllCharacteristics = NULL;
	info.nt_headers.OptionalHeader.SizeOfStackReserve = 4096 * 256;
	info.nt_headers.OptionalHeader.SizeOfStackCommit = 4096;
	info.nt_headers.OptionalHeader.SizeOfHeapReserve = 4096 * 256;
	info.nt_headers.OptionalHeader.SizeOfHeapCommit = 4096;
	info.nt_headers.OptionalHeader.LoaderFlags = NULL;
	info.nt_headers.OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

	CConsole::the().info("  Relative address to entry point: 0x{:08X}", info.nt_headers.OptionalHeader.AddressOfEntryPoint);
	CConsole::the().info("  Relative address to code segment: 0x{:08X}", info.nt_headers.OptionalHeader.BaseOfCode);
	CConsole::the().info("  Relative address to data segment: 0x{:08X}", info.nt_headers.OptionalHeader.BaseOfData);
	CConsole::the().info("  Image base address: 0x{:08X}", info.nt_headers.OptionalHeader.ImageBase);
	CConsole::the().info("  File section alignment: {}/0x{:08X} bytes", info.nt_headers.OptionalHeader.FileAlignment, info.nt_headers.OptionalHeader.FileAlignment);
	CConsole::the().info("  PE headers alignment: {}/0x{:08X} bytes", info.nt_headers.OptionalHeader.SizeOfHeaders, info.nt_headers.OptionalHeader.SizeOfHeaders);

	CConsole::the().info("NT optional header done...");

	// File offset or file pointer to the raw data of sections.
	// Section raw data is stored continuosly inside the file, right 
	// after the PE headers (0x1000 file offset).
	uint32_t sec_fileoffset = info.nt_headers.OptionalHeader.SizeOfHeaders;

	CConsole::the().info("Building section table...");

	// Build section data for OPT header
	for (uint16_t i = 0; i < blob_hdr->m_wSectionCount; i++)
	{
		blob_section_t* sec = &blob_sections[i];

		char sec_name[IMAGE_SIZEOF_SHORT_NAME];

		// We can get the name from the ordinary section table
		if (i < ORD_SIZE)
			strncpy(sec_name, ordinary_sections[i].m_szName, IMAGE_SIZEOF_SHORT_NAME);
		else
			snprintf(sec_name, IMAGE_SIZEOF_SHORT_NAME, "sec%hu", i);

		CConsole::the().info("{}:", sec_name);

		uint32_t flag = (i < ORD_SIZE) ? ordinary_sections[i].m_dwCharacteristics : ORDINARY_SCN_CHARACTERISTICS;

		// Structures has to be aligned to this boudary
		const uint32_t file_alignment = info.nt_headers.OptionalHeader.FileAlignment;

		// Increase individual section data
		info.nt_headers.OptionalHeader.SizeOfImage += ALIGN_AS(sec->m_dwVirtualSize, file_alignment);

		// .text
		if (flag & IMAGE_SCN_CNT_CODE)
		{
			CConsole::the().info("  Code section {}/0x{:08X} bytes", sec->m_dwVirtualSize, sec->m_dwVirtualSize);
			info.nt_headers.OptionalHeader.SizeOfCode += ALIGN_AS(sec->m_dwVirtualSize, file_alignment);
		}

		// .rdata
		if (flag & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			CConsole::the().info("  Initialized data section {}/0x{:08X} bytes", sec->m_dwVirtualSize, sec->m_dwVirtualSize);
			info.nt_headers.OptionalHeader.SizeOfInitializedData += ALIGN_AS(sec->m_dwVirtualSize, file_alignment);
		}

		// .bss
		if (flag & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
		{
			CConsole::the().info("  Unitialized data section {}/0x{:08X} bytes", sec->m_dwVirtualSize, sec->m_dwVirtualSize);
			info.nt_headers.OptionalHeader.SizeOfUninitializedData += ALIGN_AS(sec->m_dwVirtualSize, file_alignment);
		}

		// Build PE header sections
		IMAGE_SECTION_HEADER pe_sec;
		memset(&pe_sec, NULL, sizeof(pe_sec));
		
		// Section name we get from ordinary sections
		strcpy((char*)pe_sec.Name, sec_name);
		
		// Build up this pe_sec data
		pe_sec.Misc.VirtualSize = sec->m_dwVirtualSize;
		pe_sec.VirtualAddress = sec->m_dwVirtualAddress - blob_hdr->m_dwImageBase;
		pe_sec.SizeOfRawData = ALIGN_AS(sec->m_dwDataSize, info.nt_headers.OptionalHeader.FileAlignment); // Must be aligned to the FileAlignment value
		// For uninitialized data this is always zero
		pe_sec.PointerToRawData = (flag & IMAGE_SCN_CNT_UNINITIALIZED_DATA) ? 0 : sec_fileoffset; // File pointer to the raw data
		pe_sec.PointerToRelocations = 0; // File pointer
		pe_sec.PointerToLinenumbers = 0;
		pe_sec.NumberOfRelocations = 0;
		pe_sec.NumberOfLinenumbers = 0;
		pe_sec.Characteristics = (i < ORD_SIZE) ? ordinary_sections[i].m_dwCharacteristics : ORDINARY_SCN_CHARACTERISTICS;
		
		CConsole::the().info("  Section RVA: 0x{:08X}", pe_sec.VirtualAddress);
		CConsole::the().info("  Section at file offset: 0x{:08X}", pe_sec.PointerToRawData);
		CConsole::the().info("  Characteristics: 0x{:08X}", pe_sec.Characteristics);

		// Move to next section
		sec_fileoffset += ALIGN_AS(sec->m_dwDataSize, 4096);
		
		info.pe_section_table.emplace_back(pe_sec);
	}

	info.nt_headers.OptionalHeader.SizeOfImage += info.nt_headers.OptionalHeader.SizeOfHeaders;

	CConsole::the().info("Total size of image: {}/0x{:08X} bytes", info.nt_headers.OptionalHeader.SizeOfImage, info.nt_headers.OptionalHeader.SizeOfImage);

	build_data_directories(filebuffer, blob_hdr, blob_sections);

	CConsole::the().info("Building PE header finish");

	return info;
}

void CPEBuilder::build_data_directories(uint8_t* filebuffer, blob_hdr_t* blob_hdr, blob_section_t* blob_sections)
{
	CConsole::the().info("Building data directories...");

	// Data directories we're able to process. Others are unattainable.
	auto idd = &info.nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto edd = &info.nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	auto rdd = &info.nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
	auto iatdd = &info.nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];

	// We need this in order to process exports...
	uint32_t export_last_thunk_func;

	CConsole::the().info("Building imports...");
	process_imports(filebuffer, idd, iatdd, blob_hdr, blob_sections, &export_last_thunk_func);
	
	CConsole::the().info("Building exports...");
	process_exports(filebuffer, edd, blob_hdr, blob_sections, export_last_thunk_func);

	CConsole::the().info("Building resources...");
	process_resources(filebuffer, rdd, blob_hdr, blob_sections);
}

void CPEBuilder::print_data_directory(const char* name, PIMAGE_DATA_DIRECTORY idd)
{
	CConsole::the().info("--- {} data directory ---", name);
	CConsole::the().info("  VirtualAddress: 0x{:08X}", idd->VirtualAddress);
	CConsole::the().info("  Size:           0x{:08X}", idd->Size);
}

void CPEBuilder::process_imports(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* idd, IMAGE_DATA_DIRECTORY* iatdd,
								 blob_hdr_t* blob_hdr, blob_section_t* blob_sections, uint32_t* export_last_thunk_func)
{
	uint32_t iid_file_pointer = rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, blob_hdr->m_dwImportTable);
	auto iid = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(filebuffer + iid_file_pointer);

	// IAT data
	uint32_t iat_first_thunk = ~0, iat_last_thunk = 0;

	// Variables needed in order to locate export table
	*export_last_thunk_func = 0;

	CConsole::the().info("Found import descriptor at file offset 0x{:08X}", iid_file_pointer);
	CConsole::the().info("Processing imports...");

	uint32_t num_iid = 0, num_thunks = 0;
	while (iid->Name)
	{
		reconstructed_import_entry_t import_entry;

		// FirstThunk is a RVA from the image base, unlike the m_dwImportTable, which is a VA containing the image base.
		// Same applies for Name. In this case, we have to add the image base to the calculation.
		auto thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(
			filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, iid->FirstThunk + blob_hdr->m_dwImageBase));

		// RVA from image base
		auto name = reinterpret_cast<const char*>(
			filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, iid->Name + blob_hdr->m_dwImageBase));
		
		// Get the first thunk address of where the iat is
		if (iid->FirstThunk < iat_first_thunk)
			iat_first_thunk = iid->FirstThunk;

		// Check for furthest descriptor name
		if (iid->Name > *export_last_thunk_func)
			*export_last_thunk_func = iid->Name;

		CConsole::the().info("module: {:>16}", name);
		
		uint32_t this_idd_thunks = 0;
		while (thunk->u1.AddressOfData)
		{			
			// Check if the most-significant ordinal bit is set, if yes, we're importing
			// by an ordinal, not by a function name. We have to handle this..
			if (IMAGE_SNAP_BY_ORDINAL(thunk->u1.Ordinal))
			{
				import_entry.ordinal = thunk->u1.Ordinal;
				import_entry.fn_name = "";
				import_entry.hint = 0;
			}
			else
			{
				// Name of the thunk is also an RVA from the image base
				auto name = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
					filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, blob_hdr->m_dwImageBase + thunk->u1.AddressOfData));

				import_entry.ordinal = thunk->u1.Ordinal;
				import_entry.fn_name = name->Name;
				import_entry.hint = name->Hint;

				const uint32_t thunk_fn_offset = thunk->u1.Function + sizeof(WORD); // Skip the second data field (i.e. Hint)

				// Get furthermost address of import thunk functions. 
				// It seems that it's common behaviour that the export table lies
				// right after the iat. So we can do some hackyhack job in order
				// to locate it.
				if (thunk_fn_offset > *export_last_thunk_func)
					*export_last_thunk_func = thunk_fn_offset;
			}

			import_entry.va = blob_hdr->m_dwImageBase + iid->FirstThunk;

			CConsole::the().info(" {:<32}: VA=0x{:08X} ord={} hint={}", 
								 import_entry.fn_name, import_entry.va, import_entry.ordinal, import_entry.hint);

			info.m_imports.push_back(import_entry);
			
			thunk++;
			this_idd_thunks++; // counting only for this descriptor entry
		}

		// Get the last thunk address
		const uint32_t thunk_size = (sizeof(IMAGE_IMPORT_BY_NAME) * (this_idd_thunks + 1)); // Don't forget the null thunk
		if (iid->FirstThunk + thunk_size > iat_last_thunk)
			iat_last_thunk = iid->FirstThunk + thunk_size;

		CConsole::the().info(" ({:>3} thunk{})", this_idd_thunks, this_idd_thunks > 1 ? "s" : " ");

		iid++;
		num_thunks += this_idd_thunks; // overall count
		num_iid++;
	}

	CConsole::the().info("Found {} import descriptors", num_iid);
	CConsole::the().info("Found {} thunk routines", num_thunks);

	// IDD
	idd->VirtualAddress = blob_hdr->m_dwImportTable - blob_hdr->m_dwImageBase; // VA to the image base
	idd->Size = sizeof(IMAGE_IMPORT_DESCRIPTOR) * (num_iid + 1); // + 1 for the null descriptor at the end
	print_data_directory("Import", idd);

	// IATDD
	iatdd->VirtualAddress = iat_first_thunk; // RVA to the first thunk
	iatdd->Size = iat_last_thunk - iat_first_thunk;
	print_data_directory("Import Address Table", iatdd);

	CConsole::the().info("  {} entries inside IAT", iatdd->Size / sizeof(DWORD));
}

void CPEBuilder::process_exports(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* edd, blob_hdr_t* blob_hdr, blob_section_t* blob_sections, uint32_t export_last_thunk_func)
{
	// Note:
	//	Valve's blob file representation does not share any information about image
	//	export table, thus it's impossible for us to locate it. However inside the
	//	PE image, there are some common patterns that we can abuse. For example, it's
	//	a common fact that the export table lies right after the end of import thunks, 
	//	thus locating the last thunk in the iat and then iterating to the end of the
	//	last routine name, we can kind of locate the export directory using this way.
	//	This is probably the only option, since as said before, there's zero information
	//	exposed to us from the blob headers, since there's really no point in storing
	//	such information when we only wan't to load the file into the memory - we need
	//	only imports for that, exports aren't needed at all.

	// Get the address to this location
	uint8_t* p = filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, export_last_thunk_func + blob_hdr->m_dwImageBase);
	uint8_t* p_start = p;

	// Go to the end of the last thunk's functio name.
	while (*p++);

	// After we're here, there's usually one more character at the very end of the 
	// iat, so we have to skip it
	p++;

	// Now usually there's a zone of bytes that are just null, so we skip them and
	// right after these zero bytes should lie the export directory information.
	while (!*p) p++;

	// At this point, 'usually' once again, the Characteristics field from export
	// descriptor is null, meaning that our loop above would just skip it. Again, 
	// this method is really theoretical and hardcoding, so it may not work every
	// time. So now we have to revert back 4 bytes, in order to get the start of
	// the export directory structure.
	p -= sizeof(DWORD);

	auto ied = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(p);

	CConsole::the().info("Had to travel {} bytes for the export directory.", p - p_start);
	CConsole::the().info("Found export descriptor.");
	CConsole::the().info("Processing imports...");

	// We can use this as a sanity check...
	if (ied->Characteristics != NULL)
	{
		CConsole::the().warning("Couldn't process exports because the export directory isn't valid!");
		return;
	}

	// These should be always null
	if (ied->MajorVersion != NULL || ied->MinorVersion != NULL)
	{
		CConsole::the().warning("Couldn't process exports because the versions aren't null!");
		return;
	}

	// We can detect if this dll has no exports using this
	if (ied->NumberOfFunctions == NULL || ied->NumberOfNames == NULL)
	{
		CConsole::the().warning("There aren't any exports in this file!");
		return;
	}

	CConsole::the().info("Found {} entries inside export directory", ied->NumberOfNames);

	// Following information is an RVA from the base image, meaning that we have to 
	// add the image base to it when calculating the RVA from the start of the section.

	// Pointer to functions
	auto functions_ptr = reinterpret_cast<uint32_t*>(
		filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, ied->AddressOfFunctions + blob_hdr->m_dwImageBase));

	// Pointer to names
	auto names_ptr = reinterpret_cast<uint32_t*>(
		filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, ied->AddressOfNames + blob_hdr->m_dwImageBase));

	// Pointer to ordinals. They're 16bits in size!
	auto ordinals_ptr = reinterpret_cast<uint16_t*>(
		filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, ied->AddressOfNameOrdinals + blob_hdr->m_dwImageBase));

	uint32_t export_last_function = 0, fn_name_size = 0;
	for (uint32_t i = 0; i < ied->NumberOfNames; i++)
	{
		reconstructed_export_entry_t export_entry;

		// Get the function name. It is stored inside a table and we can access it using this index.
		uint32_t* fn_name_addr = reinterpret_cast<uint32_t*>(
			filebuffer + rva_to_u32_offset(blob_sections, blob_hdr->m_wSectionCount, names_ptr[i] + blob_hdr->m_dwImageBase));
		
		// Name of the function
		const char* fn_name = reinterpret_cast<const char*>(fn_name_addr);

		export_entry.fn_name = fn_name;
		export_entry.ordinal = ordinals_ptr[i];
		export_entry.va = functions_ptr[ordinals_ptr[i]] + blob_hdr->m_dwImageBase;

		CConsole::the().info(" {:<32}: VA=0x{:08X} ord={}", export_entry.fn_name, export_entry.va, export_entry.ordinal);

		const uint32_t thunk_fn_offset = (uint32_t)fn_name_addr;

		// Get furthermost address of import thunk functions. 
		// It seems that it's common behaviour that the export table lies
		// right after the iat. So we can do some hackyhack job in order
		// to locate it.
		if (thunk_fn_offset > export_last_function)
		{
			export_last_function = thunk_fn_offset;
			fn_name_size = strlen(fn_name); // Get the length of the function so we can then add it into final length
		}

		info.m_exports.push_back(export_entry);
	}

	// Get the delta how far away we're from the start of the section
	p -= (DWORD)(filebuffer + blob_sections[ORD_SEC_RDATA].m_dwDataAddress);

	// Now add the VA not including the base address
	edd->VirtualAddress = (DWORD)((uint8_t*)p + blob_sections[ORD_SEC_RDATA].m_dwVirtualAddress) - blob_hdr->m_dwImageBase;
	edd->Size = (DWORD)((uint8_t*)export_last_function - (uint8_t*)ied) + fn_name_size + 1; // + 1 for the terminator char
	print_data_directory("Export Address Table", edd);
}

void CPEBuilder::process_resources(uint8_t* filebuffer, IMAGE_DATA_DIRECTORY* rdd, blob_hdr_t* blob_hdr, blob_section_t* blob_sections)
{
	if (blob_hdr->m_wSectionCount < 4)
	{
		CConsole::the().warning("Cannot process resources because image doesn't have any");
		return;
	}

	rdd->VirtualAddress = blob_sections[ORD_SEC_RSRC].m_dwVirtualAddress - blob_hdr->m_dwImageBase;
	rdd->Size = blob_sections[ORD_SEC_RSRC].m_dwVirtualSize;
	print_data_directory("Resource ", rdd);
}

std::string CPEBuilder::timestamp_as_string(std::time_t timestamp)
{
	const auto gmt = std::gmtime(&timestamp);
	if (gmt)
		return std::asctime(gmt);

	return "Unknown";
}