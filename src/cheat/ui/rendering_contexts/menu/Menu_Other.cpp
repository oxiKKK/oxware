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

VarBoolean ui_menu_resizable("ui_menu_resizable", "This is a beta feature!", false);

void MenuChilden::Other::VariableList::contents()
{
	static char buffer[256];
	static bool once = false;
	if (!once)
	{
		strcpy_s(buffer, "");
		once = true;
	}

	if (g_gui_widgets_i->add_text_input_ex("Search for a variable", buffer, sizeof(buffer), Vector2D(-1.0f, 0.0f)))
	{
	};

	bool should_filter = buffer[0];

	g_gui_widgets_i->add_table(
		"variable_list", 7,
		ImGuiTableFlags_HeaderTextOnly | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
		[]()
		{
			static auto flags = ImGuiTableColumnFlags_NoReorder;

			float value_w = 70.0f;
			g_gui_widgets_i->table_setup_column_fixed_width("", 25.0f, flags);
			g_gui_widgets_i->table_setup_column("Name", flags);
			g_gui_widgets_i->table_setup_column_fixed_width("Value", value_w, flags);
			g_gui_widgets_i->table_setup_column_fixed_width("Default", value_w, flags);
			g_gui_widgets_i->table_setup_column_fixed_width("Min", value_w, flags);
			g_gui_widgets_i->table_setup_column_fixed_width("Max", value_w, flags);
			g_gui_widgets_i->table_setup_column("Description", flags);
			
			g_gui_widgets_i->table_headers_row();
		},
		[&]()
		{
			auto small_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular);
			g_gui_widgets_i->push_font(small_font);

			int n = 0;
			g_variablemgr_i->for_each_variable(
				[&](BaseVariable* var)
				{
					if (should_filter && !strstr(var->get_name(), buffer))
					{
						return;
					}

					static auto text_props = TEXTPROP_ColorBlack;

					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(std::format("{}", ++n), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_name(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_value_string(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_default_value_string(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_min_string(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_max_string(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(var->get_description(), text_props);
				});

			g_gui_widgets_i->pop_font();

		});
}

void MenuChilden::Other::CommandList::contents()
{
	static char buffer[256];
	static bool once = false;
	if (!once)
	{
		strcpy_s(buffer, "");
		once = true;
	}

	if (g_gui_widgets_i->add_text_input_ex("Search for a variable", buffer, sizeof(buffer),
										   Vector2D(-1.0f, 0.0f)))
	{
	};

	bool should_filter = buffer[0];

	g_gui_widgets_i->add_table(
		"command_list", 4,
		ImGuiTableFlags_HeaderTextOnly | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
		[]()
		{
			static auto flags = ImGuiTableColumnFlags_NoReorder;

			g_gui_widgets_i->table_setup_column_fixed_width("", 25.0f, flags);
			g_gui_widgets_i->table_setup_column("Name", flags);
			g_gui_widgets_i->table_setup_column("Usage", flags);
			g_gui_widgets_i->table_setup_column("Description", flags);

			g_gui_widgets_i->table_headers_row();
		},
		[&]()
		{
			auto small_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular);
			g_gui_widgets_i->push_font(small_font);

			int n = 0;
			g_variablemgr_i->for_each_command(
				[&](BaseCommand* cmd)
				{
					if (should_filter && !strstr(cmd->get_name(), buffer))
					{
						return;
					}

					static auto text_props = TEXTPROP_ColorBlack;

					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(std::format("{}", ++n), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(cmd->get_name(), text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(cmd->has_usage() ? cmd->get_usage() : "", text_props);
					g_gui_widgets_i->table_next_column();
					g_gui_widgets_i->add_text(cmd->get_description(), text_props);
				});

			g_gui_widgets_i->pop_font();
		});
}

void MenuChilden::Other::UI::contents()
{
	CUIMenuWidgets::the().add_checkbox("Enable rain", &ui_background_rain);
	CUIMenuWidgets::the().add_checkbox("Enable background fade", &ui_background_fade);

	g_gui_widgets_i->add_separtor_with_text("Feature list");
	CUIMenuWidgets::the().feature_enabled_section(
	&ui_render_feature_list,
	[]()
	{
		CUIMenuWidgets::the().add_listbox("Font type", &ui_feature_list_font, { "Bold", "Regular" });
		CUIMenuWidgets::the().add_listbox("Font size", &ui_feature_list_font_size, { "Smaller", "Bigger" });

		CUIMenuWidgets::the().add_checkbox("Check for overlow", &ui_feature_list_overflow);
	}, "Show");

	g_gui_widgets_i->add_separtor_with_text("Fonts");

	static bool no_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_NoHinting;
	static bool no_auto_hint = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_NoAutoHint;
	static bool force_auto_hint = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_ForceAutoHint;
	static bool light_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_LightHinting;
	static bool mono_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_MonoHinting;

	if (g_gui_widgets_i->add_checkbox("No hinting", &no_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_NoHinting, no_hinting);
	if (g_gui_widgets_i->add_checkbox("No auto-hint", &no_auto_hint)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_NoAutoHint, no_auto_hint);
	if (g_gui_widgets_i->add_checkbox("Force auto-hint", &force_auto_hint)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_ForceAutoHint, force_auto_hint);
	if (g_gui_widgets_i->add_checkbox("Light hinting", &light_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_LightHinting, light_hinting);
	if (g_gui_widgets_i->add_checkbox("Mono hinting", &mono_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_MonoHinting, mono_hinting);
}

void MenuChilden::Other::Storage::contents()
{
	auto font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	auto cheat_directory = g_filesystem_i->get_appdata_path();

	static uintmax_t cheat_directory_size = 0, logs_directory_size = 0, cfg_directory_size = 0;

	static uint32_t t1 = GetTickCount();

	if (GetTickCount() - t1 > 250)
	{
		cheat_directory_size = g_filesystem_i->directory_size(cheat_directory);
		logs_directory_size = g_filesystem_i->directory_size(cheat_directory / "log");
		cfg_directory_size = g_filesystem_i->directory_size(cheat_directory / "config");

		t1 = GetTickCount();
	}

	g_gui_widgets_i->add_text(std::format("Currently occupied space on your computer: {}", 
										  CStringTools::the().pretify_file_size((float)cheat_directory_size)), TEXTPROP_Wrapped, font);

	g_gui_widgets_i->add_separator();
	
	g_gui_widgets_i->add_text(std::format("logs: {}",
										  CStringTools::the().pretify_file_size((float)logs_directory_size)), TEXTPROP_Wrapped, font);
	g_gui_widgets_i->add_text(std::format("configs: {}", 
										  CStringTools::the().pretify_file_size((float)cfg_directory_size)), TEXTPROP_Wrapped, font);

	g_gui_widgets_i->add_separator();

	if (g_gui_widgets_i->add_button("open cheat storage directory", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
	{
		CGenericUtil::the().open_folder_inside_explorer(cheat_directory.string());
	}

	if (g_gui_widgets_i->add_button("clear unnecessary files", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
	{
		g_filesystem_i->iterate_through_files(
			cheat_directory / "log", false, 
			[](const std::filesystem::path& dir)
			{
				if (g_filesystem_i->is_file(dir))
				{
					g_filesystem_i->remove(dir);
				}
			});
	}

	VarInteger* var = (VarInteger*)g_variablemgr_i->query_variable("num_logfiles_to_keep");

	if (var)
	{
		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Logfiles to keep", "%0.0f", var);
	}
}

void MenuChilden::Other::Debug::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&debug,
	[]()
	{
		CUIMenuWidgets::the().add_description_text("This feature is used for developement only. You have been warned.");

		g_gui_widgets_i->begin_tab("debug_tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		float tab_height = -1.0f;

		g_gui_widgets_i->add_tab_item(
			"Rendering", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&debug_render_info,
				[]()
				{
					CUIMenuWidgets::the().add_checkbox("Misc", &debug_render_info_misc);
					g_gui_widgets_i->add_spacing();
					g_gui_widgets_i->add_separtor_with_text("Movement");
					g_gui_widgets_i->add_spacing();
					CUIMenuWidgets::the().add_checkbox("Bhop", &debug_render_info_movement_bhop);
					CUIMenuWidgets::the().add_checkbox("Strafe hack", &debug_render_info_movement_strafe);
					CUIMenuWidgets::the().add_checkbox("Strafe helper", &debug_render_info_movement_strafe_helper);
					CUIMenuWidgets::the().add_checkbox("Players", &debug_render_player_info);
				});
			});

		g_gui_widgets_i->end_tab();
	});
}
