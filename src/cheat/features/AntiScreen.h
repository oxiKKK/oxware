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

#ifndef ANTISCREEN_H
#define ANTISCREEN_H
#pragma once

extern VarBoolean antiscreen_enable;
extern VarInteger antiscreen_time;

class CAntiScreen
{
public:
	DECL_BASIC_CLASS(CAntiScreen);

public:
	void update();

	inline bool hide_visuals() const { return m_disable_visual_features; }

	// used only inside glreadpixels
	inline bool glreadpixels_called_by_antiscreen() { return m_antiscreen_reading_pixels; }

	void detour_pixels(uint8_t* pixels);

	void set_disable_visuals(bool disable)
	{
		m_disable_visual_features = disable;
		m_forced_disable_visuals = disable;
	}

private:
	void grab_clean_pixels();

	void handle_panic();

	bool m_disable_visual_features = false;
	bool m_forced_disable_visuals = false;

	// true when we call glReadPixels from AntiScreen code. this way we can distinguish the caller.
	bool m_antiscreen_reading_pixels;

	int m_nth_frame = 0;
	uint32_t m_time_last_took = 0;

	std::vector<uint8_t> m_clean_screen_pixel_buffer; // a clean screen pixel buffer.
};

#endif // ANTISCREEN_H