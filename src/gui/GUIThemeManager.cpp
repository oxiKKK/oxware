/*
*	OXWARE developed by oxiKKK
*
*	Copyright (c) 2023
*/

#include "precompiled.h"

// We'll use internal imgui here
#include <imgui_internal.h>

struct PushedColor
{
	EGUIColor id;
	CColor original_color;
};

IGUIThemeManager* g_gui_thememgr_i = nullptr;

class CGUIThemeManager : public IGUIThemeManager
{
public:
	CGUIThemeManager();
	~CGUIThemeManager();

	void initialize();

	GUIThemeObject_t* get_theme_by_name(const char* name_identifier);
	GUIThemeObject_t* get_current_theme();
	void set_new_theme(const char* name_identifier);

	void push_color(EGUIColor id, const CColor& color);
	void pop_color(size_t amount = 1);

private:
	void sync_with_imgui();

	void add_themes();

private:
	GUIThemeObject_t* m_current_theme; // always set

	std::unordered_map<std::string, GUIThemeObject_t> m_themes;

	std::vector<PushedColor> m_pushed_colors;
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
	add_themes();

	CConsole::the().info("GUI ThemeManager initialized");
}

GUIThemeObject_t* CGUIThemeManager::get_theme_by_name(const char* name_identifier)
{
	for (auto& [name, theme] : m_themes)
	{
		if (name == name_identifier)
			return &theme;
	}

	assert(0);
	return nullptr;
}

GUIThemeObject_t* CGUIThemeManager::get_current_theme()
{
	assert(m_current_theme);
	return m_current_theme;
}

void CGUIThemeManager::set_new_theme(const char* name_identifier)
{
	auto theme = get_theme_by_name(name_identifier);
	if (!theme)
	{
		CConsole::the().error("Couldn't find theme: {}", name_identifier);
		return;
	}

	m_current_theme = theme;
	sync_with_imgui();
}

void CGUIThemeManager::push_color(EGUIColor id, const CColor& color)
{
	m_pushed_colors.push_back({ id, get_current_theme()->get_color(id)});
	get_current_theme()->set_using_color(id, color);
}

void CGUIThemeManager::pop_color(size_t amount)
{
	assert(amount <= m_pushed_colors.size());

	while (amount--)
	{
		auto last = m_pushed_colors.back();
		get_current_theme()->set_using_color(last.id, last.original_color);
		m_pushed_colors.pop_back();
	}
}

void CGUIThemeManager::sync_with_imgui()
{
	auto& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = get_current_theme()->get_color<GUICLR_WindowBackground>();
	style.Colors[ImGuiCol_ChildBg] = get_current_theme()->get_color<GUICLR_ChildBackground>();
	style.Colors[ImGuiCol_PopupBg] = get_current_theme()->get_color<GUICLR_PopupBackground>();

	style.Colors[ImGuiCol_Text] = get_current_theme()->get_color<GUICLR_TextDark>();

	style.Colors[ImGuiCol_ScrollbarBg] = get_current_theme()->get_color<GUICLR_ScrollbarBackground>();
	style.Colors[ImGuiCol_ScrollbarGrab] = get_current_theme()->get_color<GUICLR_ScrollbarGrab>();

	style.Colors[ImGuiCol_Tab] = get_current_theme()->get_color<GUICLR_Tab>();
	style.Colors[ImGuiCol_TabHovered] = get_current_theme()->get_color<GUICLR_TabHovered>();
	style.Colors[ImGuiCol_TabActive] = get_current_theme()->get_color<GUICLR_TabActive>();

	style.Colors[ImGuiCol_ResizeGrip] = get_current_theme()->get_color<GUICLR_ResizeGrip>();
	style.Colors[ImGuiCol_ResizeGripHovered] = get_current_theme()->get_color<GUICLR_ResizeGripHovered>();
	style.Colors[ImGuiCol_ResizeGripActive] = get_current_theme()->get_color<GUICLR_ResizeGripActive>();

	style.Colors[ImGuiCol_TableHeaderBg] = get_current_theme()->get_color<GUICLR_TableHeaderBg>();

	style.ScrollbarSize = 9.f;
	style.ChildRounding = style.FrameRounding = style.PopupRounding = 4.0f;
	style.ScrollbarRounding = 0.f;
	style.WindowPadding = { 10.f, 15.f };
}

//------------------------------------------------------------------------------------
// Themes
//

void CGUIThemeManager::add_themes()
{
	// White
	m_themes.insert({
		"white",
		{
			{
				CColor(  0,   0,   0,   0), // GUICLR_NONE

				CColor(161, 161, 161, 255), // GUICLR_TextLight
				CColor(112, 112, 112, 255), // GUICLR_TextRegular
				CColor( 84,  84,  84, 255), // GUICLR_TextDark
				CColor( 44,  44,  44, 255), // GUICLR_TextBlack
				CColor(255, 255, 255, 255), // GUICLR_TextWhite

				CColor(255, 255, 255, 255), // GUICLR_WindowBackground
				CColor(240, 240, 240, 255), // GUICLR_ChildBackground
				CColor(240, 240, 240, 255), // GUICLR_PopupBackground
				
				CColor(112, 112, 112,  51), // GUICLR_Button
				CColor(112, 112, 112, 128), // GUICLR_ButtonHovered
				CColor(112, 112, 112, 170), // GUICLR_ButtonActive
				
				CColor(112, 112, 112, 255), // GUICLR_HyperTextLink
				CColor(  0, 136, 255, 170), // GUICLR_HyperTextLinkHovered
				CColor(  0, 136, 255, 255), // GUICLR_HyperTextLinkActive

				CColor(240, 240, 240, 255), // GUICLR_ScrollbarBackground
				CColor(150, 150, 150, 255), // GUICLR_ScrollbarGrab

				CColor(255, 255, 255, 255), // GUICLR_CheckBoxBackground,
				CColor(196, 196, 196, 255), // GUICLR_CheckBoxOutline,
				CColor(  0, 136, 255, 255), // GUICLR_CheckBoxSelected,
				CColor(255, 255, 255, 255), // GUICLR_CheckBoxCheckmark,

				CColor(196, 196, 196, 255), // GUICLR_SliderFrameBg,
				CColor(  0, 136, 255, 255), // GUICLR_SliderGrab,
				CColor(  0, 136, 255,  90), // GUICLR_SliderHovered,
				CColor(  0, 136, 255, 255), // GUICLR_SliderActive,

				CColor(255, 255, 255, 255), // GUICLR_InputTextBg,

				CColor(  0, 136, 255, 255), // GUICLR_ColorButtonBorderHovered,

				CColor(196, 196, 196, 255), // GUICLR_Separator

				CColor(112, 112, 112,  51), // GUICLR_Tab
				CColor(112, 112, 112, 170), // GUICLR_TabHovered
				CColor(112, 112, 112, 128), // GUICLR_TabActive

				CColor(   0, 136, 255, 255),// GUICLR_ProgressBar

				CColor(112, 112, 112,  51), // GUICLR_ResizeGrip
				CColor(112, 112, 112, 128), // GUICLR_ResizeGripHovered
				CColor(112, 112, 112, 170), // GUICLR_ResizeGripActive

				CColor(  0,   0,   0,   0), // GUICLR_TableHeaderBg
			}
		}});

	// TODO: More themes

	set_new_theme("white"); // default
}
