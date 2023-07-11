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

void MenuChilden::Movement::AirStuck::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().airstuck,
	[]()
	{
		const char* format = movement_air_stuck_type.get_value() == 0 ? "%0.0f" : "%0.2f";

		g_gui_widgets_i->add_text("Intensity");
		CUIMenuWidgets::the().add_slider_nolabel("Intensity", format, &movement_air_stuck_intensity, "froze");

		CUIMenuWidgets::the().add_listbox("Type", &movement_air_stuck_type, { "Server speedhack", "Engine speedhack" });
	});
}

void MenuChilden::Movement::Visualization::contents()
{
	g_gui_widgets_i->add_separtor_with_text("Graph plotter");

	CUIMenuWidgets::the().feature_enabled_section(
	&movement_plot,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Scale", "%0.0fx", &movement_plot_scale);
		CUIMenuWidgets::the().add_slider("Row height", "%0.0fpx", &movement_plot_row_height);

		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_checkbox("Stop collection", &movement_plot_stop);
	});
}

void MenuChilden::Movement::GroundStrafe::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().gs,
	[]()
	{
		g_gui_widgets_i->add_separtor_with_text("No slowdown");

		CUIMenuWidgets::the().feature_enabled_section(
		&movement_gs_mode_noslowdown,
		[]()
		{
			CUIMenuWidgets::the().add_listbox("Method##nsdn", &movement_gs_mode_noslowdown_method, { "Server speedhack", "Engine speedhack", });

			g_gui_widgets_i->add_spacing();

			CUIMenuWidgets::the().add_slider("Speed factor", "%0.0fx", &movement_gs_mode_noslowdown_factor, NULL, "full speed");
		}, "Enable##nsdn");

		g_gui_widgets_i->add_separtor_with_text("Method");
		CUIMenuWidgets::the().add_listbox("Mode", &movement_gs_mode, { "Legit", "Rage" });

		g_gui_widgets_i->add_spacing();
		g_gui_widgets_i->add_separator();

		if (movement_gs_mode.get_value() == 0) // legit
		{
			g_gui_widgets_i->add_separtor_with_text("Scroll emulation");
			g_gui_widgets_i->add_text("Ground distance to start scrolling");
			CUIMenuWidgets::the().add_description_text("Represents min&max distance from the ground where the randomized scrolling will begin. Same as in bunnyhop.");
			g_gui_widgets_i->add_spacing();
			CUIMenuWidgets::the().add_slider_nolabel("min", "%0.0f units", &movement_gs_legit_ground_dist_min);
			CUIMenuWidgets::the().add_slider_nolabel("max", "%0.0f units", &movement_gs_legit_ground_dist_max);

			int min = movement_gs_legit_ground_dist_min.get_value();
			int max = movement_gs_legit_ground_dist_max.get_value();

			// auto-correct nonsense values
			if (min > max)
			{
				movement_gs_legit_ground_dist_min.set_value(max);
			}
			if (max < min)
			{
				movement_gs_legit_ground_dist_max.set_value(min);
			}

			CUIMenuWidgets::the().add_slider("Scroll density", "%0.0f", &movement_gs_legit_scroll_density, NULL, "full");

			CUIMenuWidgets::the().add_listbox("Efficiency", &movement_gs_legit_efficiency, { "Helper", "Normal", "Random FOG based" });

			CUIMenuWidgets::the().add_description_text("Same settings as in bunnyhop. I don't want to repeat myself here 😉");
		}
		else if (movement_gs_mode.get_value() == 1) // rage
		{
			CUIMenuWidgets::the().add_description_text(
				"Rage bhop is highly obviously to someone, who analyzes your demo - be careful where you use it. "
				"\n\nHowever, it suits well for HVH scenarious or simply for servers where no one cares about bhop hacks. "
				"It also has no-slow-down capabilities.");
		}
	});
}

void MenuChilden::Movement::Strafehack::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CFrameSkipper::the().frame_skipper,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("Allow on surf", &movement_strafe_hack_allow_on_surf);
		CUIMenuWidgets::the().add_checkbox("Reset to forward", &movement_strafe_hack_dir_reset);

		CUIMenuWidgets::the().feature_enabled_section(
		&movement_strafe_hack_dir_auto,
		[]()
		{
			CUIMenuWidgets::the().add_listbox("Direction", &movement_strafe_hack_dir, { "Forward", "Right", "Back", "Left" });
		},
		"Automatic Direction",
			false);

		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Boost", "%0.2f", &movement_strafe_hack_boost);

		CUIMenuWidgets::the().feature_enabled_section(
		&movement_strafe_hack_limit_velocity,
		[]()
		{
			CUIMenuWidgets::the().add_slider_nolabel("Max", "%0.0f units", &movement_strafe_hack_limit_velocity_max);
		},
		"Limit velocity");
	});
}

void MenuChilden::Movement::StrafeHelper::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().strafe_helper,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Accumulation", "%0.2f", &movement_strafe_helper_accumulation);//
		CUIMenuWidgets::the().add_checkbox("Accumulation on ground", &movement_strafe_helper_accumulation_on_ground);

		CUIMenuWidgets::the().feature_enabled_section(
		&movement_strafe_helper_strafe_with_mouse,
		[]()
		{
			CUIMenuWidgets::the().add_listbox("Direction", &movement_strafe_helper_strafe_dir, { "Forward", "Right", "Back", "Left" });
			CUIMenuWidgets::the().add_checkbox("Deduce automatically", &movement_strafe_helper_strafe_dir_auto);
		}, "Strafe with mouse");
	});
}

void MenuChilden::Movement::Bunnyhop::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().bunnyhop,
	[]()
	{
		g_gui_widgets_i->add_separtor_with_text("No slowdown");
		CUIMenuWidgets::the().feature_enabled_section(
		&movement_bhop_mode_noslowdown,
		[]()
		{
			CUIMenuWidgets::the().add_listbox("Method##nsdn", &movement_bhop_mode_noslowdown_method, { "Server speedhack", "Engine speedhack" });

			g_gui_widgets_i->add_spacing();
			CUIMenuWidgets::the().add_slider("Speed factor", "%0.0fx", &movement_bhop_mode_noslowdown_factor, NULL, "full speed");

			g_gui_widgets_i->add_spacing();

			CUIMenuWidgets::the().add_description_text("Note that this uses speedhack, which most of the anti-cheats can detect.");
		}, "Enable##nsdn");

		g_gui_widgets_i->add_separtor_with_text("Others");
		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Jump repeat", "%0.0f ms", &movement_bhop_repeat_ms, "no limit");

		CUIMenuWidgets::the().add_checkbox("Jump if on ladder", &movement_bhop_jump_on_ladder);
		CUIMenuWidgets::the().add_checkbox("Jump if in water", &movement_bhop_jump_in_water);

		g_gui_widgets_i->add_separtor_with_text("Method");
		CUIMenuWidgets::the().add_listbox("Mode##nolabel", &movement_bhop_mode, { "Legit", "Rage" });

		g_gui_widgets_i->add_spacing();
		g_gui_widgets_i->add_separator();

		if (movement_bhop_mode.get_value() == 0) // legit
		{
			g_gui_widgets_i->add_separtor_with_text("Scroll emulation");
			g_gui_widgets_i->add_text("Ground distance to start scrolling");
			CUIMenuWidgets::the().add_description_text("Represents min&max distance from the ground where the randomized scrolling will begin.");
			g_gui_widgets_i->add_spacing();
			CUIMenuWidgets::the().add_slider_nolabel("min", "%0.0f units", &movement_bhop_legit_ground_dist_min);
			CUIMenuWidgets::the().add_slider_nolabel("max", "%0.0f units", &movement_bhop_legit_ground_dist_max);

			int min = movement_bhop_legit_ground_dist_min.get_value();
			int max = movement_bhop_legit_ground_dist_max.get_value();

			// auto-correct nonsense values
			if (min > max)
			{
				movement_bhop_legit_ground_dist_min.set_value(max);
			}
			if (max < min)
			{
				movement_bhop_legit_ground_dist_max.set_value(min);
			}

			CUIMenuWidgets::the().add_slider("Scroll density", "%0.0f", &movement_bhop_legit_scroll_density, NULL, "full");

			CUIMenuWidgets::the().add_listbox("Efficiency", &movement_bhop_legit_efficiency, { "Helper", "Normal", "Random FOG based" });

			CUIMenuWidgets::the().add_description_text("Helper tries to predict whenever you'll be on ground next frame - therefore can execute a perfect jump on that next frame.");
		}
		else if (movement_bhop_mode.get_value() == 1) // rage
		{
			CUIMenuWidgets::the().add_description_text(
				"Rage bhop is highly obviously to someone, who analyzes your demo - be careful where you use it. "
				"\n\nHowever, it suits well for HVH scenarious or simply for servers where no one cares about bhop hacks. "
				"It also has no-slow-down capabilities.");
		}
	});
}

void MenuChilden::Movement::Edgebug::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().eb,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("Enable on ramps", &movement_eb_enable_on_ramps);

		g_gui_widgets_i->add_separtor_with_text("Tweaking");

		CUIMenuWidgets::the().add_description_text("Each of these specified the scenario where the edgebug can happen.");

		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Intensity", "%0.1fx", &movement_eb_intensity);
		CUIMenuWidgets::the().add_slider("Fall velocity", "%0.0f units", &movement_eb_min_fall_velocity);
		CUIMenuWidgets::the().add_slider("Edge dist", "%0.0f units", &movement_eb_edge_dist);
		CUIMenuWidgets::the().add_slider("Ground dist", "%0.0f units", &movement_eb_ground_dist);
	});
}

void MenuChilden::Movement::Fastrun::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().fastrun,
	[]()
	{
		CUIMenuWidgets::the().add_checkbox("No slowdown", &movement_fastrun_no_slowdown);
		CUIMenuWidgets::the().add_slider("Max speed##0", "%0.0f u/s", &movement_fastrun_max_speed);

		g_gui_widgets_i->add_separtor_with_text("Fast walk");
		CUIMenuWidgets::the().add_checkbox("Enable", &movement_fastrun_fast_walk);
		CUIMenuWidgets::the().add_slider("Max speed##1", "%0.0f u/s", &movement_fastrun_fast_walk_max_speed);
	});
}

void MenuChilden::Movement::AutoJOF::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
	&CMovement::the().auto_jof,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Interval", "%0.0f ms", &movement_auto_jof_interval);

		g_gui_widgets_i->add_separtor_with_text("Min distance & velocity");
		CUIMenuWidgets::the().feature_enabled_section(
		&movement_auto_jof_auto,
		[]()
		{
			CUIMenuWidgets::the().add_slider("Distance", "%0.1f units", &movement_auto_jof_min_distance);
		},
		"Automatic", FALSE);

		CUIMenuWidgets::the().add_slider("Min velocity", "%0.0f u/s", &movement_auto_jof_activation_vel_min, "off");
	});
}
