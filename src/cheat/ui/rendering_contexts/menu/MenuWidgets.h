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

#ifndef UIMENUWIDGETS_H
#define UIMENUWIDGETS_H
#pragma once

class CUIMenuWidgets
{
public:
	DECL_BASIC_CLASS(CUIMenuWidgets);

public:
	bool add_checkbox(const std::string& label, VarBoolean* var, const char* additional_desc = nullptr);
	bool add_checkbox_with_color(const std::string& label, VarBoolean* toggle_var, VarColor* colors_var, bool alpha = false, const char* additional_desc = nullptr);
	bool add_checkbox_with_incommand_keypress_button(const std::string& label, VarBoolean* var, const char* incommand_name, const char* additional_desc = nullptr);
	void add_ingame_cvar_toggleable_checkbox(const std::string& label, hl::cvar_t* cvar, std::pair<bool, bool>& disable_pair¨, float off_value, float on_value);

	bool add_color_edit(const std::string& label, VarColor* colors_var, const char* additional_desc = nullptr);

	bool add_slider(const std::string& label, const char* format, VarFloat* var, const char* min_value_label = nullptr, const char* max_value_label = nullptr, const char* additional_desc = nullptr);
	bool add_slider(const std::string& label, const char* format, VarInteger* var, const char* min_value_label = nullptr, const char* max_value_label = nullptr, const char* additional_desc = nullptr);
	bool add_slider_nolabel(const std::string& label, const char* format, VarFloat* var, const char* min_value_label = nullptr, const char* max_value_label = nullptr, const char* additional_desc = nullptr);
	bool add_slider_nolabel(const std::string& label, const char* format, VarInteger* var, const char* min_value_label = nullptr, const char* max_value_label = nullptr, const char* additional_desc = nullptr);

	void add_pair_textinput(const std::string& label, VarKeyValue* var, const char* first_column_header = nullptr, const char* second_column_header = nullptr);

	void add_listbox(const std::string& label, VarInteger* var, const std::vector<std::string>& items);

	void add_description_text(const char* additional_desc, const char* readmore_string = nullptr, bool no_padding = false);
	void add_description_text_ex(const char* additional_desc, const std::function<void()>& callback = nullptr, bool no_padding = false);

	void feature_enabled_section(VarBoolean* var_boolean, const std::function<void()>& callback, const std::string& title = "Enable", bool see_if_enabled = true);
	void feature_enabled_section(VarBoolean* var_boolean, VarColor* colors_var, const std::function<void()>& callback, bool alpha = false, const std::string& title = "Enable", bool see_if_enabled = true);

	// additions for incommands, since they use different checkbox widget
	void feature_enabled_section_incommands(BaseInCommand* in_cmd, const std::function<void()>& callback, const std::string& title = "Enable", bool see_if_enabled = true);

	void section_unavailable_for_builds(const std::function<bool(int current_bn)>& disable_condition, const std::string& avalable, const std::function<void()>& callback);

private:
	void handle_widget_hover(BaseVariable* var);

	template<typename T>
	bool add_slider_t(const std::string& label, const char* format, T* var, bool no_label, const char* min_value_label = nullptr, const char* max_value_label = nullptr, const char* additional_desc = nullptr);

	struct collapsible_child_data_t
	{
		bool	collapsed;
		float	initial_height, height;
		bool	hovered;
	};
	std::unordered_map<std::string, collapsible_child_data_t> m_collapsible_child_data;
};

#endif // UIMENUWIDGETS_H