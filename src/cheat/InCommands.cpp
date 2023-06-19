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

IInCommands* g_in_commands_i = nullptr;

class CInCommands : public IInCommands
{
public:
	CInCommands();
	~CInCommands();

	void initialize();
	void update();

	void update_activation_conditions();

	void render_interactible_incommand_list();

	void add_command(BaseInCommand* in_cmd);

	void register_incommands_per_module(StaticInCommandContainer* incommand_container, const char* module_name);

	void create_incommands_from_json(const nlohmann::json& json);
	void export_incommands_to_json(nlohmann::json& json);

	void for_each_incommand(const std::function<void(BaseInCommand* in_cmd)>& callback);

	BaseInCommand* get_incommand(const std::string& id);

	bool is_key_bound_and_active(int vk);

	bool does_meet_activation_conditions(EActivationCondition act_cond);

	void add_keyscan_button(BaseInCommand* in_cmd, const Vector2D& size);

private:
	void end_key_binding_mode(int vk_pressed);
	BaseInCommand* m_in_cmd_to_be_rebound;

	void provide_cfg_load_export_callbacks();

private:
	std::unordered_map<std::string, BaseInCommand*> m_in_commands;

	EActivationCondition m_activation_conditions_for_this_frame = IN_ACTCOND_None;
};

CInCommands g_in_commands;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInCommands, IInCommands, IINCOMMANDS_INTERFACEID, g_in_commands);

//-------------------------------------------------------------------------------
//
// CInCommands implementation
//
//-------------------------------------------------------------------------------

CInCommands::CInCommands()
{
	g_in_commands_i = this;
}

CInCommands::~CInCommands()
{
	g_in_commands_i = nullptr;
}

void CInCommands::initialize()
{
	register_incommands_per_module(&g_static_incommand_container, MODULE_CHEAT);

	for (const auto& [name, in_cmd] : m_in_commands)
	{
		in_cmd->register_cmd();
	}

	CConsole::the().info("Registered {} InCommands.", m_in_commands.size());

	provide_cfg_load_export_callbacks();
}

void CInCommands::update_activation_conditions()
{
	m_activation_conditions_for_this_frame = IN_ACTCOND_None;

	if (CGameUtil::the().is_fully_connected())
	{
		m_activation_conditions_for_this_frame |= IN_ACTCOND_Connected;
	}

	auto local = CEntityMgr::the().get_local_player();
	if (local && local->is_valid() && local->is_alive())
	{
		m_activation_conditions_for_this_frame |= IN_ACTCOND_Alive;
	}
}

void CInCommands::add_command(BaseInCommand* in_cmd)
{
	m_in_commands.insert({ in_cmd->get_name(), in_cmd });

	CConsole::the().info("Added InCommand: '{}'.", in_cmd->get_name());
}

void CInCommands::register_incommands_per_module(StaticInCommandContainer* incommand_container, const char* module_name)
{
	auto& list = incommand_container->get_incommand_list();

	if (list.empty())
	{
		CConsole::the().info("Module {} has no incommands to be registered.", module_name);
		return;
	}

	CConsole::the().info("Registering {} incommands from module {}...", list.size(), module_name);

	// Merge vars from other modules into one continuous list.
	for (BaseInCommand* var : list)
	{
		add_command(var);
	}
}

void CInCommands::create_incommands_from_json(const nlohmann::json& json)
{
	const nlohmann::json* in_commands = nullptr;

	try
	{
		in_commands = &json.at("in_commands");
	}
	catch (...)
	{
		// there aren't any incommands
		return;
	}

	/*
		"in_commands": {
			"movement_bhop": {
				"key":	"space"
			}, 
			...
		}
	*/

	for (auto& [incmd_name, in_cmd] : in_commands->items())
	{
		std::string key_str;

		auto try_to_parse = [&](const std::string& what, std::string& output, bool doesnt_matter = false) {
			try
			{
				auto& key = in_cmd.at(what);
				output = key.get<std::string>();
				return true;
			}
			catch (...)
			{
				if (!doesnt_matter)
				{
					CConsole::the().error("Error parsing '{}' field for key '{}'", what, incmd_name);
				}
				output = "";
				return false;
			}
		};

		// commnad
		if (!try_to_parse("key", key_str))
		{
			continue;
		}

		auto in_cmd = get_incommand(incmd_name);
		if (!in_cmd)
		{
			continue;
		}

		int vk = NULL;
		if (key_str != "unbound")
		{
			vk = g_user_input_i->string_to_virtual_key(key_str);
			if (vk != NULL)
			{
				in_cmd->rebind_key_to(vk);
			}
		}
	}
}

void CInCommands::export_incommands_to_json(nlohmann::json& json)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		int vk = in_cmd->get_key_bound();
		if (vk == NULL) // unbound
		{
			json["in_commands"][name]["key"] = "unbound";
			continue;
		}

		json["in_commands"][name]["key"] = g_user_input_i->virtual_key_to_string(vk);
	}
}

void CInCommands::for_each_incommand(const std::function<void(BaseInCommand*in_cmd)>& callback)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		callback(in_cmd);
	}
}

BaseInCommand* CInCommands::get_incommand(const std::string& id)
{
	try
	{
		return m_in_commands.at(id);
	}
	catch (...)
	{
		CConsole::the().error("Unknown InCommand: '{}'", id);
		return nullptr;
	}
}

bool CInCommands::is_key_bound_and_active(int vk)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		if (in_cmd->get_key_bound() == vk && in_cmd->is_active() && does_meet_activation_conditions(in_cmd->get_activation_condition()))
		{
			return true;
		}
	}

	return false;
}

void CInCommands::update()
{
	// We can check this way if we started the key binding mode from our code.
	// Because it could be started elsewhere, e.g. bindmanager code.
	bool did_start_key_binding_mode = m_in_cmd_to_be_rebound != nullptr;

	if (did_start_key_binding_mode)
	{
		int vk_pressed = g_user_input_i->get_bound_key();
		if (vk_pressed != NULL && g_user_input_i->is_in_key_binding_mode())
		{
			// a key was just bound from the UserInput code

			end_key_binding_mode(vk_pressed);
		}
	}
}

void CInCommands::add_keyscan_button(BaseInCommand* in_cmd, const Vector2D& size)
{
	std::string key_name;
	
	int vk_bound = in_cmd->get_key_bound();
	if (vk_bound != NULL)
	{
		key_name = g_user_input_i->virtual_key_to_string(in_cmd->get_key_bound());
	}
	else
	{
		key_name = "unbound";
	}

	bool b = g_gui_widgets_i->add_button(std::format("{}##{}", key_name, in_cmd->get_name()), size, false, BUTTONFLAG_CenterLabel);
	if (b)
	{
		assert(m_in_cmd_to_be_rebound == nullptr && "InCommand binding wasn't finished! m_in_cmd_to_be_rebound isn't null!");

		m_in_cmd_to_be_rebound = in_cmd;
		g_user_input_i->activate_key_binding_mode();
	}
}

void CInCommands::end_key_binding_mode(int vk_pressed)
{
	if (vk_pressed != VK_LBUTTON && vk_pressed != VK_ESCAPE)
	{
		m_in_cmd_to_be_rebound->rebind_key_to(vk_pressed);
	}

	m_in_cmd_to_be_rebound = nullptr;
	g_user_input_i->reset_bound_key();
}

void CInCommands::provide_cfg_load_export_callbacks()
{
	auto cheat_settings = g_config_mgr_i->query_config_file_type("cheat_settings");

	//
	// provide config load function.
	//

	auto load_fn = [](nlohmann::json& json)
	{
		g_in_commands_i->create_incommands_from_json(json);
	};

	cheat_settings->provide_load_fn(load_fn);

	//
	// provide config export function.
	//

	auto export_fn = [](nlohmann::json& json)
	{
		g_in_commands_i->export_incommands_to_json(json);
	};

	cheat_settings->provide_export_fn(export_fn);
}

bool CInCommands::does_meet_activation_conditions(EActivationCondition act_cond)
{
	return m_activation_conditions_for_this_frame == act_cond;
}

void CInCommands::render_interactible_incommand_list()
{
	auto last_cursor_pos = g_gui_widgets_i->get_cursor_pos();

	CUIMenuWidgets::the().add_menu_child(
		"InCommands", CMenuStyle::child_full_width(-1.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			g_gui_widgets_i->add_spacing();

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_CellPadding, { 2.0f, 1.0f });

			if (g_gui_widgets_i->begin_columns("incommands_column", 4))
			{
				g_gui_widgets_i->setup_column_fixed_width(100.0f);
				g_gui_widgets_i->setup_column_fixed_width(50.0f);
				g_gui_widgets_i->setup_column_fixed_width(50.0f);
				g_gui_widgets_i->setup_column_fixed_width(283.0f);

				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Key name");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Enable");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Always");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Command");

				g_gui_widgets_i->end_columns();
			}

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

			g_gui_widgets_i->add_separator();

			g_gui_widgets_i->add_child(
				"incommands_list", Vector2D(-1.0f, -1.0f), false, ImGuiWindowFlags_None,
				[&]()
				{
					if (g_gui_widgets_i->begin_columns("binds_column_nested", 4))
					{
						g_gui_widgets_i->setup_column_fixed_width(100.0f);
						g_gui_widgets_i->setup_column_fixed_width(50.0f);
						g_gui_widgets_i->setup_column_fixed_width(50.0f);
						g_gui_widgets_i->setup_column_fixed_width(285.0f);

						for (const auto& [name, in_cmd] : m_in_commands)
						{
							g_gui_widgets_i->goto_next_column();

							int vk = in_cmd->get_key_bound();

							add_keyscan_button(in_cmd, Vector2D(-1.0f, 25.0f));

							g_gui_widgets_i->goto_next_column();

							auto toggle_var = in_cmd->get_toggle_var();
							CUIMenuWidgets::the().add_checkbox(std::format("##{}_0", name), toggle_var);

							g_gui_widgets_i->goto_next_column();
							
							auto always_enabled_var = in_cmd->get_always_enabled_var();
							CUIMenuWidgets::the().add_checkbox(std::format("##{}_1", name), always_enabled_var);

							g_gui_widgets_i->goto_next_column();

							g_gui_widgets_i->add_text(name);
						}

						g_gui_widgets_i->end_columns();
					}
				});

			g_gui_widgets_i->pop_stylevar();
			g_gui_widgets_i->pop_stylevar();
		});

#if 0
	if (g_gui_widgets_i->add_floating_button("?", last_cursor_pos, { window_size.x - 23.0f - button_size.x - 3.0f - button_size.x, 30.0f },
											 button_size, false, BUTTONFLAG_CenterLabel))
	{
		COxWareUI::the().schedule_popup(
			"InCommands help", { 450, 400 },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("What is it");
			});
	}
#endif
}
