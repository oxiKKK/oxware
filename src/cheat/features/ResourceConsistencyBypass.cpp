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
// ResourceConsistencyBypass.cpp -- Bypass for "rechecker"-type plugins.
//

#include "precompiled.h"

#include "util/vanilla_halflife_dir_file_database.h"

VarBoolean consistencybypass_enable("consistencybypass_enable", "Enables resource consistency bypass.", false);
VarBoolean consistencybypass_log("consistencybypass_log", "Logs every file that gets requested to be processed by the server", false);

extern const std::unordered_multiset<std::string> s_8684_vanilla_halflife_directory_files;

bool CResourceConsistencyBypass::detour_consistency_info(hl::sizebuf_t* msg)
{
	//
	// Some servers can check for dll/exe/asi/cfg files inside the Half-Life directory (and others, of course)
	// to detect whenever you have any cheats there. If you do, the server can refuse. We can bypass it here
	// by detouring this function.
	//

	if (!consistencybypass_enable.get_value())
	{
		return false;
	}

	if (!CHLNetMessageIO::the().is_ready())
	{
		return false;
	}

	auto ifs = CHLInterfaceHook::the().IFileSystem();
	if (!ifs)
	{
		return false;
	}

	bool is_steam = CGenericUtil::the().does_cmdline_parameter_exist("-steam");

	auto cl = CMemoryHookMgr::the().cl().get();

	auto& studio_bounds_fn = CMemoryFnHookMgr::the().R_GetStudioBounds();
	auto& hash_file_fn = CMemoryFnHookMgr::the().MD5_Hash_File();

	if (!cl->need_force_consistency_response)
	{
		return true; // server didn't request any consistency response
	}

	cl->need_force_consistency_response = FALSE;

	CHLNetMessageIO::the().write_byte(msg, clc_fileconsistency);
	int msg_start_pos = msg->cursize; // Save starting position of the message

	CHLNetMessageIO::the().write_short(msg, 0); // 16 bits reserverd to be then set as
	// total bytes written at the end.

	//
	// For every consistency entry, flush data over to the remote.
	//
	CHLNetMessageIO::the().start_bit_writing(msg);

	std::string filename, steam_filename;
	bool file_exists = true;
	byte md5[16];
	Vector mins, maxs;

	if (consistencybypass_log.get_value())
	{
		CConsole::the().info("Server wants to check {} files inside your Half-Life directory.", cl->num_consistency);
		CConsole::the().info("{:<3}  {:<48} {:<32} {}", "id", "resource name", "check type", "response");
		CConsole::the().info("-------------------------------------------------------------------------------------------------");
	}

	for (int i = 0; i < cl->num_consistency; i++)
	{
		auto item = &cl->consistency_list[i];
		if (!item)
		{
			continue;
		}

		CHLNetMessageIO::the().write_bits(1, 1); // One bit of padding for every consistency
		CHLNetMessageIO::the().write_bits(item->orig_index, MAX_RESOURCE_INDEX_BITS);

		filename = std::format("{}{}", item->issound ? "sound/" : "", item->filename);

		std::string response;

		file_exists = TRUE;

		if (is_steam)
		{
			steam_filename = CGenericUtil::the().fix_directory_separators(filename);

			if (!ifs->FileExists(steam_filename.c_str()))
			{
				file_exists = FALSE;
			}
		}

		// there are some CRC checks here to see whenever the file has been modified or not... screw that!

		switch (item->check_type)
		{
			// This file has to match the one which the server ones.
			case hl::force_exactfile:
			{
				//
				// Search list of vanilla HalfLife directory files and if found, send the md5 has of the file. Otherwise
				// act as if the file wasn't there (e.g. a cheat dll/config/exe/asi/bin etc.)
				//
				bool is_vanilla_file = false;
				for (const auto& file : s_8684_vanilla_halflife_directory_files)
				{
					if (!stricmp(file.c_str(), CGenericUtil::the().get_filename_from_path(filename).c_str()))
					{
						is_vanilla_file = true;
						break;
					}
				}

				//if (strstr(filename.c_str(), "imgui.ini") || filename == "sprites/unrealsmoke_2.spr")
				//	__debugbreak();

				bool file_allowed_to_md5_check = is_vanilla_file;

				if (is_vanilla_file)
				{
					// if the function returns true here, 
					hash_file_fn.call(md5, (char*)filename.c_str(), FALSE, FALSE, NULL);
				}
				else
				{
					// foreign file, we have to perform harcoded filtration.. this file must be from downloads or somewhere else..
					std::string ext = std::filesystem::path(filename).extension().string();
					bool skip_checking = false;
					if (ext != ".spr" &&
						ext != ".mdl" &&
						ext != ".txt" &&
						ext != ".res" &&
						ext != ".wad" &&
						ext != ".sc" &&
						ext != ".bsp" &&
						ext != ".tga" &&
						ext != ".wav")
					{
						// the server probably wants to search for dlls or cheat files/configs, etc..
						skip_checking = true;
						file_allowed_to_md5_check = false;
					}
					
					if (!skip_checking)
					{
						hash_file_fn.call(md5, (char*)filename.c_str(), FALSE, FALSE, NULL);
					}
				}

				// 5.188.108.140:27015 "gas_puff_gray_transparent.spr"

				item->value = *(int*)(md5);

				//
				// Server plugins expect non-zero md5 values. Zero md5 hashes only happen if you have a modified model sitting on your
				// machine, that the server requested md5 for. Otherwise, the server expects always non-zero md5 hash, even if the hash
				// (md5 variable) would be uninitialized! They then compare it with their hash.
				//

				CHLNetMessageIO::the().write_bits(item->value, sizeof(int) * 8);

				response = std::format("{} (0x{:08X})", file_allowed_to_md5_check ? "allowed" : "denied", (uint32_t)item->value);

				break;
			}
			// Check for model bounds
			case hl::force_model_samebounds:
			case hl::force_model_specifybounds:
			{
				if (!studio_bounds_fn.call(filename.c_str(), mins, maxs))
				{
					CMessageBox::display_error("Unable to find {}", filename);
					exit(-1);
				}

				CHLNetMessageIO::the().write_bit_data(mins, sizeof(Vector));
				CHLNetMessageIO::the().write_bit_data(maxs, sizeof(Vector));

				response = std::format("mins: {}, maxs: {}", mins, maxs);

				break;
			}
			case hl::force_model_specifybounds_if_avail:
			{
				if (file_exists && !studio_bounds_fn.call(filename.c_str(), mins, maxs))
				{
					CMessageBox::display_error("Unable to find {}", filename);
					exit(-1);
				}

				CHLNetMessageIO::the().write_bit_data(mins, sizeof(Vector));
				CHLNetMessageIO::the().write_bit_data(maxs, sizeof(Vector));

				response = std::format("mins: {}, maxs: {}", mins, maxs);

				break;
			}
			default:
			{
				CMessageBox::display_error("Unknown consistency type {}", item->check_type);
				exit(-1);
			}
		}

		if (consistencybypass_log.get_value())
		{
			CConsole::the().info("{:<3}: {:<48} {:<32} {}", item->orig_index, filename, (hl::FORCE_TYPE)item->check_type, response);
		}
	}

	CHLNetMessageIO::the().write_bits(0, 1); // 1-bit padding, at last
	CHLNetMessageIO::the().end_bit_writing(msg);

	//
	// Write the total amount of bytes written to the preserved two bytes
	// at the beggining of the message.
	//
	unsigned totalsize = msg->cursize - msg_start_pos - sizeof(short);
	*(unsigned short*)(&msg->data[msg_start_pos]) = totalsize;

	GoldSrcMungificationAlgorithms::the().enmungify(msg->data + msg_start_pos + sizeof(short), totalsize, cl->servercount);
	return true;
}
