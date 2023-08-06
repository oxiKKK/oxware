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

void MenuChilden::World::Removals::contents()
{
	CUIMenuWidgets::the().add_checkbox("Remove screenshake", &remove_screenshake);
	CUIMenuWidgets::the().add_checkbox("Remove viewmodel", &remove_viewmodel);
	CUIMenuWidgets::the().add_checkbox("Remove MOTD", &remove_motd);
	CUIMenuWidgets::the().add_checkbox("Remove sniper scopes", &remove_sniper_scope);

	g_gui_widgets_i->add_separtor_with_text("Remove players");

	CUIMenuWidgets::the().add_listbox("Team", &remove_players_team, { "Off", "Ts", "CTs", "Teammates", "Enemy", "Both" });

	g_gui_widgets_i->add_separtor_with_text("Remove HUD");

	CUIMenuWidgets::the().feature_enabled_section(
	&remove_hud_enable,
	[]()
	{
		g_gui_widgets_i->add_padding({ 0, 5.0f });

		if (g_gui_widgets_i->begin_columns("removals_hud", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Weapons", &remove_hud_weapons);
			CUIMenuWidgets::the().add_checkbox("Crosshair", &remove_hud_crosshair);
			CUIMenuWidgets::the().add_checkbox("Flashlight", &remove_hud_flashlight);

			g_gui_widgets_i->goto_next_column();

			CUIMenuWidgets::the().add_checkbox("Health", &remove_hud_health);
			CUIMenuWidgets::the().add_checkbox("Timer", &remove_hud_timer);
			CUIMenuWidgets::the().add_checkbox("Money", &remove_hud_money);

			g_gui_widgets_i->end_columns();
		}
	});

	g_gui_widgets_i->add_separtor_with_text("Other");

	static std::pair<bool, bool> disable_fog{};
	CUIMenuWidgets::the().add_ingame_cvar_toggleable_checkbox("Disable FOG", CGoldSrcCommandMgr::the().get_cvar("gl_fog"), disable_fog, 
															  0.0f, 1.0f);
}

void MenuChilden::World::Thirdperson::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CThirdPerson::the().thirdperson,
	[]()
	{
		g_gui_widgets_i->add_spacing();
		g_gui_widgets_i->add_spacing();

		CUIMenuWidgets::the().add_slider("Distance", "%0.0f units", &thirdperson_dist);
		CUIMenuWidgets::the().add_checkbox("Block wall", &thirdperson_block_wall);
	});
}

void MenuChilden::World::AntiDemoRecorder::contents()
{
	CUIMenuWidgets::the().add_checkbox("Enable", &anti_demo_recorder_enable);

	CUIMenuWidgets::the().add_description_text(
		"If enabled, server will not be able to record demos on your game.\n"
		"You'll be notified every time server wants to start a demo inside the console.");
}

void MenuChilden::World::AntiScreen::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&antiscreen_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Frames to wait", "%0.0f second(s)", &antiscreen_time);
	});
}

void MenuChilden::World::Automation::contents()
{
	CUIMenuWidgets::the().add_checkbox("Auto reload", &auto_reload);
	CUIMenuWidgets::the().add_checkbox("Auto pistol", &auto_pistol);
	CUIMenuWidgets::the().add_checkbox("Auto zoom", &auto_zoom);
}

void MenuChilden::World::SpeedControl::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&speed_control_enable,
	[]()
	{
		if (g_gui_widgets_i->begin_columns("speed_control", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Invert", &speed_control_invert);

			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Negative", &speed_control_negative);

			g_gui_widgets_i->end_columns();
		}

		g_gui_widgets_i->add_text("Engine");
		CUIMenuWidgets::the().add_slider_nolabel("Amount", "%0.1f", &speed_control_amount_engine, "off");

		g_gui_widgets_i->add_text("Server");
		CUIMenuWidgets::the().add_slider_nolabel("Amount##1", "%0.0f", &speed_control_amount_server, "off");

		g_gui_widgets_i->add_text("Net");
		CUIMenuWidgets::the().add_slider_nolabel("Amount##2", "%0.01f", &speed_control_amount_net, "off");
	});
}

void MenuChilden::World::ViewmodelOffset::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&viewmodel_offset_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Amount", "%0.1f", &viewmodel_offset_value);
	});
}

void MenuChilden::World::Backtrack::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&backtrack_enable,
	[]()
	{
		CUIMenuWidgets::the().add_listbox("Team", &backtrack_team, { "Ts", "CTs", "Teammates", "Enemy", "Both" });
		CUIMenuWidgets::the().add_checkbox("Self", &backtrack_self);
	});
}

void MenuChilden::World::SmokeVisuals::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&smoke_visuals,
	[]()
	{
		CUIMenuWidgets::the().add_color_edit("Color", &smoke_color);
		CUIMenuWidgets::the().add_checkbox("Rainbow smoke 🌈", &smoke_rainbow);

		CUIMenuWidgets::the().add_slider("Opacity", "%0.0f %%", &smoke_opacity);

		CUIMenuWidgets::the().add_description_text("Everything changed here will take effect only on new smoke creation.");
	});
}

void MenuChilden::World::NonSteamFpsFix::contents()
{
	CUIMenuWidgets::the().section_unavailable_for_builds(
		[](int current_bn) { return current_bn >= FIRST_SDL_BUILD; }, "< 5943",
		[]()
		{
			CUIMenuWidgets::the().add_checkbox("Enable", &nonsteam_fps_fix);

			CUIMenuWidgets::the().add_description_text(
				"This fixes the FPS movement dependence, where with higher fps you would get slomotion movement and \"higher\" gravity."
				"\nFor more technical details, follow this link:");

			g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
			if (g_gui_widgets_i->add_url_text("github.com/ValveSoftware/halflife/issues/1940"))
			{
				CGenericUtil::the().open_link_inside_browser("https://github.com/ValveSoftware/halflife/issues/1940");
			}
			g_gui_widgets_i->pop_font();
	});
}

void MenuChilden::World::WorldVisuals::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&world_visuals_enable,
	[]()
	{
		if (g_gui_widgets_i->begin_columns("world_visuals", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("On sky", &world_visuals_dimlight_sky);
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("On world", &world_visuals_dimlight_world);

			g_gui_widgets_i->end_columns();
		}
		CUIMenuWidgets::the().add_slider("Dimlight", "%0.0f %%", &world_visuals_dimlight, NULL, "off");

		CUIMenuWidgets::the().add_checkbox("Rainbow world", &world_visuals_rainbow);

		g_gui_widgets_i->add_separtor_with_text("World fog");

		CUIMenuWidgets::the().feature_enabled_section(
		&world_visuals_fog, &world_visuals_fog_color,
		[]()
		{
			CUIMenuWidgets::the().add_slider("Density", "%0.1f", &world_visuals_fog_density);
		});
	});
}

void MenuChilden::World::EnvironmentalEffects::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&env_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Generate radius", "%0.0f units", &env_radius);

		g_gui_widgets_i->add_separtor_with_text("Ground fog");
		CUIMenuWidgets::the().feature_enabled_section(
		&env_ground_fog,
		[]()
		{
			CUIMenuWidgets::the().add_slider("Density", "%0.0f", &env_ground_fog_density);
			CUIMenuWidgets::the().add_slider("Size", "%0.1fx", &env_ground_fog_size);
		});

		g_gui_widgets_i->add_separtor_with_text("Other");
		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Wind speed", "%0.1fx", &env_wind_speed);
		CUIMenuWidgets::the().add_slider("Fall speed", "%0.1fx", &env_particle_fallspeed);

		g_gui_widgets_i->begin_tab("env_effects", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		float tab_height = 160.0f;

		g_gui_widgets_i->add_tab_item(
			"Rain", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&env_rain,
				[]()
				{
					CUIMenuWidgets::the().add_slider("Density", "%0.1f", &env_rain_density);
					
					g_gui_widgets_i->add_separtor_with_text("Ambience");
					CUIMenuWidgets::the().add_checkbox("Ambient sound", &env_rain_ambient);
					CUIMenuWidgets::the().add_checkbox("Occasional Thunder", &env_rain_ambient_thunder);
					CUIMenuWidgets::the().add_slider("Volume", "%0.1f", &env_rain_ambient_volume);
				});
			});

		g_gui_widgets_i->add_tab_item(
			"Snow", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&env_snow,
				[]()
				{
					CUIMenuWidgets::the().add_slider("Density", "%0.1f", &env_snow_density);
					CUIMenuWidgets::the().add_slider("Flake size", "%0.0fx", &env_snow_flake_size);
					CUIMenuWidgets::the().add_slider("Die time", "%0.1f seconds", &env_snow_flake_die_time);

					g_gui_widgets_i->add_separtor_with_text("Ambience");
					CUIMenuWidgets::the().add_checkbox("Ambient sound", &env_snow_ambient);
					CUIMenuWidgets::the().add_slider("Volume", "%0.1f", &env_snow_ambient_volume);
				});
			});

		g_gui_widgets_i->end_tab();
				});
}
