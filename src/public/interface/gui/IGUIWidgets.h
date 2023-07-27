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

#ifndef IGUIWIDGETS_H
#define IGUIWIDGETS_H
#pragma once

#include "commondefs.h"

#include <imgui.h>

#include <functional>

enum ETextProperties
{
	TEXTPROP_None = 0,

	// decorations
	TEXTPROP_Slim = BIT(1),
	TEXTPROP_Disabled = BIT(2),

	// colors
	TEXTPROP_ColorWhite = BIT(3),
	TEXTPROP_ColorRegular = BIT(4),
	TEXTPROP_ColorDark = BIT(5),
	TEXTPROP_ColorBlack = BIT(6),

	// properties
	TEXTPROP_Wrapped = BIT(7),
};

enum EButtonFlags
{
	BUTTONFLAG_None = 0,
	BUTTONFLAG_CenterLabel = BIT(0),
	BUTTONFLAG_NoHover = BIT(1),
};

enum EArrowDirection
{
	ARROWDIR_None = -1,
	ARROWDIR_Left = 0,
	ARROWDIR_Right = 1,
	ARROWDIR_Up = 2,
	ARROWDIR_Down = 3,
	ARROWDIR_COUNT
};


struct GLFWwindow;
struct ImFont;

typedef void(__cdecl*SectionTabFn)(void* arg);

class IGUIWidgets : public IBaseInterface
{
public:
	//
	// Windows
	//

	virtual void set_next_window_size(const Vector2D& size, ImGuiCond condition = ImGuiCond_None) = 0;
	virtual void set_next_window_pos(const Vector2D& pos, ImGuiCond condition = ImGuiCond_None) = 0;
	virtual void set_next_window_size_constrains(const Vector2D& min, const Vector2D& max) = 0;
	virtual void center_next_window_pos(ImGuiCond condition) = 0;
	virtual void set_next_window_rounding(float rounding, ImDrawFlags flags = ImDrawFlags_None) = 0;

	virtual void create_new_window(const std::string& name, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents) = 0;

	virtual void block_input_on_all_except_popup(bool block) = 0;

	virtual void add_child(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents) = 0;

	virtual Vector2D get_current_window_pos() = 0;
	virtual Vector2D get_current_window_size() = 0;

	virtual void schedule_simple_popup_dialog(const std::string& label) = 0;
	virtual bool execute_simple_popup_popup(const std::string& name, const Vector2D& size, ImGuiWindowFlags flags, const std::function<void()>& callback) = 0;

	virtual void show_font_atlas() = 0;

	//
	// Properties
	//

	virtual void push_disabled() = 0;
	virtual void pop_disabled() = 0;

	virtual void begin_group() = 0;
	virtual void end_group() = 0;

	virtual void push_stylevar(ImGuiStyleVar idx, float val) = 0;
	virtual void push_stylevar(ImGuiStyleVar idx, const Vector2D& val) = 0;
	virtual void pop_stylevar(size_t amount = 1) = 0;

	virtual void push_color(ImGuiCol idx, const CColor& color) = 0;
	virtual void pop_color(size_t amount = 1) = 0;

	virtual void push_font(ImFont* font) = 0;
	virtual void pop_font() = 0;

	virtual void set_cursor_pos(const Vector2D& pos) = 0;
	virtual Vector2D get_cursor_pos() = 0;
	virtual Vector2D get_cursor_screen_pos() = 0;

	virtual Vector2D get_content_region_avail() = 0;

	virtual void sameline(float offset_from_start_x = 0.0f, float spacing = -1.0f) = 0;

	virtual bool is_last_widget_hovered() = 0;

	virtual void set_item_default_focus() = 0;
	virtual void set_keyboard_focus_here(int offset = 0) = 0;
	virtual bool is_last_item_focused() = 0;

	virtual void set_scroll_here_y(float center_y_ratio = 0.5f) = 0;

	virtual void mark_last_as_edited() = 0;

	virtual void close_current_popup() = 0;

	virtual void push_executing_popup_code() = 0;
	virtual void pop_executing_popup_code() = 0;

	virtual Vector2D get_current_scroll() = 0;
	virtual void set_scroll(const Vector2D& xy) = 0; // provide -1 for no scroll

	virtual ImGuiStyle& get_imgui_style() = 0;

	virtual bool is_last_widget_focused() = 0;
	virtual bool is_widget_focused(const char* str_id) = 0;

	virtual void push_id(const std::string& id) = 0;
	virtual void push_id(int id) = 0;
	virtual void pop_id() = 0;

	//
	// Widgets
	//

	virtual void add_text(const std::string& text, ETextProperties properties = TEXTPROP_None, ImFont* font = nullptr) = 0;
	virtual void add_bullet_text(const std::string& text, ETextProperties properties = TEXTPROP_None, ImFont* font = nullptr) = 0;
	virtual void add_colored_text(const CColor& color, const std::string& text, ETextProperties properties = TEXTPROP_None) = 0;
	virtual void add_window_centered_text(const std::string& text, ImFont* font = nullptr) = 0;
	virtual void add_window_centered_text_disabled(const std::string& text, ImFont* font = nullptr) = 0;

	virtual bool add_button(const std::string& label, const Vector2D& size, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None) = 0;
	virtual bool add_toggle_button(const std::string& label, const Vector2D& size, bool selected = false, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None, CColor unselected_color = CColor(0, 0, 0, 0)) = 0;
	virtual bool add_toggle_button_custom(const std::string& label, const Vector2D& size, bool selected = false, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None) = 0;
	virtual bool add_invisible_button_behaviour(const std::string& label, const Vector2D& pos, const Vector2D& size) = 0;

	virtual bool add_url_text(const std::string& label) = 0;

	virtual bool add_checkbox(const std::string& label, float* value) = 0;
	virtual bool add_checkbox(const std::string& label, bool* value) = 0;
	virtual bool add_checkbox_with_color(const std::string& label, float* value, float rgba[4], bool input_values = false, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None) = 0;

	virtual bool add_color_edit(const std::string& label, float rgba[4], bool input_values = false, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None) = 0;

	virtual bool add_slider(const std::string& label, float* value, float* min, float* max, const char* format, bool no_label = false) = 0;
	virtual bool add_slider(const std::string& label, int* value, int* min, int* max, const char* format, bool no_label = false) = 0;

	virtual bool add_text_input(const std::string& label, char* buffer, size_t buffer_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, bool no_title = false, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) = 0;
	virtual bool add_text_input_ex(const std::string& label, char* buffer, size_t buffer_size, Vector2D input_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr, ImFont* font = 0) = 0;

	virtual void add_padding(const Vector2D& size) = 0;
	virtual void add_spacing() = 0;

	virtual void add_separtor_with_text(const std::string& text) = 0;
	virtual void add_separator() = 0;

	virtual void render_clipped_contents(size_t size, const std::function<void(int line_no)>& callback) = 0;

	virtual void add_readmore_on_hover_widget(const char* text) = 0;
	virtual void add_readmore_on_hover_widget_ex(const std::function<void()>& callback) = 0;
	virtual void dialog_on_hover_widget(const std::function<void()>& callback) = 0;

	virtual void add_progress_bar(const std::string& id, const Vector2D& size, float current, float max) = 0;

	virtual bool begin_listbox(const std::string& label, const std::string& preview_label, ImGuiComboFlags flags = 0) = 0;
	virtual void end_listbox() = 0;
	virtual bool add_simple_listbox(const std::string& label, int* value, const std::vector<std::string>& items, const std::function<void(int i)>& on_entry_click = nullptr) = 0;

	virtual bool add_selectable(const std::string& label, bool selected = false, ImGuiSelectableFlags flags = 0, const Vector2D& size = Vector2D(0, 0)) = 0;

	virtual bool add_floating_button(const std::string& label, const Vector2D& last_cursor_pos, const Vector2D& button_pos, const Vector2D& button_size = Vector2D(0, 0), bool disabled = false, EButtonFlags flags = BUTTONFLAG_None) = 0;

	virtual bool add_tree_node(const std::string& label) = 0;
	virtual void pop_tree_node() = 0;

	virtual void add_separated_heading(const std::string& label) = 0;

	virtual void add_image(ImTextureID id, const Vector2D& size) = 0;

	//
	// Tables/lists/columns
	//
	
	// Adds new imgui table
	virtual void add_table(const std::string& name, uint32_t columns, ImGuiTableFlags flags, const std::function<void()>& header_callback, const std::function<void()>& body_callback) = 0;

	// Use for simple tables such as:
	// 
	// name       value
	// name1      value1
	// ...
	// nameN      valueN
	//
	virtual void add_undecorated_simple_table(const std::string& name, uint32_t columns, const std::function<void()>& body_callback) = 0;

	virtual void table_setup_column_fixed_width(const std::string& name, float width, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None) = 0;
	virtual void table_setup_column(const std::string& name, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None) = 0;

	virtual void table_next_column() = 0;

	virtual void table_headers_row() = 0;

	// columns using tables api
	virtual bool begin_columns(const std::string& label, int count_columns, ImGuiTableFlags flags = 0) = 0;
	virtual void end_columns() = 0;
	virtual void setup_column_fixed_width(float width, ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None) = 0;
	virtual void setup_column(ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None) = 0;
	virtual void goto_next_column() = 0;
	virtual void goto_next_row() = 0;

	//
	// Input text buffer operations
	//

	virtual void delete_chars_on_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, int bytes_count) = 0;
	virtual void insert_chars_to_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, const char* text, const char* text_end = NULL) = 0;

	//
	// Tabs
	//

	virtual void begin_tab(const std::string& label, ImGuiTabBarFlags flags) = 0;
	virtual void add_tab_item(const std::string& label, bool border, const Vector2D& size, const std::function<void()>& pfn_contents, const Vector2D& child_padding = {}) = 0;
	virtual void end_tab() = 0;

	//
	// Custom widgets
	//
	virtual void add_console() = 0;

	//
	// rendering
	//

	virtual void render_arrow(const Vector2D& pos, const CColor& color, EArrowDirection direction) = 0;
};

extern IGUIWidgets* g_gui_widgets_i;

#define IGUIWIDGETS_INTERFACEID "IGUIWidgets"

#endif // IGUIWIDGETS_H