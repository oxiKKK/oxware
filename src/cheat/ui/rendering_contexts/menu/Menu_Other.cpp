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

void MenuChilden::Other::VariableList::contents()
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
		"variable_list", 7,
		ImGuiTableFlags_HeaderTextOnly | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
		[]()
		{
	static auto flags = ImGuiTableColumnFlags_NoReorder;

	g_gui_widgets_i->table_setup_column_fixed_width("N", 25.0f, flags);
	g_gui_widgets_i->table_setup_column("Name", flags);
	g_gui_widgets_i->table_setup_column_fixed_width("Value", 60.0f, flags);
	g_gui_widgets_i->table_setup_column_fixed_width("Default", 60.0f, flags);
	g_gui_widgets_i->table_setup_column_fixed_width("Min", 60.0f, flags);
	g_gui_widgets_i->table_setup_column_fixed_width("Max", 60.0f, flags);
	g_gui_widgets_i->table_setup_column("Description", flags);
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

			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(std::format("{}", ++n));
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_name());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_value_string());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_default_value_string());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_min_string());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_max_string());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(var->get_description());
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

	g_gui_widgets_i->table_setup_column_fixed_width("N", 25.0f, flags);
	g_gui_widgets_i->table_setup_column("Name", flags);
	g_gui_widgets_i->table_setup_column("Usage", flags);
	g_gui_widgets_i->table_setup_column("Description", flags);
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

			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(std::format("{}", ++n));
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(cmd->get_name());
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(cmd->has_usage() ? cmd->get_usage() : "");
			g_gui_widgets_i->table_next_column();
			g_gui_widgets_i->add_text(cmd->get_description());
				});

			g_gui_widgets_i->pop_font();

		});
}