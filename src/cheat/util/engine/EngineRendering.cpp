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

VarBoolean debug_render_info("debug_render_info", "Renders some information on the screen", false);
VarBoolean debug_render_info_misc("debug_render_info_misc", "Renders miscellaneous information on the screen", false);

//-----------------------------------------------------------------------------

int EngineFont::render_text(const Vector2D& where, bool background, const std::string& text) const
{
	return render_text_internal(where, background, CColor(230, 230, 230), text);
}

int EngineFont::render_text_colored(const Vector2D& where, bool background, const CColor& color, const std::string& text) const
{
	return render_text_internal(where, background, color, text);
}

int EngineFont::calc_text_width(const std::string& text) const
{
	assert(m_handle && "called " __FUNCTION__ " on uninitialized (NULL) handle.");

	int width = 0;
	for (size_t i = 0; i < text.length(); i++)
	{
		width += CEngineFontRendering::the().calc_char_width(m_handle, (int)text[i]);
	}

	return width;
}

int EngineFont::render_text_internal(const Vector2D& where, bool background, const CColor& color, const std::string& text) const
{
	auto enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	int text_width_px = calc_text_width(text);

	if (background)
	{
		enginefuncs->pfnFillRGBABlend((int)where.x, (int)where.y,
									  text_width_px, m_font_height,
									  30, 30, 30, 140);
	}

	enginefuncs->pfnDrawSetTextColor(color.r, color.g, color.b);
	enginefuncs->pfnDrawConsoleString((int)where.x, (int)where.y, (char*)text.c_str());
	return text_width_px;
}

void EngineFont::initialize()
{
	m_font_height = CEngineFontRendering::the().get_char_height(m_handle);
}

//-----------------------------------------------------------------------------

void CEngineFontRendering::initialize()
{
	auto current_scheme = CHLInterfaceHook::the().ISchemeManager()->GetIScheme(CHLInterfaceHook::the().ISchemeManager()->GetDefaultScheme());

	m_console_font = current_scheme->GetFont("EngineFont", true);
	m_credits_font = current_scheme->GetFont("Legacy_CreditsFont", true);

	m_debug_font = m_console_font;
}

void CEngineFontRendering::repaint()
{
	debug_repaint();
}

int CEngineFontRendering::calc_char_width(hl::vgui2::HFont font_handle, wchar_t ch)
{
	auto isurface = CHLInterfaceHook::the().ISurface();
	int a, b, c;
	isurface->GetCharABCwide(font_handle, ch, a, b, c);
	return c + b + a;
}

int CEngineFontRendering::get_char_height(hl::vgui2::HFont font_handle)
{
	auto isurface = CHLInterfaceHook::the().ISurface();
	return isurface->GetFontTall(font_handle);
}

void CEngineFontRendering::render_debug_impl(const std::string& text)
{
	if (g_cheat_info_i->is_game_ui_running())
	{
		return;
	}

	if (!debug.get_value() || !debug_render_info.get_value())
	{
		return;
	}

	auto& pos = m_current_cursor_pos[m_render_side];

	if (pos.IsZero())
	{
		// initialize
		switch (m_render_side)
		{
			case LEFT:
			{
				pos = Vector2D(50, 50); // top left
				break;
			}
			case RIGHT:
			{
				pos = Vector2D(CVideoModeUtil::the().get_real_screen_size().x - 50.0f - 200.0f, 50); // top right
				break;
			}
		}
	}
	else
	{
		// already initialized, inrement on y axis
		pos.y += m_debug_font.get_text_height();
	}
	m_debug_text.push_back({ text, pos });
}

void CEngineFontRendering::render_information()
{
	if (!debug.get_value() || !debug_render_info_misc.get_value())
	{
		return;
	}

	auto cmd = CClientMovementPacket::the().get_cmd();

	if (!cmd)
	{
		return;
	}

	render_debug("--- Miscellaneous ---");
	
	float gnd_dist = CLocalState::the().get_ground_dist();
	float edge_dist = CLocalState::the().get_edge_dist();
	float gnd_angle = CLocalState::the().get_ground_angle();
	bool is_surfing = CLocalState::the().is_surfing();
	float fall_vel = CLocalState::the().get_fall_velocity();
	float vel_2d = CLocalState::the().get_local_velocity_2d();
	Vector vel_vec = CLocalState::the().get_local_velocity_vec();
	auto local = CLocalState::the().local_player();

	auto pmove = *CMemoryHookMgr::the().pmove().get();
	auto cl = CMemoryHookMgr::the().cl().get();
	auto frame = CLocalState::the().get_current_frame();
	auto clientdata = CLocalState::the().get_current_frame_clientdata();

	float velocity = CLocalState::the().get_local_velocity_2d();
	static float last_velocity = velocity;

	static float rolling_accel = 0.0f;

	rolling_accel = 0.9f * rolling_accel + (1.0f - 0.9f) * (velocity - last_velocity);
	last_velocity = velocity;

	float movespeed = sqrt((cmd->forwardmove * cmd->forwardmove) + (cmd->sidemove * cmd->sidemove) + (cmd->upmove * cmd->upmove));

	render_debug("Acceleration: {:0.3f} u/frame", rolling_accel);
	render_debug("Ground distance: {:0.3f} units", gnd_dist);
	render_debug("Edge distance: {:0.3f} units", edge_dist);
	render_debug("Ground angle: {:0.1f} a", gnd_angle);
	render_debug("Fall velocity: {:0.3f} u/s", fall_vel);
	render_debug("Velocity 2D: {:0.3f} u/s", vel_2d);
	render_debug("Velocity 3D: {} u/s", vel_vec);
	render_debug("Origin: {}", pmove->origin);
	render_debug("Is surfing: {}", is_surfing);
	if (local)
	{
		render_debug("Is alive: {}", true);
		render_debug("Min/Max: {}/{}", local->get_bounding_box_min(), local->get_bounding_box_max());
	}
	else
	{
		render_debug("Is alive: {}", false);
	}
	render_debug("Water level: {}", pmove->waterlevel);
	render_debug("Water type: {}", pmove->watertype);
	render_debug("Water jump time: {}", pmove->waterjumptime);
	render_debug("Forwardmove: {}", cmd->forwardmove);
	render_debug("Sidemove: {}", cmd->sidemove);
	render_debug("Maxspeed: {}", pmove->maxspeed);
	render_debug("Movespeed: {}", movespeed);
	render_debug("Movetype: {}", pmove->movetype);
	render_debug("Viewangles: {}", cmd->viewangles);
	render_debug("prediction_error: {}", cl->prediction_error.Length());
	render_debug("Yaw: {} a", cmd->viewangles[YAW]);
	render_debug("usehull: {}", pmove->usehull);
	render_debug("iuser1: {}", clientdata->iuser1);
	render_debug("iuser2: {}", clientdata->iuser2);
	render_debug("key_dest: {}", (int)*CMemoryHookMgr::the().key_dest().get());
	render_debug("playernum: {}", cl->playernum);

	auto va_delta = CLocalState::the().get_viewangle_delta();
	render_debug("VA delta: {}", va_delta);
	render_debug("light level: {}", cmd->lightlevel);

	render_debug("--- Network ---");
	static double stable_latency = 0.0, stable_fakel = 0.0;
	stable_latency = 0.9f * stable_latency + (1.0f - 0.9f) * (frame->latency);
	stable_fakel = 0.9f * stable_fakel + (1.0f - 0.9f) * (CNetchanSequenceHistory::the().get_desired_fake_latency());
	render_debug("latency: {:0.3f} ms / fake: {:0.3f} ms", stable_latency * 1000.0, stable_fakel * 1000);
}

void CEngineFontRendering::debug_repaint()
{
	if (!debug.get_value() || !debug_render_info.get_value())
	{
		return;
	}
	
	for (auto& [text, pos] : m_debug_text)
	{
		m_debug_font.render_text(pos, true, text);
	}

	m_debug_text.clear();
	for (int i = 0; i < NUMSIDES; i++)
	{
		m_current_cursor_pos[i].Clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------

void CEngineRendering::initialize()
{
	// initialize fonts
	CEngineFontRendering::the().initialize();
}

void CEngineRendering::repaint()
{
	static bool once = true;
	if (once)
	{
		initialize();
		once = false;
	}

	CIngameScreenRendering::the().better_cl_showfps();

	CEngineFontRendering::the().repaint();

	if (!g_cheat_info_i->is_game_ui_running())
	{
		auto local = CLocalState::the().local_player();
		if (local) // render only if alive
		{
			CMovementVisualInfo::the().render();
		}
	}
}

void CEngineRendering::render_circle_opengl(float cx, float cy, float radius, int num_segments, float width, bool blend, int r, int g, int b, int a)
{
	// https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, blend ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);

	glColor4ub(r, g, b, a);

	glLineWidth(width);

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * std::numbers::pi_v<float> *float(ii) / float(num_segments); // get the current angle

		float x = radius * cosf(theta); // calculate the x component
		float y = radius * sinf(theta); // calculate the y component

		glVertex2f(x + cx, y + cy); // output vertex

	}
	glEnd();

	glColor3ub(255, 255, 255);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void CEngineRendering::render_line_opengl(const Vector2D& from, const Vector2D& to, float width, bool blend, int r, int g, int b, int a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, blend ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);

	glColor4ub(r, g, b, a);

	glLineWidth(width);

	glBegin(GL_LINES);

	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);

	glEnd();

	glColor3ub(255, 255, 255);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
