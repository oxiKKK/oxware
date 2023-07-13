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
// ManualMappedDll.cpp -- Defines a ManualMap injector interface.
// 

#include "precompiled.h"

bool CManualMappedDllCurrentProcess::start_injection()
{
	m_successfully_injected = false;
	m_start_time = std::chrono::high_resolution_clock::now();

	if (!map_dll_to())
	{
		CMessageBox::display_error("Failed to inject '{}' into current process.", m_dll_filepath.string());
		return false;
	}

	m_successfully_injected = true;

	return true;
}

void CManualMappedDllCurrentProcess::unload(bool tell_client_dll)
{
	// NOTE: before calling this function, let the dll know that we're unloading it! We don't have the communication capabilities here...

	CConsole::the().info("Unloading... {}", m_dll_filepath.string());

	deallocate_all_memory();

	m_dll_to_be_injected.destroy();
	close_process_handle();

	CConsole::the().info("{} unloaded successfully.", m_dll_filepath.string());
	m_unloaded = true;
}

bool CManualMappedDllCurrentProcess::communicate_with_injected_dll(EClient2Injector& code)
{
	// this injection doesn't provide any communication methods
	return false;
}

void CManualMappedDllCurrentProcess::on_successfull_initialize()
{
	if (!m_injected_module_successfully_initialized)
	{
		m_initialization_time = std::chrono::high_resolution_clock::now();

		CConsole::the().info("Manual-mapped module successfully initialized. Took {} seconds to fully load and initialize.", 
							 std::chrono::duration<float, std::ratio<1, 1>>(m_initialization_time - m_start_time).count());

		m_injected_module_successfully_initialized = true;
	}
}

bool CManualMappedDllCurrentProcess::map_dll_to()
{
	CConsole::the().info("Trying to inject '{}' into current process...", m_dll_filepath.string());

	if (!std::filesystem::exists(m_dll_filepath))
	{
		CConsole::the().error("The DLL specified couldn't be found.");
		return false;
	}

	m_target_process_handle = CGenericUtil::the().get_current_process_handle();

	if (!m_dll_to_be_injected.create(m_dll_filepath))
	{
		return false;
	}

	bool is_communicative = m_dll_to_be_injected.find_export(COMMUNICATIVEDLLENTRYPOINT_PROCNAME);
	if (is_communicative)
	{
		CConsole::the().error("Can't inject communicative dll into current process...");
		return false;
	}

	if (!allocate_memory())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!copy_dll_image_data())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!relocate_dll_image_data())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	if (!execute_shellcode())
	{
		deallocate_all_memory();
		close_process_handle();
		return false;
	}

	on_successfull_initialize();

	// successfully injected in current process memory
	return true;
}

bool CManualMappedDllCurrentProcess::read_execution_context_back(manualmap_shellcode_execution_context_t* sec)
{
	if (!CVirtualMemoryManager::the().read<manualmap_shellcode_execution_context_t>(
		m_target_process_handle,
		(PVOID)get_target_process_shellcode_execution_context_addr(), sec))
	{
		CConsole::the().error("Couldn't read target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

	return true;
}

bool CManualMappedDllCurrentProcess::allocate_memory()
{
	size_t total_allocation_block_size = m_dll_to_be_injected.get_size_of_image();

	CConsole::the().info("Total allocation size: {} bytes", total_allocation_block_size);

	// Allocate space for the DLL image.
	m_allocation_block_ptr = CVirtualMemoryManager::the().allocate<uint8_t>(
		nullptr, total_allocation_block_size, PAGE_EXECUTE_READWRITE);
	if (!m_allocation_block_ptr)
	{
		return false;
	}

	CConsole::the().info("Allocated all memory needed.");

	return true;
}

void CManualMappedDllCurrentProcess::deallocate_all_memory()
{
	if (m_allocation_block_ptr)
	{
		CVirtualMemoryManager::the().deallocate(m_allocation_block_ptr);
		m_allocation_block_ptr = nullptr;
	}

	CConsole::the().info("Deallocated all allocated memory.");
}

bool CManualMappedDllCurrentProcess::copy_dll_image_data()
{
	// Copy NT & DOS headers
	memcpy(m_allocation_block_ptr, m_dll_to_be_injected.get_byte_buffer().get_raw(), m_dll_to_be_injected.get_size_of_headers());

	// Copy image sections
	auto first_section = m_dll_to_be_injected.get_image_first_section();
	for (uint16_t i = 0; i < m_dll_to_be_injected.get_num_of_sections(); i++)
	{
		auto current_section = &first_section[i];

		if (current_section->SizeOfRawData == 0)
		{
			CConsole::the().warning("section '{}' without raw data... skipping", (const char*)current_section->Name);
			continue;
		}

		uintptr_t from = (uintptr_t)(m_allocation_block_ptr + current_section->VirtualAddress);
		uintptr_t to = (uintptr_t)(m_dll_to_be_injected.get_byte_buffer().get_raw() + current_section->PointerToRawData);

		memcpy((void*)from, (void*)to, current_section->SizeOfRawData);

		CConsole::the().info("[{:02}] Created {:<24} at 0x{:08X}", i, (const char*)current_section->Name, to);
	}

	CConsole::the().info("Created write-able DLL memory image.");

	// then write the communication interface block
	if (is_communicative_dll())
	{
		if (!write_ipc_block((LPVOID)get_target_process_ipc_block_addr()))
		{
			return false;
		}

		CConsole::the().info("Communication interface block at 0x{:08X}", get_target_process_ipc_block_addr());
	}

	return true;
}

bool CManualMappedDllCurrentProcess::relocate_dll_image_data()
{
	// relocate only if the base address specified by the DLL doesn't corespond to the real base address. Then everything needs to be relocated.
	uintptr_t* amount_to_relocate = (uintptr_t*)(m_allocation_block_ptr - m_dll_to_be_injected.get_image_base());
	if (amount_to_relocate == 0)
	{
		// No need for relocation0x000aaa00
		return true;
	}

	CConsole::the().info("Image relocation is needed.");

	auto& ddir = m_dll_to_be_injected.get_data_directory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
	assert(ddir.Size != 0);

	auto ibr = (PIMAGE_BASE_RELOCATION)(m_allocation_block_ptr + ddir.VirtualAddress);
	size_t total_size = 0;
	while (ddir.Size > total_size && ibr->SizeOfBlock > 0)
	{
		typedef uint16_t relocation_entry;

		auto relative_info = (relocation_entry*)(ibr + sizeof(uint8_t));

		// size of a complete block without the header divided by the entry size.
		size_t num_of_relocations = (ibr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(relocation_entry);
		for (size_t i = 0; i < num_of_relocations; i++)
		{
			uint16_t type = (relative_info[i] & 0xF000) >> 12;
			uint16_t offs = (relative_info[i] & 0x0FFF);

			switch (type)
			{
				case IMAGE_REL_BASED_HIGHLOW:
				{
					uintptr_t* address_to_patch = (uintptr_t*)(m_allocation_block_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += (uintptr_t)amount_to_relocate;
					break;
				}
				case IMAGE_REL_BASED_HIGH:
				{
					uint16_t* address_to_patch = (uint16_t*)(m_allocation_block_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += HIWORD((uintptr_t)amount_to_relocate);
					break;
				}
				case IMAGE_REL_BASED_LOW:
				{
					uint16_t* address_to_patch = (uint16_t*)(m_allocation_block_ptr + ibr->VirtualAddress + offs);
					*address_to_patch += LOWORD((uintptr_t)amount_to_relocate);
					break;
				}
				case IMAGE_REL_BASED_ABSOLUTE:
				{
					// This is skipped
					break;
				}
				default:
				{
					CConsole::the().warning("Unhandled relocation type: {}", type);
					break;
				}
			}
		}

		total_size += ibr->SizeOfBlock;
		ibr = (PIMAGE_BASE_RELOCATION)((uint8_t*)ibr + ibr->SizeOfBlock);
	}

	CConsole::the().info("Successfuly relocated the DLL image.");

	return true;
}

bool CManualMappedDllCurrentProcess::execute_shellcode()
{
	// xddd just call the shellcode fucking locally, who cares lol. This isn't a target process injection, so it's kinda
	// silly to call it a "shellcode", but yea, whatever dude
	if (NULL == shellcode_routine())
	{
		CMessageBox::display_error("Shellcode routine exited with error.");
		return false;
	}

	return true;
}
