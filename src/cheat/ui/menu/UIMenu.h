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

#ifndef UIMENU_H
#define UIMENU_H
#pragma once

// constants that describe some of the paddings and margins used in the menu
class CMenuStyle
{
public:
	static const float k_rounding_factor;
	static const Vector2D k_menu_rect_size;
	static const Vector2D k_menu_rect_size_max;
	static const float k_tab_select_width;
	static const float k_top_region_size_h;
	static const float k_menu_contents_padding;
	static const float k_menu_contents_padding_bottom;
	static const float k_bottom_reserved_rect_h;
	static const float k_bottom_right_timestamp_rightside_padding;
	static const float k_bottom_right_timestamp_bottomside_padding;
	static const Vector2D k_bottom_left_desc_padding;
	static const float k_child_contents_rounding;
	static const Vector2D k_child_contents_padding;
	static const float k_child_width;
	static const Vector2D k_unload_button_padding;
	static const Vector2D k_unload_button_size;
	static const Vector2D k_about_button_size;
	static const Vector2D k_tab_section_button_size;
	static const Vector2D k_tab_section_button_padding;
	static const float k_childhdr_text_padding_x;
	static const float k_childhdr_text_padding_y;
	static const Vector2D k_childhdr_line_padding;
	static const float k_childhdr_contents_padding_y;

	static Vector2D calc_child_size(float height);
	static Vector2D calc_child_size_2x(float height);
	static float get_child_width_w_padding();

	// use this instead of width begin -1.0f
	static Vector2D child_full_width(float height);
};

//----------------------------------------------------------------------------------------------------------------------
//
//	Menu tabs
//

enum EMenuTabId
{
	UIMENU_Blank,

	// AI
	UIMENU_RageBot,
	UIMENU_LegitBot,
	UIMENU_AntiBot,
	UIMENU_WayBot,

	// Visuals
	UIMENU_Viewmodel,
	UIMENU_World,
	UIMENU_Rendering,
	UIMENU_Screen,

	// Miscellaneous
	UIMENU_Exploits,
	UIMENU_Movement,
	UIMENU_Miscellaneous2,
	UIMENU_Miscellaneous3,

	// Config
	UIMENU_Config,
	UIMENU_Theme,
	UIMENU_Binds,
	UIMENU_InCommands,

	// Other
	UIMENU_Console,
	UIMENU_Others,

	UIMENU_Max,
};

// currently opened menu tab section id
using TabSectionId = std::pair<EMenuTabId, std::string>;

class CUIMenu;

enum EMenuChildHeightType
{
	MENUCHILD_Constant,
	MENUCHILD_2x,
	MENUCHILD_SameAsWidth
};

class IMenuChild
{
public:
	IMenuChild(const std::string& header, int height, bool collapsible, EMenuChildHeightType type = MENUCHILD_Constant)
		: m_header(header), m_supports_collapse(collapsible)
	{
		switch (type)
		{
			case MENUCHILD_Constant:
			{
				m_child_size = CMenuStyle::calc_child_size((float)height);
				break;
			}
			case MENUCHILD_2x:
			{
				m_child_size = CMenuStyle::calc_child_size_2x((float)height);
				break;
			}
			case MENUCHILD_SameAsWidth:
			{
				m_child_size = (float)height;
				break;
			}
		}

		validate();
	}

public:
	void render();

	inline const std::string& get_header() const { return m_header; }
	inline const Vector2D& get_child_size() const { return m_child_size; }
	inline Vector2D get_collapsible_child_size() const
	{
		return Vector2D(m_child_size.x, m_is_collapsed ? k_collapsed_height : m_child_size.y);
	}

	Vector2D m_computed_position;

private:
	void validate()
	{
		// validity checks
		assert(m_child_size.y != 0 && "Child height cannot be 0.");
		assert(m_header.length() != 0 && "Invalid child header.");
	}

	void render_header();

protected:
	virtual void contents() { /*implement me*/ }

protected:
	std::string m_header;

	Vector2D m_child_size;

	// collapse metadata
	bool m_supports_collapse = false, m_is_collapsed = false;
	bool m_is_hovered = false;

	inline static constexpr float k_collapsed_height = 35.0f;
};

// represents a section within a section
class MenuTabSection
{
public:
	MenuTabSection(const std::string& description, const std::vector<IMenuChild*/*, IMenuChild::less*/>& children)
		: m_children(children), m_desc(description)
	{
	}

	void render_button(const Vector2D& button_size, const std::string& id, TabSectionId& current_tab_section_id);

	std::vector<IMenuChild*/*, IMenuChild::less*/> m_children;

	std::string m_desc;
};

// represents an entry within a tab section
class MenuTab
{
public:
	MenuTab()
	{
	}

	MenuTab(const char* label, const char* desc, const std::unordered_map<std::string, MenuTabSection>& sections)
		: m_label(label), m_desc(desc), m_sections(sections)
	{
	}

	void render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId id, TabSectionId& current_tab_section_id);

	std::string m_label, m_desc;

	std::unordered_map<std::string, MenuTabSection> m_sections;
};

// represents group containing several menu entries
class MenuTabGroup
{
public:
	void set_label(const char* label)
	{
		m_label = label;
	}

	void add_tab(EMenuTabId id, const MenuTab& item)
	{
		m_menu_tabs.insert({ id, item });
	}

	void render(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size, TabSectionId& current_tab_section_id);

private:
	void render_current_label(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size);

public:
	std::unordered_map<EMenuTabId, MenuTab> m_menu_tabs;

	std::string m_label;
};

//----------------------------------------------------------------------------------------------------------------------

class CMenuContentsGrid
{
public:
	DECL_BASIC_CLASS(CMenuContentsGrid);

public:
	// precompute the grid for a tab section
	void precompute(const Vector2D& contents_size, const MenuTabSection& tab_section, bool force_compute = false);

	inline float get_accumulated_height() { return m_accumulated_height; }

	void force_precompute_next_frame()
	{
		m_scheduled_precomputation = true;
	}

private:
	Vector2D m_contents_size;

	// total height of all children
	float m_accumulated_height;

	uint32_t last_compute_time;

	bool m_scheduled_precomputation = false;
};

//----------------------------------------------------------------------------------------------------------------------
//
// Menu class
//

class CUIMenu final : public IRenderingContext_Generic
{
public:
	CUIMenu(const std::string& id) :
		IRenderingContext_Generic(id)
	{
	}

	void on_initialize();
	void on_render();
	void on_destroy();

private:
	// tab callbacks
	void tab_render();
	void tab_screen();
	void tab_visuals4();
	void tab_exploits();
	void tab_movement();
	void tab_miscellaneous2();
	void tab_miscellaneous3();
	void tab_config();
	void tab_binds();
	void tab_incommands();
	void tab_cmdlist();
	void tab_varlist();
	void tab_others();

	// menu contents
	void handle_menu_contents_rendering();

	// misc decoration rendering
	void render_github_repo_link_decor();
	void render_menu_decoration(const Vector2D& window_pos, const Vector2D& window_size);

	MenuTabGroup m_tabgroup_Visuals;
	MenuTabGroup m_tabgroup_Miscellaneous;
	MenuTabGroup m_tabgroup_Configuration;
	MenuTabGroup m_tabgroup_Other;

	TabSectionId m_active_tab_section;

	// Active offset for rendering tabs & section labels.
	// Relative active offset is not counting the child position.
	Vector2D m_sectab_active_offs, m_sectab_relative_active_offset;

	std::vector<MenuTabGroup*> m_tab_groups;
};

//----------------------------------------------------------------------------------------------------------------------

class CMenuWidgetDescription
{
public:
	DECL_BASIC_CLASS(CMenuWidgetDescription);

public:
	void set_desc(const char* desc) { m_description = desc; };
	const char* get_desc() { return m_description.c_str(); };
	void clear_desc() { m_description.clear(); };

private:
	std::string m_description;
};

//----------------------------------------------------------------------------------------------------------------------

#define DECL_CHILD(name)									\
class name final : public IMenuChild						\
{															\
public:														\
	name(const IMenuChild& child) : IMenuChild(child)		\
	{														\
	}														\
															\
	void contents() override;								\
}

// just a namespace, really
struct MenuChilden
{
	struct World
	{
		DECL_CHILD(Removals);
		DECL_CHILD(Thirdperson);
		DECL_CHILD(AntiDemoRecorder);
		DECL_CHILD(AntiScreen);
		DECL_CHILD(Automation);
		DECL_CHILD(SpeedControl);
		DECL_CHILD(ViewmodelOffset);
		DECL_CHILD(Backtrack);
		DECL_CHILD(SmokeVisuals);
		DECL_CHILD(NonSteamFpsFix);
		DECL_CHILD(WorldVisuals);
		DECL_CHILD(EnvironmentalEffects);
	};

	struct Rendering
	{
		DECL_CHILD(FieldOfView);
		DECL_CHILD(AspectRatio);
		DECL_CHILD(StudioRenderer);
		DECL_CHILD(ModelChams);
		DECL_CHILD(HUDRendering);
	};

	struct Screen
	{
		DECL_CHILD(FlashbangFade);
		DECL_CHILD(ESP);
		DECL_CHILD(CustomCrosshair);
		DECL_CHILD(ClShowFPSEnhancement);
	};

	struct Movement
	{
		DECL_CHILD(AirStuck);
		DECL_CHILD(Visualization);
		DECL_CHILD(GroundStrafe);
		DECL_CHILD(Strafehack);
		DECL_CHILD(StrafeHelper);
		DECL_CHILD(Bunnyhop);
		DECL_CHILD(Edgebug);
		DECL_CHILD(Fastrun);
		DECL_CHILD(AutoJOF);
	};

	struct Exploits
	{
		DECL_CHILD(BypassGameConstrains);
		DECL_CHILD(ServerCommandFilter);
		DECL_CHILD(FramerateSimulation);
		DECL_CHILD(ConsistencyBypass);
		DECL_CHILD(FakeLatency);
		DECL_CHILD(CvarSandbox);
	};

	struct Configuration
	{
		DECL_CHILD(Config);
		DECL_CHILD(Theme);
		DECL_CHILD(KeyBinding);
		DECL_CHILD(InCommands);
	};

	struct Other
	{
		DECL_CHILD(VariableList);
		DECL_CHILD(CommandList);
	};
};

#endif // UIMENU_H