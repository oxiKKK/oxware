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

VarInteger ui_feature_list_font("ui_feature_list_font", "0 for bold, 1 for regular", 1, 0, 1);
VarInteger ui_feature_list_font_size("ui_feature_list_font_size", "0 for smaller, 1 for bigger", 1, 0, 1);
VarBoolean ui_feature_list_overflow("ui_feature_list_overflow", "Controls overflow, if there're too much features enabled", true);

void CUIFeatureList::on_initialize()
{
}

void CUIFeatureList::on_render()
{
	if (!ui_render_feature_list.get_value())
	{
		return;
	}

	g_gui_widgets_i->set_next_window_pos({}, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_size({ g_imgui_platform_layer_i->get_screen_size().x / 2, g_imgui_platform_layer_i->get_screen_size().y }, ImGuiCond_Always);

	static constexpr auto window_flags = 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNav;
	
	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	float widest_feature_name_so_far_px = 0.0f;
	float current_y_position = 0.0f;
	int n_shifts = 0;

	g_gui_widgets_i->create_new_window(
		"feature_list", window_flags, [&]()
		{
			CFeatureManager::the().for_each_feature(
				[&](BaseFeature* f)
				{
					if (f->is_enabled())
					{
						EFontDecoration decor = ui_feature_list_font.get_value() == 0 ? FDC_Bold : FDC_Regular;
						EFontSize fsize = ui_feature_list_font_size.get_value() == 0 ? FSZ_16px : FSZ_18px;

						auto feature_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, fsize, decor);

						auto label_size = g_gui_fontmgr_i->calc_font_text_size(feature_font, f->get_name().c_str());

						bool check_for_overflow = ui_feature_list_overflow.get_value();

						if (check_for_overflow)
						{
							if (label_size.x > widest_feature_name_so_far_px)
							{
								widest_feature_name_so_far_px = label_size.x;
							}
						}

						g_gui_widgets_i->set_cursor_pos({ (widest_feature_name_so_far_px + 5.0f) * n_shifts, current_y_position });

						g_gui_widgets_i->add_text(f->get_name(), TEXTPROP_ColorWhite, feature_font);

						current_y_position += label_size.y + 1.0f;

						if (check_for_overflow)
						{
							// if there isn't enough space below us to fit all features, shift to the side and start again.
							if (current_y_position + (label_size.y + 2.0f) > g_imgui_platform_layer_i->get_screen_size().y)
							{
								n_shifts++;
								current_y_position = 0;
							}
						}
					}
				});
		});

	g_gui_widgets_i->pop_stylevar(1);
}

void CUIFeatureList::on_destroy()
{
}
