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

void MenuChilden::Rendering::FieldOfView::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&custom_fov,
	[]()
	{
		CUIMenuWidgets::the().add_slider("FOV scale", "%0.1fx", &custom_fov_value);
	});
}

void MenuChilden::Rendering::AspectRatio::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&aspect_ratio_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Aspect ratio", "%0.1f", &aspect_ratio_value);
	});
}

void MenuChilden::Rendering::StudioRenderer::contents()
{
	g_gui_widgets_i->add_separtor_with_text("Player skeleton 💀");

	CUIMenuWidgets::the().add_checkbox("Enable ##skelly", &mdlchams_player_skeleton);

	g_gui_widgets_i->add_separtor_with_text("Player head hitbox");

	CUIMenuWidgets::the().add_checkbox_with_color("Enable ##hbox", &mdlchams_head_box_enable, &mdlchams_head_box_color);

	g_gui_widgets_i->add_separtor_with_text("Other");

	CUIMenuWidgets::the().add_checkbox("Real player model ##skelly", &mdlchams_render_real_playermodel,
									   "Renders \"Real playermodel\". Hitboxes of this playermodel are used for hit registration, no matter what the acutal model is.");

	CUIMenuWidgets::the().add_description_text_ex("You can see this why this is useful here:", nullptr, true);
	g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	if (g_gui_widgets_i->add_url_text("https://youtu.be/xMd9m3McNvo"))
	{
		CGenericUtil::the().open_link_inside_browser("https://youtu.be/xMd9m3McNvo");
	}
	g_gui_widgets_i->pop_font();

	g_gui_widgets_i->add_padding({ 0, 5.0f });
	CUIMenuWidgets::the().add_checkbox("Disable animations", &mdlchams_disable_animations);
	CUIMenuWidgets::the().add_checkbox("Enforce default VM", &mdlchams_force_default_viewmodel);

	g_gui_widgets_i->add_spacing();
}

void MenuChilden::Rendering::ModelChams::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&mdlchams_enable,
	[]()
	{
		g_gui_widgets_i->begin_tab("model_chams_tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		float tab_height = 150.0f;

		g_gui_widgets_i->add_tab_item(
			"VM", false,
			{ -1.0f, tab_height },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("Viewmodel");
				CUIMenuWidgets::the().add_checkbox_with_color("Enable ##VM", &mdlchams_viewmodel_enable, &mdlchams_viewmodel_color);
				CUIMenuWidgets::the().add_slider("Shadelight", "%0.0f", &mdlchams_viewmodel_shadelight, "off");
				CUIMenuWidgets::the().add_slider("Ambientlight", "%0.0f", &mdlchams_viewmodel_ambientlight, "off");
				CUIMenuWidgets::the().add_listbox("Type", &mdlchams_viewmodel_type, { "Flat", "Shaded", "Lightened", "Texture", "Wireframe" });
			});

		g_gui_widgets_i->add_tab_item(
			"T", false,
			{ -1.0f, tab_height },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("Terrorists");
				CUIMenuWidgets::the().add_checkbox_with_color("Enable ##T", &mdlchams_players_t_enable, &mdlchams_players_t_color);
				CUIMenuWidgets::the().add_slider("Shadelight", "%0.0f", &mdlchams_players_t_shadelight, "off");
				CUIMenuWidgets::the().add_slider("Ambientlight", "%0.0f", &mdlchams_players_t_ambientlight, "off");
				CUIMenuWidgets::the().add_listbox("Type", &mdlchams_players_t_type, { "Flat", "Shaded", "Lightened", "Texture", "Wireframe" });
			});

		g_gui_widgets_i->add_tab_item(
			"CT", false,
			{ -1.0f, tab_height },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("Counter-Terrorists");
				CUIMenuWidgets::the().add_checkbox_with_color("Enable ##CT", &mdlchams_players_ct_enable, &mdlchams_players_ct_color);
				CUIMenuWidgets::the().add_slider("Shadelight", "%0.0f", &mdlchams_players_ct_shadelight, "off");
				CUIMenuWidgets::the().add_slider("Ambientlight", "%0.0f", &mdlchams_players_ct_ambientlight, "off");
				CUIMenuWidgets::the().add_listbox("Type", &mdlchams_players_ct_type, { "Flat", "Shaded", "Lightened", "Texture", "Wireframe" });
			});

		g_gui_widgets_i->add_tab_item(
			"Backtrack", false,
			{ -1.0f, tab_height },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("Backtrack");

				CUIMenuWidgets::the().add_checkbox_with_color("Enable", &mdlchams_players_backtrack_enable, &mdlchams_players_backtrack_color);
				CUIMenuWidgets::the().add_slider("Shadelight", "%0.0f", &mdlchams_players_backtrack_shadelight, "off");
				CUIMenuWidgets::the().add_slider("Ambientlight", "%0.0f", &mdlchams_players_backtrack_ambientlight, "off");
				CUIMenuWidgets::the().add_listbox("Type", &mdlchams_players_backtrack_type, { "Flat", "Shaded", "Lightened", "Texture", "Wireframe" });
			});

		g_gui_widgets_i->end_tab();

		g_gui_widgets_i->add_separtor_with_text("Properties");
		CUIMenuWidgets::the().add_checkbox("Flat-shaded", &mdlchams_flatshaded);
		CUIMenuWidgets::the().add_checkbox("Blend", &mdlchams_blend);

		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_checkbox("Rainbow colors", &mdlchams_rainbow);
		CUIMenuWidgets::the().add_slider("Speed", "%0.0fx", &mdlchams_rainbow_speed);
	});
}

void MenuChilden::Rendering::HUDRendering::contents()
{
	g_gui_widgets_i->add_padding({ 0, 5.0f });
	g_gui_widgets_i->add_separtor_with_text("Custom rendering");

	CUIMenuWidgets::the().feature_enabled_section(
	&hud_render,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("Current weapon", &hud_render_current_weapon);
		CUIMenuWidgets::the().add_checkbox("Velocity", &hud_render_velocity);
	});

	g_gui_widgets_i->add_padding({ 0, 5.0f });
	g_gui_widgets_i->add_separtor_with_text("Colors");

	CUIMenuWidgets::the().add_checkbox_with_color("HUD Color", &hud_color_enable, &hud_color);
}

void MenuChilden::Rendering::Panic::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CPanic::the().panic,
	[]()
	{
	});
}
