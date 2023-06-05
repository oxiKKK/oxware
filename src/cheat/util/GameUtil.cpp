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

float CGameUtil::compute_ground_angle_for_origin(const Vector& origin, float trace_distance)
{
	Vector trace_end = Vector(origin.x, origin.y, -trace_distance); // trace as back as this

	hl::pmtrace_t* tr = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnPM_TraceLine(
		(Vector)origin, trace_end, PM_TRACELINE_ANYVISIBLE, CLocalState::the().get_current_hull(), -1);
	
	// plane normal returns the [0, 1] scale of rotation of the plane [0°, 90°].
	//
	//	  plane			|  		
	//		|  			|  		   0
 	//		|-----> 1	|   90°----^----- plane
	//		|			|  
	//		0°			|  
	//
	return CMathUtil::the().rad2deg(acosf(tr->plane.normal.z));
}

float CGameUtil::compute_distance_to_ground(const Vector& origin, float trace_distance)
{
	Vector trace_end = Vector(origin.x, origin.y, -trace_distance); // trace as back as this

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	
	hl::pmtrace_t* tr = cl_enginefuncs->pfnPM_TraceLine(
		(Vector)origin, trace_end, PM_TRACELINE_ANYVISIBLE, CLocalState::the().get_current_hull(), -1);
	
	return origin.z - tr->endpos.z;
}

float CGameUtil::compute_edge_distance(const Vector& origin, float edge_trace_distance)
{
#if 0
	//
	// this method is slow and doesn't work. Just saying, because some cheats
	// use it anyways. Lol.
	//

	float starting_distance = 250.0f;

	// X...X...X
	// .\  |  /.
	// . \ | / .
	// .  \|/  .
	// X---o---X
	// .  /|\  .
	// . / | \ .
	// ./  |  \.
	// X...X...X
	static Vector2D vertices[] =
	{
		{ -1,  0 },
		{  1,  0 },
		{  0,  1 },
		{  0, -1 },
		{ -1, -1 },
		{  1,  1 },
		{  1, -1 },
		{ -1,  1 },
	};

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();

	for (int i = 0; i < 8; i++)
	{
		//
		// get a traseresult from tracing to the ground
		//

		Vector trace_end = Vector(origin.x, origin.y, -4096.0f); // trace as back as this

		hl::pmtrace_t* gnd_tr = cl_enginefuncs->pfnPM_TraceLine(
			(Vector)origin, trace_end, PM_TRACELINE_ANYVISIBLE, CLocalState::the().get_current_hull(), -1);

		//
		// get distance to a nearest edge
		//

		// fraction is a [0, 1] scale of the distance between two traced points
		Vector vec_dist = (trace_end - origin) * gnd_tr->fraction;
		Vector surface_beneath = origin;
		surface_beneath.z -= vec_dist.z;
		surface_beneath.z -= 0.1f; // trace a little bit beneath the surface, so we can check for the edge.

		Vector desired_edge = surface_beneath;
		desired_edge.x += vertices[i][0] * starting_distance;
		desired_edge.y += vertices[i][1] * starting_distance;

		hl::pmtrace_t* edge_tr = cl_enginefuncs->pfnPM_TraceLine(
			desired_edge, surface_beneath, PM_TRACELINE_ANYVISIBLE, CLocalState::the().get_current_hull(), -1);

		// lower the distance on the lowest possible distance, therefore we find the closest edge
		// we start from the "ending" point and end on "starting" point for a reason. It is to check
		// if there's air or still a surface.
		if (edge_tr->startsolid == FALSE)
		{
			Vector dt = edge_tr->endpos - surface_beneath;
			starting_distance = dt.Length2D();
		}
	};

	return starting_distance;
#else

	//
	// Note: Doesn't work on ramps.
	//

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();

//	auto t1 = std::chrono::high_resolution_clock::now();

	float desired_edge_dist = FLT_MAX;

	static auto trace_endpos_helper = [&](const Vector& start, const Vector& end, int N) -> float
	{
		hl::pmtrace_t* tr = cl_enginefuncs->pfnPM_TraceLine((Vector)start, (Vector)end, PM_TRACELINE_ANYVISIBLE, CLocalState::the().get_current_hull(), -1);
		return tr->endpos[N];
	};

	Vector trace_end = Vector(origin.x, origin.y, -4096.0f); // trace as back as this
	
	hl::pmtrace_t* surface_tr = cl_enginefuncs->pfnPM_TraceLine(
		(Vector)origin, trace_end, PM_TRACELINE_ANYVISIBLE,
		CLocalState::the().get_current_hull(), -1);

	Vector surface_beneath = surface_tr->endpos;
	surface_beneath.z -= 0.1f; // trace a little bit beneath the surface, so we can check for the edge.

	//
	// find the closest edge.
	//

	// how far to trace from the origin
	float d = edge_trace_distance;

	desired_edge_dist = std::min(desired_edge_dist,						 trace_endpos_helper(surface_beneath + Vector(d, 0.0f, 0.0f), surface_beneath, 0) - surface_beneath[0]);
	desired_edge_dist = std::min(desired_edge_dist, surface_beneath[0] - trace_endpos_helper(surface_beneath - Vector(d, 0.0f, 0.0f), surface_beneath, 0));
	desired_edge_dist = std::min(desired_edge_dist,						 trace_endpos_helper(surface_beneath + Vector(0.0f, d, 0.0f), surface_beneath, 1) - surface_beneath[1]);
	desired_edge_dist = std::min(desired_edge_dist, surface_beneath[1] - trace_endpos_helper(surface_beneath - Vector(0.0f, d, 0.0f), surface_beneath, 1));

//	CConsole::the().info("{} microseconds", std::chrono::duration<float, std::micro>(std::chrono::high_resolution_clock::now() - t1).count());

	return desired_edge_dist == edge_trace_distance ? 0.0f : desired_edge_dist;
#endif
}

hl::CBasePlayerWeapon* CGameUtil::get_current_weapon()
{
	auto& client_wpns_hook = CMemoryHookCBaseStuff::the().ClientWeapons();
	if (!client_wpns_hook.is_installed())
	{
		return NULL;
	}

	auto weapons = *client_wpns_hook.get();

	auto cldata = CLocalState::the().get_current_frame_clientdata();
	if (!cldata)
	{
		return NULL;
	}

	return weapons[cldata->m_iId];
}

std::string CGameUtil::get_modelname_from_weapon(int wpnidx)
{
	if (wpnidx >= hl::WEAPON_P228 && wpnidx <= hl::WEAPON_P90)
	{
		switch (wpnidx)
		{
			case hl::WEAPON_P228:			return "models/v_p228.mdl";
			case hl::WEAPON_GLOCK:			return "models/v_glock18.mdl";
			case hl::WEAPON_SCOUT:			return "models/v_scout.mdl";
			case hl::WEAPON_HEGRENADE:		return "models/v_hegrenade.mdl";
			case hl::WEAPON_XM1014:			return "models/v_xm1014.mdl";
			case hl::WEAPON_C4:				return "models/v_c4.mdl";
			case hl::WEAPON_MAC10:			return "models/v_mac10.mdl";
			case hl::WEAPON_AUG:			return "models/v_aug.mdl";
			case hl::WEAPON_SMOKEGRENADE:	return "models/v_smokegrenade.mdl";
			case hl::WEAPON_ELITE:			return "models/v_elite.mdl";
			case hl::WEAPON_FIVESEVEN:		return "models/v_fiveseven.mdl";
			case hl::WEAPON_UMP45:			return "models/v_ump45.mdl";
			case hl::WEAPON_SG550:			return "models/v_sg550.mdl";
			case hl::WEAPON_GALIL:			return "models/v_galil.mdl";
			case hl::WEAPON_FAMAS:			return "models/v_famas.mdl";
			case hl::WEAPON_USP:			return "models/v_usp.mdl";
			case hl::WEAPON_GLOCK18:		return "models/v_glock18.mdl";
			case hl::WEAPON_AWP:			return "models/v_awp.mdl";
			case hl::WEAPON_MP5N:			return "models/v_mp5.mdl";
			case hl::WEAPON_M249:			return "models/v_m249.mdl";
			case hl::WEAPON_M3:				return "models/v_m3.mdl";
			case hl::WEAPON_M4A1:			return "models/v_m4a1.mdl";
			case hl::WEAPON_TMP:			return "models/v_tmp.mdl";
			case hl::WEAPON_G3SG1:			return "models/v_g3sg1.mdl";
			case hl::WEAPON_FLASHBANG:		return "models/v_flashbang.mdl";
			case hl::WEAPON_DEAGLE:			return "models/v_deagle.mdl";
			case hl::WEAPON_SG552:			return "models/v_sg552.mdl";
			case hl::WEAPON_AK47:			return "models/v_ak47.mdl";
			case hl::WEAPON_KNIFE:			return "models/v_knife.mdl";
			case hl::WEAPON_P90:			return "models/v_p90.mdl";
		}
	}

	return "";
}


int CGameUtil::get_weapon_accuracy_flags(int weapon_id, int weapon_flags)
{
	int flags = 0;

	switch (weapon_id)
	{
		case hl::WEAPON_AUG:
		case hl::WEAPON_GALIL:
		case hl::WEAPON_M249:
		case hl::WEAPON_SG552:
		case hl::WEAPON_AK47:
		case hl::WEAPON_P90:
			flags = ACCURACY_AIR | ACCURACY_SPEED;

			break;
		case hl::WEAPON_P228:
		case hl::WEAPON_FIVESEVEN:
		case hl::WEAPON_DEAGLE:
			flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK;

			break;
		case hl::WEAPON_GLOCK18:
			if (flags & WPNSTATE_GLOCK18_BURST_MODE)
				flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK;
			else
				flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK | ACCURACY_MULTIPLY_BY_14_2;

			break;
		case hl::WEAPON_MAC10:
		case hl::WEAPON_UMP45:
		case hl::WEAPON_MP5N:
		case hl::WEAPON_TMP:
			flags = ACCURACY_AIR;

			break;
		case hl::WEAPON_M4A1:
			if (flags & WPNSTATE_USP_SILENCED)
				flags = ACCURACY_AIR | ACCURACY_SPEED;
			else
				flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_MULTIPLY_BY_14;

			break;
		case hl::WEAPON_FAMAS:
			if (flags & WPNSTATE_FAMAS_BURST_MODE)
				flags = ACCURACY_AIR | ACCURACY_SPEED;
			else
				flags = ACCURACY_AIR | ACCURACY_SPEED | (1 << 4);

			break;
		case hl::WEAPON_USP:
			if (flags & WPNSTATE_USP_SILENCED)
				flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK;
			else
				flags = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK | ACCURACY_MULTIPLY_BY_14;
	}

	return flags;
}

#pragma pack(push, 1)
struct hud_command_t
{
	byte	cmd;
	float	time;
	int		frame;
	char	szNameBuf[64];
};
#pragma pack(pop)

void CGameUtil::record_hud_command(const char* cmdname)
{
	auto cls = CMemoryHookMgr::the().cls().get();

	if (!cls->demorecording || cls->spectator)
	{
		return;
	}

	auto host_framecount = CMemoryHookMgr::the().host_framecount().get();
	static int prev_frame = 0;
	if (*host_framecount == prev_frame)
	{
		return;
	}

	auto realtime = CMemoryHookMgr::the().realtime().get();

	// Prepare new command header
	hud_command_t cmd;
	cmd.cmd = 3;
	cmd.time = *realtime - cls->demostarttime;
	cmd.frame = *host_framecount - cls->demostartframe;

	// Prepare name buffer
	memset(cmd.szNameBuf, NULL, sizeof(cmd.szNameBuf));
	strncpy(cmd.szNameBuf, cmdname, sizeof(cmd.szNameBuf) - 1);
	cmd.szNameBuf[sizeof(cmd.szNameBuf) - 4] = '\0';

	CHLInterfaceHook::the().IFileSystem()->Write(&cmd, sizeof(hud_command_t), cls->demofile);

	prev_frame = *host_framecount;
}

void CGameUtil::classic_cs16_cheating_scene_speedhack(double new_speed)
{
	// Well oh well. Here we come, with the CS 1.6 classic.. the "Texture load: %6.1fms\n" speedhack...
	//
	// You may seem this speedhack in several old cheat sources.. But what it acutally hooks may fucking shock
	// you to your fucking core.. So, it hooks a compiler-generated constant, a 1000.0 ... You don't believe me?
	// Well, take a look at this.
	// 
	//													   This is it
	//													   v
	// .text:01D40960 DC 0D E0 D2 E1 01         fmul    >ds:random_1k_speedhack_modifier_constant<
	// .text:01D40966 DD 1C 24                  fstp    [esp+553D4h+var_553D4] ; char
	// .text:01D40969 68 64 1B E4 01            push    offset aTextureLoad61f ; "Texture load: %6.1fms\n"
	//
	// But man, this is just a normal global variable declared by Valve, what are you talking about?! Weeell... let's see then...
	// Let's take a look at the code (and btw, this is Mod_LoadTextures function, but this global variable can be found elsewhere.)
	// 
	// --------------------------------------------------------------------
	// }
	// LABEL_82:
	//   v39 = Sys_FloatTime();
	//   Con_DPrintf("Texture load: %6.1fms\n", (v39 - v43) * 1000.0);
	// }
	// --------------------------------------------------------------------
	// 
	// So where is the global variable? :thinking: .... Well, it's the fucking "1000.0"! Yes, that's fucking right!
	// And now.. what does that mean? Well, it means that this variable is used within the code in several functions inside
	// mathematical formulas, why? because it's a fucking 1000.0... for example used inside:
	// 
	//	CL_Move, CL_AdjustClock, CL_RunUsercmd <<< btw, all of these client-side functions related to movement.
	//	and more...
	// 
	// Inside all of these functions, it is used in mathematical formulas, as said earlier, in particular, inside time-related
	// formulas, such as this one inside CL_RunUsercmd:
	// 
	// --------------------------------------------------------------------
	// pmove->time = *pfElapsed * 1000.0; <<< the variable on the right
	// --------------------------------------------------------------------
	//
	// and in other places similar to this one, too...
	//
	// Well, so if we hook this variable, and modify it let's say by 0.001, we modify the time base of the playermove code? Hell yeah!
	// But what is the most disturbing about this is that the occurence of this global variable is totally "random", and anyone who hooked
	// it first, didn't have a fucking clue (I assume) what it will actually do. I assume it went like this: "Oh, let me just fucking hook
	// random memory location that is used in a bunch of client-side functions, and see what it will do if a multiply it by a number... Wow
	// I just created a speedhack!"... well, pretty hilarious.. isn't it?
	// 
	// How impressive or dumb or hilarious or idiotic it may seem, it really works well... Jeez... :D
	//

	static double prev_random_thing_xd = 1.0;
	if (prev_random_thing_xd != new_speed)
	{
		double* totally_random_stuff_ptr = CMemoryHookMgr::the().random_1k_speedhack_modifier_constant().get();

		CGenericUtil::the().push_page_protection((uintptr_t)totally_random_stuff_ptr, sizeof(double), PAGE_EXECUTE_READWRITE);
		*totally_random_stuff_ptr = new_speed * 1000.0;
		CGenericUtil::the().pop_page_protection();

		CConsole::the().info("spd: {}", *totally_random_stuff_ptr);

		prev_random_thing_xd = new_speed;
	}
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
