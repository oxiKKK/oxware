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

void CUIPlayerList::render_ui()
{
	bool is_connected = CGameUtil::the().is_fully_connected();
	if (!is_connected)
	{
		g_gui_widgets_i->add_window_centered_text_disabled("Not connected to any server.");
		return;
	}

	// NOTE: the "-2" in child vertical sizes is due to random scrollbar appearing when we set -1.0f, I have no idea why..

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->setup_column_fixed_width(200);

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text(std::format("Connected players: {}", CEntityMgr::the().m_known_players.size()));

		auto font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_14px, FDC_Bold);
		g_gui_widgets_i->push_font(font);

		g_gui_widgets_i->add_child(
			"players", { -1.0f, -2.0f }, true, ImGuiWindowFlags_None,
			[this]()
			{
				render_players();
			});

		g_gui_widgets_i->pop_font();

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Selected player information");

		g_gui_widgets_i->push_font(font);

		g_gui_widgets_i->add_child(
			"player_info", { -1.0f, -2.0f }, true, ImGuiWindowFlags_None,
			[this]()
			{
				render_selected_player_info();
			});

		g_gui_widgets_i->pop_font();

		g_gui_widgets_i->end_columns();
	}
}

void CUIPlayerList::initialize()
{
	static bool initialized = false;
	if (initialized)
	{
		return;
	}

	// unknown profile picture
	m_unknown_profile_picture = g_texture_manager_i->create_and_register_new_texture_from_filedata("unknown_pfp", g_unknown_pfp, g_unknown_pfp_size);

	initialized = true;
}

void CUIPlayerList::render_players()
{
	manage_timers();

	bool found_selected_plr = false;

	for (auto& [id, player] : CEntityMgr::the().m_known_players)
	{
		auto pinfo = player.get_playerinfo();

		if (pinfo && pinfo->name && pinfo->name[0])
		{
			bool is_selected = (m_selected_plr_idx == id);
			if (g_gui_widgets_i->add_toggle_button(pinfo->name, { -1.0f, 0.0f }, is_selected, false))
			{
				if (is_selected)
				{
					m_selected_plr_idx = -1;
				}
				else
				{
					m_selected_plr_idx = id;
				}
			}
		}
		else
		{
			g_gui_widgets_i->add_colored_text(CColor(112, 0, 0, 170), std::format("{:<2}: invalid information", id));
		}

		// look if the selected player is still valid
		if (m_selected_plr_idx != -1 && !found_selected_plr && id == m_selected_plr_idx)
		{
			found_selected_plr = true;
		}

		if (m_update_player_cache)
		{
			create_cached_player_info(id, player);
		}
	}

	if (!found_selected_plr)
	{
		m_selected_plr_idx = -1;
	}
}

void CUIPlayerList::render_selected_player_info()
{
	if (m_selected_plr_idx == -1)
	{
		g_gui_widgets_i->add_window_centered_text_disabled("No player selected! 😶");
		return;
	}

	auto& cached_player = m_cached_players[m_selected_plr_idx];

	auto window_size = g_gui_widgets_i->get_current_window_size();

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		// NOTE that this should be 64x64, because we retreive a 64x64 avatar picture from steam.
		const float avatar_size = 84.0f;
		g_gui_widgets_i->setup_column_fixed_width(window_size.x - (avatar_size + 20.0f));

		g_gui_widgets_i->goto_next_column();

		render_player_info(cached_player);

		g_gui_widgets_i->goto_next_column();

		render_player_avatar(cached_player, avatar_size);

		g_gui_widgets_i->end_columns();
	}
}

void CUIPlayerList::create_cached_player_info(int id, const CGenericPlayer& player)
{
	auto& cached_player = m_cached_players[id];

	cached_player.m_steam_id = player.get_steam_id();

	// we're re-precaching avatar images this frame
	if (m_precache_avatar_picture)
	{
		update_avatars(id, cached_player);
	}
}

void CUIPlayerList::render_player_info(const cached_player_info_t& cached_player)
{
	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		// NOTE that this should be 64x64, because we retreive a 64x64 avatar picture from steam.
		const float avatar_size = 64.0f;
		g_gui_widgets_i->setup_column_fixed_width(50.0f);

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("SteamID2");
		g_gui_widgets_i->add_text("SteamID3");

		g_gui_widgets_i->goto_next_column();

		std::string sid2 = CSteamIDUtil::the().render_steam2_id(cached_player.m_steam_id);
		std::string sid3 = CSteamIDUtil::the().render_steam3_id(cached_player.m_steam_id);
		g_gui_widgets_i->add_text(sid2);
		g_gui_widgets_i->add_text(sid3);

		g_gui_widgets_i->end_columns();
	}

	std::string url = CSteamIDUtil::the().steam_id_to_url(cached_player.m_steam_id);
	if (g_gui_widgets_i->add_hypertext_link("Open steam profile in browser"))
	{
		CGenericUtil::the().open_link_inside_browser(url);
	}
}

void CUIPlayerList::render_player_avatar(const cached_player_info_t & cached_player, float avatar_size)
{
	auto& player_avatar = cached_player.m_avatar_picture;

	g_gui_widgets_i->add_image((GLuint*)player_avatar.get_gl_id(), Vector2D(avatar_size, avatar_size));
}

void CUIPlayerList::manage_timers()
{
	// update player avatars every 10s
	if (GetTickCount() - m_last_avatar_precache_time > 10000)
	{
		m_precache_avatar_picture = true;
		m_last_avatar_precache_time = GetTickCount();
	}
	else
	{
		m_precache_avatar_picture = false;
	}

	// update cache player info every 250ms
	if (GetTickCount() - m_last_cache_update_time > 250)
	{
		m_update_player_cache = true;
		m_last_cache_update_time = GetTickCount();
	}
	else
	{
		m_update_player_cache = false;
	}
}

void CUIPlayerList::update_avatars(int id, cached_player_info_t& cached_player)
{
	if (!cached_player.m_steam_id.m_unAll64Bits || !CSteamIDUtil::the().is_steam_user(cached_player.m_steam_id))
	{
		// invalid steam id for some reason, default to checkerboard
		cached_player.m_avatar_picture = m_unknown_profile_picture;
		return;
	}

	// get 64x64 avatar picture
	auto image_id = CHLInterfaceHook::the().ISteamFriends()->GetMediumFriendAvatar(cached_player.m_steam_id.m_unAll64Bits);
	if (image_id == 0) // no avatar is set, use default one
	{
		cached_player.m_avatar_picture = m_unknown_profile_picture;
		return;
	}

	std::string avatar_texture_id = get_avatar_texture_playerid(id);

	uint32_t w, h;
	if (!CHLInterfaceHook::the().ISteamUtils()->GetImageSize(image_id, &w, &h))
	{
		CConsole::the().derror("Couldn't get player {} steam avatar size. (image id={}, sid={})", id, image_id, cached_player.m_steam_id.m_unAll64Bits);
		return;
	}

	// retreive image data straight from steam
	int avatar_image_size = w * h * 4;
	uint8_t* avatar_image_data = new uint8_t[avatar_image_size];
	bool does_exist = CHLInterfaceHook::the().ISteamUtils()->GetImageRGBA(image_id, avatar_image_data, avatar_image_size);

	if (!does_exist)
	{
		CConsole::the().derror("Couldn't get player {} steam avatar. (image id={}, sid={})", id, image_id, cached_player.m_steam_id.m_unAll64Bits);
		cached_player.m_avatar_picture = m_unknown_profile_picture;
		return;
	}
	
	// reupload in case it was already uploaded
	cached_player.m_avatar_picture = g_texture_manager_i->reupload_texture_from_pixeldata(avatar_texture_id.c_str(), avatar_image_data, Vector2D((float)w, (float)h));

	delete[] avatar_image_data;
}

std::string CUIPlayerList::get_avatar_texture_playerid(int id)
{
	return std::format("avatar_{}", id);
}
