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

//
// GUIWidgets.cpp -- Implementation of the IGLFWApp interface
//

#include "precompiled.h"

// We'll use internal imgui here, so we can create new widgets etc..
#include <imgui_internal.h>

using namespace ImGui;

IGUIWidgets* g_gui_widgets_i = nullptr;

class CGUIWidgets : public IGUIWidgets
{
public:
	CGUIWidgets();
	~CGUIWidgets();

	//
	// Windows
	//

	void set_next_window_size(const Vector2D& size, ImGuiCond condition);
	void set_next_window_pos(const Vector2D& pos, ImGuiCond condition);
	void set_next_window_size_constrains(const Vector2D& min, const Vector2D& max);
	void center_next_window_pos(ImGuiCond condition);
	void set_next_window_rounding(float rounding, ImDrawFlags flags);

	void create_new_window(const std::string& name, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents);

	void block_input_on_all_except_popup(bool block);

	void add_child(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents);

	Vector2D get_current_window_pos();
	Vector2D get_current_window_size();

	void schedule_simple_popup_dialog(const std::string& label);
	bool execute_simple_popup_popup(const std::string& name, const Vector2D& size, ImGuiWindowFlags flags, const std::function<void()>& callback);

	void show_font_atlas();

	//
	// Properties
	//

	void push_disabled();
	void pop_disabled();

	void begin_group();
	void end_group();

	void push_stylevar(ImGuiStyleVar idx, float val);
	void push_stylevar(ImGuiStyleVar idx, const Vector2D& val);
	void pop_stylevar(size_t amount = 1);

	void push_color(ImGuiCol idx, const CColor& color);
	void pop_color(size_t amount = 1);

	void push_font(ImFont* font);
	void pop_font();

	void set_cursor_pos(const Vector2D& pos);
	Vector2D get_cursor_pos();
	Vector2D get_cursor_screen_pos();

	Vector2D get_content_region_avail();

	void sameline(float offset_from_start_x = 0.0f, float spacing = -1.0);

	bool is_last_widget_hovered();

	void set_item_default_focus();
	void set_keyboard_focus_here(int offset = 0);
	bool is_last_item_focused();

	void set_scroll_here_y(float center_y_ratio = 0.5f);

	void mark_last_as_edited();

	void close_current_popup();

	void push_executing_popup_code();
	void pop_executing_popup_code();

	Vector2D get_current_scroll(); 
	void set_scroll(const Vector2D& xy);

	ImGuiStyle& get_imgui_style();

	bool is_last_widget_focused();
	bool is_widget_focused(const char* str_id);

	void push_id(const std::string& id);
	void push_id(int id);
	void pop_id();

	//
	// Widgets
	//

	void add_text(const std::string& text, ETextProperties properties = TEXTPROP_None, ImFont* font = nullptr);
	void add_bullet_text(const std::string& text, ETextProperties properties = TEXTPROP_None, ImFont* font = nullptr);
	void add_colored_text(const CColor& color, const std::string& text, ETextProperties properties = TEXTPROP_None);
	void add_window_centered_text(const std::string& text, ImFont* font = nullptr);
	void add_window_centered_text_disabled(const std::string& text, ImFont* font = nullptr);

	bool add_button(const std::string& label, const Vector2D& size, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None);
	bool add_toggle_button(const std::string& label, const Vector2D& size, bool selected = false, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None, CColor unselected_color = CColor(0, 0, 0, 0));
	bool add_toggle_button_custom(const std::string& label, const Vector2D& size, bool selected = false, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None);
	bool add_invisible_button_behaviour(const std::string& label, const Vector2D& pos, const Vector2D& size);

	bool add_url_text(const std::string& label);

	bool add_checkbox(const std::string& label, float* value);
	bool add_checkbox(const std::string& label, bool* value);
	bool add_checkbox_with_color(const std::string& label, float* value, float rgba[4], bool input_values, ImGuiColorEditFlags flags);

	bool add_color_edit(const std::string& label, float rgba[4], bool input_values, ImGuiColorEditFlags flags);

	bool add_slider(const std::string& label, float* value, float* min, float* max, const char* format, bool no_label);
	bool add_slider(const std::string& label, int* value, int* min, int* max, const char* format, bool no_label);

	bool add_text_input(const std::string& label, char* buffer, size_t buffer_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, bool no_title = false, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
	bool add_text_input_ex(const std::string& label, char* buffer, size_t buffer_size, Vector2D input_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr, ImFont* font = 0);

	void add_padding(const Vector2D& size);
	void add_spacing();

	void add_separtor_with_text(const std::string& text);
	void add_separator();

	void render_clipped_contents(size_t size, const std::function<void(int line_no)>& callback);

	void add_readmore_on_hover_widget(const char* text);
	void add_readmore_on_hover_widget_ex(const std::function<void()>& callback);
	void dialog_on_hover_widget(const std::function<void()>& callback);

	void add_progress_bar(const std::string& id, const Vector2D& size, float current, float max);

	bool begin_listbox(const std::string& label, const std::string& preview_label, ImGuiComboFlags flags = 0);
	void end_listbox();
	bool add_simple_listbox(const std::string& label, int* value, const std::vector<std::string>& items, const std::function<void(int i)>& on_entry_click = nullptr);

	bool add_selectable(const std::string& label, bool selected = false, ImGuiSelectableFlags flags = 0, const Vector2D& size = Vector2D(0, 0));

	bool add_floating_button(const std::string& label, const Vector2D& last_cursor_pos, const Vector2D& button_pos, const Vector2D& button_size = Vector2D(0, 0), bool disabled = false, EButtonFlags flags = BUTTONFLAG_None);

	bool add_tree_node(const std::string& label);
	void pop_tree_node();

	void add_separated_heading(const std::string& label);

	void add_image(ImTextureID id, const Vector2D& size);

	//
	// Tables/lists/columns
	//
	
	// Adds new imgui table
	void add_table(const std::string& name, uint32_t columns, ImGuiTableFlags flags, const std::function<void()>& header_callback, const std::function<void()>& body_callback);

	// Use for simple tables such as:
	// 
	// name       value
	// name1      value1
	// ...
	// nameN      valueN
	//
	void add_undecorated_simple_table(const std::string& name, uint32_t columns, const std::function<void()>& body_callback);

	void table_setup_column_fixed_width(const std::string& name, float width, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None);
	void table_setup_column(const std::string& name, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None);

	void table_next_column();

	void table_headers_row();

	// columns using tables api
	bool begin_columns(const std::string& label, int count_columns, ImGuiTableFlags flags);
	void end_columns();
	void setup_column_fixed_width(float width, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None);
	void setup_column(ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None);
	void goto_next_column();
	void goto_next_row();

	//
	// Input text buffer operations
	//

	void delete_chars_on_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, int bytes_count);
	void insert_chars_to_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, const char* text, const char* text_end = NULL);

	//
	// Tabs
	//

	void begin_tab(const std::string& label, ImGuiTabBarFlags flags);
	void add_tab_item(const std::string& label, bool border, const Vector2D& size, const std::function<void()>& pfn_contents, const Vector2D& child_padding);
	void end_tab();

	//
	// Custom widgets
	//
	void add_console();

	//
	// rendering
	//

	void render_arrow(const Vector2D& pos, const CColor& color, EArrowDirection direction);

private:
	void add_bullet_text_internal(const std::string& text, ETextProperties properties = TEXTPROP_None, ImFont* font = nullptr);

	void add_window_centered_text_ex(const std::string& text, const CColor& text_color, ImFont* font);

	bool add_button_internal(const char* label, const Vector2D& size, bool selected, EButtonFlags flags);

	template<typename T>
	bool add_checkbox_internal(const char*label, T* value);
	bool add_checkbox_with_color_internal(const char* label, float* value, float rgba[4], bool input_values, ImGuiColorEditFlags flags);

	void add_text_wrapped(const char* text);

	bool color_edit_4_internal(const char* label, float rgba[4], bool input_values, ImGuiColorEditFlags flags);
	bool color_picker_4_internal(const char* label, float rgba[4], bool input_values, ImGuiColorEditFlags flags, float* ref_col = NULL);
	void color_picker_options_popup(const float* ref_col, bool input_values, ImGuiColorEditFlags flags);
	bool color_button_internal(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg = {});

	bool begin_combo_internal(const char* label, const char* preview_label, ImGuiComboFlags flags = 0);

	template<typename T>
	bool add_slider_internal(const char* label, T* value, T* min, T* max, const char* format, ImGuiDataType data_type, bool no_label, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	// helpers
	template<EGUIColorId clr> requires(clr > GUICLR_Invalid && clr < GUICLR_MAX)
	const CColor& get_color() const { return g_gui_thememgr_i->get_current_theme()->get_color<clr>(); }
	template<EGUIColorId clr> requires(clr > GUICLR_Invalid && clr < GUICLR_MAX)
	const uint32_t get_color_u32() const { return g_gui_thememgr_i->get_current_theme()->get_color_u32<clr>(); }

	bool m_block_input_on_all = false;

	bool m_executing_popup_code = false;
};

CGUIWidgets g_gui_widgets;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGUIWidgets, IGUIWidgets, IGUIWIDGETS_INTERFACEID, g_gui_widgets);

//-------------------------------------------------------------------------------
//
// CGUIWidgets implementation
// 
//-------------------------------------------------------------------------------

CGUIWidgets::CGUIWidgets()
{
	g_gui_widgets_i = this;
}

CGUIWidgets::~CGUIWidgets()
{
	g_gui_widgets_i = nullptr;
}

void CGUIWidgets::set_next_window_size(const Vector2D& size, ImGuiCond condition)
{
	SetNextWindowSize(size, condition);
}

void CGUIWidgets::set_next_window_pos(const Vector2D& pos, ImGuiCond condition)
{
	SetNextWindowPos(pos, condition);
}

void CGUIWidgets::set_next_window_size_constrains(const Vector2D& min, const Vector2D& max)
{
	SetNextWindowSizeConstraints(min, max);
}

void CGUIWidgets::center_next_window_pos(ImGuiCond condition)
{
	// Always center this window when appearing
	ImVec2 center = GetMainViewport()->GetCenter();
	SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
}

void CGUIWidgets::set_next_window_rounding(float rounding, ImDrawFlags flags)
{
	SetNextWindowRounding(rounding, flags);
}

void CGUIWidgets::create_new_window(const std::string& name, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents)
{
	PushStyleColor(ImGuiCol_WindowBg, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_WindowBackground>());

	bool should_disable_all_interaction = m_block_input_on_all && (name != "popup_window") && !m_executing_popup_code;
	
	if (should_disable_all_interaction)
	{
		flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav;
	}

	Begin(name.c_str(), NULL, flags);

	if (should_disable_all_interaction)
	{
		push_disabled();
	}

	if (pfn_contents)
	{
		if (should_disable_all_interaction)
		{
			// like.. this is really dumb, but whatever dude xd
			SetWindowFocus("popup_window");
		}

		pfn_contents();
	}

	if (should_disable_all_interaction)
	{
		pop_disabled();
	}

	End();

	PopStyleColor(1);
}

void CGUIWidgets::block_input_on_all_except_popup(bool block)
{
	m_block_input_on_all = block;
}

void CGUIWidgets::add_child(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents)
{
	PushStyleColor(ImGuiCol_ChildBg, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_ChildBackground>());

	bool should_disable_all_interaction = m_block_input_on_all && stricmp(GetCurrentWindow()->Name, "popup_window") && !m_executing_popup_code;

	if (should_disable_all_interaction)
	{
		flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav;
	}

	BeginChild(label.c_str(), size, border, flags);

	if (pfn_contents)
	{
		pfn_contents();
	}

	EndChild();

	PopStyleColor(1);
}

Vector2D CGUIWidgets::get_current_window_pos()
{
	return GetWindowPos();
}

Vector2D CGUIWidgets::get_current_window_size()
{
	return GetWindowSize();
}

void CGUIWidgets::schedule_simple_popup_dialog(const std::string& label)
{
	OpenPopup(label.c_str()); 
}

bool CGUIWidgets::execute_simple_popup_popup(const std::string& name, const Vector2D& size, ImGuiWindowFlags flags, const std::function<void()>& callback)
{
	SetNextWindowSize(size, ImGuiCond_Always);

	if (BeginPopup(name.c_str(), flags))
	{
		if (callback)
		{
			callback();
		}

		EndPopup();

		return true;
	}

	return false;
}

void CGUIWidgets::show_font_atlas()
{
	ImGuiContext& g = *GImGui;
	ShowFontAtlas(g.IO.Fonts);
}

void CGUIWidgets::push_disabled()
{
	BeginDisabled();
}

void CGUIWidgets::pop_disabled()
{
	EndDisabled();
}

void CGUIWidgets::begin_group()
{
	BeginGroup();
}

void CGUIWidgets::end_group()
{
	EndGroup();
}

void CGUIWidgets::push_stylevar(ImGuiStyleVar idx, float val)
{
	PushStyleVar(idx, val);
}

void CGUIWidgets::push_stylevar(ImGuiStyleVar idx, const Vector2D& val)
{
	PushStyleVar(idx, val);
}

void CGUIWidgets::pop_stylevar(size_t amount)
{
	PopStyleVar(amount);
}

void CGUIWidgets::push_color(ImGuiCol idx, const CColor& color)
{
	PushStyleColor(idx, color);
}

void CGUIWidgets::pop_color(size_t amount)
{
	PopStyleColor(amount);
}

void CGUIWidgets::push_font(ImFont * font)
{
	PushFont(font);
}

void CGUIWidgets::pop_font()
{
	PopFont();
}

void CGUIWidgets::set_cursor_pos(const Vector2D& pos)
{
	SetCursorPos(pos);
}

Vector2D CGUIWidgets::get_cursor_pos()
{
	return GetCursorPos();
}

Vector2D CGUIWidgets::get_cursor_screen_pos()
{
	return GetCursorScreenPos();
}

Vector2D CGUIWidgets::get_content_region_avail()
{
	return GetContentRegionAvail();
}

void CGUIWidgets::sameline(float offset_from_start_x, float spacing)
{
	SameLine(offset_from_start_x, spacing);
}

bool CGUIWidgets::is_last_widget_hovered()
{
	return IsItemHovered();
}

void CGUIWidgets::set_item_default_focus()
{
	SetItemDefaultFocus();
}

void CGUIWidgets::set_keyboard_focus_here(int offset)
{
	SetKeyboardFocusHere(offset);
}

bool CGUIWidgets::is_last_item_focused()
{
	return IsItemFocused();
}

void CGUIWidgets::set_scroll_here_y(float center_y_ratio)
{
	SetScrollHereY(center_y_ratio);
}

void CGUIWidgets::mark_last_as_edited()
{
	ImGuiContext& g = *GImGui;
	g.ActiveIdHasBeenEditedThisFrame = true;
	g.ActiveIdHasBeenEditedBefore = true;
	g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;
}

void CGUIWidgets::close_current_popup()
{
	CloseCurrentPopup();
}

void CGUIWidgets::push_executing_popup_code()
{
	assert(!m_executing_popup_code && "Miscall inside " __FUNCTION__ "! You propably didn't pop!");
	m_executing_popup_code = true;
}

void CGUIWidgets::pop_executing_popup_code()
{
	assert(m_executing_popup_code && "Miscall inside " __FUNCTION__ "! You propably didn't push!");
	m_executing_popup_code = false;
}

Vector2D CGUIWidgets::get_current_scroll()
{
	ImGuiWindow* window = GImGui->CurrentWindow;
	return window->Scroll;
}

void CGUIWidgets::set_scroll(const Vector2D& xy)
{
	ImGuiWindow* window = GImGui->CurrentWindow;
	if (xy.x != -1.0f)
	{
		window->Scroll.x = xy.x;
	}
	if (xy.y != -1.0f)
	{
		window->Scroll.y = xy.y;
	}
}

ImGuiStyle& CGUIWidgets::get_imgui_style()
{
	return GImGui->Style;
}

bool CGUIWidgets::is_last_widget_focused()
{
	return GetItemID() == GImGui->ActiveId;
}

bool CGUIWidgets::is_widget_focused(const char* str_id)
{
	return GetID(str_id) == GImGui->ActiveId;
}

void CGUIWidgets::push_id(const std::string& id)
{
	PushID(id.c_str());
}

void CGUIWidgets::push_id(int id)
{
	PushID(id);
}

void CGUIWidgets::pop_id()
{
	PopID();
}

void CGUIWidgets::add_text(const std::string& text, ETextProperties properties, ImFont* font)
{
	//
	// decorations
	//

	if (font)
	{
		PushFont(font);
	}

	if (properties & TEXTPROP_Slim)
	{
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	}

	if (properties & TEXTPROP_Disabled)
	{
		BeginDisabled();
	}

	//
	// colors
	//

	if (properties & TEXTPROP_ColorWhite)
	{
		PushStyleColor(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextWhite>());
	}
	else if (properties & TEXTPROP_ColorRegular)
	{
		PushStyleColor(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextRegular>());
	}
	else if (properties & TEXTPROP_ColorDark)
	{
		PushStyleColor(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextDark>());
	}
	else if (properties & TEXTPROP_ColorBlack)
	{
		PushStyleColor(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextBlack>());
	}

	//
	// other
	//

	if (properties & TEXTPROP_Wrapped)
	{
		add_text_wrapped(text.c_str());
	}
	else
	{
		TextUnformatted(text.c_str());
	}

	if ((properties & TEXTPROP_ColorWhite) ||
		(properties & TEXTPROP_ColorRegular) ||
		(properties & TEXTPROP_ColorDark) ||
		(properties & TEXTPROP_ColorBlack))
	{
		PopStyleColor();
	}

	if (properties & TEXTPROP_Disabled)
	{
		EndDisabled();
	}

	if (properties & TEXTPROP_Slim)
	{
		PopStyleVar();
	}

	if (font)
	{
		PopFont();
	}
}

void CGUIWidgets::add_bullet_text(const std::string& text, ETextProperties properties, ImFont* font)
{
	return add_bullet_text_internal(text, properties, font);
}

void CGUIWidgets::add_colored_text(const CColor& color, const std::string& text, ETextProperties properties)
{
	PushStyleColor(ImGuiCol_Text, color);
	add_text(text, properties);
	PopStyleColor();
}

void CGUIWidgets::add_window_centered_text(const std::string& text, ImFont* font)
{
	auto text_color = GetStyle().Colors[ImGuiCol_Text];

	if (!font)
	{
		font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);
	}
	add_window_centered_text_ex(text, text_color, font);
}

void CGUIWidgets::add_window_centered_text_disabled(const std::string& text, ImFont* font)
{
	auto text_color = GetStyle().Colors[ImGuiCol_TextDisabled];

	if (!font)
	{
		font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);
	}
	add_window_centered_text_ex(text, text_color, font);
}

bool CGUIWidgets::add_button(const std::string& label, const Vector2D& size, bool disabled, EButtonFlags flags)
{
	if (disabled)
		push_disabled();

	bool ret = add_button_internal(label.c_str(), size, false, flags);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	if (disabled)
		pop_disabled();

	return ret;
}

bool CGUIWidgets::add_toggle_button(const std::string& label, const Vector2D& size, bool selected, bool disabled, EButtonFlags flags, CColor unselected_color)
{
	if (disabled)
		push_disabled();

	// use the default button color as hover color and disable the default one.
	auto prev_default_color = g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_Button>();
	g_gui_thememgr_i->push_color(GUICLR_Button, unselected_color);
	g_gui_thememgr_i->push_color(GUICLR_FrameBgHovered, prev_default_color);

	bool ret = add_button_internal(label.c_str(), size, selected, flags);
	
	g_gui_thememgr_i->pop_color(2);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	if (disabled)
		pop_disabled();

	return ret;
}

bool CGUIWidgets::add_toggle_button_custom(const std::string& label, const Vector2D& size, bool selected, bool disabled, EButtonFlags flags)
{
	if (disabled)
		push_disabled();

	bool ret = add_button_internal(label.c_str(), size, selected, flags);
	
	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	if (disabled)
		pop_disabled();

	return ret;
}

bool CGUIWidgets::add_invisible_button_behaviour(const std::string& label, const Vector2D& pos, const Vector2D& size)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// Cannot use zero-size for InvisibleButton(). Unlike Button() there is not way to fallback using the label size.
	assert(size.x != 0.0f && size.y != 0.0f);

	const ImGuiID id = window->GetID(label.c_str());
	ImVec2 item_size = CalcItemSize(size, 0.0f, 0.0f);
	const ImRect bb(pos, pos + item_size);
	ItemSize(item_size);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_None);

	return pressed;
}

bool CGUIWidgets::add_url_text(const std::string& label)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImGuiID id = window->GetID(label.c_str());
	ImVec2 label_size = CalcTextSize(label.c_str(), NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 item_size = CalcItemSize({0.0f, 0.0f}, label_size.x, label_size.y);

	const ImRect bb(pos, pos + item_size);
	ItemSize(item_size, 0.0f);
	if (!ItemAdd(bb, id))
	{
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_None);

	// Render
	CColor color_4f;
	if (held && hovered)
	{
		color_4f = get_color<GUICLR_FrameActive>();
	}
	else if (hovered)
	{
		color_4f = get_color<GUICLR_URLHovered>();
	}
	else
	{
		color_4f = get_color<GUICLR_TextRegular>();
	}

	push_color(ImGuiCol_Text, color_4f);

	RenderTextClipped({ bb.Min.x, bb.Min.y },
					  bb.Max, label.c_str(), NULL, &label_size, {0.f, 0.f}, &bb);

	pop_color();

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return pressed;
}

bool CGUIWidgets::add_checkbox(const std::string& label, float* value)
{
	bool ret = add_checkbox_internal(label.c_str(), value);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_checkbox(const std::string& label, bool* value)
{
	bool ret = add_checkbox_internal(label.c_str(), value);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_checkbox_with_color(const std::string& label, float* value, float rgba[4], bool input_values, ImGuiColorEditFlags flags)
{
	bool ret = add_checkbox_with_color_internal(label.c_str(), value, rgba, input_values, flags);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_color_edit(const std::string& label, float rgba[4], bool input_values, ImGuiColorEditFlags flags)
{
	add_text(label);

	SameLine(GetWindowContentRegionMax().x - 30.f);

	char buffer[256];
	sprintf(buffer, "##%s", label.c_str());

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	PushStyleVar(ImGuiStyleVar_PopupRounding, 6.0f);

	bool ret = color_edit_4_internal(buffer, rgba, input_values,
									 flags |
									 ImGuiColorEditFlags_NoInputs |
//									 ImGuiColorEditFlags_NoOptions |
									 ImGuiColorEditFlags_NoTooltip |
									 ImGuiColorEditFlags_NoLabel |
									 ImGuiColorEditFlags_NoDragDrop);

	PopStyleVar(2);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_slider(const std::string& label, float* value, float* min, float* max, const char* format, bool no_label)
{
	bool ret = add_slider_internal(label.c_str(), value, min, max, format, ImGuiDataType_Float, no_label);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_slider(const std::string & label, int* value, int* min, int* max, const char* format, bool no_label)
{
	bool ret = add_slider_internal(label.c_str(), value, min, max, format, ImGuiDataType_S32, no_label);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

bool CGUIWidgets::add_text_input(const std::string& label, char* buffer, size_t buffer_size, 
								 ImGuiInputTextFlags flags, bool no_title, ImGuiInputTextCallback callback, void* user_data)
{
	if (!no_title)
	{
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.0f, 1.0f));

		add_text(label, TEXTPROP_ColorRegular);
	}

	bool ret = add_text_input_ex(label, buffer, buffer_size, Vector2D(-1, 0), flags, callback, user_data);

	if (!no_title)
	{
		PopStyleVar(1);
	}

	return ret;
}

bool CGUIWidgets::add_text_input_ex(const std::string& label, char* buffer, size_t buffer_size, Vector2D input_size,
									ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data, ImFont* font)
{
	PushStyleColor(ImGuiCol_FrameBg, get_color<GUICLR_InputTextBg>());

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

	if (!font)
	{
		font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_21px, FDC_Light);
	}
	PushFont(font);

	bool ret = InputTextEx(label.c_str(), NULL, buffer, buffer_size, input_size, flags | ImGuiInputTextFlags_NoLabel, callback, user_data);

	PopFont();

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Input);
	}

	PopStyleVar(1);

	PopStyleColor(1);

	return ret;
}

void CGUIWidgets::add_padding(const Vector2D& size)
{
	Dummy(size);
}

void CGUIWidgets::add_spacing()
{
	Spacing();
}

bool CGUIWidgets::begin_columns(const std::string& label, int count_columns, ImGuiTableFlags flags)
{
	return BeginTable(label.c_str(), count_columns, flags | ImGuiTableFlags_HeaderTextOnly);
}

void CGUIWidgets::setup_column_fixed_width(float width, ImGuiTableColumnFlags flags)
{
	TableSetupColumn(nullptr, flags | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, width);
}

void CGUIWidgets::setup_column(ImGuiTableColumnFlags flags)
{
	TableSetupColumn(nullptr, flags | ImGuiTableColumnFlags_NoHeaderLabel);
}

void CGUIWidgets::goto_next_column()
{
	TableNextColumn();
}

void CGUIWidgets::goto_next_row()
{
	TableNextRow();
}

void CGUIWidgets::end_columns()
{
	EndTable();
}

void CGUIWidgets::add_separtor_with_text(const std::string& text)
{
	auto font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_24px, FDC_Regular);
	add_text(text, TEXTPROP_Slim, font);

	Separator();
}

void CGUIWidgets::add_separator()
{
	Separator();
}

void CGUIWidgets::render_clipped_contents(size_t size, const std::function<void(int line_no)>& callback)
{
	ImGuiListClipper clipper;
	clipper.Begin(size);
	while (clipper.Step())
	{
		for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
		{
			callback(line_no);
		}
	}
}

void CGUIWidgets::add_readmore_on_hover_widget(const char* text)
{
	push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	add_text("Read more...", TEXTPROP_None);
	if (IsItemHovered(ImGuiHoveredFlags_DelayNormal))
	{
		set_next_window_rounding(4.0f, ImDrawFlags_RoundCornersAll);
		BeginTooltip();
		PushTextWrapPos(GetFontSize() * 25.0f);
		add_text(text, TEXTPROP_Wrapped);
		add_padding({ 0.0f, 3.0f });
		PopTextWrapPos();
		EndTooltip();
	}
	pop_font();
}

void CGUIWidgets::add_readmore_on_hover_widget_ex(const std::function<void()>& callback)
{
	push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	add_text("Read more...", TEXTPROP_None);
	if (IsItemHovered(ImGuiHoveredFlags_DelayNormal))
	{
		set_next_window_rounding(4.0f, ImDrawFlags_RoundCornersAll);
		BeginTooltip();
		PushTextWrapPos(GetFontSize() * 25.0f);
		if (callback)
		{
			callback();
		}
		PopTextWrapPos();
		EndTooltip();
	}
	pop_font();
}

void CGUIWidgets::dialog_on_hover_widget(const std::function<void()>& callback)
{
	push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));
	if (IsItemHovered(ImGuiHoveredFlags_DelayNormal))
	{
		set_next_window_rounding(4.0f, ImDrawFlags_RoundCornersAll);
		BeginTooltip();
		PushTextWrapPos(GetFontSize() * 25.0f);
		if (callback)
		{
			callback();
		}
		PopTextWrapPos();
		EndTooltip();
	}
	pop_font();
}

void CGUIWidgets::add_progress_bar(const std::string& id, const Vector2D& size, float current, float max)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	float percentage = (current / max) * 100.0f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImGuiID _id = window->GetID(id.c_str());
	ImVec2 label_size = CalcTextSize(std::format("{} %", percentage).c_str(), NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 item_size = CalcItemSize(size, label_size.x + style.FramePadding.x, (label_size.y + style.FramePadding.y) * 0.2f);

	ImRect bb(pos, pos + ImVec2(item_size.x * (current / max), item_size.y));
	ImRect bb1(pos, pos + item_size);
	ItemSize(item_size, style.FramePadding.y);
	if (!ItemAdd(bb, _id) || !ItemAdd(bb1, _id))
	{
		return;
	}

	auto clr = get_color<GUICLR_FrameActive>();
	auto clr1 = get_color<GUICLR_FrameActive>();

	//clr.a = (current / max);
	clr1.a = 0.1f;

	RenderFrame(bb.Min, bb.Max, clr.as_u32(), true, 4.0f);
	RenderFrame(bb1.Min, bb1.Max, clr1.as_u32(), true, 4.0f);
}

bool CGUIWidgets::begin_listbox(const std::string& label, const std::string & preview_label, ImGuiComboFlags flags)
{
	bool ret = begin_combo_internal(label.c_str(), preview_label.c_str(), flags);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

void CGUIWidgets::end_listbox()
{
	EndPopup();
}

bool CGUIWidgets::add_simple_listbox(const std::string & label, int* value, const std::vector<std::string>& items, 
									 const std::function<void(int i)>& on_entry_click)
{
	if (label.find("##") == std::string::npos)
	{
		add_text(label);
	}

	bool edited = false;
	if (begin_listbox(label, items[*value]))
	{
		push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));
		push_stylevar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f });

		for (size_t i = 0; i < items.size(); i++)
		{
			bool selected = (i == *value);
			if (add_toggle_button(items[i], Vector2D(-1.0f, 0.0f), selected))
			{
				*value = i;
				if (on_entry_click)
				{
					on_entry_click(i);
				}
				close_current_popup();
				edited = true;
				break;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (selected)
				set_item_default_focus();
		}

		pop_stylevar();
		pop_font();

		end_listbox();
	}

	//if (edited)
	{
		// to close the list box after we select an item.
		close_current_popup();
	}

	return edited;
}

bool CGUIWidgets::add_selectable(const std::string& label, bool selected, ImGuiSelectableFlags flags, const Vector2D& size)
{
	return Selectable(label.c_str(), selected, flags, size);
}

bool CGUIWidgets::add_floating_button(const std::string& label, const Vector2D& last_cursor_pos, const Vector2D& button_pos, const Vector2D& button_size,
									  bool disabled, EButtonFlags flags)
{
	push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	set_cursor_pos(button_pos);

	bool ret = false;

	add_child(
		std::format("fbutton_{}", label),
		button_size,
		false, ImGuiWindowFlags_NoDecoration,
		[&]()
		{
			if (add_button(label, button_size, false, BUTTONFLAG_CenterLabel))
			{
				ret = true;
			}
		});

	pop_stylevar(1); // WindowPadding
	set_cursor_pos(last_cursor_pos);
	return ret;
}

bool CGUIWidgets::add_tree_node(const std::string& label)
{
	bool ret = TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

	if (IsItemHovered())
	{
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
	}

	return ret;
}

void CGUIWidgets::pop_tree_node()
{
	TreePop();
}

void CGUIWidgets::add_separated_heading(const std::string& label)
{
	ImGuiWindow* window = GetCurrentWindow();

	auto font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	float separator_padding = 1.0f;
	float extra_padding_beneath = 1.0f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label.c_str());
	auto label_size = CalcTextSize(label.c_str(), NULL, true);
	ImVec2 item_size = { window->Size.x - style.FramePadding.x * 2.0f - style.ScrollbarSize, label_size.y + separator_padding + 1.0f + extra_padding_beneath };

	ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + item_size);

	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	PushStyleColor(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextLight));

	// section title
	RenderText(pos + Vector2D(3.0f, 0.0f), label.c_str());

	PopStyleColor();

	pos.y += label_size.y + separator_padding;

	auto separator_color = g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator);

	// separator underneath
	window->DrawList->AddLine(pos, pos + Vector2D(item_size.x, 0.0f), separator_color.as_u32());
}

void CGUIWidgets::add_image(ImTextureID id, const Vector2D& size)
{
	Image(id, size);
}

void CGUIWidgets::add_table(const std::string& name, uint32_t columns, ImGuiTableFlags flags, 
							const std::function<void()>& header_callback, const std::function<void()>& body_callback)
{
	if (BeginTable(name.c_str(), columns, flags))
	{
		if (header_callback)
		{
			header_callback();
			//TableHeadersRow();
		}

		if (body_callback)
			body_callback();

		EndTable();
	}
}

void CGUIWidgets::add_undecorated_simple_table(const std::string& name, uint32_t columns, const std::function<void()>& body_callback)
{
	const auto header_callback = [&]()
	{
		for (uint32_t i = 0; i < columns; i++)
			table_setup_column(std::format("column{}", columns), ImGuiTableColumnFlags_None);
	};

	add_table(name, columns, ImGuiTableFlags_None, header_callback, body_callback);
}

void CGUIWidgets::table_setup_column_fixed_width(const std::string& name, float width, ImGuiTableColumnFlags flags)
{
	TableSetupColumn(!name.empty() ? name.c_str() : nullptr, flags | ImGuiTableColumnFlags_WidthFixed, width);
}

void CGUIWidgets::table_setup_column(const std::string& name, ImGuiTableColumnFlags flags)
{
	TableSetupColumn(!name.empty() ? name.c_str() : nullptr, flags);
}

void CGUIWidgets::table_next_column()
{
	TableNextColumn();
}

void CGUIWidgets::table_headers_row()
{
	TableHeadersRow();
}

void CGUIWidgets::delete_chars_on_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, int bytes_count)
{
	data->DeleteChars(pos, bytes_count);
}

void CGUIWidgets::insert_chars_to_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, const char* text, const char* text_end)
{
	data->InsertChars(pos, text, text_end);
}

void CGUIWidgets::begin_tab(const std::string& label, ImGuiTabBarFlags flags)
{
	BeginTabBar(label.c_str(), flags);
}

void CGUIWidgets::add_tab_item(const std::string& label, bool border, const Vector2D& size, const std::function<void()>& pfn_contents, const Vector2D& child_padding)
{
	if (BeginTabItem(label.c_str()))
	{
		if (!child_padding.IsZero())
		{
			PushStyleVar(ImGuiStyleVar_WindowPadding, child_padding);
		}

		BeginChild(label.c_str(), size, border);

		if (pfn_contents)
		{
			pfn_contents();
		}

		EndChild();

		if (!child_padding.IsZero())
		{
			PopStyleVar(); // padding
		}

		EndTabItem();
	}
}

void CGUIWidgets::end_tab()
{
	EndTabBar();
}

void CGUIWidgets::add_console()
{
	g_devconsole_i->render();
}

void CGUIWidgets::render_arrow(const Vector2D& pos, const CColor& color, EArrowDirection direction)
{
	RenderArrow(GetWindowDrawList(), pos, color.as_u32(), direction);
}

//-----------------------------------------------------------------------------------------------------------------
//
//	Internal API
//

void CGUIWidgets::add_bullet_text_internal(const std::string& text, ETextProperties properties, ImFont* font)
{
	Bullet();
	add_text(text, properties, font);
}

void CGUIWidgets::add_window_centered_text_ex(const std::string& text, const CColor& text_color, ImFont* font)
{
	auto window_size = GetWindowSize();
	auto cursor_screen = GetCursorScreenPos();

	auto label_size = g_gui_fontmgr_i->calc_font_text_size(font, text.c_str());

	auto& style = GetStyle();

	ImVec2 center_pos =
	{
		cursor_screen.x + window_size.x / 2 - label_size.x / 2 - style.WindowPadding.x,
		cursor_screen.y + window_size.y / 2 - label_size.y / 2 - style.WindowPadding.y,
	};

	GetWindowDrawList()->AddText(font, font->FontSize, center_pos, text_color.as_u32(), text.c_str());
}

bool CGUIWidgets::add_button_internal(const char* label, const Vector2D& size, bool selected, EButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImGuiID id = window->GetID(label);
	ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 item_size = CalcItemSize(size, label_size.x + style.FramePadding.x, label_size.y + style.FramePadding.y);

	const ImRect bb(pos, pos + item_size);
	ItemSize(item_size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
	{
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	CColor color_4f;
	if (held && hovered)
	{
		color_4f = get_color<GUICLR_FrameBgActive>();
	}
	else if (hovered)
	{
		color_4f = get_color<GUICLR_FrameBgHovered>();
	}
	else if (selected)
	{
		color_4f = get_color<GUICLR_FrameBgSelected>();
	}
	else
	{
		color_4f = get_color<GUICLR_Button>();
	}
	

	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, color_4f.as_u32(), true, 4.0f);

	if (flags & BUTTONFLAG_CenterLabel)
	{
		RenderTextClipped({ bb.Min.x, bb.Min.y },
						  bb.Max, label, NULL, &label_size, { 0.5f, 0.5f }, &bb);
	}
	else
	{
		RenderTextClipped({ bb.Min.x + style.FramePadding.x, bb.Min.y },
						  bb.Max, label, NULL, &label_size, { 0.f, 0.f }, &bb);
	}

	return pressed;
}

template<typename T>
bool CGUIWidgets::add_checkbox_internal(const char* label, T* value)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const float square_sz = GetFrameHeight();
	ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
											label_size.y + style.FramePadding.y * 2.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id))
	{
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*value = !(*value);
		MarkItemEdited(id);
	}

	CColor color_4f;
	if (*value)
	{
		color_4f = get_color<GUICLR_FrameActive>();
	}
	else if (held)
	{
		color_4f = get_color<GUICLR_FrameActive>();
	}
	else
	{
		color_4f = get_color<GUICLR_CheckBoxBackground>();
	}

	ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
	RenderNavHighlight(total_bb, id);
	RenderFrame(check_bb.Min, check_bb.Max, color_4f.as_u32(), false, 4.0f);

	// Only handle hovered state when not selected
	if (!*value)
	{
		if (hovered)
		{
			PushStyleColor(ImGuiCol_Border, get_color_u32<GUICLR_FrameActive>());
			PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
		}

		RenderFrameBorder(check_bb.Min, check_bb.Max, 4.0f);

		if (hovered)
		{
			PopStyleVar(1);
			PopStyleColor(1);
		}
	}

	ImU32 check_col = get_color_u32<GUICLR_CheckBoxCheckMark>();
	bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
	if (mixed_value)
	{
		// Undocumented tristate/mixed/indeterminate checkbox (#2644)
		// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
		ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
		window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
	}
	else if (*value)
	{
		const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 5.0f));
		RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
	}

	// - 1.f due to not perfect center alignment.
	ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y - 1.f);
	if (label_size.x > 0.0f)	
	{
		RenderText(label_pos, label);
	}

	return pressed;
}

bool CGUIWidgets::add_checkbox_with_color_internal(const char* label, float* value, float rgba[4], bool input_values, ImGuiColorEditFlags flags)
{
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImGuiWindow* window = g.CurrentWindow;

	BeginGroup();

	bool pressed = add_checkbox_internal(label, value);

	float color_picker_rect_size = 18.0f;
	SameLine();
	SetCursorPosX(window->Size.x - style.WindowPadding.x - style.ScrollbarSize - color_picker_rect_size);

	char buffer[256];
	sprintf(buffer, "##%s", label);

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	PushStyleVar(ImGuiStyleVar_PopupRounding, 6.0f);

	color_edit_4_internal(buffer, rgba, input_values,
						  flags |
						  ImGuiColorEditFlags_NoInputs |
						  ImGuiColorEditFlags_NoOptions |
						  ImGuiColorEditFlags_NoTooltip |
						  ImGuiColorEditFlags_NoLabel | 
						  ImGuiColorEditFlags_NoDragDrop);

	PopStyleVar(2);

	EndGroup();

	return pressed;
}

void CGUIWidgets::add_text_wrapped(const char* text)
{
	ImGuiContext& g = *GImGui;
	const bool need_backup = (g.CurrentWindow->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
	if (need_backup)
		PushTextWrapPos(0.0f);
	TextEx(text, NULL, ImGuiTextFlags_NoWidthForLargeClippedText);
	if (need_backup)
		PopTextWrapPos();
}

bool CGUIWidgets::color_button_internal(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(desc_id);
	const float default_size = GetFrameHeight();
	const ImVec2 size(size_arg.x == 0.0f ? default_size : size_arg.x, size_arg.y == 0.0f ? default_size : size_arg.y);
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (flags & ImGuiColorEditFlags_NoAlpha)
		flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImVec4 col_rgb = col;
	if (flags & ImGuiColorEditFlags_InputHSV)
		ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

	ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
	float grid_step = ImMin(size.x, size.y) / 2.99f;
	float rounding = /*ImMin(g.Style.FrameRounding, grid_step * 0.5f)*/20.0f; // oxware: we do "circle"-like rounded color buttons.
	ImRect bb_inner = bb;
	float off = 0.0f;
	if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
	{
		off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
		bb_inner.Expand(off);
	}
	if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
	{
		float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
		RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawFlags_RoundCornersRight);
		window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_rgb_without_alpha), rounding, ImDrawFlags_RoundCornersLeft);
	}
	else
	{
		// Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
		ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;
		window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding);
	}
	RenderNavHighlight(bb, id);
	if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
	{
		if (g.Style.FrameBorderSize > 0.0f)
		{
			if (hovered)
				PushStyleColor(ImGuiCol_Border, get_color<GUICLR_FrameActive>());

			RenderFrameBorder(bb.Min, bb.Max, rounding);

			if (hovered)
				PopStyleColor();
		}
		else
		{
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border
		}
	}

	// Drag and Drop Source
	// NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
	if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
	{
		if (flags & ImGuiColorEditFlags_NoAlpha)
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
		else
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
		color_button_internal(desc_id, col, flags);
		SameLine();
		TextEx("Color");
		EndDragDropSource();
	}

	// Tooltip
	if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
		ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

	return pressed;
}

bool CGUIWidgets::begin_combo_internal(const char* label, const char* preview_label, ImGuiComboFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
	g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

	const float height_factor = 0.3f;
	const ImVec2 arrow_size = { (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight(), GetFrameHeight() * height_factor };
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const float full_w = window->Size.x - style.FramePadding.x * 1.5f;
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size.x : window->Size.x - style.WindowPadding.x - style.ScrollbarSize;
	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f * height_factor));
	const ImRect total_bb(bb.Min, bb.Max + ImVec2(style.ItemInnerSpacing.x, 0.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &bb))
		return false;

	// Open on click
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);
	const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
	bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
	if (pressed && !popup_open)
	{
		OpenPopupEx(popup_id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	// Render shape
	const ImU32 frame_col = get_color<GUICLR_ListBoxBackground>().as_u32();
	const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size.x);
	RenderNavHighlight(bb, id);
	if (!(flags & ImGuiComboFlags_NoPreview))
		window->DrawList->AddRectFilled(bb.Min, ImVec2(value_x2, bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		ImU32 bg_col = get_color<GUICLR_ListBoxArrowBackground>().as_u32();
		ImU32 text_col = GetColorU32(ImGuiCol_Text);
		window->DrawList->AddRectFilled(ImVec2(value_x2, bb.Min.y), bb.Max, bg_col, style.FrameRounding, (w <= arrow_size.x) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
		if (value_x2 + arrow_size.x - style.FramePadding.x <= bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), text_col, !popup_open ? ImGuiDir_Down : ImGuiDir_Left, 0.8f);
	}

	float border_size = 0.0f;

	if (hovered)
	{
		PushStyleColor(ImGuiCol_Border, get_color_u32<GUICLR_FrameActive>());
		border_size = 2.0f;
	}
	else
	{
		PushStyleColor(ImGuiCol_Border, get_color_u32<GUICLR_ListBoxBorder>());
		border_size = 0.0f;
	}

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, border_size);

	RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);

	PopStyleVar(1);
	PopStyleColor(1);

	// Custom preview
	if (flags & ImGuiComboFlags_CustomPreview)
	{
		g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
		IM_ASSERT(preview_label == NULL || preview_label[0] == 0);
		preview_label = NULL;
	}

	PushFont(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));

	// Render preview and label
	if (preview_label != NULL && !(flags & ImGuiComboFlags_NoPreview))
	{
		if (g.LogEnabled)
			LogSetNextTextDecoration("{", "}");
		RenderTextClipped(bb.Min + style.FramePadding - ImVec2(0.0f, 3.0f), ImVec2(value_x2, bb.Max.y), preview_label, NULL, NULL);
	}

	PopFont();

	if (!popup_open)
		return false;

	g.NextWindowData.Flags = backup_next_window_data_flags;
	return BeginComboPopup(popup_id, bb, flags);
}

bool CGUIWidgets::color_edit_4_internal(const char * label, float rgba[4], bool input_values, ImGuiColorEditFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float square_sz = GetFrameHeight();
	const float w_full = CalcItemWidth();
	const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
	const float w_inputs = w_full - w_button;
	const char* label_display_end = FindRenderedTextEnd(label);
	g.NextItemData.ClearFlags();

	BeginGroup();
	PushID(label);
	const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
	if (set_current_color_edit_id)
		g.ColorEditCurrentID = window->IDStack.back();

	// If we're not showing any slider there's no point in doing any HSV conversions
	const ImGuiColorEditFlags flags_untouched = flags;
	if (flags & ImGuiColorEditFlags_NoInputs)
		flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

	// Context menu: display and modify options (before defaults are applied)
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorEditOptionsPopup(rgba, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags_DisplayMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
	if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
	if (!(flags & ImGuiColorEditFlags_PickerMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
	if (!(flags & ImGuiColorEditFlags_InputMask_))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
	flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
	const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
	const int components = alpha ? 4 : 3;

	// Convert to the formats we need
	float f[4] = { rgba[0], rgba[1], rgba[2], alpha ? rgba[3] : 1.0f };
	if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
		ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
	else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
	{
		// Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
		ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
		ColorEditRestoreHS(rgba, &f[0], &f[1], &f[2]);
	}
	int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

	bool value_changed = false;
	bool value_changed_as_float = false;

	const ImVec2 pos = window->DC.CursorPos;
	const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
	window->DC.CursorPos.x = pos.x + inputs_offset_x;

	if (input_values && (flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB/HSV 0..255 Sliders
		const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
		const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

		const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
		static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		static const char* fmt_table_int[2][4] =
		{
			{   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
			{ "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
		};
		static const char* fmt_table_float[3][4] =
		{
			{   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
			{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
		};
		const int fmt_idx = hide_prefix ? 0 : 1;

		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

			// FIXME: When ImGuiColorEditFlags_HDR flag is passed HS values snap in weird ways when SV values go below 0.
			if (flags & ImGuiColorEditFlags_Float)
			{
				value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
				value_changed_as_float |= value_changed;
			}
			else
			{
				value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
			}
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
		}
	}
	else if (input_values && (flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB Hexadecimal Input
		char buf[64];
		if (alpha)
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
		SetNextItemWidth(w_inputs);
		if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
		{
			value_changed = true;
			char* p = buf;
			while (*p == '#' || ImCharIsBlankA(*p))
				p++;
			i[0] = i[1] = i[2] = 0;
			i[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
			int r;
			if (alpha)
				r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
			else
				r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
			IM_UNUSED(r); // Fixes C6031: Return value ignored: 'sscanf'.
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
	}

	ImGuiWindow* picker_active_window = NULL;
	if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
	{
		const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
		window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

		const ImVec4 col_v4(rgba[0], rgba[1], rgba[2], alpha ? rgba[3] : 1.0f);
		if (color_button_internal("##ColorButton", col_v4, flags))
		{
			if (!(flags & ImGuiColorEditFlags_NoPicker))
			{
				// Store current color and open a picker
				g.ColorPickerRef = col_v4;
				OpenPopup("picker");
				SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

		if (BeginPopup("picker"))
		{
			if (g.CurrentWindow->BeginCount == 1)
			{
				picker_active_window = g.CurrentWindow;
				if (label != label_display_end)
				{
					TextEx(label, label_display_end);
					Spacing();
				}
				ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
				ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
				SetNextItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
				value_changed |= color_picker_4_internal("##picker", rgba, input_values, picker_flags, &g.ColorPickerRef.x);
			}
			EndPopup();
		}
	}

	if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
	{
		// Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
		// but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
		SameLine(0.0f, style.ItemInnerSpacing.x);
		window->DC.CursorPos.x = pos.x + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x);
		TextEx(label, label_display_end);
	}

	// Convert back
	if (value_changed && picker_active_window == NULL)
	{
		if (!value_changed_as_float)
			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
		{
			g.ColorEditSavedHue = f[0];
			g.ColorEditSavedSat = f[1];
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
			g.ColorEditSavedID = g.ColorEditCurrentID;
			g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
		}
		if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
			ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

		rgba[0] = f[0];
		rgba[1] = f[1];
		rgba[2] = f[2];
		if (alpha)
			rgba[3] = f[3];
	}

	if (set_current_color_edit_id)
		g.ColorEditCurrentID = 0;
	PopID();
	EndGroup();

	// Drag and Drop Target
	// NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
	if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
	{
		bool accepted_drag_drop = false;
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
		{
			memcpy((float*)rgba, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
			value_changed = accepted_drag_drop = true;
		}
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			memcpy((float*)rgba, payload->Data, sizeof(float) * components);
			value_changed = accepted_drag_drop = true;
		}

		// Drag-drop payloads are always RGB
		if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
			ColorConvertRGBtoHSV(rgba[0], rgba[1], rgba[2], rgba[0], rgba[1], rgba[2]);
		EndDragDropTarget();
	}

	// When picker is being actively used, use its active id so IsItemActive() will function on color_edit_4_internal().
	if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
		g.LastItemData.ID = g.ActiveId;

	if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

bool CGUIWidgets::color_picker_4_internal(const char * label, float rgba[4], bool input_values, ImGuiColorEditFlags flags, float * ref_col)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImDrawList* draw_list = window->DrawList;
	ImGuiStyle& style = g.Style;
	ImGuiIO& io = g.IO;

	const float width = CalcItemWidth();
	g.NextItemData.ClearFlags();

	PushID(label);
	const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
	if (set_current_color_edit_id)
		g.ColorEditCurrentID = window->IDStack.back();
	BeginGroup();

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
		flags |= ImGuiColorEditFlags_NoSmallPreview;

	// Context menu: display and store options.
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		color_picker_options_popup(rgba, input_values, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags_PickerMask_))
		flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
	if (!(flags & ImGuiColorEditFlags_InputMask_))
		flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

	// Setup
	int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
	bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
	ImVec2 picker_pos = window->DC.CursorPos;
	float square_sz = GetFrameHeight();
	float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
	float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
	float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
	float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
	float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

	float backup_initial_col[4];
	memcpy(backup_initial_col, rgba, components * sizeof(float));

	float wheel_thickness = sv_picker_size * 0.08f;
	float wheel_r_outer = sv_picker_size * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

	// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
	float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
	ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
	ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
	ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

	float H = rgba[0], S = rgba[1], V = rgba[2];
	float R = rgba[0], G = rgba[1], B = rgba[2];
	if (flags & ImGuiColorEditFlags_InputRGB)
	{
		// Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
		ColorConvertRGBtoHSV(R, G, B, H, S, V);
		ColorEditRestoreHS(rgba, &H, &S, &V);
	}
	else if (flags & ImGuiColorEditFlags_InputHSV)
	{
		ColorConvertHSVtoRGB(H, S, V, R, G, B);
	}

	bool value_changed = false, value_changed_h = false, value_changed_sv = false;

	PushItemFlag(ImGuiItemFlags_NoNav, true);
	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Hue wheel + SV triangle logic
		InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
		if (IsItemActive())
		{
			ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
			ImVec2 current_off = g.IO.MousePos - wheel_center;
			float initial_dist2 = ImLengthSqr(initial_off);
			if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
			{
				// Interactive with Hue wheel
				H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
				if (H < 0.0f)
					H += 1.0f;
				value_changed = value_changed_h = true;
			}
			float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
			float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
			if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
			{
				// Interacting with SV triangle
				ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
				if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
					current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
				float uu, vv, ww;
				ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
				V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
				S = ImClamp(uu / V, 0.0001f, 1.0f);
				value_changed = value_changed_sv = true;
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{
		// SV rectangle logic
		InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
		if (IsItemActive())
		{
			S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
			V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			ColorEditRestoreH(rgba, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
			value_changed = value_changed_sv = true;
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

		// Hue bar logic
		SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
		InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
		if (IsItemActive())
		{
			H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			value_changed = value_changed_h = true;
		}
	}

	// Alpha bar logic
	if (alpha_bar)
	{
		SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
		InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
		if (IsItemActive())
		{
			rgba[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
			value_changed = true;
		}
	}
	PopItemFlag(); // ImGuiItemFlags_NoNav

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
	{
		SameLine(0, style.ItemInnerSpacing.x);
		BeginGroup();
	}

	if (!(flags & ImGuiColorEditFlags_NoLabel))
	{
		const char* label_display_end = FindRenderedTextEnd(label);
		if (label != label_display_end)
		{
			if ((flags & ImGuiColorEditFlags_NoSidePreview))
				SameLine(0, style.ItemInnerSpacing.x);
			TextEx(label, label_display_end);
		}
	}

	if (!(flags & ImGuiColorEditFlags_NoSidePreview))
	{
#if 0
		PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
		ImVec4 col_v4(rgba[0], rgba[1], rgba[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : rgba[3]);
		if ((flags & ImGuiColorEditFlags_NoLabel))
			Text("Current");

		ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip;
		color_button_internal("##current", col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2));
		if (ref_col != NULL)
		{
			Text("Original");
			ImVec4 ref_col_v4(ref_col[0], ref_col[1], ref_col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : ref_col[3]);
			if (color_button_internal("##original", ref_col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2)))
			{
				memcpy(rgba, ref_col, components * sizeof(float));
				value_changed = true;
			}
		}
		PopItemFlag();
#endif
		EndGroup();
	}

	// Convert back color to RGB
	if (value_changed_h || value_changed_sv)
	{
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			ColorConvertHSVtoRGB(H, S, V, rgba[0], rgba[1], rgba[2]);
			g.ColorEditSavedHue = H;
			g.ColorEditSavedSat = S;
			g.ColorEditSavedID = g.ColorEditCurrentID;
			g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(rgba[0], rgba[1], rgba[2], 0));
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			rgba[0] = H;
			rgba[1] = S;
			rgba[2] = V;
		}
	}

	// R,G,B and H,S,V slider color editor
	bool value_changed_fix_hue_wrap = false;
	if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
		ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
		ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
		if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
			if (color_edit_4_internal("##rgb", rgba, input_values, sub_flags | ImGuiColorEditFlags_DisplayRGB))
			{
				// FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
				// For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
				value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
				value_changed = true;
			}
		if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
			value_changed |= color_edit_4_internal("##hsv", rgba, input_values, sub_flags | ImGuiColorEditFlags_DisplayHSV);
		if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
			value_changed |= color_edit_4_internal("##hex", rgba, input_values, sub_flags | ImGuiColorEditFlags_DisplayHex);
		PopItemWidth();
	}

	// Try to cancel hue wrap (after color_edit_4_internal call), if any
	if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
	{
		float new_H, new_S, new_V;
		ColorConvertRGBtoHSV(rgba[0], rgba[1], rgba[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0)
		{
			if (new_V <= 0 && V != new_V)
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, rgba[0], rgba[1], rgba[2]);
			else if (new_S <= 0)
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, rgba[0], rgba[1], rgba[2]);
		}
	}

	if (value_changed)
	{
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			R = rgba[0];
			G = rgba[1];
			B = rgba[2];
			ColorConvertRGBtoHSV(R, G, B, H, S, V);
			ColorEditRestoreHS(rgba, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			H = rgba[0];
			S = rgba[1];
			V = rgba[2];
			ColorConvertHSVtoRGB(H, S, V, R, G, B);
		}
	}

	const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
	const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
	const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
	const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
	const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

	ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
	ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
	ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

	ImVec2 sv_cursor_pos;

	if (flags & ImGuiColorEditFlags_PickerHueWheel)
	{
		// Render Hue Wheel
		const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
		const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
		for (int n = 0; n < 6; n++)
		{
			const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
			const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
			const int vert_start_idx = draw_list->VtxBuffer.Size;
			draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
			draw_list->PathStroke(col_white, 0, wheel_thickness);
			const int vert_end_idx = draw_list->VtxBuffer.Size;

			// Paint colors over existing vertices
			ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
			ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
			ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
		}

		// Render Cursor + preview on Hue Wheel
		float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
		float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
		ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
		float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
		int hue_cursor_segments = ImClamp((int)(hue_cursor_rad / 1.4f), 9, 32);
		draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
		draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

		// Render SV triangle (rotated according to hue)
		ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
		ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
		ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
		ImVec2 uv_white = GetFontTexUvWhitePixel();
		draw_list->PrimReserve(6, 6);
		draw_list->PrimVtx(tra, uv_white, hue_color32);
		draw_list->PrimVtx(trb, uv_white, hue_color32);
		draw_list->PrimVtx(trc, uv_white, col_white);
		draw_list->PrimVtx(tra, uv_white, 0);
		draw_list->PrimVtx(trb, uv_white, col_black);
		draw_list->PrimVtx(trc, uv_white, 0);
		draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
		sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
	}
	else if (flags & ImGuiColorEditFlags_PickerHueBar)
	{
		// Render SV Square
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
		RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
		sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
		sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

		// Render Hue Bar
		for (int i = 0; i < 6; ++i)
			draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
		float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
		RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
		RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
	}

	// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
	float sv_cursor_rad = value_changed_sv ? 10.0f : 6.0f;
	draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, 12);
	draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, 12);
	draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, 12);

	// Render alpha bar
	if (alpha_bar)
	{
		float alpha = ImSaturate(rgba[3]);
		ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
		RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
		draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
		float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
		RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
		RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
	}

	EndGroup();

	if (value_changed && memcmp(backup_initial_col, rgba, components * sizeof(float)) == 0)
		value_changed = false;
	if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
		MarkItemEdited(g.LastItemData.ID);

	if (set_current_color_edit_id)
		g.ColorEditCurrentID = 0;
	PopID();

	return value_changed;
}

void CGUIWidgets::color_picker_options_popup(const float* ref_col, bool input_values, ImGuiColorEditFlags flags)
{
	bool allow_opt_picker = !(flags & ImGuiColorEditFlags_PickerMask_);
	bool allow_opt_alpha_bar = !(flags & ImGuiColorEditFlags_NoAlpha) && !(flags & ImGuiColorEditFlags_AlphaBar);
	if ((!allow_opt_picker && !allow_opt_alpha_bar) || !BeginPopup("context"))
		return;
	ImGuiContext& g = *GImGui;
	if (allow_opt_picker)
	{
		ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
		PushItemWidth(picker_size.x);
		for (int picker_type = 0; picker_type < 2; picker_type++)
		{
			// Draw small/thumbnail version of each picker type (over an invisible button for selection)
			if (picker_type > 0) Separator();
			PushID(picker_type);
			ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | (flags & ImGuiColorEditFlags_NoAlpha);
			if (picker_type == 0) picker_flags |= ImGuiColorEditFlags_PickerHueBar;
			if (picker_type == 1) picker_flags |= ImGuiColorEditFlags_PickerHueWheel;
			ImVec2 backup_pos = GetCursorScreenPos();
			if (Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
				g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_PickerMask_) | (picker_flags & ImGuiColorEditFlags_PickerMask_);
			SetCursorScreenPos(backup_pos);
			ImVec4 previewing_ref_col;
			memcpy(&previewing_ref_col, ref_col, sizeof(float) * ((picker_flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4));
			color_picker_4_internal("##previewing_picker", &previewing_ref_col.x, input_values, picker_flags);
			PopID();
		}
		PopItemWidth();
	}
	if (allow_opt_alpha_bar)
	{
		if (allow_opt_picker) Separator();
		CheckboxFlags("Alpha Bar", &g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar);
	}
	EndPopup();
}

template<typename T>
bool CGUIWidgets::add_slider_internal(const char* label, T* value, T* min, T* max, const char* format, ImGuiDataType data_type, bool no_label, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = window->Size.x - style.WindowPadding.x - style.ScrollbarSize;

	ImVec2 label_size = CalcTextSize(label, NULL, true);

	if (no_label)
	{
		label_size.x = 0;
	}

	ImRect frame_bb({ window->DC.CursorPos.x + (no_label ? 0.0f : (window->Size.x / 2.f)), window->DC.CursorPos.y },
					window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	char label_fixed[256];
	strcpy(label_fixed, label);

	if (label_size.x > 0.0f && label_size.x > frame_bb.Min.x - window->DC.CursorPos.x - 1.f)
	{
		const float char_size = label_size.x / strlen(label_fixed);

		int char_last = (int)((window->Size.x / 2.f) / char_size);

		label_fixed[char_last - 3] = '.';
		label_fixed[char_last - 2] = '.';
		label_fixed[char_last - 1] = '.';
		label_fixed[char_last] = '\0';

		label_size = CalcTextSize(label_fixed, NULL, true);
	}

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb, false))
		return false;

	// Default format string when passing NULL
	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered = ItemHoverable(frame_bb, id);
	const bool clicked = hovered && IsMouseClicked(0, id);
	const bool make_active = (clicked);
	if (make_active && clicked)
		SetKeyOwner(ImGuiKey_MouseLeft, id);

	if (make_active)
	{
		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
		g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
	}

	// - 1.f due to not perfect center alignment.
	if (label_size.x > 0.0f)
		RenderText(ImVec2(window->DC.CursorPos.x, frame_bb.Min.y + style.FramePadding.y - 3.f), label_fixed);

	// Slider behavior
	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, value, min, max, format, flags, &grab_bb);
	if (value_changed)
		MarkItemEdited(id);

	const float grab_bb_oversize_add = 1.5f;
	const ImVec2 grab_bb_size = {
		(grab_bb.Max.x - grab_bb.Min.x) / 2.f,
		(grab_bb.Max.y - grab_bb.Min.y) / 2.f };

	// Draw frame
	CColor frame_col;

	if (hovered || g.ActiveId == id)
	{
		frame_col = get_color<GUICLR_SliderHovered>();
	}
	else
	{
		frame_col = get_color<GUICLR_FrameBg>();
	}

	const float width = 5.0f;
	ImVec2 frame_bb_thin_min = { frame_bb.Min.x, frame_bb.Min.y + ((frame_bb.Max.y - frame_bb.Min.y) / 2.f) - width / 2.f };
	ImVec2 frame_bb_thin_max = { frame_bb.Max.x, frame_bb.Max.y - ((frame_bb.Max.y - frame_bb.Min.y) / 2.f) + width / 2.f };
	ImVec2 frame_bb_thin_size = frame_bb_thin_max - frame_bb_thin_min;
	RenderFrame(frame_bb_thin_min, frame_bb_thin_max, frame_col.as_u32(), false, 1.f);

	// Render grab
	if (grab_bb.Max.x > grab_bb.Min.x)
		window->DrawList->AddCircleFilled(grab_bb.Min + grab_bb_size, grab_bb_size.x + grab_bb_oversize_add,
										  get_color_u32<GUICLR_FrameActive>(), 12);

	// Render blue active field behind grab
	RenderFrame(frame_bb_thin_min,
				{ grab_bb.Min.x + grab_bb_size.x, frame_bb_thin_max.y },
				get_color_u32<GUICLR_FrameActive>(), false, 1.f);

	// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, value, format);

	const ImVec2 value_label_size = CalcTextSize(value_buf, NULL, true);

	PushFont(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Bold));

	const float value_label_offset = 1.f;
	RenderTextClipped({ frame_bb_thin_min.x + frame_bb_thin_size.x / 2.f - value_label_size.x / 2.f, frame_bb_thin_min.y - value_label_size.y - value_label_offset },
					  { frame_bb_thin_max.x - frame_bb_thin_size.x / 2.f + value_label_size.x / 2.f, frame_bb_thin_min.y },
					  value_buf, value_buf_end, NULL);

	PopFont();

	return value_changed;
}
