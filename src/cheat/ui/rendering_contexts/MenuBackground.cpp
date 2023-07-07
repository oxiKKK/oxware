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

void CUIMenuBackground::on_initialize()
{
	m_fade_density = 0.0f;
}

// fast rand
static unsigned long x = 123456789, y = 362436069, z = 521288629;
unsigned long xorshf96() {          //period 2^96-1
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

void CUIMenuBackground::on_render()
{
	update_density();

	if (ui_background_fade.get_value())
	{
		g_gui_window_rendering_i->render_box(g_gui_window_rendering_i->get_background_drawlist(),
											 { 0.0f, 0.0f }, g_imgui_platform_layer_i->get_screen_size(), { 0.0f, 0.0f, 0.0f, m_fade_density });
	}

	//
	// rain
	//

	if (ui_background_rain.get_value())
	{
		render_rain();
	}
}

void CUIMenuBackground::on_destroy()
{
}

void CUIMenuBackground::update_density()
{
	if (m_is_rendering_timeless)
	{
		m_fade_density = time_elapsed_since_opened() / m_threshold;
		m_fade_density = std::min(m_fade_density, k_max_fade_density);
	}
	else
	{
		if (time_elapsed_since_closed() < m_threshold)
		{
			m_fade_density = k_max_fade_density - (time_elapsed_since_closed() / m_threshold);
		}
	}
}

void CUIMenuBackground::render_rain()
{
	auto screen = g_imgui_platform_layer_i->get_screen_size();
	if (screen.IsZero())
	{
		return;
	}

	if (m_rain.size() > 1000)
		m_rain.pop_front();

	// for time-based calculations, rather then fps-based
	static uint32_t prev_cross_time = GetTickCount();

	bool flip_every_N_ms = false;
	if (GetTickCount() - prev_cross_time > 5)
	{
		flip_every_N_ms = true;
		prev_cross_time = GetTickCount();
	}

	if (flip_every_N_ms)
	{
		m_rain.push_back(
		{
			Vector2D((xorshf96() % (int)screen.x), -10.0f),
			Vector2D((xorshf96() % 7), 1),
			(float)(xorshf96() % ((115 - 30) + 1) + 30),
			(xorshf96() % (1 - 0 + 1)) == 1
		});
	}

	for (auto it = m_rain.begin(); it != m_rain.end(); it++)
	{
		if (it->relative_pos.y < screen.y)
		{
			if (flip_every_N_ms)
			{
				if (it->direction)
				{
					it->velocity.x *= -1.0f;
				}

				it->relative_pos.x = it->relative_pos.x + (it->velocity.x * it->random_factor) * 0.001f;
				it->relative_pos.y = it->relative_pos.y + (it->velocity.y * it->random_factor) * (0.45f / 4.f);
			}

			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_background_drawlist(),
				it->relative_pos, it->relative_pos + Vector2D(0.0f, it->random_factor),
				CColor(230, 230, 230, (int)(m_fade_density * (180.0f / 255.0f) * 255.0f)), 1);
		}
		else
		{
			// out of bounds. we could erase it here, but I think that that would be too slow
		}
	}
}
