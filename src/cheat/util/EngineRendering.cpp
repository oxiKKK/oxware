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

void CEngineRendering::initialize()
{
	auto enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	
	// get the width and height of a character drawn in "console" engine font this way
	// this should be 
	int dummy; // stupid valve func doesn't accept nullptrs xd
	enginefuncs->pfnDrawConsoleStringLen(" ", &dummy, &m_console_text_height);
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

void CEngineRendering::render_engine_text(const Vector2D& where, bool background, const CColor& color, const std::string& text, int length)
{
	auto enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	
	if (background)
	{
		enginefuncs->pfnFillRGBABlend((int)where.x, (int)where.y, 
									  length, m_console_text_height, 
									  30, 30, 30, 140);
	}

	enginefuncs->pfnDrawSetTextColor(color.r, color.g, color.b);
	enginefuncs->pfnDrawConsoleString((int)where.x, (int)where.y, (char*)text.c_str());
}

int CEngineRendering::calculate_width_for_console_text(const std::string& text)
{
	auto screen_info = CGameUtil::the().get_engine_screen_info();

	int width = 0;
	for (size_t i = 0; i < text.length(); i++)
	{
		width += get_font_wide((int)text[i], 20);
	}

	return width;
}

int CEngineRendering::get_font_wide(int ch, unsigned int font)
{
	auto surface = CHLInterfaceHook::the().ISurface();
	if (!surface)
	{
		return 0;
	}

	int a, b, c;
	surface->GetCharABCwide(font, ch, a, b, c);
	return c + b + a;
}
