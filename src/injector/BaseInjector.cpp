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
// BaseInjector.cpp -- Defines base injector class
//

#include "precompiled.h"

uint32_t CInjectorUtils::get_running_process_id(const char* execuatable_name)
{
	PROCESSENTRY32 process_entry;
	process_entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot == INVALID_HANDLE_VALUE)
		return -1;

	if (Process32First(snapshot, &process_entry))
	{
		do
		{
			if (!stricmp(execuatable_name, process_entry.szExeFile))
			{
				CloseHandle(snapshot);
				return process_entry.th32ProcessID;
			}
		} while (Process32Next(snapshot, &process_entry));
	}

	CloseHandle(snapshot);

	return -1;
}

HANDLE CInjectorUtils::open_process(const char* execuatable_name)
{
	uint32_t pid = get_running_process_id(execuatable_name);
	if (pid == -1)
	{
		return INVALID_HANDLE_VALUE;
	}

	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, NULL, NULL, NULL, NULL);

	CLIENT_ID cid;
	cid.UniqueProcess = (HANDLE)pid;
	cid.UniqueThread = NULL;

	HANDLE hProcess;
	if (!NT_SUCCESS(NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &cid)))
	{
		return INVALID_HANDLE_VALUE;
	}

	return hProcess;
}

size_t CInjectorUtils::calc_func_size(uintptr_t function_base)
{
	// walk the memory untill we hit \xCC like three times in a row. That determines the end of the function.
	size_t shellcode_routine_size_bytes = 0;

	uint8_t cmp[3] = { 0xCC, 0xCC, 0xCC };

	uint8_t* b = (uint8_t*)(function_base);
	while (true) // this should not deadlock
	{
//		CConsole::the().info("0x{:02X}", *b);

		if (!memcmp(b, cmp, sizeof(cmp)))
		{
			break;
		}

		b++;
		shellcode_routine_size_bytes++;
	}

	return shellcode_routine_size_bytes;

}

//-------------------------------------------------------------------------------------

void IInjectableModuleObject::close_process_handle()
{
	if (!m_inject_to_current_process)
	{
		NtClose(m_target_process_handle);
	}
	m_target_process_handle = NULL;
}

//-------------------------------------------------------------------------------------

void CBaseInjector::destroy()
{
	for (auto& [key, module] : m_injected_dlls)
	{
		if (module->is_unloaded())
		{
			continue;
		}

		if (module->is_communicative_dll())
		{
			// tell the dll, first
			module->unload(true);

			CConsole::the().info("Waiting for communicative dll to acknowledge its unload...");
		
			// wait till the communicative module responds
			auto wait_start = std::chrono::high_resolution_clock::now();
			while (true) 
			{
				auto result = hang_till_module_unloads(key, module, wait_start);

				bool exit_loop = false;
				switch (result)
				{
					case MODWAIT_Hang:
					{
						break; // keep waiting
					}
					case MODWAIT_CanUnload:
					{
						exit_loop = true;
						module->unload(false);
						break; // good, the dll has acknowledged the unload..
					}
					case MODWAIT_Error:
					{
						exit_loop = true;
						break; // can't do much here, really. In most cases, the dll will crash 
							   // when we unload it without the dll actually considering it..
					}
					default:
					{
						assert(0);
						break;
					}
				}

				if (exit_loop)
				{
					break;
				}
			}
		}
		else
		{
			// for non-communicative modules just force the unload.
			module->unload(false);
		}
	}

	m_injected_dlls.clear();
}

void CBaseInjector::update()
{
	// update only each #
	static uint32_t last_check_time = GetTickCount();
	if (GetTickCount() - last_check_time < k_update_threshold_ms)
	{
		return;
	}

	if (m_should_reinject)
	{
		CConsole::the().info("Re-Injecting '{}' into target process '{}'...", m_reinjection_data.dll_path.filename(), m_reinjection_data.exe_name);

		inject_to_target_process(m_reinjection_data.exe_name.c_str(), m_reinjection_data.dll_path);

		m_reinjection_data = {};
		m_should_reinject = false;
	}

	for (auto it = m_injected_dlls.begin(); it != m_injected_dlls.end(); it++)
	{
		EInjectorEvent ev = INE_None;
		
		if (it->second->is_unloaded())
		{
			goto erase_current_entry;
		}

		if (!it->second->is_communicative_dll())
		{
			// only communicative modules
			continue;
		}

		EClient2Injector code;
		if (!it->second->communicate_with_injected_dll(code))
		{
			ev = INE_ModuleCommunicationHalt;
			
			// couldn't communicate with the injected dll, hard unload.
			CMessageBox::display_error("Couldn't reach module {} while trying to communicate with it. The module probably crashed."
										"\n\nYou may want to check the console log file for further information.\n{}",
										it->first.string(),
										g_devconsole_i->get_logfile_path().parent_path() / "cheater_*");
			it->second->unload(false);
			goto erase_current_entry;
		}

		static EClient2Injector last_code = C2I_None;
		if (last_code != code)
		{
			switch (code)
			{
				case C2I_None:
				{
					break;
				}
				//
				// initialization status codes
				//
				case C2I_Init_OK:
				{
					ev = INE_ModuleSuccessfulInitialization;

					it->second->on_successfull_initialize();

					execute_on_event_callbacks(ev, it->second);
					break;
				}
				case C2I_Init_Failed:
				{
					ev = INE_ModuleFailedInitialization;

					CConsole::the().error("Module failed to initialize while being injected.");

					const char* error_message = it->second->get_errorneous_message();
					if (error_message[0])
					{
						CMessageBox::display_error("Error from injected dll: {}", error_message);
					}
					else
					{
						CMessageBox::display_error("The cheat failed to initialize while injecting. "
												   "\n\nYou may want to check the console log file for further information.\n{}",
												   g_devconsole_i->get_logfile_path().parent_path() / "cheater_*");

					}

					// unload the module itself
					it->second->unload(false);

					last_code = code;
					goto erase_current_entry;
				}
				//
				// runtime status codes
				//
				case C2I_Unloading:
				{
					ev = INE_ModuleUnloading;

					it->second->unload(false);

					last_code = code;
					goto erase_current_entry;
				}
				case C2I_Restarting:
				{
					ev = INE_ModuleRestarting;
					execute_on_event_callbacks(ev, it->second); // execute now so that the external code can catch it before we reinject

					prepare_for_reinjection(it->second->get_dll_exe_name().c_str(), it->second->get_dll_filepath());

					it->second->unload(false);

					last_code = code;
					goto erase_current_entry;
				}
				default:
				{
					assert(0);
					break;
					// shouldn't happen
				}
			}

			last_code = code;
		}

		continue;

	erase_current_entry: // ik this is ugly as hell but whatever :----)
		execute_on_event_callbacks(ev, it->second);

		m_injected_dlls.erase(it);
		if (m_injected_dlls.empty())
		{
			break;
		}
	}

	last_check_time = GetTickCount();
}

bool CBaseInjector::inject_to_target_process(const char* execuatable_name, const std::filesystem::path& dll_path)
{
	auto injector_by_type = create_injected_dll(execuatable_name, dll_path);

	auto [iter, did_insert] = m_injected_dlls.insert(std::make_pair(dll_path, injector_by_type));
	if (!did_insert)
	{
		CConsole::the().warning("Failed to insert new injectable dll to the list. Maybe already in the list?");
		return true;
	}

	if (!iter->second->start_injection())
	{
		m_injected_dlls.erase(iter);
		return false;
	}

	return true;
}

void CBaseInjector::prepare_for_reinjection(const char* execuatable_name, const std::filesystem::path& dll_path)
{
	CConsole::the().info("Client module requested its re-injection... preparing stuff...");

	m_reinjection_data = { execuatable_name, dll_path };
	m_should_reinject = true;
}

EModuleUnloadWaitResult CBaseInjector::hang_till_module_unloads(const std::filesystem::path& module_path, IInjectableModuleObject* module,
																std::chrono::high_resolution_clock::time_point wait_start)
{
	if (std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - wait_start).count()
		> k_max_module_unload_wait_time_sec)
	{
		// we could unload the module at this point, really. But if the module is detouring some functions and we unload its
		// memory, when the target process calls these functons, it will crash.. so yeah, there're aren't many options rather
		// than just leave the dll injected...
		CConsole::the().info("Module failed to acknowledge its unload in {} seconds. Oh well, the module will retain injected in the process...", k_max_module_unload_wait_time_sec);
		return MODWAIT_Error; // TODO: Exit the target process? It will crash anyway
	}

	if (module->is_unloaded())
	{
		return MODWAIT_Error;
	}

	EClient2Injector code;
	if (!module->communicate_with_injected_dll(code))
	{
		// couldn't communicate with the injected dll, hard unload.
		CConsole::the().warning("Couldn't reach module {} while trying to communicate with it. The module has been kept injected...", module_path.string());
		return MODWAIT_Error;
	}

	if (code == C2I_Unloading)
	{
		CConsole::the().info("Module acknowledged, we can uload it.");
		// fine, module acknowledged.
		return MODWAIT_CanUnload;
	}

	return MODWAIT_Hang;
}
