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
VarBoolean esp_background("esp_background", "Enables background on the box", true);
VarInteger esp_box_type("esp_box_type", "Type of the esp box (box, corners, etc.)", 1, 0, 1);
VarBoolean esp_only_enemy_team("esp_only_enemy_team", "Esp only on enemy team", false);

VarBoolean esp_entity_enable("esp_entity_enable", "Toggles entity esp", false);

VarBoolean esp_player_enable("esp_player_enable", "Toggles player esp", false);
VarBoolean esp_player_name("esp_name", "Toggles player name esp", false); 

VarBoolean esp_sound_enable("esp_sound_enable", "Toggles sound esp", false);
VarFloat esp_sound_interval("esp_sound_interval", "Time of which the esp node is alive", 1.0f, 0.5f, 5.0f);
VarBoolean esp_sound_filter_local("esp_sound_filter_local", "Doesn't apply sound esp on local player", true);
VarBoolean esp_sound_resolver("esp_sound_resolver", "Tries to resolve entity indexes when anti-cheat is used", true);
VarInteger esp_sound_resolver_distace("esp_sound_resolver_distace", "How far you want to resolve to", 64, 20, 100);

void CESP::initialize_gui()
{
	COxWareUI::the().add_background_rendering_code([this]() { on_render(); });
	COxWareUI::the().add_background_rendering_constrain(
		[this]() 
		{
			return esp_enable.get_value() && 
				((esp_player_enable.get_value() && !CEntityMgr::the().m_known_players.empty()) ||
				 (esp_entity_enable.get_value() && !CEntityMgr::the().m_known_entities.empty()) ||
				 (esp_sound_enable.get_value() && !CSoundEsp::the().m_stepsounds.empty()));
		});
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
	for (auto& [index, player] : CEntityMgr::the().m_known_players)
	{
		// check if the player is valid
		if (!player.is_valid() || !player.is_alive() || player.is_out_of_update_for(1.0f) || player.is_local_player())
		{
			continue;
		}

		auto& model = player.associated_model();

		// check if model is valid
		if (!model.is_valid())
		{
			continue;
		}

		// check for player team
		if (esp_only_enemy_team.get_value() && !CGameUtil::the().is_player_on_enemy_team(index))
		{
			continue;
		}

		auto cl_ent = player.cl_entity();

		// filter out invalid origins
		if (cl_ent->origin.IsZero())
		{
			continue;
		}

		auto plr_info = player.get_playerinfo();

		Vector2D box_top2d, box_bot2d;
		Vector box_top3d = cl_ent->origin + Vector(0.0f, 0.0f, player.get_bounding_box_max().z); // add a little bit of height
		Vector box_bot3d = cl_ent->origin + Vector(0.0f, 0.0f, player.get_bounding_box_min().z);
		if (CGameUtil::the().world_to_screen(box_top3d, box_top2d) &&
			CGameUtil::the().world_to_screen(box_bot3d, box_bot2d))
		{
			//box_top2d.x += 240;
			//box_bot2d.x += 240;

			float box_tall_half = box_bot2d.y - box_top2d.y;
			float box_wide_half = box_tall_half / (4.6f/* * (player.is_ducking() ? 2 : 1)*/); // tweaked ration to perfection

 			Vector2D box_topleft = { box_top2d[0] - box_wide_half, box_top2d[1] };
			Vector2D box_topright = { box_top2d[0] + box_wide_half, box_top2d[1] };
			Vector2D box_botleft = { box_bot2d[0] - box_wide_half, box_bot2d[1] };
			Vector2D box_botright = { box_bot2d[0] + box_wide_half, box_bot2d[1] };

			auto team_color = player.get_color_based_on_team();

			render_box_for_four_points(box_topleft, box_topright, box_botright, box_botleft, team_color, box_tall_half);

			//
			// render text
			//

			if (esp_player_name.get_value())
			{
				auto text_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FontSize::UIText.small(), FDC_Bold);

				const char* label_text = player.get_playerinfo()->name;
				if (!label_text)
				{
					label_text = "none";
				}
				auto label_size = g_gui_fontmgr_i->calc_font_text_size(text_font, label_text);

				float label_padding = 1.0f + 1.0f; // 1 for the box outline thickness
				Vector2D label_topleft = { box_botright.x - (box_wide_half + label_size.x / 2), box_botright.y + label_padding };

				g_gui_window_rendering_i->render_text_with_background(
					g_gui_window_rendering_i->get_current_drawlist(),
					text_font,
					label_topleft,
					CColor(255, 255, 255, 255),
					label_text);
			}
		}
	}
}

void CESP::render_entities()
{
	for (auto& [index, entity] : CEntityMgr::the().m_known_entities)
	{
		// see if entity is valid
		if (!entity.is_valid() || entity.is_out_of_update_for(1.0f))
		{
			continue;
		}

		auto& model = entity.associated_model();

		// see if model is valid
		if (!model.is_valid())
		{
			continue;
		}

		auto cl_ent = entity.cl_entity();

		// filter out invalid origins
		if (cl_ent->origin.IsZero())
		{
			continue;
		}

		auto bbox_min = entity.get_bounding_box_min();
		auto bbox_max = entity.get_bounding_box_max();

		// correct bounding boxes
		if (bbox_min.IsZero()) bbox_min.z = -5.0f;
		if (bbox_max.IsZero()) bbox_max.z = 5.0f;

		Vector2D box_top2d, box_bot2d;
		Vector box_top3d = cl_ent->origin + Vector(0.0f, 0.0f, bbox_max.z); // add a little bit of height
		Vector box_bot3d = cl_ent->origin + Vector(0.0f, 0.0f, bbox_min.z);
		if (CGameUtil::the().world_to_screen(box_top3d, box_top2d) &&
			CGameUtil::the().world_to_screen(box_bot3d, box_bot2d))
		{
			float box_tall_half = box_bot2d.y - box_top2d.y;
			float box_wide_half = box_tall_half;

			Vector2D box_topleft = { box_top2d[0] - box_wide_half, box_top2d[1] };
			Vector2D box_topright = { box_top2d[0] + box_wide_half, box_top2d[1] };
			Vector2D box_botleft = { box_bot2d[0] - box_wide_half, box_bot2d[1] };
			Vector2D box_botright = { box_bot2d[0] + box_wide_half, box_bot2d[1] };

			// pinkish color
			auto color = CColor(199, 21, 133, 230);

			render_box_for_four_points(box_topleft, box_topright, box_botright, box_botleft, color, box_tall_half);

			//
			// render text
			//

			const char* label_text = model.hl_model()->name;
			if (!label_text)
			{
				label_text = "none";
			}
			auto text_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FontSize::UIText.small(), FDC_Bold);
			auto label_size = g_gui_fontmgr_i->calc_font_text_size(text_font, label_text);

			float label_padding = 1.0f + 1.0f; // 1 for the box outline thickness
			Vector2D label_topleft = { box_botright.x - (box_wide_half + label_size.x / 2), box_botright.y + label_padding };

			g_gui_window_rendering_i->render_text_with_background(
				g_gui_window_rendering_i->get_current_drawlist(),
				text_font,
				label_topleft,
				CColor(255, 255, 255, 255),
				label_text);
		}
	}
}

void CESP::render_sound()
{
	uint32_t time_limit = ((uint32_t)esp_sound_interval.get_value()) * 1000;

	CSoundEsp::the().update(time_limit);

	for (const auto& step : CSoundEsp::the().m_stepsounds)
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
			uint32_t life_dur = step.get_life_duration_ms();
			if (life_dur == 0)
			{
				// assert there's no division by 0
				life_dur = 1.0f / time_limit;
			}

			float current_ratio = (life_dur / time_limit); // get 0-1 ratio
			int animated_alpha = (int)(255.0f - current_ratio * 255.0f);
			float animated_scale = 5.0f + (current_ratio * 10.0f);
			float animated_rouding = 1.0f + (current_ratio * 20.0f);

			CColor step_color = CColor(230, 230, 230);

			if (CGameUtil::the().is_player_index(step.entid))
			{
				auto player = CEntityMgr::the().get_player_by_id(step.entid);

				if (player && player->is_valid())
				{
					step_color = player->get_color_based_on_team();
				}
			}

			g_gui_window_rendering_i->render_box_outline(
				g_gui_window_rendering_i->get_current_drawlist(), 
				screen - Vector2D(animated_scale, animated_scale), 
				screen + Vector2D(animated_scale, animated_scale), 
				CColor(), 
				animated_rouding, 
				CColor(step_color.r, step_color.g, step_color.b, animated_alpha / 255.0f), 1.5f);
		}
	}
}

void CESP::render_box_for_four_points(const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right, 
									  const Vector2D& bottom_left, const CColor& color, float box_tall_half)
{
	//
	// render 4 corners using lines. this fixes visual deformation of the box when faced on the far side of fisheye effect.
	//

	switch (esp_box_type.get_value())
	{
		case 0: // normal
		{
			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_left, top_right,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_right, bottom_right,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_right, bottom_left,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_left, top_left,
				color);

			break;
		}
		case 1: // corner
		{
			float length = box_tall_half * 0.1; // px
			length = std::clamp(length, 1.0f, 50.0f);

			//
			// top left
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_left + Vector2D(0.0f, length), top_left,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_left, top_left + Vector2D(length, 0.0f),
				color);

			//
			// top right
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_right - Vector2D(length, 0.0f), top_right,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				top_right, top_right + Vector2D(0.0f, length),
				color);

			//
			// bottom right
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_right - Vector2D(length, 0.0f), bottom_right,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_right, bottom_right - Vector2D(0.0f, length),
				color);

			//
			// bottom left
			//

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_left + Vector2D(length, 0.0f), bottom_left,
				color);

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				bottom_left, bottom_left - Vector2D(0.0f, length),
				color);

			break;
		}
	}

	//
	// render box background
	//

	if (esp_background.get_value())
	{
		auto bg_color = color;
		bg_color.a *= 0.2;
		g_gui_window_rendering_i->render_quad(
			g_gui_window_rendering_i->get_current_drawlist(),
			top_left, top_right, bottom_right, bottom_left,
			bg_color);
	}
}

//---------------------------------------------------------------------------------------

void CSoundEsp::register_player_step(const Vector& sound_origin, int entid)
{
	if (esp_sound_resolver.get_value())
	{
		resolve_sound_entid(sound_origin, entid);
	}

	m_stepsounds.push_back({ sound_origin, entid, GetTickCount() });
}

void CSoundEsp::update(uint32_t time_limit)
{
	if (!m_stepsounds.empty())
	{
		auto& last_sound = m_stepsounds.front();

		if (last_sound.get_life_duration_ms() > time_limit)
		{
			m_stepsounds.pop_front();
		}
	}
}

void CSoundEsp::reset_cache()
{
	CConsole::the().info("Reseting sound esp resolver cache... ({} entries)", m_resolver_cache.bucket_count());
	m_resolver_cache.clear();
}

void CSoundEsp::resolve_sound_entid(const Vector& sound_origin, int& entid)
{
	if (CGameUtil::the().is_player_index(entid))
	{
		return; // valid
	}

	// try to lookup the cache
	auto it = m_resolver_cache.find(entid);
	if (it != m_resolver_cache.end())
	{
		//CConsole::the().info("grabbed from cache {} to {}", entid, (*it).second.resolved_index);
		entid = (*it).second.resolved_index;
		return; // found a match!
	}

	// some servers have anticheat for sound-esp that they basically send invalid entid when a stepsound is
	// made, hence confusing our team-recognition code. 
	for (auto& [index, plr] : CEntityMgr::the().m_known_players)
	{
		auto ent = plr.cl_entity();
		if (!ent)
		{
			continue;
		}

		if (ent->curstate.origin.Distance(sound_origin) < esp_sound_resolver_distace.get_value())
		{
			int unresolved = entid;
			entid = index;
			m_resolver_cache[unresolved] = index;
			//CConsole::the().info("resolved {} to {}", unresolved, index);
			return;
		}
	}

	//CConsole::the().error("Failed to resolve: {}", entid);
}
