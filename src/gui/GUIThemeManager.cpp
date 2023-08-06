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

// We'll use internal imgui here
#include <imgui_internal.h>

struct PushedColor
{
	EGUIColorId id;
	CColor original_color;
};

IGUIThemeManager* g_gui_thememgr_i = nullptr;

class CGUIThemeManager : public IGUIThemeManager, IConfigIOOperations
{
public:
	CGUIThemeManager();
	~CGUIThemeManager();

	void initialize();
	void load_font_from_file_if_present();

	std::optional<const CGUITheme*> get_theme_by_id(const std::string& id);

	bool does_theme_exist(const std::string& id);

	CGUITheme& create_theme(const std::string& id);

	bool set_new_theme(const std::string& id);

	CGUITheme* get_current_theme();
	std::string get_current_theme_name();

	void push_color(EGUIColorId id, const CColor& color);
	void pop_color(size_t amount = 1);

	void load_theme_from_json(const nh::json& json);
	void export_theme_to_json(nh::json& json);

	void set_current_theme_export_name(const std::string& name);
	void reset_current_theme_export_name();

	EGUIColorId string_to_color_id(const std::string& id);
	std::string color_id_to_string(EGUIColorId id);

	void initialize_imgui();

	std::string extract_theme_name_from_file(const std::filesystem::path& theme_file);

	void sync_colors_with_imgui();

	size_t get_num_themes();

	void for_each_built_in(const std::function<void(const std::string& id, const CGUITheme& theme)>& callback);

	bool is_builtin_theme(const std::string& id);

private:
	void initialize_default_themes();

	// config I/O provider
	void provide_cfg_load_export_callbacks();

private:
	// theme name and object
	std::pair<std::string, CGUITheme*> m_current_theme; // always set

	std::unordered_map<std::string, CGUITheme> m_themes;

	std::vector<PushedColor> m_pushed_colors;
	std::unordered_map<EGUIColorId, CColor> m_original_colors;

	// translation lookup cache tables
	std::unordered_map<std::string, EGUIColorId> m_string_to_id;
	std::unordered_map<EGUIColorId, std::string> m_id_to_string;
	
	std::vector<std::string> m_builtin_themes;

	std::string m_current_exported_theme_name;

	void create_translation_lookup_cache_tables();

	bool m_imgui_initialized = false;
};

CGUIThemeManager g_gui_thememgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGUIThemeManager, IGUIThemeManager, IGUITHEMEMANAGER_INTERFACEID, g_gui_thememgr);

//-------------------------------------------------------------------------------
//
// CGUIThemeManager implementation
// 
//-------------------------------------------------------------------------------

CGUIThemeManager::CGUIThemeManager()
{
	g_gui_thememgr_i = this;
}

CGUIThemeManager::~CGUIThemeManager()
{
	g_gui_thememgr_i = nullptr;
}

void CGUIThemeManager::initialize()
{
	// do this as a first thing
	create_translation_lookup_cache_tables();

	initialize_default_themes();

	// set default theme before we load the saved one.
	set_new_theme("white");

	g_config_mgr_i->provide_on_save_callback(
		[]()
		{
			auto current_theme_name = g_gui_thememgr_i->get_current_theme_name();
			g_registry_i->write_string(REG_OXWARE, "theme_file", current_theme_name.c_str());
		});

	provide_cfg_load_export_callbacks();

	CConsole::the().info("GUI ThemeManager initialized.");
}

void CGUIThemeManager::load_font_from_file_if_present()
{
	// look for saved font
	const char* theme_file = g_registry_i->read_string(REG_OXWARE, "theme_file");
	if (theme_file != nullptr)
	{
		// look for this first
		auto built_in_theme = get_theme_by_id(theme_file);
		if (built_in_theme)
		{
			set_new_theme(theme_file);
		}
		else
		{
			// look for a file
			std::filesystem::path theme_path = "theme\\" + std::string(theme_file);
			g_config_mgr_i->fix_config_file_extension(theme_path);
			if (!g_config_mgr_i->load_configuration(CFG_CheatTheme, theme_path.string()))
			{
				// default to white...
				set_new_theme("white");
			}
		}
	}
}

std::optional<const CGUITheme*> CGUIThemeManager::get_theme_by_id(const std::string& id)
{
	try
	{
		return &m_themes.at(id);
	}
	catch (...)
	{
		return std::nullopt;
	}
}

bool CGUIThemeManager::does_theme_exist(const std::string& id)
{
	return m_themes.find(id) != m_themes.end();
}

CGUITheme& CGUIThemeManager::create_theme(const std::string& id)
{
	// just create a blank theme object
	return m_themes[id];
}

CGUITheme* CGUIThemeManager::get_current_theme()
{
	// should not return nullptr.
	assert(m_current_theme.second && "Current theme pointer wasn't initialized yet.");
	return m_current_theme.second;
}

std::string CGUIThemeManager::get_current_theme_name()
{
	return m_current_theme.first;
}

bool CGUIThemeManager::set_new_theme(const std::string& id)
{
	auto theme = get_theme_by_id(id);
	if (!theme)
	{
		CConsole::the().error("Unknown theme ID: {}", id);
		return false;
	}

	m_current_theme = std::make_pair(id, (CGUITheme*)theme.value());
	sync_colors_with_imgui();
	return true;
}

void CGUIThemeManager::push_color(EGUIColorId id, const CColor& color)
{
	m_pushed_colors.push_back({ id, get_current_theme()->get_color(id) });
	get_current_theme()->set_color(id, color);
}

void CGUIThemeManager::pop_color(size_t amount)
{
	assert(amount <= m_pushed_colors.size());

	while (amount--)
	{
		auto last = m_pushed_colors.back();
		get_current_theme()->set_color(last.id, last.original_color);
		m_pushed_colors.pop_back();
	}
}

void CGUIThemeManager::load_theme_from_json(const nh::json& json)
{
	/*
	"theme_name": "blahblah",
	"colors": {
		"window_bg": ...
		...
	}
	*/

	std::string theme_name = "";

	try
	{
		theme_name = json.at("theme_name").get<std::string>();
	}
	catch (...)
	{
	}

	if (theme_name.empty())
	{
		CConsole::the().error("Got empty theme name.");
		return;
	}

	if (does_theme_exist(theme_name) && is_builtin_theme(theme_name))
	{
		CConsole::the().warning("Theme with same name ({}) already exist", theme_name);
		return;
	}

	auto& new_theme = create_theme(theme_name);

	try
	{
		auto& colors = json.at("colors");

		if (colors.empty())
		{
			CConsole::the().error("Got theme '{}' with no colors.", theme_name);
			return;
		}

		for (auto& [key, value] : colors.items())
		{
			if (key == "invalid")
			{
				continue; // skip the invalid entry.
			}

			CColor color = value.get<CColor>();

			CConsole::the().dinfo("'{}: {}'", key, color);

			EGUIColorId id = string_to_color_id(key);
			if (id == GUICLR_Invalid)
			{
				CConsole::the().error("Unrecognized color: '{}'", key);
				continue;
			}

			// re-set colors inside the list
			new_theme.initialize_colors(id, color);
		}

		set_new_theme(theme_name);
		sync_colors_with_imgui();
	}
	catch (const nh::json::exception& e)
	{
		CConsole::the().error("JSON: {}", e.what());
		return;
	}
}

void CGUIThemeManager::export_theme_to_json(nh::json& json)
{
	/*
	"theme_name": "blahblah",
	"colors": {
		"window_bg": ...
		...
	}
	*/

	if (m_current_exported_theme_name.empty())
	{
		// should not happen. current theme should be ALWAYS set.
		return;
	}

	json["theme_name"] = m_current_exported_theme_name;

	try
	{
		auto current_theme = get_current_theme();

		for (size_t i = GUICLR_TextLight; i < GUICLR_MAX; i++)
		{
			auto id = (EGUIColorId)i;
			auto color = current_theme->get_immutable_color(id);
			std::string name = color_id_to_string(id); // cannot possibly fail (I hope :D)

			json["colors"][name] = color;
		}
	}
	catch (const nh::json::exception& e)
	{
		CConsole::the().error("JSON: {}", e.what());
		return;
	}
}

void CGUIThemeManager::set_current_theme_export_name(const std::string& name)
{
	if (m_current_exported_theme_name.empty()) // only if hasn't been set already.
	{
		m_current_exported_theme_name = name;
	}
}

void CGUIThemeManager::reset_current_theme_export_name()
{
	m_current_exported_theme_name.clear();
}

EGUIColorId CGUIThemeManager::string_to_color_id(const std::string& id)
{
	try
	{
		return m_string_to_id.at(id);
	}
	catch (...)
	{
		return GUICLR_Invalid;
	}
}

std::string CGUIThemeManager::color_id_to_string(EGUIColorId id)
{
	try
	{
		return m_id_to_string.at(id);
	}
	catch (...)
	{
		return "";
	}
}

void CGUIThemeManager::initialize_imgui()
{
	m_imgui_initialized = true; // must be set before the call!

	sync_colors_with_imgui();

	// init imgui style
	auto& style = ImGui::GetStyle();
	style.ScrollbarSize = 9.0f;
	style.ScrollbarRounding = 8.0f;
	style.GrabMinSize = 12.0f;
	style.ChildRounding = style.FrameRounding = style.PopupRounding = 4.0f;
	style.WindowPadding = { 10.f, 15.f };
}

std::string CGUIThemeManager::extract_theme_name_from_file(const std::filesystem::path& theme_file)
{
	std::ifstream ifs(theme_file);

	if (!ifs.good())
	{
		CConsole::the().error("Could not open the theme file.");
		return "";
	}

	nh::json json;

	try
	{
		json = nh::json::parse(ifs);
	}
	catch (const nh::json::exception& e)
	{
		CConsole::the().error("Couldn't parse the json object using the theme file.");
		return "";
	}

	ifs.close(); // we're done reading the file

	// now read the theme name
	std::string theme_name;
	try
	{
		theme_name = json.at("theme_name");
	}
	catch (const nh::json::exception& e)
	{
		CConsole::the().error("The 'theme_name' field isn't inside the json file.");
		return "";
	}

	return theme_name;
}

void CGUIThemeManager::sync_colors_with_imgui()
{
	if (!m_imgui_initialized)
	{
		// cannot call this before imgui, since it needs to be called after ImGui::CreateContext().
		return;
	}

	auto& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = get_current_theme()->get_color<GUICLR_TextDark>();

	style.Colors[ImGuiCol_WindowBg] = get_current_theme()->get_color<GUICLR_WindowBackground>();
	style.Colors[ImGuiCol_ScrollbarBg] = 
		style.Colors[ImGuiCol_PopupBg] = 
		style.Colors[ImGuiCol_ChildBg] = get_current_theme()->get_color<GUICLR_ChildBackground>();

	style.Colors[ImGuiCol_FrameBg] = get_current_theme()->get_color<GUICLR_FrameBg>();
	style.Colors[ImGuiCol_FrameBgHovered] = get_current_theme()->get_color<GUICLR_FrameBgHovered>();
	style.Colors[ImGuiCol_FrameBgActive] = get_current_theme()->get_color<GUICLR_FrameBgActive>();

	style.Colors[ImGuiCol_ScrollbarGrab] = get_current_theme()->get_color<GUICLR_ScrollbarGrab>();

	style.Colors[ImGuiCol_Tab] = get_current_theme()->get_color<GUICLR_Tab>();
	style.Colors[ImGuiCol_TabHovered] = get_current_theme()->get_color<GUICLR_TabHovered>();
	style.Colors[ImGuiCol_TabActive] = get_current_theme()->get_color<GUICLR_TabActive>();

	style.Colors[ImGuiCol_ResizeGrip] = get_current_theme()->get_color<GUICLR_ResizeGrip>();
	style.Colors[ImGuiCol_ResizeGripHovered] = get_current_theme()->get_color<GUICLR_ResizeGripHovered>();
	style.Colors[ImGuiCol_ResizeGripActive] = get_current_theme()->get_color<GUICLR_ResizeGripActive>();

	style.Colors[ImGuiCol_TableHeaderBg] = get_current_theme()->get_color<GUICLR_TableHeaderBg>();
}

size_t CGUIThemeManager::get_num_themes()
{
	return m_themes.size();
}

void CGUIThemeManager::for_each_built_in(const std::function<void(const std::string&id, const CGUITheme&theme)>& callback)
{
	for (auto& [id, theme] : m_themes)
	{
		if (!is_builtin_theme(id))
		{
			continue;
		}

		callback(id, theme);
	}
}

bool CGUIThemeManager::is_builtin_theme(const std::string & id)
{
	return std::find(m_builtin_themes.begin(), m_builtin_themes.end(), id) != m_builtin_themes.end();
}

void CGUIThemeManager::provide_cfg_load_export_callbacks()
{
	auto cheat_theme = g_config_mgr_i->query_config_file_type("cheat_theme");
	if (!cheat_theme)
	{
		return;
	}

	//
	// provide config load function.
	//

	auto load_fn = [](nh::json& json)
	{
		g_gui_thememgr_i->load_theme_from_json(json);
	};

	(*cheat_theme)->provide_load_fn(load_fn);

	//
	// provide config export function.
	//

	auto export_fn = [](nh::json& json)
	{
		g_gui_thememgr_i->set_current_theme_export_name(g_gui_thememgr_i->get_current_theme_name());
		g_gui_thememgr_i->export_theme_to_json(json);
	};

	(*cheat_theme)->provide_export_fn(export_fn);
}

void CGUIThemeManager::create_translation_lookup_cache_tables()
{
	//
	// string->id
	//
	for (size_t i = 0; i < GUICLR_MAX; i++)
	{
		m_string_to_id[g_gui_color_translation[i].name] = static_cast<EGUIColorId>(i);
	}

	//
	// id->string
	//
	for (size_t i = 0; i < GUICLR_MAX; i++)
	{
		m_id_to_string[g_gui_color_translation[i].id] = g_gui_color_translation[i].name;
	}
}

void CGUIThemeManager::initialize_default_themes()
{
	auto& white = create_theme("white");
	m_builtin_themes.push_back("white");

	// text
	white.initialize_colors(GUICLR_TextLight,					CColor(161, 161, 161, 255));
	white.initialize_colors(GUICLR_TextRegular,					CColor(112, 112, 112, 255));
	white.initialize_colors(GUICLR_TextDark,					CColor(84, 84, 84, 255));
	white.initialize_colors(GUICLR_TextBlack,					CColor(44, 44, 44, 255));
	white.initialize_colors(GUICLR_TextWhite,					CColor(255, 255, 255, 255));

	// frame
	white.initialize_colors(GUICLR_FrameBg,						CColor(196, 196, 196, 255));
	white.initialize_colors(GUICLR_FrameBgHovered,				CColor(112, 112, 112, 128));
	white.initialize_colors(GUICLR_FrameBgActive,				CColor(112, 112, 112, 170));
	white.initialize_colors(GUICLR_FrameBgSelected,				CColor(112, 112, 112, 101));
	white.initialize_colors(GUICLR_FrameActive,					CColor(0, 136, 255, 255));

	// window
	white.initialize_colors(GUICLR_WindowBackground,			CColor(255, 255, 255, 255));
	white.initialize_colors(GUICLR_ChildBackground,				CColor(240, 240, 240, 255));

	// button
	white.initialize_colors(GUICLR_Button,						CColor(112, 112, 112, 51));

	// url
	white.initialize_colors(GUICLR_URLHovered,					CColor(0, 136, 255, 170));

	// scrollbar
	white.initialize_colors(GUICLR_ScrollbarGrab,				CColor(150, 150, 150, 255));

	// checkbox
	white.initialize_colors(GUICLR_CheckBoxBackground,			CColor(255, 255, 255, 255));
	white.initialize_colors(GUICLR_CheckBoxCheckMark,			CColor(255, 255, 255, 255));

	// slider
	white.initialize_colors(GUICLR_SliderHovered,				CColor(0, 136, 255, 90));

	// input text
	white.initialize_colors(GUICLR_InputTextBg,					CColor(255, 255, 255, 255));

	// separator
	white.initialize_colors(GUICLR_Separator,					CColor(196, 196, 196, 255));

	// tab
	white.initialize_colors(GUICLR_Tab,							CColor(112, 112, 112, 51));
	white.initialize_colors(GUICLR_TabHovered,					CColor(112, 112, 112, 170));
	white.initialize_colors(GUICLR_TabActive,					CColor(112, 112, 112, 128));

	// resize grip
	white.initialize_colors(GUICLR_ResizeGrip,					CColor(112, 112, 112, 51));
	white.initialize_colors(GUICLR_ResizeGripHovered, 			CColor(112, 112, 112, 128));
	white.initialize_colors(GUICLR_ResizeGripActive,			CColor(112, 112, 112, 170));

	// table
	white.initialize_colors(GUICLR_TableHeaderBg,				CColor(0, 0, 0, 0));

	// listbox
	white.initialize_colors(GUICLR_ListBoxBackground,			CColor(20, 20, 20, 20));
	white.initialize_colors(GUICLR_ListBoxBorder,				CColor(170, 170, 170, 170));
	white.initialize_colors(GUICLR_ListBoxArrowBackground,		CColor( 40,  40,  40,  40));
	
	//---------------------------------------------------------------------------------------------------------
	
	auto& dark = create_theme("dark");
	m_builtin_themes.push_back("dark");

	// text
	dark.initialize_colors(GUICLR_TextLight,					CColor(161, 161, 161, 255));
	dark.initialize_colors(GUICLR_TextRegular,					CColor(149, 149, 149, 255));
	dark.initialize_colors(GUICLR_TextDark,						CColor(125, 125, 125, 255));
	dark.initialize_colors(GUICLR_TextBlack,					CColor(165, 165, 165, 255));
	dark.initialize_colors(GUICLR_TextWhite,					CColor(215, 215, 215, 255));

	// frame
	dark.initialize_colors(GUICLR_FrameBg,						CColor(50, 50, 50, 255));
	dark.initialize_colors(GUICLR_FrameBgHovered,				CColor(112, 112, 112, 128));
	dark.initialize_colors(GUICLR_FrameBgActive,				CColor(112, 112, 112, 170));
	dark.initialize_colors(GUICLR_FrameBgSelected,				CColor(112, 112, 112, 101));
	dark.initialize_colors(GUICLR_FrameActive,					CColor(0, 76, 142, 255));

	// window
	dark.initialize_colors(GUICLR_WindowBackground,				CColor(19, 19, 19, 255));
	dark.initialize_colors(GUICLR_ChildBackground,				CColor(30, 30, 30, 255));

	// button
	dark.initialize_colors(GUICLR_Button,						CColor(112, 112, 112, 51));

	// url
	dark.initialize_colors(GUICLR_URLHovered,					CColor(0, 83, 157, 206));

	// scrollbar
	dark.initialize_colors(GUICLR_ScrollbarGrab,				CColor(58, 58, 58, 255));

	// checkbox
	dark.initialize_colors(GUICLR_CheckBoxBackground,			CColor(49, 49, 49, 255));
	dark.initialize_colors(GUICLR_CheckBoxCheckMark,			CColor(157, 157, 157, 255));

	// slider
	dark.initialize_colors(GUICLR_SliderHovered,				CColor(0, 83, 157, 207));

	// input text
	dark.initialize_colors(GUICLR_InputTextBg,					CColor(32, 32, 32, 255));

	// separator
	dark.initialize_colors(GUICLR_Separator,					CColor(71, 71, 71, 255));

	// tab
	dark.initialize_colors(GUICLR_Tab,							CColor(112, 112, 112, 51));
	dark.initialize_colors(GUICLR_TabHovered,					CColor(112, 112, 112, 170));
	dark.initialize_colors(GUICLR_TabActive,					CColor(112, 112, 112, 128));

	// resize grip
	dark.initialize_colors(GUICLR_ResizeGrip,					CColor(112, 112, 112, 51));
	dark.initialize_colors(GUICLR_ResizeGripHovered, 			CColor(112, 112, 112, 128));
	dark.initialize_colors(GUICLR_ResizeGripActive,				CColor(112, 112, 112, 170));

	// table
	dark.initialize_colors(GUICLR_TableHeaderBg,				CColor(29, 29, 29, 255));

	// listbox
	dark.initialize_colors(GUICLR_ListBoxBackground,			CColor(20, 20, 20, 20));
	dark.initialize_colors(GUICLR_ListBoxBorder,				CColor(170, 170, 170, 170));
	dark.initialize_colors(GUICLR_ListBoxArrowBackground,		CColor(40, 40, 40, 40));
}
