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
	TEXTPROP_Bigger = BIT(1),
	TEXTPROP_Slim = BIT(2),
	TEXTPROP_Disabled = BIT(3),

	// colors
	TEXTPROP_ColorWhite = BIT(4),
	TEXTPROP_ColorRegular = BIT(5),
	TEXTPROP_ColorDark = BIT(6),
	TEXTPROP_ColorBlack = BIT(7),

	// properties
	TEXTPROP_Wrapped = BIT(8),
};

enum EButtonFlags
{
	BUTTONFLAG_None = 0,
	BUTTONFLAG_CenterLabel = BIT(0),
	BUTTONFLAG_NoHover = BIT(1),
};

class CGUIWidgetsStyle
{
public:
	inline static constexpr float k_childhdr_text_padding_y = 3.0f;
	inline static constexpr Vector2D k_childhdr_line_padding = { 5.0f, 2.0f };
	inline static constexpr float k_childhdr_contents_padding_y = 5.0f;
};

struct GLFWwindow;
struct FontObject_t;

typedef void(__cdecl*SectionTabFn)(void* arg);

class IGUIWidgets : public IBaseInterface
{
public:
	//
	// Windows
	//

	virtual void set_next_window_size(const Vector2D& size, ImGuiCond condition = ImGuiCond_None) = 0;
	virtual void set_next_window_pos(const Vector2D& pos, ImGuiCond condition = ImGuiCond_None) = 0;
	virtual void center_next_window_pos(ImGuiCond condition) = 0;
	virtual void set_next_window_rounding(float rounding, ImDrawFlags flags = ImDrawFlags_None) = 0;

	virtual void create_new_window(const std::string& name, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents) = 0;

	virtual void block_input_on_all_except_popup(bool block) = 0;

	virtual void add_child(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents) = 0;
	virtual void add_child_with_header(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents) = 0;

	virtual Vector2D get_current_window_pos() = 0;
	virtual Vector2D get_current_window_size() = 0;

	virtual void schedule_modal_dialog(const std::string& label) = 0;
	virtual void execute_modal_popup(const std::string& name, ImGuiWindowFlags flags, const std::function<bool()>& callback) = 0;

	//
	// Properties
	//

	virtual void push_disbled() = 0;
	virtual void pop_disabled() = 0;

	virtual void push_stylevar(ImGuiStyleVar idx, float val) = 0;
	virtual void push_stylevar(ImGuiStyleVar idx, const Vector2D& val) = 0;
	virtual void pop_stylevar(size_t amount = 1) = 0;

	virtual void push_color(ImGuiCol idx, const CColor& color) = 0;
	virtual void pop_color(size_t amount = 1) = 0;

	virtual void push_font(ImFont* font) = 0;
	virtual void pop_font() = 0;

	virtual void set_cursor_pos(const Vector2D& pos) = 0;

	virtual void sameline(float offset_from_start_x = 0.0f, float spacing = -1.0f) = 0;

	virtual bool is_last_widget_hovered() = 0;

	virtual void set_item_default_focus() = 0;
	virtual void set_keyboard_focus_here(int offset = 0) = 0;

	virtual void set_scroll_here_y(float center_y_ratio = 0.5f) = 0;

	//
	// Widgets
	//

	virtual void add_text(const std::string& text, ETextProperties properties = TEXTPROP_None) = 0;
	virtual void add_colored_text(const CColor& color, const std::string& text, ETextProperties properties = TEXTPROP_None) = 0;
	virtual void add_window_centered_text(const std::string& text, FontObject_t* font = nullptr) = 0;
	virtual void add_window_centered_text_disabled(const std::string& text, FontObject_t* font = nullptr) = 0;

	virtual bool add_button(const std::string& label, const Vector2D& size, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None) = 0;
	virtual bool add_toggle_button(const std::string& label, const Vector2D& size, bool selected = false, bool disabled = false, EButtonFlags flags = BUTTONFLAG_None) = 0;

	virtual bool add_hypertext_link(const std::string& label) = 0;

	virtual bool add_checkbox(const std::string& label, float* value) = 0;
	virtual bool add_checkbox_with_color(const std::string& label, float* value, float rgba[4], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None) = 0;

	virtual bool add_color_edit(const std::string& label, float rgba[4], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None) = 0;

	virtual bool add_slider(const std::string& label, float* value, float* min, float* max, const char* format) = 0;
	virtual bool add_slider(const std::string& label, int* value, int* min, int* max, const char* format) = 0;

	virtual bool add_text_input(const std::string& label, char* buffer, size_t buffer_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, bool no_title = false, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) = 0;
	virtual bool add_text_input_ex(const std::string& label, char* buffer, size_t buffer_size, Vector2D input_size, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, bool no_title = false, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) = 0;

	virtual void add_padding(const Vector2D& size) = 0;
	virtual void add_spacing() = 0;

	virtual void begin_columns(const std::string& label, int count_columns) = 0;
	virtual void set_column_width(int column, float width) = 0;
	virtual void goto_next_column() = 0;
	virtual void end_columns(int count_columns) = 0;

	virtual void add_separtor_with_text(const std::string& text) = 0;
	virtual void add_separator() = 0;

	virtual void render_clipped_contents(size_t size, const std::function<void(int line_no)>& callback) = 0;

	virtual void add_readmore_on_hover_widget(const char* text) = 0;

	virtual void add_progress_bar(const std::string& id, const Vector2D& size, float current, float max) = 0;

	//
	// Input text buffer operations
	//

	virtual void delete_chars_on_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, int bytes_count) = 0;
	virtual void insert_chars_to_textinput_buffer(ImGuiInputTextCallbackData* data, int pos, const char* text, const char* text_end = NULL) = 0;

	//
	// Tabs
	//

	virtual void begin_tab(const std::string& label, ImGuiTabBarFlags flags) = 0;
	virtual void add_tab_item(const std::string& label, const std::function<void()>& pfn_contents) = 0;
	virtual void end_tab() = 0;

	//
	// Custom widgets
	//
	virtual void add_console() = 0;
};

extern IGUIWidgets* g_gui_widgets_i;

#define IGUIWIDGETS_INTERFACEID "IGUIWidgets"

#endif // IGUIWIDGETS_H