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

class CUIMenu;

enum EMenuChildHeightType
{
	MCH_Constant,
	MCH_2x,
	MCH_2_5x,
	MCH_3x,
	MCH_4x,
	MCH_SameAsWidth
};

enum EMenuChildFlags
{
	MCHILDF_None = BIT(-1),

	MCHILDF_DontApplyFilter = BIT(0),	// don't apply search filter
};

class IMenuChild
{
public:
	IMenuChild(const std::string& header, int height, bool collapsible, EMenuChildHeightType type = MCH_Constant, EMenuChildFlags flags = MCHILDF_None)
		: m_header(header), m_supports_collapse(collapsible), m_flags(flags)
	{
		float h = (float)height;

		switch (type)
		{
			case MCH_Constant:
			{
				m_child_size = MenuStyle::calc_child_size(h);
				break;
			}
			case MCH_2x:
			{
				m_child_size = MenuStyle::calc_child_size_n(2.0f, h);
				break;
			}
			case MCH_2_5x:
			{
				m_child_size = MenuStyle::calc_child_size_n(2.5f, h);
				break;
			}
			case MCH_3x:
			{
				m_child_size = MenuStyle::calc_child_size_n(3.0f, h);
				break;
			}
			case MCH_4x:
			{
				m_child_size = MenuStyle::calc_child_size_n(4.0f, h);
				break;
			}
			case MCH_SameAsWidth:
			{
				m_child_size = h;
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

	inline auto get_flags() const { return m_flags; }

	Vector2D m_computed_position;

private:
	void validate()
	{
		// validity checks
		assert(m_child_size.y != 0 && "Child height cannot be 0.");
		assert(m_header.length() != 0 && "Invalid child header.");
	}

protected:
	virtual void contents() { /*implement me*/ }

protected:
	std::string m_header;

	Vector2D m_child_size;

	// collapse metadata
	bool m_supports_collapse = false, m_is_collapsed = false;
	bool m_is_hovered = false;

	inline static constexpr float k_collapsed_height = 35.0f;

	EMenuChildFlags m_flags = MCHILDF_None;
};

//----------------------------------------------------------------------------------------------------------------------

enum EMenuTabId
{
	MENU_TAB_Blank,

	// Visuals
	MENU_TAB_World,
	MENU_TAB_Rendering,
	MENU_TAB_Screen,

	// Miscellaneous
	MENU_TAB_Exploits,
	MENU_TAB_Movement,
	MENU_TAB_PlayerList,

	// Config
	MENU_TAB_Config,
	MENU_TAB_Theme,
	MENU_TAB_Language,
	MENU_TAB_Binds,
	MENU_TAB_InCommands,

	// Other
	MENU_TAB_VariableList,
	MENU_TAB_CommandList,
	MENU_TAB_Others,
	MENU_TAB_Debug,

	MENU_TAB_Max,
};

enum EMenuTabGroupId
{
	MENU_TAB_GROUP_Visuals,
	MENU_TAB_GROUP_Miscellaneous,
	MENU_TAB_GROUP_Configuration,
	MENU_TAB_GROUP_Other,

	MENU_TAB_GROUP_COUNT,
};

// used inside menu to determine what is currently selected
struct ContentsSelectionContext
{
	EMenuTabId	id;
	std::string	section;
};

//----------------------------------------------------------------------------------------------------------------------

// optional division of menu tab
class MenuTabSection
{
public:
	MenuTabSection()
	{
	}

	void initialize(const char* desc)
	{
		m_desc = desc;
	}

	bool render_button(const Vector2D& button_size, const std::string& section_id, ContentsSelectionContext& contents_selection);

	std::vector<IMenuChild*> m_children;

	Vector2D m_scroll;

	std::string m_desc;
};

//----------------------------------------------------------------------------------------------------------------------

// represents an entry within a tab section
class MenuTab
{
public:
	MenuTab()
	{
	}

	void initialize(const char* label, const char* desc) 
	{
		m_label = label;
		m_desc = desc;
	}

	bool render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId id, ContentsSelectionContext& contents_selection);

	std::unordered_map<std::string, MenuTabSection> m_sections;

	std::string m_label, m_desc;
};

//----------------------------------------------------------------------------------------------------------------------

// represents group containing several menu entries
class MenuTabGroup
{
public:
	MenuTabGroup()
	{
	}

	void initialize(const char* label)
	{
		m_label = label;
	}

	bool render(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size, ContentsSelectionContext& contents_selection);

	std::unordered_map<EMenuTabId, MenuTab> m_tabs;

	std::string m_label;

private:
	void render_current_label(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size);
};

//----------------------------------------------------------------------------------------------------------------------

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
	// menu contents
	void handle_menu_contents_rendering();

	// render section buttons
	void render_menu_contents_section_buttons();

	// tabs on the left
	void render_menu_tabs();

	// misc decoration rendering
	void render_github_repo_link_decor();
	void render_menu_decoration(const Vector2D& window_pos, const Vector2D& window_size);

	ContentsSelectionContext m_current_context_selection;

	// Active offset for rendering tabs & section labels.
	// Relative active offset is not counting the child position.
	Vector2D m_sectab_absolute_active_offset, m_sectab_relative_active_offset;

	std::unordered_map<EMenuTabGroupId, MenuTabGroup> m_tab_groups;

	bool m_menu_contents_changed = false;

	// helper
	std::optional<MenuTabSection*> get_selected_section();
};

//----------------------------------------------------------------------------------------------------------------------

class CMenuContentsGrid
{
public:
	DECL_BASIC_CLASS(CMenuContentsGrid);

public:
	// precompute the grid for a tab section
	void precompute(const Vector2D& contents_size, const MenuTabSection& section, bool force_compute = false);

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

class CMenuSearchFilterContext
{
public:
	// call every frame
	void update();

	void render_search_box(const Vector2D& position);

	inline bool filter_active() { return m_menu_search_input[0] != 0; }
	inline const char* get_input_search_buffer() { return m_menu_search_input; }

	inline bool have_at_least_one_result()
	{
		return m_at_least_one_result;
	}

	bool apply_filter(IMenuChild* child);

private:
	char m_menu_search_input[64];

	static int search_input_callback_stub(ImGuiInputTextCallbackData* data)
	{
		auto _this = (CMenuSearchFilterContext*)data->UserData;
		return _this->search_input_callback(data);
	}
	int search_input_callback(ImGuiInputTextCallbackData* data);

	bool m_at_least_one_result = false;
};

extern CMenuSearchFilterContext g_search_filter_context;

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
		DECL_CHILD(Panic);
	};

	struct Screen
	{
		DECL_CHILD(FlashbangFade);
		DECL_CHILD(ESP);
		DECL_CHILD(CustomCrosshair);
		DECL_CHILD(ClShowFPSEnhancement);
		DECL_CHILD(MovementInfo);
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
		DECL_CHILD(FrameSkipper);
		DECL_CHILD(ConsistencyBypass);
		DECL_CHILD(FakeLatency);
		DECL_CHILD(CvarSandbox);
		DECL_CHILD(SIDSpoofer);
	};

	struct PlayerList
	{
		DECL_CHILD(PlayerList_);
	};

	struct Configuration
	{
		DECL_CHILD(Config);
		DECL_CHILD(Theme);
		DECL_CHILD(Language);
		DECL_CHILD(KeyBinding);
		DECL_CHILD(InCommands);
	};

	struct Other
	{
		DECL_CHILD(VariableList);
		DECL_CHILD(CommandList);
		DECL_CHILD(UI);
		DECL_CHILD(Storage);
		DECL_CHILD(Debug);
	};
};

#endif // UIMENU_H