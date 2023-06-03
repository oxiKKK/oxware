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

//-----------------------------------------------------------------------------

void EngineFont::render_text(const Vector2D& where, bool background, const std::string& text) const
{
	render_text_internal(where, background, CColor(230, 230, 230), text);
}

void EngineFont::render_text_colored(const Vector2D& where, bool background, const CColor& color, const std::string& text) const
{
	render_text_internal(where, background, color, text);
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

void EngineFont::render_text_internal(const Vector2D& where, bool background, const CColor& color, const std::string& text) const
{
	auto enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();

	int text_width_px = calc_text_width(text);

	if (background)
	{
		enginefuncs->pfnFillRGBABlend((int)where.x, (int)where.y,
									  text_width_px, m_font_height,
									  30, 30, 30, 140);
	}

	enginefuncs->pfnDrawSetTextColor(color.r, color.g, color.b);
	enginefuncs->pfnDrawConsoleString((int)where.x, (int)where.y, (char*)text.c_str());
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
	if (debug_render_info.get_value())
	{
		if (m_current_cursor_pos.IsZero())
		{
			// initialize
			m_current_cursor_pos = Vector2D(50, 50); // top left
		}
		else
		{
			// already initialized, inrement on y axis
			m_current_cursor_pos.y += m_debug_font.get_text_height();
		}
		m_debug_text.push_back({ text, m_current_cursor_pos });
	}
}

void CEngineFontRendering::debug_repaint()
{
	if (debug_render_info.get_value())
	{
		for (auto& [text, pos] : m_debug_text)
		{
			m_debug_font.render_text(pos, true, text);
		}

		m_debug_text.clear();
		m_current_cursor_pos.Clear();
	}
}

//-----------------------------------------------------------------------------

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
