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

		g_gui_widgets_i->add_child(
			"players", { -1.0f, -2.0f }, true, ImGuiWindowFlags_None,
			[this]()
			{
				render_players();
			});

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Selected player information");

		g_gui_widgets_i->add_child(
			"player_info", { -1.0f, -2.0f }, true, ImGuiWindowFlags_None,
			[this]()
			{
				render_selected_player_info();
			});

		g_gui_widgets_i->end_columns();
	}
}

void CUIPlayerList::render_players()
{
	bool found_selected_plr = false;

	for (auto& [id, player] : CEntityMgr::the().m_known_players)
	{
		auto pinfo = player.get_playerinfo();

		if (pinfo && pinfo->name)
		{
			std::string label = std::format("{:<2}: {}", id, pinfo->name);

			bool is_selected = (m_selected_plr_idx == id);
			if (g_gui_widgets_i->add_toggle_button(label, { -1.0f, 0.0f }, is_selected, false))
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

	assert(CEntityMgr::the().m_known_players.find(m_selected_plr_idx) != CEntityMgr::the().m_known_players.end());

	auto& player = CEntityMgr::the().m_known_players[m_selected_plr_idx];
	auto pinfo = player.get_playerinfo();

	auto window_size = g_gui_widgets_i->get_current_window_size();

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		float pfp_size = 65.0f;
		g_gui_widgets_i->setup_column_fixed_width(window_size.x - (pfp_size + 20.0f));

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("asd");

		g_gui_widgets_i->goto_next_column();

		auto unknown_pfp = g_texture_manager_i->get_texture("unknown_pfp");
		if (unknown_pfp)
		{
			g_gui_widgets_i->add_image((GLuint*)unknown_pfp->get_gl_id(), Vector2D(pfp_size, pfp_size));
		}

		g_gui_widgets_i->end_columns();
	}
}
