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

VarBoolean antiscreen_enable("antiscreen_enable", "Enables Anti-Screen hacks", false);
VarInteger antiscreen_time("antiscreen_time", "Amount of seconds to wait before the screenshot will be taken.", 5, 1, 30);

void CAntiScreen::update()
{
	if (!COxWare::the().is_fully_initialized())
	{
		return;
	}

	if (!antiscreen_enable.get_value())
	{
		return;
	}

	int time_to_wait = antiscreen_time.get_value();
	if (GetTickCount() - m_time_last_took > (uint32_t)time_to_wait * 1000)
	{
		static constexpr int n_frames_to_wait = 4;

		if (!m_disable_visual_features)
		{
//			CConsole::the().dinfo("Time's up. Waiting {} frames.", n_frames_to_wait);
		}

		// someone has enforced this, don't change it.
		if (!m_forced_disable_visuals)
		{
			m_disable_visual_features = true;
		}

		// wait a little bit in order to have a clean frame
		if (++m_nth_frame > n_frames_to_wait)
		{
			//
			// now everything should be disabled, grab clean pixels
			//

			grab_clean_pixels();

			// reset stuff
			m_nth_frame = 0;
			if (!m_forced_disable_visuals)
			{
				m_disable_visual_features = false;
			}
			m_time_last_took = GetTickCount();
		}
	}

	m_forced_disable_visuals = false;
}

void CAntiScreen::detour_pixels(uint8_t* pixels)
{
	// return artifical clean pixels we've stored a while ago
	memcpy(pixels, m_clean_screen_pixel_buffer.data(), m_clean_screen_pixel_buffer.size());
}

void CAntiScreen::grab_clean_pixels()
{
	// the snapshot/screenshot cvars take the entire screen into a count (including blackbars)
	auto screen_size_full = CVideoModeUtil::the().get_full_screen_size();

	// pixels + 3 color channels (RGB)
	m_clean_screen_pixel_buffer.clear();
	m_clean_screen_pixel_buffer.resize((size_t)(screen_size_full.x * screen_size_full.y * 3.0f));

//	CConsole::the().dinfo("Grabbing clean pixels ({}x{})", (int)screen_size_full.x, (int)screen_size_full.y);

	// retreive clean pixels
	m_antiscreen_reading_pixels = true;
	glReadPixels(0, 0, (int)screen_size_full.x, (int)screen_size_full.y, GL_RGB, GL_UNSIGNED_BYTE, m_clean_screen_pixel_buffer.data());
	m_antiscreen_reading_pixels = false;
}

void CAntiScreen::handle_panic()
{
	m_forced_disable_visuals = true;
}
