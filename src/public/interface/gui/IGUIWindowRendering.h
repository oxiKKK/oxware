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

#ifndef IGUIWINDOWRENDERING_H
#define IGUIWINDOWRENDERING_H
#pragma once

class IGUIWindowRendering : public IBaseInterface
{
public:
	virtual ImDrawList* get_current_drawlist() = 0;
	virtual ImDrawList* get_background_drawlist() = 0;
	virtual ImDrawList* get_foreground_drawlist() = 0;

	virtual void render_text(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what) = 0;

	virtual void render_text_with_background(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what) = 0;

	virtual void render_line(ImDrawList* current, const Vector2D& from, const Vector2D& to, const CColor& color, float thicc = 1.0f) = 0;

	// box without and with an outline
	virtual void render_box(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding = 1.0f) = 0;
	virtual void render_box_outline(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding = 1.0f,
									const CColor& outline_color = {}, float outline_thickness = 1.0f) = 0;

	virtual void render_quad(ImDrawList* current, const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right, 
							 const Vector2D& bottom_left, const CColor& color) = 0;

	virtual void render_circle(ImDrawList* current, const Vector2D& center, float radius, int num_segments, const CColor& color, float thicc = 1.0f) = 0;
};

extern IGUIWindowRendering* g_gui_window_rendering_i;

#define IGUIWINDOWRENDERING_INTERFACEID "IGUIWindowRendering"

#endif // IGUIWINDOWRENDERING_H