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
VarBoolean esp_box_outline("esp_box_outline", "Enables box outline", true);

VarBoolean esp_entity_enable("esp_entity_enable", "Toggles entity esp", false);

VarBoolean esp_player_enable("esp_player_enable", "Toggles player esp", false);
VarBoolean esp_player_name("esp_name", "Toggles player name esp", false); 
VarBoolean esp_player_enemy("esp_player_enemy", "Player esp for enemy", false); 
VarBoolean esp_player_teammates("esp_player_teammates", "Player esp for teammates", false); 
VarBoolean esp_player_details("esp_player_details", "Renders details about the player", false); 

VarBoolean esp_sound_enable("esp_sound_enable", "Toggles sound esp", false);
VarFloat esp_sound_interval("esp_sound_interval", "Time of which the esp node is alive", 1.0f, 0.5f, 5.0f);
VarBoolean esp_sound_filter_local("esp_sound_filter_local", "Doesn't apply sound esp on local player", true);
VarBoolean esp_sound_resolver("esp_sound_resolver", "Tries to resolve entity indexes when anti-cheat is used", true);
VarInteger esp_sound_resolver_distace("esp_sound_resolver_distace", "How far you want to resolve to", 64, 20, 100);
VarInteger esp_sound_type("esp_sound_type", "Sound ESP type", 0, 0, 2);

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

	if (CPanic::the().panicking())
	{
		return;
	}

	if (CLocalState::the().is_in_spectator_mapview())
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

	// experimental
	//render_bomb_info();
}

void CESP::render_players()
{
	for (auto& [index, player] : CEntityMgr::the().m_known_players)
	{
		render_player_esp(index, player);
	}
}

void CESP::render_entities()
{
	for (auto& [index, entity] : CEntityMgr::the().m_known_entities)
	{
		render_entity_esp(index, entity);
	}
}

void CESP::render_sound()
{
	uint32_t time_limit = ((uint32_t)esp_sound_interval.get_value()) * 1000;

	CSoundEsp::the().update(time_limit);

	for (const auto& step : CSoundEsp::the().m_stepsounds)
	{
		render_sound_esp(step, time_limit);
	}
}

void CESP::render_bomb_info()
{
	auto& bomb_info = CEntityMgr::the().get_bomb_info();

	if (bomb_info.m_flag == BOMB_FLAG_DROPPED)
	{
		render_dropped_bomb_esp(bomb_info);
	}
	else if (bomb_info.m_flag == BOMB_FLAG_PLANTED)
	{
		render_planted_bomb_esp(bomb_info);
	}
}

bool CESP::render_player_esp(int index, const CGenericPlayer& player)
{
	// check if the player is valid
	if (!player.is_valid() || !player.is_alive() || player.is_out_of_update_for(1.0f) || player.is_local_player() || player.is_local_spectating())
	{
		return false;
	}

	auto cl_ent = player.cl_entity();
	if (cl_ent->origin.IsZero())
	{
		return false;
	}

#if 0
	auto& model = entity.associated_model();

	// see if model is valid
	if (!model.is_valid())
	{
		return false;
	}
#endif

	if (!decide_player_enemy(&esp_player_enemy, &esp_player_teammates, index))
	{
		return false;
	}

	ESPBoxMetrics metrics;
	if (origin_to_2d_box(cl_ent->origin, player.get_bounding_box_min(), player.get_bounding_box_max(), 
						 get_playerbox_ratio(&player), metrics))
	{
		render_esp_box(metrics, player.get_color_based_on_team());

		//
		// player name
		//

		if (esp_player_name.get_value())
		{
			const char* label_text = player.get_playerinfo()->name;
			if (!label_text)
			{
				label_text = "none";
			}

			render_esp_label(metrics, label_text);
		}

		//
		// details
		//

		if (esp_player_details.get_value())
		{
			render_esp_details(metrics, player);
		}
	}

	return true;
}

bool CESP::render_entity_esp(int index, const CGenericEntity& entity)
{
	// see if entity is valid
	if (!entity.is_valid() || entity.is_out_of_update_for(1.0f))
	{
		return false;
	}

	auto cl_ent = entity.cl_entity();

	ESPBoxMetrics metrics;
	if (origin_to_2d_box(cl_ent->origin, entity.get_bounding_box_min(), entity.get_bounding_box_max(), 1.0f, metrics))
	{
		render_esp_box(metrics, CColor(199, 21, 133, 230), 2.5f);

		//
		// render text
		//

		const char* label_text = "none";

		auto& model = entity.associated_model();
		if (model.is_valid())
		{
			auto hl_model = model.hl_model();
			if (hl_model->name)
			{
				label_text = hl_model->name;
			}
		}

		render_esp_label(metrics, label_text);
	}

	return true;
}

bool CESP::render_sound_esp(const PlayerStepSound& step, uint32_t time_limit)
{
	if (esp_sound_filter_local.get_value() && CGameUtil::the().is_local_player(step.entid))
	{
		return false;
	}

	if (!decide_player_enemy(&esp_player_enemy, &esp_player_teammates, step.entid))
	{
		return false;
	}

	if (step.origin.IsZero())
	{
		return false;
	}

	uint32_t life_dur = step.get_life_duration_ms();
	if (life_dur == 0)
	{
		// assert there's no division by 0
		life_dur = 1;
	}

	float current_ratio = ((float)life_dur / time_limit); // get 0-1 ratio
	int animated_alpha = (int)(255.0f - current_ratio * 255.0f);
	float animated_scale = 1.0f + (current_ratio * 10.0f);
	float animated_rouding = 1.0f + (current_ratio * 20.0f);

	CColor step_color = CColor(230, 230, 230);

	if (CGameUtil::the().is_player_index(step.entid))
	{
		auto player = CEntityMgr::the().get_player_by_id(step.entid);

		if (player && (*player)->is_valid())
		{
			step_color = (*player)->get_color_based_on_team();
		}
	}

	int type = esp_sound_type.get_value();

	Vector ground_origin = step.origin - Vector(0, 0, 36.0f);

	Vector2D screen;
	if (CGameUtil::the().world_to_screen(ground_origin, screen))
	{
		if (type == 0) // 2d
		{
			render_circle_with_outline(
				screen, animated_scale * 1.7f, 16,
				CColor(step_color.r, step_color.g, step_color.b, animated_alpha / 255.0f),
				1.5f);
		}
	}

	//
	// cases rendered outside of the worldtoscreen function
	//
	if (type == 1) // 3d
	{
		render_space_rotated_circle_with_outline(
			ground_origin, animated_scale * 2.0f, 32,
			CColor(step_color.r, step_color.g, step_color.b, animated_alpha / 255.0f),
			1.5f);
	}
	else if (type == 2)
	{
		//
		// render player esp on stepsound origin
		//

		auto player = CEntityMgr::the().get_player_by_id(step.entid);
		if (!player)
		{
			return false;
		}

		// only if the player is out of pvs
		if (!(*player)->is_out_of_update_for(1.0f))
		{
			return true;
		}

		ESPBoxMetrics metrics;
		if (origin_to_2d_box(step.origin, (*player)->get_default_bounding_box_min(), 
							 (*player)->get_default_bounding_box_max(), get_playerbox_ratio(*player), metrics))
		{
			render_esp_box(metrics, (*player)->get_color_based_on_team());

			//
			// player name
			//

			if (esp_player_name.get_value())
			{
				const char* label_text = (*player)->get_playerinfo()->name;
				if (!label_text)
				{
					label_text = "none";
				}

				render_esp_label(metrics, label_text);
			}
		}
	}

	return true;
}

bool CESP::decide_player_enemy(VarBoolean* player_enemy, VarBoolean* player_teammates, int index)
{
	int enemy_st = CGameUtil::the().is_player_on_enemy_team(index);

	// filter error cases
	switch (enemy_st)
	{
		case -3: // player is unassigned or spectator
		{
			return false;
		}
		case -2: // local is unassigned or spectator
		case -1: // could not get player or local
		{
			return true;
		}
	}

	// check for player team
	if (!player_enemy->get_value() && enemy_st == 1)
	{
		return false;
	}

	if (!player_teammates->get_value() && enemy_st == 0)
	{
		return false;
	}

	return true; // should not happen
}

float CESP::get_playerbox_ratio(const CGenericPlayer* player)
{
	if (player->is_ducking())
	{
		return 1.0f / 2.6f;
	}
	else
	{
		return 1.0f / 4.6f;
	}
}

bool CESP::render_dropped_bomb_esp(const BombInfo& bomb_info)
{
	CConsole::the().info("{}", bomb_info.m_origin);

	ESPBoxMetrics metrics;
	if (origin_to_2d_box(bomb_info.m_origin, DROPPED_BOMB_BBOX_MIN, DROPPED_BOMB_BBOX_MAX, 1.0f, metrics))
	{
		auto brownish = CColor(216, 161, 117, 230);
		render_esp_box(metrics, brownish);
	}

	return true;
}

bool CESP::render_planted_bomb_esp(const BombInfo& bomb_info)
{
	ESPBoxMetrics metrics;
	if (origin_to_2d_box(bomb_info.m_origin, DROPPED_BOMB_BBOX_MIN, DROPPED_BOMB_BBOX_MAX, 1.0f, metrics))
	{
		auto yellow = CColor(210, 230, 0, 230);
		render_esp_box(metrics, yellow);
	}

	return true;
}

//------------------------------------------------------------------------------------------------

bool CESP::origin_to_2d_box(const Vector& origin, const Vector& min, const Vector& max, float box_aspect, ESPBoxMetrics& metrics)
{
	// filter out invalid origins
	if (origin.IsZero())
	{
		return false;
	}

	auto bbox_min = min;
	auto bbox_max = max;

	// correct bounding boxes
	if (bbox_min.IsZero()) bbox_min.z = -5.0f;
	if (bbox_max.IsZero()) bbox_max.z = 5.0f;

	Vector box_top3d = origin + Vector(0.0f, 0.0f, bbox_max.z); // add a little bit of height
	Vector box_bot3d = origin + Vector(0.0f, 0.0f, bbox_min.z);

	Vector2D box_top2d, box_bot2d;
	if (!CGameUtil::the().world_to_screen(box_top3d, box_top2d) || !CGameUtil::the().world_to_screen(box_bot3d, box_bot2d))
	{
		return false;
	}

	// add the artifical aspect ratio, if we set one
	if (aspect_ratio_enable.get_value())
	{
		box_aspect *= 1.0f / aspect_ratio_value.get_value();
	}

	metrics.box_tall_half = box_bot2d.y - box_top2d.y;
	metrics.box_wide_half = metrics.box_tall_half * box_aspect;

	metrics.box_topleft = { box_top2d[0] - metrics.box_wide_half, box_top2d[1] };
	metrics.box_topright = { box_top2d[0] + metrics.box_wide_half, box_top2d[1] };
	metrics.box_botleft = { box_bot2d[0] - metrics.box_wide_half, box_bot2d[1] };
	metrics.box_botright = { box_bot2d[0] + metrics.box_wide_half, box_bot2d[1] };

	return true;
}

void CESP::render_esp_box(const ESPBoxMetrics& metrics, const CColor& color, float corner_ratio)
{
	render_box_for_four_points(metrics.box_topleft, metrics.box_topright, metrics.box_botright, metrics.box_botleft, color, metrics.box_tall_half, corner_ratio);
}

void CESP::render_esp_label(const ESPBoxMetrics& metrics, const std::string& label)
{
	auto text_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_10px, FDC_Bold);
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(text_font, label.c_str());

	float label_padding = 1.0f + 1.0f; // 1 for the box outline thickness
	Vector2D label_bottom_centered = { metrics.box_botright.x - (metrics.box_wide_half + label_size.x / 2), metrics.box_botright.y + label_padding };

	g_gui_window_rendering_i->render_text_with_background(
		g_gui_window_rendering_i->get_current_drawlist(),
		text_font,
		label_bottom_centered,
		CColor(255, 255, 255, 255),
		label.c_str());
}

void CESP::render_esp_details(const ESPBoxMetrics& metrics, const CGenericPlayer& player)
{
	auto text_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_10px, FDC_Bold);

	std::string label = "";

	label += std::format("id: {}\n", player.cl_entity()->index);
	label += std::format("mins/maxs: {} {}\n", player.get_bounding_box_min(), player.get_bounding_box_max());
	label += std::format("movetype: {}\n", player.cl_entity()->curstate.movetype);
//	label += std::format("activity: {}\n", player.);

	Vector2D label_top_right = { metrics.box_topright.x + 3.0f, metrics.box_topright.y };

	g_gui_window_rendering_i->render_text_with_background(
		g_gui_window_rendering_i->get_current_drawlist(),
		text_font,
		label_top_right,
		CColor(255, 255, 255, 255),
		label.c_str());
}

void CESP::render_box_for_four_points(const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right,
									  const Vector2D& bottom_left, const CColor& color, float box_tall_half, float corner_ratio)
{
	//
	// render 4 corners using lines. this fixes visual deformation of the box when faced on the far side of fisheye effect.
	//

	switch (esp_box_type.get_value())
	{
		case 0: // normal
		{
			render_line_with_outline(top_left, top_right, color);
			render_line_with_outline(top_right, bottom_right, color);
			render_line_with_outline(bottom_right, bottom_left, color);
			render_line_with_outline(bottom_left, top_left, color);

			break;
		}
		case 1: // corner
		{
			float length = box_tall_half * 0.1 * corner_ratio; // px
			length = std::clamp(length, 1.0f, 50.0f);

			// top left
			render_line_with_outline(top_left + Vector2D(0.0f, length), top_left, color);
			render_line_with_outline(top_left, top_left + Vector2D(length, 0.0f), color);

			// top right
			render_line_with_outline(top_right - Vector2D(length, 0.0f), top_right, color);
			render_line_with_outline(top_right, top_right + Vector2D(0.0f, length), color);

			// bottom right
			render_line_with_outline(bottom_right - Vector2D(length, 0.0f), bottom_right, color);
			render_line_with_outline(bottom_right, bottom_right - Vector2D(0.0f, length), color);

			// bottom left
			render_line_with_outline(bottom_left + Vector2D(length, 0.0f), bottom_left, color);
			render_line_with_outline(bottom_left, bottom_left - Vector2D(0.0f, length), color);

			break;
		}
	}

	//
	// render box background
	//

	if (esp_background.get_value())
	{
		auto bg_color = color;
		bg_color.a *= 0.2f;
		g_gui_window_rendering_i->render_quad(
			g_gui_window_rendering_i->get_current_drawlist(),
			top_left, top_right, bottom_right, bottom_left,
			bg_color);
	}
}

void CESP::render_line_with_outline(const Vector2D& from, const Vector2D& to, const CColor& color, float thicc)
{
	if (esp_box_outline.get_value())
	{
		auto c = s_outline_color; // to preserve the alpha, in case it changes
		c.a = color.a;
		g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(), from, to, c, thicc + 0.5f);
	}
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(), from, to, color, thicc);
}

void CESP::render_circle_with_outline(const Vector2D& center, float radius, int num_segments, const CColor& color, float thicc)
{
	if (esp_box_outline.get_value())
	{
		auto c = s_outline_color; // to preserve the alpha, in case it changes
		c.a = color.a;
		g_gui_window_rendering_i->render_circle(g_gui_window_rendering_i->get_current_drawlist(), center, radius, num_segments, c, thicc + 0.5f);
	}
	g_gui_window_rendering_i->render_circle(g_gui_window_rendering_i->get_current_drawlist(), center, radius, num_segments, color, thicc);
}

void CESP::render_space_rotated_circle_with_outline(const Vector& origin, float radius, int num_segments, const CColor& color, float thicc)
{
	const float stepp = std::numbers::pi * 2.0f / num_segments;

	for (float angle = 0; angle < (std::numbers::pi * 2.0f); angle += stepp)
	{
		Vector vStart(radius * cosf(angle) + origin.x, radius * sinf(angle) + origin.y, origin.z);
		Vector vEnd(radius * cosf(angle + stepp) + origin.x, radius * sinf(angle + stepp) + origin.y, origin.z);

		Vector2D vScreenFrom, vScreenTo;
		if (CGameUtil::the().world_to_screen(vStart, vScreenFrom) && CGameUtil::the().world_to_screen(vEnd, vScreenTo))
		{
			//auto c = s_outline_color; // to preserve the alpha, in case it changes
			//c.a = color.a;
			//render_line_with_outline(ImVec2(vScreenFrom.x + 1.f, vScreenFrom.y + 1.f),
			//						 ImVec2(vScreenTo.x + 1.f, vScreenTo.y + 1.f),
			//						 c, thicc);
			render_line_with_outline(ImVec2(vScreenFrom.x, vScreenFrom.y),
									 ImVec2(vScreenTo.x, vScreenTo.y),
									 color, thicc);
		}
	}
}

//---------------------------------------------------------------------------------------

void CSoundEsp::register_player_step(const Vector& sound_origin, int entid)
{
	if (sound_origin.IsZero())
	{
		CConsole::the().warning("Got null origin for sound generated by: {} entity", entid);
	}

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
