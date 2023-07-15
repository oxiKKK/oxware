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

#ifndef ENGINERENDERING_H
#define ENGINERENDERING_H
#pragma once

extern VarBoolean debug_render_info;
extern VarBoolean debug_render_info_misc;

class EngineFont
{
public:
	EngineFont()
	{
	}
	EngineFont(hl::vgui2::HFont font_handle)
	{
		m_handle = font_handle;
		initialize();
	}

	int render_text(const Vector2D& where, bool background, const std::string& text) const;
	int render_text_colored(const Vector2D& where, bool background, const CColor& color, const std::string& text) const;

	// we do need to calculate the text width because it's not monospaced however, the height is always the same, hence the naming.
	int calc_text_width(const std::string& text) const;
	int get_text_height() const { return m_font_height; }

private:
	int render_text_internal(const Vector2D& where, bool background, const CColor& color, const std::string& text) const;

	void initialize();

private:
	hl::vgui2::HFont m_handle = NULL; // font handle

	// we can't precompute this, it doesn't change
	int m_font_height = 0;
};

class CEngineFontRendering
{
public:
	DECL_BASIC_CLASS(CEngineFontRendering);

public:
	void initialize();
	void repaint();

	const EngineFont& console_font() const { return m_console_font; }
	const EngineFont& credits_font() const { return m_credits_font; }

	//
	// utils
	//

	int calc_char_width(hl::vgui2::HFont font_handle, wchar_t ch);
	int get_char_height(hl::vgui2::HFont font_handle);

	//
	// debug
	//

	enum ESide { LEFT, RIGHT, NUMSIDES };

	template <class... _Types>
	void render_debug(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		render_debug_impl(std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
	}

	// call this in CL_CreateMove
	void render_information();

	void push_render_side(ESide side)
	{
		m_prev_render_side = m_render_side;
		m_render_side = side;
	}

	void pop_render_side()
	{
		m_render_side = m_prev_render_side;
	}

private:
	void render_debug_impl(const std::string& text);

private:
	EngineFont m_console_font, m_credits_font;

	struct debug_text_t
	{
		std::string text;
		Vector2D screen_pos;
	};
	std::vector<debug_text_t> m_debug_text;
	Vector2D m_current_cursor_pos[NUMSIDES];

	EngineFont m_debug_font;

	ESide m_render_side = LEFT, m_prev_render_side;

	void debug_repaint();
};

class CEngineRendering
{
public:
	DECL_BASIC_CLASS(CEngineRendering);

public:
	void initialize();
	void repaint();

	//
	// utils
	//

	void render_circle_opengl(float cx, float cy, float radius, int num_segments, float width, bool blend, int r, int g, int b, int a);
	void render_line_opengl(const Vector2D& from, const Vector2D& to, float width, bool blend, int r, int g, int b, int a);
};

#endif // ENGINERENDERING_H