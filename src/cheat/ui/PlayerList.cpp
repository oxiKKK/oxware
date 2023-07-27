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
	
	initialize();

	auto cursor_pos = g_gui_widgets_i->get_cursor_pos();
	g_gui_widgets_i->set_cursor_pos({ 148.0f, 0.0f });

	if (g_gui_widgets_i->add_button("filter", { 50.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
	{
		g_gui_widgets_i->schedule_simple_popup_dialog("apply_filer");
	}

	g_gui_widgets_i->set_cursor_pos(cursor_pos);

	g_gui_widgets_i->execute_simple_popup_popup(
		"apply_filer", { 135, 90 }, ImGuiWindowFlags_NoMove,
		[&]()
		{
			g_gui_widgets_i->add_checkbox("Allow Steam", &m_filter_allow_steam);
			g_gui_widgets_i->add_checkbox("Allow Non-Steam", &m_filter_allow_non_steam);
			g_gui_widgets_i->add_checkbox("Allow BOTs", &m_filter_allow_bots);
		});

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

	m_filter_allow_bots = true;
	m_filter_allow_steam = true;
	m_filter_allow_non_steam = true;

	// unknown profile picture
	m_bot_pfp = g_texture_manager_i->create_and_register_new_texture_from_filedata("bot_pfp", g_bot_pfp, g_bot_pfp_size);
	m_nonsteam_pfp = g_texture_manager_i->create_and_register_new_texture_from_filedata("nonsteam_pfp", g_nonsteam_pfp, g_nonsteam_pfp_size);
	m_invalid_pfp = g_texture_manager_i->create_and_register_new_texture_from_filedata("invalid_pfp", g_invalid_pfp, g_invalid_pfp_size);
	m_avatar_not_found = g_texture_manager_i->create_and_register_new_texture_from_filedata("avatar_not_found", g_avatar_not_found, g_avatar_not_found_size);

	initialized = true;
}

void CUIPlayerList::render_players()
{
	m_is_local_steam = (COxWare::the().get_build_number() >= 8684) ? true : false;

	manage_timers();

	if (m_update_player_cache)
	{
		// clear the cache first, before we re-fill it
		for (auto& cached_player : m_cached_players)
		{
			// we want to clear everything except avatar picture. this is one way of doing it.
			// and it should not create any harm, the struct is small and independent.
			BaseTexture avatar_save = cached_player.m_avatar_picture;
			cached_player = {};
			cached_player.m_avatar_picture = avatar_save;
		}
	}

	bool found_selected_plr = false;

	// shrink items vertically
	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_ItemSpacing, { 1.0f, 1.0f });

	auto& known_players = CEntityMgr::the().m_known_players;
	for (auto& [one_based_id, player] : known_players)
	{
		int zero_based_id = one_based_id - 1;

		if (apply_filter(player))
		{
			continue;
		}

		auto pinfo = player.get_playerinfo();

		if (pinfo && pinfo->name && pinfo->name[0])
		{
			// display player names in color based on their team.
			g_gui_widgets_i->push_color(ImGuiCol_Text, player.get_color_based_on_team());

			bool is_selected = (m_selected_plr_idx == zero_based_id);
			if (g_gui_widgets_i->add_toggle_button(pinfo->name, { -1.0f, 0.0f }, is_selected, false))
			{
				m_selected_plr_idx = zero_based_id;
			}

			g_gui_widgets_i->pop_color();
		}
		// we may have player connecting to a server here. or just an invalid one. :)
		else
		{
			// if player is connecting, the server doesn't send us any info, only the userid
			if (pinfo && pinfo->name && pinfo->userid != 0)
			{
				g_gui_widgets_i->push_color(ImGuiCol_Text, CColor(12, 123, 231, 230));

				bool is_selected = (m_selected_plr_idx == zero_based_id);
				if (g_gui_widgets_i->add_toggle_button(std::format("player (id {}) connecting...", zero_based_id), { -1.0f, 0.0f }, is_selected, true))
				{
				}

				g_gui_widgets_i->pop_color();
			}
			else
			{
				g_gui_widgets_i->push_color(ImGuiCol_Text, CColor(230, 50, 50, 230));

				bool is_selected = (m_selected_plr_idx == zero_based_id);
				if (g_gui_widgets_i->add_toggle_button(std::format("invalid player ({})", zero_based_id), { -1.0f, 0.0f }, is_selected, true))
				{
				}

				g_gui_widgets_i->pop_color();
			}
		}

		// look if the selected player is still valid
		if (m_selected_plr_idx != -1 && !found_selected_plr && zero_based_id == m_selected_plr_idx)
		{
			found_selected_plr = true;
		}

		if (m_update_player_cache)
		{
			create_cached_player_info(zero_based_id, player);
		}
	}

	g_gui_widgets_i->pop_stylevar(); // item spacing

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
	auto cursor_pos = g_gui_widgets_i->get_cursor_pos();

	render_player_info(cached_player);

	// NOTE that this should be 64x64, because we retreive a 64x64 avatar picture from steam.
	const float avatar_size = 84.0f;

	g_gui_widgets_i->set_cursor_pos({ window_size.x - (avatar_size + 5.0f), cursor_pos.y });
	render_player_avatar(cached_player, avatar_size);
}

void CUIPlayerList::create_cached_player_info(int id, const CGenericPlayer& player)
{
	auto& cached_player = m_cached_players[id];

	cached_player.m_steam3_id = player.get_steam_id();
	cached_player.m_steam2_id = CSteamIDUtil::the().to_steam2(cached_player.m_steam3_id);
	cached_player.m_player_id = id;
	cached_player.m_user_id = player.get_playerinfo()->userid;
	cached_player.m_team = player.get_team_as_string();
	cached_player.m_is_steam_user = CSteamIDUtil::the().is_steam_user(cached_player.m_steam3_id);

	// steam-only data
	if (cached_player.m_is_steam_user && m_is_local_steam)
	{
		cached_player.m_steam_name = player.get_steam_persona_name();
		cached_player.m_persona_state = player.get_persona_state();
		cached_player.m_local_is_friend_with = player.is_local_friend_with();
	}

	// we're re-precaching avatar images this frame
	if (m_precache_avatar_picture)
	{
		update_avatars(id, cached_player);
	}
}

static const float k_left_column_width = 70.0f;

void CUIPlayerList::render_player_info(const cached_player_info_t& cached_player)
{
	if (g_gui_widgets_i->begin_columns("pinfo_1", 2))
	{
		// NOTE that this should be 64x64, because we retreive a 64x64 avatar picture from steam.
		const float avatar_size = 64.0f;
		g_gui_widgets_i->setup_column_fixed_width(k_left_column_width);

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("SteamID");
		g_gui_widgets_i->add_text("Instance ID");
		g_gui_widgets_i->add_text("LocalUser ID");
		g_gui_widgets_i->add_text("Raw (dec)");
		g_gui_widgets_i->add_text("Raw (hex)");
		g_gui_widgets_i->add_text("User ID");

		g_gui_widgets_i->goto_next_column();

		std::string sid2 = CSteamIDUtil::the().render_steam2_id_variant_2(cached_player.m_steam3_id);
		g_gui_widgets_i->add_text(sid2);
		g_gui_widgets_i->add_text(std::to_string(cached_player.m_steam2_id.m_SteamInstanceID));
		g_gui_widgets_i->add_text(std::format("0x{:016X}", cached_player.m_steam2_id.m_SteamLocalUserID.As64bits));
		g_gui_widgets_i->add_text(std::to_string(cached_player.m_steam3_id.m_unAll64Bits));
		g_gui_widgets_i->add_text(std::format("0x{:016X}", cached_player.m_steam3_id.m_unAll64Bits));
		g_gui_widgets_i->add_text(std::to_string(cached_player.m_user_id));

		g_gui_widgets_i->add_spacing();

		g_gui_widgets_i->end_columns();
	}

	add_steam_community_profile_link(cached_player);

	if (g_gui_widgets_i->begin_columns("pinfo_2", 2))
	{
		g_gui_widgets_i->setup_column_fixed_width(k_left_column_width);

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("0-based ID");
		g_gui_widgets_i->add_text("Team");

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text(std::to_string(cached_player.m_player_id));
		g_gui_widgets_i->add_text(cached_player.m_team);

		g_gui_widgets_i->add_spacing();

		g_gui_widgets_i->end_columns();
	}

	if (m_is_local_steam && cached_player.m_is_steam_user)
	{
		add_additional_steam_info_about_player(cached_player);
	}
}

void CUIPlayerList::render_player_avatar(const cached_player_info_t& cached_player, float avatar_size)
{
	auto& player_avatar = cached_player.m_avatar_picture;

	g_gui_widgets_i->add_image((GLuint*)player_avatar.get_gl_id(), Vector2D(avatar_size, avatar_size));
}

void CUIPlayerList::add_steam_community_profile_link(const cached_player_info_t& cached_player)
{
	bool is_steam = CSteamIDUtil::the().is_steam_user(cached_player.m_steam3_id);

	if (!is_steam)
	{
		g_gui_widgets_i->push_disabled();
	}

	std::string url = CSteamIDUtil::the().steam_id_to_url(cached_player.m_steam3_id);
	if (g_gui_widgets_i->add_url_text("Open steam profile in browser"))
	{
		CGenericUtil::the().open_link_inside_browser(url);
	}

	if (!is_steam)
	{
		g_gui_widgets_i->pop_disabled();
	}
}

void CUIPlayerList::add_additional_steam_info_about_player(const cached_player_info_t& cached_player)
{
	if (g_gui_widgets_i->add_tree_node("Additional steam information"))
	{
		if (g_gui_widgets_i->begin_columns("add_sinfo", 2))
		{
			g_gui_widgets_i->setup_column_fixed_width(k_left_column_width);

			g_gui_widgets_i->goto_next_column();

			g_gui_widgets_i->add_text("Friends with");
			g_gui_widgets_i->add_text("Account type");
			g_gui_widgets_i->add_text("Universe");
			g_gui_widgets_i->add_text("Steam name");
			g_gui_widgets_i->add_text("Persona state");

			g_gui_widgets_i->goto_next_column();

			static auto yesno = [](bool c)->std::string { return c ? "yes" : "no"; };

			add_gui_condional_colored_text(yesno(cached_player.m_local_is_friend_with), cached_player.m_local_is_friend_with);
			g_gui_widgets_i->add_text(std::format("{}", (hl::EAccountType)cached_player.m_steam3_id.m_EAccountType));
			g_gui_widgets_i->add_text(std::format("{}", cached_player.m_steam3_id.m_EUniverse));
			g_gui_widgets_i->add_text(std::format("{}", cached_player.m_steam_name));
			g_gui_widgets_i->add_text(std::format("{}", cached_player.m_persona_state));

			g_gui_widgets_i->add_spacing();

			g_gui_widgets_i->end_columns();
		}

		g_gui_widgets_i->pop_tree_node();
	}
}

void CUIPlayerList::manage_timers()
{
	// update player avatars every 5s -- ONLY IF LOCAL IS STEAM
	if (m_is_local_steam)
	{
		if (GetTickCount() - m_last_avatar_precache_time > 5000)
		{
			m_precache_avatar_picture = true;
			m_last_avatar_precache_time = GetTickCount();
		}
		else
		{
			m_precache_avatar_picture = false;
		}
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

void CUIPlayerList::add_gui_condional_colored_text(const std::string& text, bool condition)
{
	if (condition)
	{
		g_gui_widgets_i->add_colored_text(CColor(0, 112, 0, 170), text);
	}
	else
	{
		g_gui_widgets_i->add_colored_text(CColor(112, 0, 0, 170), text);
	}
}

void CUIPlayerList::update_avatars(int id, cached_player_info_t& cached_player)
{
	if (CSteamIDUtil::the().is_bot(cached_player.m_steam3_id))
	{
		// BOTs usually have NULL sid
		cached_player.m_avatar_picture = m_bot_pfp;
		return;
	}

	if (!CSteamIDUtil::the().is_steam_user(cached_player.m_steam3_id))
	{
		// Non-Steam players
		cached_player.m_avatar_picture = m_nonsteam_pfp;
		return;
	}

	// get 64x64 avatar picture
	int image_id = CHLInterfaceHook::the().ISteamFriends()->GetLargeFriendAvatar(cached_player.m_steam3_id.m_unAll64Bits);
	if (image_id == 0) // no avatar is set, use default one
	{
		// if avatars aren't shown in scoreboard, we won't get it either. (that is if scoreboard_showavatars is 1)
		cached_player.m_avatar_picture = m_avatar_not_found; 
		return;
	}

	std::string avatar_texture_id = get_avatar_texture_playerid(id);

	uint32_t w, h;
	if (!CHLInterfaceHook::the().ISteamUtils()->GetImageSize(image_id, &w, &h))
	{
		CConsole::the().derror("Couldn't get player {} steam avatar size. (image id={}, sid={})", id, image_id, cached_player.m_steam3_id.m_unAll64Bits);
		cached_player.m_avatar_picture = m_avatar_not_found;
		return;
	}

	// retreive image data straight from steam
	int avatar_image_size = w * h * 4;
	uint8_t* avatar_image_data = new uint8_t[avatar_image_size];
	bool does_exist = CHLInterfaceHook::the().ISteamUtils()->GetImageRGBA(image_id, avatar_image_data, avatar_image_size);

	if (!does_exist)
	{
		CConsole::the().derror("Couldn't get player {} steam avatar. (image id={}, sid={})", id, image_id, cached_player.m_steam3_id.m_unAll64Bits);
		cached_player.m_avatar_picture = m_invalid_pfp;
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

bool CUIPlayerList::apply_filter(const CGenericPlayer& player)
{
	if (player.is_bot())
	{
		return !m_filter_allow_bots;
	}

	if (player.is_steam_user())
	{
		return !m_filter_allow_steam;
	}

	if (!player.is_steam_user())
	{
		return !m_filter_allow_non_steam;
	}

	return false;
}
