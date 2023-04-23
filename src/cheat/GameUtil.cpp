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

bool CGameUtil::is_fully_connected()
{
	if (CMemoryHookMgr::the().cls().get()->state != hl::ca_active)
		return false;

	auto cl = CMemoryHookMgr::the().cl().get();

	// this is set when svc_serverinfo is issued (after the first disconnect)
	// now we're fully connected and we'll soon render frames.
	//
	// this is just a sanity check for scenarious when we're in a level transition, 
	// the state remains ca_active, but it really isn't. Only after svc_serverinfo is
	// issued, it's updated back to ca_connected and levelname is set.
	if (!cl->levelname || !cl->levelname[0])
		return false;

	return true;
}

bool CGameUtil::is_sp()
{
	// if loopback is on, we're single.
	return CMemoryHookMgr::the().cls().get()->netchan.remote_address.type == hl::NA_LOOPBACK;
}

void CGameUtil::reset_all_in_states()
{
	auto cmd = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetFirstCmdFunctionHandle();

	// if you disable these, you get weird (mouse) look glitches, where you can only move on the yaw axes. So keep these blacklisted.
	static const char* except_these[] =
	{
		"klook", 
		"mlook", 
		"jlook", 
	};

	while (cmd)
	{
		// execute all 'in' commands, e.g. "-moveleft, -attack, ..."
		if (cmd->name[0] == '-')
		{
			// see for blacklisted commands.
			bool skip_this_one = false;
			for (size_t i = 0; i < OX_ARRAYSIZE(except_these); i++)
			{
				if (strstr(cmd->name, except_these[i]))
				{
					skip_this_one = true;
					break;
				}
			}

			if (!skip_this_one)
			{
				cmd->function();
			}
		}

		cmd = cmd->next;
	}
}

bool CGameUtil::world_to_screen(Vector world, Vector2D& screen)
{
	hl::qboolean z_clipped = CMemoryHookMgr::the().cl_enginefuncs().get()->pTriAPI->WorldToScreen(world, screen);

	int wide, tall;
	auto isurface = CHLInterfaceHook::the().ISurface();
	if (!isurface)
	{
		return false;
	}
	isurface->GetScreenSize(wide, tall);

	if (z_clipped)
	{
		return false;
	}

	// transform to screen scale
	if (screen.x < 1.0f && screen.x > -1.0f && 
		screen.y < 1.0f && screen.y > -1.0f)
	{
		screen.x = (screen.x * (wide / 2.f)) + (wide / 2.f);
		screen.y = (-screen.y * (tall / 2.f)) + (tall / 2.f);
		return true;
	}

	return false;
}

bool CGameUtil::is_player_index(int index)
{
	return (index > WORLD_ENTITY && index <= CMemoryHookMgr::the().cl().get()->maxclients);
}

bool CGameUtil::is_player_on_enemy_team(int index)
{
	auto local = CEntityMgr::the().get_local_player();
	if (!local || !local->is_valid())
		return false;

	return CEntityMgr::the().m_known_players[index].get_team() != local->get_team();
}

bool CGameUtil::is_local_player(int index)
{
	return index == (CMemoryHookMgr::the().cl().get()->playernum + 1);
}

typedef struct
{
	uint32_t	appid;
	const char* gamedir;
} appid_translation_t;

appid_translation_t g_appid_translation_table[] =
{
	{  10, "cstrike"	  },
	{  20, "tfc"		  },
	{  30, "dod"		  },
	{  40, "dmc"		  },
	{  50, "gearbox"	  },
	{  60, "ricochet"	  },
	{  70, "valve"		  },
	{  80, "czero"		  },
	{ 100, "czeror"		  },
	{ 130, "bshift"		  },
	{ 150, "cstrike_beta" }
};

unsigned int CGameUtil::get_current_app_id()
{
	auto cmdline = GetCommandLineA();

	// the -game launch option always gets appended, even if we don't specify it.
	auto parse_game_string = [](const std::string& cmdline) -> std::string
	{
		auto pos = cmdline.find("-game");

		auto gamedir_begin_pos = pos + (sizeof("-game") - 1) + 1;
		auto gamedir_end_pos = cmdline.find(" ", gamedir_begin_pos + 1);

		return cmdline.substr(gamedir_begin_pos, gamedir_end_pos - gamedir_begin_pos);
	};

	auto current_gamedir = parse_game_string(cmdline);
	if (current_gamedir.empty())
	{
		// this shouldn't really happen.
		return 0;
	}

	for (int i = 0; i < Q_ARRAYSIZE(g_appid_translation_table); i++)
	{
		if (!stricmp(g_appid_translation_table[i].gamedir, current_gamedir.c_str()))
		{
			return g_appid_translation_table[i].appid;
		}
	}

	// unknown appid
	CConsole::the().info("Unrecognized app id for your game: {}", current_gamedir);
	return 0;
}

int CGameUtil::get_build_number()
{
	locate_engine_compile_timestamp();

	if (m_engine_compile_date.empty())
	{
		CConsole::the().error("Couldn't get build number, because the engine compile time string is empty!");
		return 0;
	}

	return build_number(m_engine_compile_date.c_str());
}

// Spectator Movement modes (stored in pev->iuser1, so the physics code can get at them)
#define OBS_NONE				0 // not spectator
#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2 // third-person on the player
#define OBS_ROAMING				3 // free look
#define OBS_IN_EYE				4 // first person
#define OBS_MAP_FREE			5
#define OBS_MAP_CHASE			6

bool CGameUtil::is_spectator()
{
	auto local = CEntityMgr::the().get_local_player();
	if (!local || !local->is_valid())
		return false;

	auto& uptodate_state = local->cl_entity()->curstate;
	bool spectator = uptodate_state.iuser1 != OBS_NONE;
	return spectator;
}

double CGameUtil::get_engine_frametime()
{
	auto cl = CMemoryHookMgr::the().cl().get();
	double ft = cl->time - cl->oldtime;
	return ft;
}

hl::SCREENINFO CGameUtil::get_engine_screen_info()
{
	hl::SCREENINFO info;
	info.iSize = sizeof(hl::SCREENINFO); // must be set before you request it from the engine!

	// the function fails if the size differs/isn't set orl when the pointer passed is null.
	CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetScreenInfo(&info);

	return info;
}

std::string CGameUtil::parse_viewmodel_name(hl::model_t* model)
{
	if (!model->name || !model->name[0])
	{
		return "empty";
	}

	// expect something like /models
	std::string vm_str = CGenericUtil::the().remove_extension_from_path(model->name);

	vm_str = std::filesystem::path(vm_str).filename().string();

	return vm_str.substr(2);
}

void CGameUtil::locate_engine_compile_timestamp()
{
	if (!m_engine_compile_date.empty())
	{
		return;
	}

	CBytePattern version_string_pattern("Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec");

	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(L"hw.dll", SPACE_FULL);

	if (!base || !end)
	{
		CConsole::the().error("Couldn't get engine dll address space.");
		return;
	}

	uintptr_t* string_base_ptr = version_string_pattern.search_in_loaded_address_space(base, end);
	if (!string_base_ptr)
	{
		CConsole::the().error("Couldn't find the exe string inside the executable!");
		return;
	}

	uintptr_t* exe_build_string_end_ptr = (uintptr_t*)((uint8_t*)string_base_ptr - 1);
	uintptr_t* exe_build_string_start_ptr = NULL;

	static const char *mon[12] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	// walk down the binary until we come across any of the months.
	const char* date_string_final = nullptr;
	for (int i = 0; i < 11; i++)
	{
		char* b = (char*)exe_build_string_end_ptr;

		bool found = false;
		int k = 0;
		while (true)
		{
			if (k > 20)
			{
				break;
			}

			if (!strnicmp(b, mon[i], 3))
			{
				found = true;
				break;
			}

			k++;
			b--;
		}

		if (found)
		{
			date_string_final = b;
			break;
		}
	}

	if (!date_string_final)
	{
		CConsole::the().error("Fatal error while parsing the timestamp string!");
		return;
	}

	m_engine_compile_date = (const char*)date_string_final;

	if (m_engine_compile_date.empty())
	{
		CConsole::the().error("Couldn't get engine compile date string!");
		return;
	}

	CConsole::the().info("Found engine compile timestamp: '{}'", m_engine_compile_date);
}
