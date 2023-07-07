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

VarBoolean cvar_sandbox_enable("cvar_sandbox_enable", "Enables \"lying to the server\" feature. See menu for more details.", false);
VarBoolean cvar_sandbox_monitor_server("cvar_sandbox_monitor_server", "Allows to see what cvar server requests from the client. (inside the console)", false);

// convenience macros
#define DESC(x) "Specify fake value for " x " when sent to the server."
#define DEF_VAR(name, value) VarKeyValue cvar_sandbox_##name("cvar_sandbox_" #name, DESC(#name), KeyValue(#name, #value))

// fps
DEF_VAR(fps_max, 100);
DEF_VAR(fps_override, 0);
DEF_VAR(gl_vsync, 0);
DEF_VAR(developer, 0);

// movement
DEF_VAR(cl_movespeedkey, 0.52);
DEF_VAR(cl_forwardspeed, 400);
DEF_VAR(cl_sidespeed, 400);
DEF_VAR(cl_upspeed, 320);
DEF_VAR(cl_yawspeed, 210);
DEF_VAR(cl_pitchspeed, 225);

// prediction
DEF_VAR(cl_lc, 1);
DEF_VAR(cl_lw, 1);

// other
DEF_VAR(cl_minmodels, 0);

// networking
DEF_VAR(cl_cmdrate, 101);
//VarKeyValue cvar_sandbox_("cvar_sandbox_fps_override", DESC("fps_override"), KeyValue("fps_override", "0"));
//VarKeyValue cvar_sandbox_("cvar_sandbox_fps_override", DESC("fps_override"), KeyValue("fps_override", "0"));
//VarKeyValue cvar_sandbox_("cvar_sandbox_fps_override", DESC("fps_override"), KeyValue("fps_override", "0"));
//VarKeyValue cvar_sandbox_("cvar_sandbox_fps_override", DESC("fps_override"), KeyValue("fps_override", "0"));

void CCvarSandbox::initialize()
{
	// fps
	m_variables.push_back(&cvar_sandbox_fps_max);
	m_variables.push_back(&cvar_sandbox_fps_override);
	m_variables.push_back(&cvar_sandbox_gl_vsync);
	m_variables.push_back(&cvar_sandbox_developer);

	// movement
	m_variables.push_back(&cvar_sandbox_cl_movespeedkey);
	m_variables.push_back(&cvar_sandbox_cl_forwardspeed);
	m_variables.push_back(&cvar_sandbox_cl_sidespeed);
	m_variables.push_back(&cvar_sandbox_cl_upspeed);
	m_variables.push_back(&cvar_sandbox_cl_yawspeed);
	m_variables.push_back(&cvar_sandbox_cl_pitchspeed);

	// prediction
	m_variables.push_back(&cvar_sandbox_cl_lc);
	m_variables.push_back(&cvar_sandbox_cl_lw);

	// other
	m_variables.push_back(&cvar_sandbox_cl_minmodels);

	// networking
	m_variables.push_back(&cvar_sandbox_cl_cmdrate);
}

const char* CCvarSandbox::filter_cvarvalue(const char* cvar_name)
{
	// TODO: Make this list dynamic, not hardcoded...

	if (cvar_sandbox_enable.get_value())
	{
		for (const auto& var : m_variables)
		{
			if (!stricmp(cvar_name, var->get_value().key().c_str()))
			{
				return var->get_value().value().c_str();
			}
		}
	}

	// send original cvar value
	return nullptr;
}

void CCvarSandbox::render_ui()
{
	static bool once = false;
	if (!once)
	{
		initialize();
		once = true;
	}

	g_gui_widgets_i->add_child(
		"server_liar_entries", 
		{ -1.0f, -1.0f - 40.0f }, 
		true, ImGuiWindowFlags_None,
		[this]()
		{
			g_gui_widgets_i->add_table(
				"server_liar_table", 3,
				ImGuiTableFlags_HeaderTextOnly,
				[&]()
				{
					static auto column_flags = ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize;
					g_gui_widgets_i->table_setup_column_fixed_width("Cvar name", 200.0f, column_flags);
					g_gui_widgets_i->table_setup_column_fixed_width("Value", 130.0f, column_flags);

					g_gui_widgets_i->table_headers_row();
				},
				[&]()
				{
					for (const auto& var : m_variables)
					{
						render_cvar_sandbox_slot(var);
					}
				});
		});

	g_gui_widgets_i->add_padding({ 0.0f, 5.0f });

	if (keep_status_alive())
	{
		auto color = CColor(0, 112, 0, 170);

		g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_ProggyClean, FSZ_16px, FDC_Regular));
		g_gui_widgets_i->add_colored_text(color, m_current_status);
		g_gui_widgets_i->pop_font();
	}
}

void CCvarSandbox::render_cvar_sandbox_slot(VarKeyValue* var)
{
	g_gui_widgets_i->table_next_column();

	std::string input1_placeholder_name = std::string(var->get_name()) + "name";
	std::string input2_placeholder_name = std::string(var->get_name()) + "val";

#if 0
	bool reclaim_focus_value = false;
	static char buffer_name[256];
	if (g_gui_widgets_i->add_text_input_ex(input1_placeholder_name, buffer_name, sizeof(buffer_name),
										   Vector2D(-1.0f, 0.0f)))
	{
		reclaim_focus_value = true;
	};

	if (reclaim_focus_value)
	{
		g_gui_widgets_i->set_item_default_focus();
		g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
	}
#else
	// TODO: Make this more customizable, e.g. add custom cvars...

	g_gui_widgets_i->add_text(var->get_value().key());
#endif

	g_gui_widgets_i->table_next_column();

	// unique insertion
	size_t value = 0;
	try
	{
		// this should be O(1), but hehe, whatever..
		value = m_filtered_cvars.at(var->get_value().key());
	}
	catch (...)
	{
		static size_t index_counter = 0;
		auto [iter, did_insert] = m_filtered_cvars.insert(std::make_pair(var->get_value().key(), index_counter++));
		assert(index_counter < k_max_filtered_cvars && "There cannot be more than k_max_filtered_cvars entries!");
		value = iter->second;

		strcpy_s(m_input_buffers[value], var->get_value().value().c_str());
	}
	//auto& value = m_filtered_cvars[var->get_name()];
	//auto& value = var->get_value().value();

	bool reclaim_focus_key = false;
	if (g_gui_widgets_i->add_text_input_ex(input2_placeholder_name, m_input_buffers[value], sizeof(m_input_buffers[0]),
										   Vector2D(-1.0f, 0.0f)))
	{
		reclaim_focus_key = true;
	};

	// Auto-focus on window apparition
	g_gui_widgets_i->set_item_default_focus();
	if (reclaim_focus_key)
	{
		g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
	}

	g_gui_widgets_i->table_next_column();

	if (g_gui_widgets_i->add_button(std::format("Apply ##{}", var->get_value().key()), { -1.0f, 25.0f }, false, BUTTONFLAG_CenterLabel))
	{
		if (/*buffer_name[0] && */m_input_buffers[value][0])
		{
			var->set_value(KeyValue(/*buffer_name*/var->get_value().key(), m_input_buffers[value]));

			// update status text
			m_current_status = std::format("Set fake value of '{}' to '{}'", var->get_value().key(), m_input_buffers[value]);
			m_status_update_ms = GetTickCount();
		}
	}
}
