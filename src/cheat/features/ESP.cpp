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

VarBoolean esp_enable("esp_enable", "Toggles esp", false);
VarBoolean esp_only_enemy_team("esp_only_enemy_team", "Esp only on enemy team", false);

VarBoolean esp_entity_enable("esp_entity_enable", "Toggles entity esp", false);

VarBoolean esp_player_enable("esp_player_enable", "Toggles player esp", false);
VarBoolean esp_player_name("esp_name", "Toggles player name esp", false);

VarBoolean esp_sound_enable("esp_sound_enable", "Toggles sound esp", false);
VarFloat esp_sound_interval("esp_sound_interval", "Time of which the esp node is alive", 1.0f, 0.5f, 5.0f);
VarBoolean esp_sound_filter_local("esp_sound_filter_local", "Doesn't apply sound esp on local player", true);

void CESP::initialize_gui()
{
	COxWareUI::the().add_background_rendering_code([this]() { on_render(); });
	COxWareUI::the().add_background_rendering_constrain(
		[this]() 
		{
			return esp_enable.get_value() && 
				((esp_player_enable.get_value() && !CEntityMgr::the().m_known_players.empty()) ||
				 (esp_entity_enable.get_value() && !CEntityMgr::the().m_known_entities.empty()) ||
				 (esp_sound_enable.get_value() && !m_stepsounds.empty()));
		});
}

void CESP::register_player_step(const Vector& origin, int entid)
{
	m_stepsounds.push_back({ origin, entid, std::chrono::high_resolution_clock::now() });
}

void CESP::on_render()
{
	if (!esp_enable.get_value())
	{
		return;
	}

	if (esp_player_enable.get_value())
	{
		render_players();
	}

	if (esp_entity_enable.get_value())
	{
		render_entities();
	}

	if (esp_sound_enable.get_value())
	{
		render_sound();
	}
}

void CESP::render_players()
{
	for (auto& [index, plr] : CEntityMgr::the().m_known_players)
	{
		if (!plr.is_valid())
		{
			continue;
		}

		auto& model = plr.associated_model();
		auto cl_ent = plr.cl_entity();
		auto plr_info = plr.get_playerinfo();

		if (model.is_valid() && plr_info && 0)
		{
			auto hl_model = model.hl_model();

			auto text = std::format("{}: {} & {} ducking = {}",
									plr_info->name,
									cl_ent->curstate.mins,
									cl_ent->curstate.maxs, 
									plr.is_ducking());
			g_gui_widgets_i->add_text(text, TEXTPROP_ColorWhite);
		}

		if (!plr.is_alive() || plr.is_out_of_update_for(1.0f) || plr.is_local_player() || !model.is_valid())
		{
			continue;
		}

		if (esp_only_enemy_team.get_value() && !CGameUtil::the().is_player_on_enemy_team(index))
		{
			continue;
		}

		Vector2D box_top2d, box_bot2d;
		Vector box_top3d = cl_ent->origin + Vector(0.0f, 0.0f, plr.get_bounding_box_max().z); // add a little bit of height
		Vector box_bot3d = cl_ent->origin + Vector(0.0f, 0.0f, plr.get_bounding_box_min().z);
		if (CGameUtil::the().world_to_screen(box_top3d, box_top2d) &&
			CGameUtil::the().world_to_screen(box_bot3d, box_bot2d))
		{
			float box_tall_half = box_bot2d.y - box_top2d.y;
			float box_wide_half = box_tall_half / (4.6f/* * (plr.is_ducking() ? 2 : 1)*/); // tweaked ration to perfection

 			Vector2D box_topleft = { box_top2d[0] - box_wide_half, box_top2d[1] };
			Vector2D box_topright = { box_top2d[0] + box_wide_half, box_top2d[1] };
			Vector2D box_botleft = { box_bot2d[0] - box_wide_half, box_bot2d[1] };
			Vector2D box_botright = { box_bot2d[0] + box_wide_half, box_bot2d[1] };

			auto team_color = plr.get_color_based_on_team();

			//
			// render 4 corners using lines. this fixes visual deformation of the box when faced on the far side of fisheye effect.
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_topleft, box_topright,
				team_color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_topright, box_botright,
				team_color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_botright, box_botleft,
				team_color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_botleft, box_topleft,
				team_color);

			//
			// render text
			//

			if (!esp_player_name.get_value())
			{
				continue;
			}

			auto scaled_font = FONT_SMALLEST;

			auto playername_font = g_gui_fontmgr_i->get_font("proggyclean", scaled_font, FONTDEC_Regular);

			const char* label_text = plr.get_playerinfo()->name;
			if (!label_text)
			{
				label_text = "null";
			}
			auto label_size = g_gui_fontmgr_i->calc_font_text_size(playername_font, label_text);

			float label_padding = 1.0f + 1.0f; // 1 for the box outline thickness
			Vector2D label_topleft = { box_botright.x - (box_wide_half + label_size.x / 2), box_botright.y + label_padding };

			g_gui_window_rendering_i->render_text_with_background(
				g_gui_window_rendering_i->get_current_drawlist(),
				playername_font,
				label_topleft,
				CColor(255, 255, 255, 255),
				label_text);
		}
	}
}

void CESP::render_entities()
{
	for (auto& [index, ent] : CEntityMgr::the().m_known_entities)
	{
		if (!ent.is_valid())
		{
			continue;
		}

		auto& model = ent.associated_model();
		auto cl_ent = ent.cl_entity();

		if (ent.is_out_of_update_for(1.0f) || !model.is_valid())
		{
			continue;
		}

		if (cl_ent->origin.IsZero())
		{
			continue;
		}

		auto bbox_min = ent.get_bounding_box_min();
		auto bbox_max = ent.get_bounding_box_max();

		if (bbox_min.IsZero())
		{
			bbox_min.z = -5.0f;
		}

		if (bbox_max.IsZero())
		{
			bbox_max.z = 5.0f;
		}

		Vector2D box_top2d, box_bot2d;
		Vector box_top3d = cl_ent->origin + Vector(0.0f, 0.0f, bbox_max.z); // add a little bit of height
		Vector box_bot3d = cl_ent->origin + Vector(0.0f, 0.0f, bbox_min.z);
		if (CGameUtil::the().world_to_screen(box_top3d, box_top2d) &&
			CGameUtil::the().world_to_screen(box_bot3d, box_bot2d))
		{
			float box_tall_half = box_bot2d.y - box_top2d.y;
			float box_wide_half = box_tall_half; // tweaked ration to perfection

			Vector2D box_topleft = { box_top2d[0] - box_wide_half, box_top2d[1] };
			Vector2D box_topright = { box_top2d[0] + box_wide_half, box_top2d[1] };
			Vector2D box_botleft = { box_bot2d[0] - box_wide_half, box_bot2d[1] };
			Vector2D box_botright = { box_bot2d[0] + box_wide_half, box_bot2d[1] };

			auto color = CColor(199, 21, 133, 230);

			//
			// render 4 corners using lines. this fixes visual deformation of the box when faced on the far side of fisheye effect.
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_topleft, box_topright,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_topright, box_botright,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_botright, box_botleft,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				box_botleft, box_topleft,
				color);

			//
			// render text
			//

			auto scaled_font = FONT_SMALLEST;

			auto playername_font = g_gui_fontmgr_i->get_font("proggyclean", scaled_font, FONTDEC_Regular);

			const char* label_text = model.hl_model()->name;
			if (!label_text)
			{
				label_text = "null";
			}
			auto label_size = g_gui_fontmgr_i->calc_font_text_size(playername_font, label_text);

			float label_padding = 1.0f + 1.0f; // 1 for the box outline thickness
			Vector2D label_topleft = { box_botright.x - (box_wide_half + label_size.x / 2), box_botright.y + label_padding };

			g_gui_window_rendering_i->render_text_with_background(
				g_gui_window_rendering_i->get_current_drawlist(),
				playername_font,
				label_topleft,
				CColor(255, 255, 255, 255),
				label_text);
		}
	}
}

void CESP::render_sound()
{
	float max = esp_sound_interval.get_value();

	if (!m_stepsounds.empty())
	{
		auto& last_sound = m_stepsounds.front();

		if (last_sound.get_life_duration() > max)
		{
			m_stepsounds.pop_front();
		}
	}

	for (const auto& step : m_stepsounds)
	{
		if (esp_sound_filter_local.get_value() && CGameUtil::the().is_local_player(step.entid))
		{
			continue;
		}

		if (esp_only_enemy_team.get_value() && !CGameUtil::the().is_player_on_enemy_team(step.entid))
		{
			continue;
		}

		Vector2D screen;
		if (CGameUtil::the().world_to_screen(step.origin, screen))
		{
			float life_dur = step.get_life_duration();
			if (life_dur == 0)
			{
				// assert there's no division by 0
				life_dur = 1.0f / max;
			}

#if 0
			auto font = g_gui_fontmgr_i->get_font("proggyclean", FONT_SMALL, FONTDEC_Regular);
			g_gui_window_rendering_i->render_text_with_background(
					g_gui_window_rendering_i->get_current_drawlist(),
					font,
					screen,
					CColor(255, 255, 255, 255),
					"step");
#else
			float current_ratio = (life_dur / max); // get 0-1 ratio
			int animated_alpha = (int)(255.0f - current_ratio * 255.0f);
			float animated_scale = 5.0f + (current_ratio * 10.0f);
			float animated_rouding = 1.0f + (current_ratio * 20.0f);

			CColor step_color = CColor(230, 230, 230);

			if (CGameUtil::the().is_player_index(step.entid))
			{
				auto& player = CEntityMgr::the().m_known_players[step.entid];

				if (player.is_valid())
				{
					step_color = player.get_color_based_on_team();
				}
			}

			g_gui_window_rendering_i->render_box_outline(
				g_gui_window_rendering_i->get_current_drawlist(), 
				screen - Vector2D(animated_scale, animated_scale), 
				screen + Vector2D(animated_scale, animated_scale), 
				CColor(), 
				animated_rouding, 
				CColor(step_color.r, step_color.g, step_color.b, animated_alpha / 255.0f), 1.5f);
#endif
		}
	}
}

EFontSize CESP::fontsize_by_dist(float dist, float max_dist)
{
	// get [0, 1] ratio
	float dist_10 = dist / max_dist;
	float limit = dist_10 * (float)FONT_SIZE_COUNT;

	auto result = (EFontSize)(FONT_EXTRA - (EFontSize)std::round(limit));

	return std::clamp(result, FONT_SMALLEST, FONT_EXTRA);
}
