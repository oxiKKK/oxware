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

void MenuChilden::Screen::FlashbangFade::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&flashfademod_enable,
	&flashfademod_color,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Fade factor", "%0.1f", &flashfademod_fade_factor);
	});
}

void MenuChilden::Screen::ESP::contents()
{
	CUIMenuWidgets::the().section_unavailable_for_builds(
	[](int current_bn) { return current_bn <= 3266; }, "> 3266",
	[]()
	{
	CUIMenuWidgets::the().feature_enabled_section(
	&esp_enable,
	[]()
	{
		g_gui_widgets_i->add_spacing();

		CUIMenuWidgets::the().add_checkbox("Box background", &esp_background);
		CUIMenuWidgets::the().add_checkbox("Box outline", &esp_box_outline);
		CUIMenuWidgets::the().add_listbox("Box type", &esp_box_type, { "Normal", "Corners" });

		if (g_gui_widgets_i->begin_columns("esp_player", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Enemy", &esp_player_enemy);
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Teammates", &esp_player_teammates);

			g_gui_widgets_i->end_columns();
		}

		g_gui_widgets_i->begin_tab("esp_tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		float tab_height = 150.0f;

		g_gui_widgets_i->add_tab_item(
			"Players", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().add_checkbox("Enable ##player", &esp_player_enable);
				CUIMenuWidgets::the().add_checkbox("Show name", &esp_player_name);
				CUIMenuWidgets::the().add_checkbox("Details", &esp_player_details);
			});

		g_gui_widgets_i->add_tab_item(
			"Entities", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().add_checkbox("Enable ##ents", &esp_entity_enable);
			});

		g_gui_widgets_i->add_tab_item(
			"Sound", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().add_checkbox("Enable ##sound", &esp_sound_enable);
				CUIMenuWidgets::the().add_slider("Display life", "%0.1f seconds", &esp_sound_interval);
				CUIMenuWidgets::the().add_checkbox("Filter local", &esp_sound_filter_local);
				CUIMenuWidgets::the().add_listbox("Type ##hidden", &esp_sound_type, { "2D", "3D", "Player ESP box" });

				CUIMenuWidgets::the().feature_enabled_section(
				&esp_sound_resolver,
				[]()
				{
					CUIMenuWidgets::the().add_slider("Resolver dist", "%0.0f units", &esp_sound_resolver_distace);
				}, "Resolver");
			});

		g_gui_widgets_i->end_tab();
	});
	});
}

void MenuChilden::Screen::CustomCrosshair::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&crosshair_enable,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("Dynamic", &crosshair_dynamic);
		CUIMenuWidgets::the().add_checkbox("Translucent", &crosshair_translucent);
		CUIMenuWidgets::the().add_checkbox("Static", &crosshair_static);

		CUIMenuWidgets::the().add_slider("Size", "%0.0f", &crosshair_size, "vanilla");
		CUIMenuWidgets::the().add_slider("Gap", "%0.0f", &crosshair_gap, "vanilla");
		CUIMenuWidgets::the().add_slider("Thickness", "%0.0f pixels", &crosshair_thickness);

		CUIMenuWidgets::the().add_listbox("Type ##hidden", &crosshair_type, { "Classic", "T-Shaped", "Circular" });

		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_color_edit("Color", &crosshair_color);
	});
}

void MenuChilden::Screen::ClShowFPSEnhancement::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&ingamescreen_better_cl_showfps,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("Background", &ingamescreen_better_cl_showfps_background);
		CUIMenuWidgets::the().add_listbox("Position", &ingamescreen_better_cl_showfps_position, { "Top left", "Top right", "bottom right", "bottom left" });
	});
}

void MenuChilden::Screen::MovementInfo::contents()
{
	CUIMenuWidgets::the().add_checkbox("Enable", &movement_visual_info);
	CUIMenuWidgets::the().add_checkbox("Keys", &movement_visual_info_keys);
	CUIMenuWidgets::the().add_slider("Vertical pos", "%0.2f", &movement_visual_info_keys_offset);

	g_gui_widgets_i->add_spacing();
	g_gui_widgets_i->add_separator();
	CUIMenuWidgets::the().add_checkbox("Background", &movement_visual_info_background);
}
