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

VarBoolean movement_visual_info("movement_visual_info", "Displays movement-related information", false);
VarBoolean movement_visual_info_keys("movement_visual_info_keys", "Displays movement-related key information", false);
VarFloat movement_visual_info_keys_offset("movement_visual_info_keys_offset", "Vertical offset of showkeys", 1.2f, 0.1f, 1.9f);
VarBoolean movement_visual_info_background("movement_visual_info_background", "Enables background on movement-related information", false);

static CColor dynamic_color(float v, float m)
{
	return { 
		abs(cosf(v / m)),
		1.0f, 
		abs(cosf(v / m)),
		1.0f };
}

static CColor dynamic_color_clamped(float v, float threshold, float max)
{
	return { 
		v < threshold ? abs(cosf(v / max)) : 0.0f,
		1.0f, 
		v < threshold ? abs(cosf(v / max)) : 0.0f,
		1.0f };
}

void CMovementVisualInfo::render()
{

	m_background = movement_visual_info_background.get_value();

	if (movement_visual_info.get_value())
	{
		render_values();
	}

	if (movement_visual_info_keys.get_value())
	{
		render_keys();
	}
}

void CMovementVisualInfo::render_values()
{
	auto& font = CEngineFontRendering::the().console_font();

	Vector2D screen_pos = CVideoModeUtil::the().get_ingame_viewport_pos();
	Vector2D screen_size = CVideoModeUtil::the().get_real_screen_size();
	if (screen_size.IsZero())
	{
		return;
	}

	// middle bottom
	m_rendering_pos = { screen_size.x * 0.5f, screen_size.y * 0.7f };

	float velocity = CLocalState::the().get_local_velocity_2d();
	float fall_velocity = CLocalState::the().get_fall_velocity_abs();
	float ground_dist = CLocalState::the().get_ground_dist();
	float ground_angle = CLocalState::the().get_ground_angle();
	float edge_distance = CLocalState::the().get_edge_dist();

	render_unit(font, dynamic_color_clamped(velocity, 300.0f, 290.0f), "Velocity: ", std::format("{:0.3f}", velocity), " u/s");
	render_unit(font, dynamic_color(fall_velocity, 290.0f), "Fall velocity: ", std::format("{:0.3f}", fall_velocity), " u/s");
	render_unit(font, dynamic_color(ground_dist, 500.0f), "Height: ", std::format("{:0.3f}", ground_dist), " units");
	render_unit(font, dynamic_color(ground_angle, 60.0f), "Ground angle: ", std::format("{:0.3f}", ground_angle), " °");
	render_unit(font, {
		(edge_distance > 0.0f && edge_distance < 20.0f) ? abs(cosf(1.0f - (edge_distance / 20.0f))) : 1.0f,
		1.0f,
		(edge_distance > 0.0f && edge_distance < 20.0f) ? abs(cosf(1.0f - (edge_distance / 20.0f))) : 1.0f,
		1.0f }, "Edge distance: ", std::format("{:0.3f}", edge_distance), " units");
}

void CMovementVisualInfo::render_keys()
{
	auto& font = CEngineFontRendering::the().console_font();

	Vector2D screen_pos = CVideoModeUtil::the().get_ingame_viewport_pos();
	Vector2D screen_size = CVideoModeUtil::the().get_real_screen_size();
	if (screen_size.IsZero())
	{
		return;
	}

	// sligtly offset to the bottom
	Vector2D screen_center = (screen_size / 2.0f);

	CColor white(230, 230, 230, 230);

	Vector2D dir_center_pos = { screen_center.x - 40.0f , screen_center.y * movement_visual_info_keys_offset.get_value()};
	Vector2D jumpduck_center_pos = { screen_center.x + 40.0f, screen_center.y * movement_visual_info_keys_offset.get_value() };

	if (CClientMovementPacket::the().is_in(IN_MOVELEFT))
	{
		render_text(font, dir_center_pos - Vector2D(10.0f, 0.0f), white, "<");
	}

	if (CClientMovementPacket::the().is_in(IN_MOVERIGHT))
	{
		render_text(font, dir_center_pos + Vector2D(10.0f, 0.0f), white, ">");
	}

#if 0 // shit looks ugly with ascii
	if (CClientMovementPacket::the().is_in(IN_FORWARD))
	{
		render_text(font, dir_center_pos - Vector2D(0.0f, 10.0f), white, "^");
	}

	if (CClientMovementPacket::the().is_in(IN_BACK))
	{
		render_text(font, dir_center_pos + Vector2D(0.0f, 10.0f), white, "v");
	}
#endif

	if (CClientMovementPacket::the().is_in(IN_JUMP))
	{
		render_text(font, jumpduck_center_pos - Vector2D(0.0f, 10.0f), white, "jump");
	}

	if (CClientMovementPacket::the().is_in(IN_DUCK))
	{
		render_text(font, jumpduck_center_pos + Vector2D(0.0f, 10.0f), white, "duck");
	}
}

void CMovementVisualInfo::render_unit(const EngineFont& font, const CColor& color, const std::string& label, const std::string& value, const std::string& unit)
{
	float total_width = (float)font.calc_text_width(label + value + unit);
	float x_start = m_rendering_pos.x;

	m_rendering_pos.x -= (total_width * 0.5f);

	m_rendering_pos.x += font.render_text(m_rendering_pos, m_background, label);
	m_rendering_pos.x += font.render_text_colored(m_rendering_pos, m_background, color, value);
	font.render_text(m_rendering_pos, m_background, unit);

	m_rendering_pos.x = x_start;
	m_rendering_pos.y += font.get_text_height() + 1;
}

void CMovementVisualInfo::render_text(const EngineFont& font, const Vector2D& pos, const CColor& color, const std::string& label)
{
	float total_width = (float)font.calc_text_width(label);
	float total_height = (float)font.get_text_height();
	font.render_text_colored(pos - Vector2D(total_width / 2.0f, total_height / 2.0f), m_background, color, label);
}