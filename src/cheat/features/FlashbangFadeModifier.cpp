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

VarBoolean flashfademod_enable("flashfademod_enable", "Enables flashbang fade modifier", false);
VarColor flashfademod_color("flashfademod_color", "Forces flash fade color", CColor(255, 255, 255, 255));
VarFloat flashfademod_fade_factor("flashfademod_fade_factor", "Controls fade intensity factor of the flash fade", 1.0f, 0.0f, 1.0f);

void CFlashbangFadeModifier::update(int& alpha)
{
	if (!flashfademod_enable.get_value())
	{
		set_fade_color(CColor(255, 255, 255, 255));
		return;
	}

	set_fade_color(flashfademod_color.get_value());

	// set before modified
	m_current_fade_alpha = alpha;
	alpha = (int)((float)alpha * flashfademod_fade_factor.get_value());
}

void CFlashbangFadeModifier::initialize_gui()
{
	COxWareUI::the().add_background_rendering_code([this]() { on_render(); });
	COxWareUI::the().add_background_rendering_constrain(
		[this]()
		{
			return flashfademod_enable.get_value();
		});

}

float CFlashbangFadeModifier::fade_percentage()
{
	auto& sf = CMemoryHookMgr::the().cl().get()->sf;

	float fadea = (float)m_current_fade_alpha / 255.0f;
	return (fadea / 1.0f) * 100;
}

void CFlashbangFadeModifier::set_fade_color(const CColor& clr)
{
	auto& sf = CMemoryHookMgr::the().cl().get()->sf;

	sf.fader = (uint8_t)(clr.r * 255.0f);
	sf.fadeg = (uint8_t)(clr.g * 255.0f);
	sf.fadeb = (uint8_t)(clr.b * 255.0f);
}

void CFlashbangFadeModifier::on_render()
{
	if (!flashfademod_enable.get_value())
	{
		return;
	}

	float percent = fade_percentage();
	if (percent == 0.0f)
	{
		return;
	}

	auto screen_pos = CVideoModeUtil::the().get_ingame_viewport_pos();
	auto screen_size = CVideoModeUtil::the().get_real_screen_size();
	if (screen_size.IsZero())
	{
		return;
	}

	auto percentage_font = g_gui_fontmgr_i->get_font(FID_ProggyClean, FSZ_13px, FDC_Regular);

	auto label = std::format("flashed {:.1f} %", percent);
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(percentage_font, label.c_str());

	g_gui_window_rendering_i->render_box(
		g_gui_window_rendering_i->get_current_drawlist(), 
		{ screen_pos },
		{ screen_pos.x + screen_size.x * (percent * 0.01f), 3.0f },
		CColor(0, 170, 230, 200));

	g_gui_window_rendering_i->render_text_with_background(
		g_gui_window_rendering_i->get_current_drawlist(), 
		percentage_font,
		{ screen_pos.x + screen_size.x / 2 - label_size.x / 2, 10.0f },
		CColor(230, 230, 230, 230), 
		label);
}