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

#ifndef UIMENUBACKGROUND_H
#define UIMENUBACKGROUND_H
#pragma once

struct raindrop_t
{
	Vector2D relative_pos;
	Vector2D velocity;
	float random_factor;
	bool direction;
};

class CUIMenuBackground final : public IRenderingContext_ParentActivateThreshold
{
public:
	CUIMenuBackground(const std::string& id, IRenderingContext* parent_ctx, std::chrono::duration<float, std::milli> threshold) :
		IRenderingContext_ParentActivateThreshold(id, parent_ctx, threshold)
	{
	}

	void on_initialize();
	void on_render();
	void on_destroy();

private:
	void update_density();

	void render_rain();

private:
	inline static constexpr float k_max_fade_density = 0.6f; // [0, 1] scale
	float m_fade_density;

	std::deque<raindrop_t> m_rain;
};

#endif // UIMENUBACKGROUND_H