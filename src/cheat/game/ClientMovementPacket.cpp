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
// ClientMovementPacket.cpp -- Class that modifies movement packets sent to the server.
//

#include "precompiled.h"

struct in_button_t
{
	const char* in_cmd, *out_cmd;
};

static std::map<unsigned short, in_button_t> s_in_commands =
{
	{
	{ IN_ATTACK,	{ "+attack",	"-attack" } },
	{ IN_JUMP,		{ "+jump",		"-jump" } },
	{ IN_DUCK,		{ "+duck",		"-duck" } },
	{ IN_FORWARD,	{ "+forward",	"-forward" } },
	{ IN_BACK,		{ "+back",		"-back" } },
	{ IN_USE,		{ "+use",		"-use" } },
//	{ IN_CANCEL,	{ "",			"" } },
	{ IN_LEFT,		{ "+left",		"-left" } },
	{ IN_RIGHT,		{ "+right",		"-right" } },
	{ IN_MOVELEFT,	{ "+moveleft",	"-moveleft" } },
	{ IN_MOVERIGHT, { "+moveright",	"-moveright" } },
	{ IN_ATTACK2,	{ "+attack2",	"+attack2" } },
//	{ IN_RUN,		{ "",			"" } },
	{ IN_RELOAD,	{ "+reload",	"-reload" } },
	{ IN_ALT1,		{ "+alt1",		"-alt1" } },
	{ IN_SCORE,		{ "+score",		"-score" } },
	}
};

void CClientMovementPacket::update_msg_writeusercmd(hl::usercmd_t* to)
{
	// FrameTime scheduled to be modified from somewhere else
	if (m_ft_modified && m_new_ft != -1)
	{
		to->msec = m_new_ft;
	}
}

void CClientMovementPacket::update_clientmove(hl::usercmd_t* cmd)
{
	reset_ft_state();

	set_current_cmd_for_manipulation(cmd);
}

//-----------------------------------------------------------------

bool CClientMovementPacket::set_button_bit_atomic(unsigned short button)
{
	bool set = false;

	if (!was_in(button))
	{
		set_button_bit(button, true);
		set = true;
	}
	else
	{
		// previously was in action but didn't release it.
		set_button_bit(button, false);
	}

	return set;
}

void CClientMovementPacket::set_button_bit(unsigned short button, bool set)
{
	if (set)
	{
		m_current_cmd->buttons |= button;
		CGameUtil::the().record_hud_command(s_in_commands[button].in_cmd);
	}
	else
	{
		m_current_cmd->buttons &= ~button;
		CGameUtil::the().record_hud_command(s_in_commands[button].out_cmd);
	}

	m_previous_buttons_state = m_current_cmd->buttons;
}

bool CClientMovementPacket::is_moving()
{
	return is_in(IN_FORWARD) || is_in(IN_BACK) || is_in(IN_MOVELEFT) || is_in(IN_MOVERIGHT);
}

void CClientMovementPacket::svside_movement_speed_factor(uint8_t msec, bool override_previous)
{
	if (m_ft_modified && !override_previous)
	{
		// already set before, don't override unless told to do so.
		return;
	}

	// we need to modify the msec afterwards (it only takes effect if we do that from MSG_WriteUserCmd)
	m_new_ft = msec;
	m_ft_modified = true;
}

void CClientMovementPacket::reset_ft_state()
{
	m_new_ft = -1;
	m_ft_modified = false;
}

//-----------------------------------------------------------------------------------------------------

VarBoolean movement_plot("movement_plot", "Enables visualization plot for movement", false);
VarInteger movement_plot_row_height("movement_plot_row_height", "Height of each row", 30, 10, 100);
VarInteger movement_plot_scale("movement_plot_scale", "Overall scale of visualized data", 1, 1, 5);
VarBoolean movement_plot_stop("movement_plot_stop", "Stops collection of data when on", false);

void CClientMovementPacketPlot::initialize_gui()
{
	COxWareUI::the().add_background_rendering_code([this]() { on_render(); });
	COxWareUI::the().add_background_rendering_constrain(
		[this]()
		{
			return true;
		});
}

void CClientMovementPacketPlot::on_render()
{
	if (!movement_plot.get_value())
	{
		return;
	}

	if (CAntiScreen::the().hide_visuals() || CPanic::the().panicking())
	{
		return;
	}

	auto screen_pos = CVideoModeUtil::the().get_ingame_viewport_pos();
	auto screen_size = CVideoModeUtil::the().get_real_screen_size();
	if (screen_size.IsZero())
	{
		return;
	}

	m_plot_base = { screen_pos.x, screen_size.y - 100 };

	static float y_offset = 0.0f;
	float data_entry_height_total = (float)movement_plot_row_height.get_value();
	float data_entry_width_total = 100.0f;

	float plotting_data_scale = (float)movement_plot_scale.get_value();

	m_max_x_data = (screen_size.x - data_entry_width_total) / plotting_data_scale;

	auto white = CColor(230, 230, 230, 230);
	auto white_dim = CColor(230, 230, 230, 100);

	// render background
	g_gui_window_rendering_i->render_box(
		g_gui_window_rendering_i->get_current_drawlist(),
		m_plot_base - Vector2D(0, y_offset), Vector2D(m_plot_base.x + screen_size.x, m_plot_base.y),
		CColor(0, 0, 0, 120));

	// reset for new frame
	y_offset = 0.0f;

	auto text_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
	
	// render each data
	int n = 1;
	for (auto& [name, data_container] : m_data)
	{
		auto& [data, color] = data_container;

		Vector2D current_header_base = m_plot_base + Vector2D(1, -y_offset);
		Vector2D current_data_base = current_header_base + Vector2D(data_entry_width_total, 0);

		// render header
		g_gui_window_rendering_i->render_text_with_background(
			g_gui_window_rendering_i->get_current_drawlist(),
			text_font,
			current_header_base - Vector2D(0, data_entry_height_total / 2 + g_gui_fontmgr_i->calc_font_text_size(text_font, name.c_str()).y / 2),
			white, name);

		// top line
		g_gui_window_rendering_i->render_line(
			g_gui_window_rendering_i->get_current_drawlist(),
			current_header_base, current_header_base + Vector2D(screen_size.x, 0),
			white_dim);

		float x_pos = current_data_base.x;
		for (auto& [val, max, is_bool] : data)
		{
			// determine pixel y position based on the value
			float y_pos = current_data_base.y;
			if (is_bool)
			{
				if (val != 0)
				{
					bool bigger_plottinf_scale = plotting_data_scale > 1.0f;
					auto from = Vector2D(x_pos, current_data_base.y);
					auto to = Vector2D(x_pos + (bigger_plottinf_scale ? plotting_data_scale : 0), current_data_base.y - data_entry_height_total);
					if (bigger_plottinf_scale)
					{
						g_gui_window_rendering_i->render_box(
							g_gui_window_rendering_i->get_current_drawlist(),
							from, to,
							color);
					}
					else
					{
						g_gui_window_rendering_i->render_line(
							g_gui_window_rendering_i->get_current_drawlist(),
							from, to,
							color);
					}
				}
			}
			else
			{
				float y_offset = std::min((val / max) * data_entry_height_total, data_entry_height_total);
				auto pixel_position = Vector2D(x_pos, y_pos - y_offset);
				g_gui_window_rendering_i->render_box(
					g_gui_window_rendering_i->get_current_drawlist(),
					pixel_position, pixel_position + Vector2D(plotting_data_scale, 0) + 1.0f,
					color);
			}
		
			x_pos = x_pos + (1.0f * plotting_data_scale);
		}

		y_offset += data_entry_height_total;
		n++ ;
	}

	// render base
	//g_gui_window_rendering_i->render_line(
	//	g_gui_window_rendering_i->get_current_drawlist(),
	//	m_plot_base, m_plot_base + Vector2D(screen_size.x, 0),
	//	white_dim);

	// render data header separator on the right
	g_gui_window_rendering_i->render_line(
		g_gui_window_rendering_i->get_current_drawlist(),
		m_plot_base + Vector2D(data_entry_width_total, 0), m_plot_base + Vector2D(data_entry_width_total, -y_offset),
		white_dim);
}

void CClientMovementPacketPlot::feed_by_name(const std::string& name, const CColor& color, const MPVisualDataEntry& data)
{
	if (movement_plot_stop.get_value())
	{
		return;
	}

	if (CAntiScreen::the().hide_visuals() || CPanic::the().panicking())
	{
		return;
	}

	auto& entry = m_data[name];

	if (!entry.color.is_nonzero())
	{
		entry.color = color;
	}

	while (!entry.entries.empty() && 
		   entry.entries.size() >= m_max_x_data)
	{
		entry.entries.pop_front();
	}

	entry.entries.push_back(data);
}
