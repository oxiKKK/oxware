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

bool CUIMenuWidgets::add_checkbox(const std::string& label, VarBoolean* var, const char* additional_desc)
{
	float value = var->get_value();
	bool ret = g_gui_widgets_i->add_checkbox(label, &value);

	var->set_value(value);

	add_description_text(additional_desc);

	handle_widget_hover(var);

	return ret;
}

bool CUIMenuWidgets::add_checkbox_with_color(const std::string& label, VarBoolean* toggle_var, VarColor* colors_var, bool alpha, const char* additional_desc)
{
	auto color = colors_var->get_value();

	float value = (float)toggle_var->get_value();
	float c[4] = { color.r, color.g, color.b, color.a };
	bool ret = g_gui_widgets_i->add_checkbox_with_color(label, &value, c, false, alpha ? ImGuiColorEditFlags_AlphaBar : 0);

	toggle_var->set_value((int)value);
	colors_var->set_value(CColor(c[0], c[1], c[2], c[3]));

	add_description_text(additional_desc);

	handle_widget_hover(toggle_var);

	return ret;
}

bool CUIMenuWidgets::add_checkbox_with_incommand_keypress_button(const std::string& label, VarBoolean* var, const char* incommand_name, const char* additional_desc)
{
	float value = var->get_value();
	bool ret = g_gui_widgets_i->add_checkbox(label, &value);

	var->set_value(value);

	handle_widget_hover(var);
	
	auto in_cmd = g_in_commands_i->get_incommand(incommand_name);
	assert(in_cmd && "Invalid incommand name while query!");
	if (in_cmd)
	{
		g_gui_widgets_i->sameline();

		static Vector2D button_size = Vector2D(80.0f, 20.0f);

		auto& style = g_gui_widgets_i->get_imgui_style();
		auto window_size = g_gui_widgets_i->get_current_window_size();

		auto cursor_pos = g_gui_widgets_i->get_cursor_pos();
		g_gui_widgets_i->set_cursor_pos({ cursor_pos.x + (g_gui_widgets_i->get_content_region_avail().x - (button_size.x + style.ScrollbarSize)), cursor_pos.y});

		CUIInCommandKeyBinding::the().add_keyscan_button(in_cmd, button_size);

		g_gui_widgets_i->add_spacing();
	}
	
	add_description_text(additional_desc);

	return ret;
}

void CUIMenuWidgets::add_ingame_cvar_toggleable_checkbox(const std::string& label, hl::cvar_t* cvar, 
														 std::pair<bool, bool>& disable_pair, 
														 float off_value, float on_value)
{
	// the first is the value under checkbox, and the second is the "do this shit only once" boolean... 😀

	g_gui_widgets_i->add_checkbox(label, &disable_pair.first);

	if (!cvar)
	{
		assert(0 && "Cvar not found! Tried to add checkbox with non-existent in-game cvar!");
		return;
	}

	if (disable_pair.first && !disable_pair.second)
	{
		cvar->value = off_value;
		disable_pair.second = true;
	}
	else if (!disable_pair.first && disable_pair.second)
	{
		cvar->value = on_value;
		disable_pair.second = false;
	}
}

bool CUIMenuWidgets::add_color_edit(const std::string& label, VarColor* colors_var, const char* additional_desc)
{
	auto color = colors_var->get_value();
	float c[4] = { color.r, color.g, color.b, color.a };
	bool ret = g_gui_widgets_i->add_color_edit(label, c, false);

	colors_var->set_value(CColor(c[0], c[1], c[2], c[3]));

	add_description_text(additional_desc);

	handle_widget_hover(colors_var);

	return ret;
}

#pragma warning(push)
#pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'float' to 'int', possible loss of data

template bool CUIMenuWidgets::add_slider_t<VarFloat>(const std::string&, const char*, VarFloat*, bool, const char*, const char*, const char*);
template bool CUIMenuWidgets::add_slider_t<VarInteger>(const std::string&, const char*, VarInteger*, bool, const char*, const char*, const char*);

template<typename T>
bool CUIMenuWidgets::add_slider_t(const std::string& label, const char* format, T* var, bool no_label, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	assert(var->has_bounds() && "Variable used in slider must have bounds.");

	// get bounds
	float min = (float)var->get_min(), max = (float)var->get_max();
	float value = (float)var->get_value();

	if (min_value_label && value == min)
	{
		format = min_value_label;
	}
	else if (max_value_label && value == max)
	{
		format = max_value_label;
	}

	bool ret = g_gui_widgets_i->add_slider(label, &value, &min, &max, format, no_label);

	// we have to distinguish in order to call appropriate function
	var->set_value(value);

	add_description_text(additional_desc);

	handle_widget_hover(var);

	return ret;
}

bool CUIMenuWidgets::add_slider(const std::string& label, const char* format, VarFloat* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarFloat>(label, format, var, false, min_value_label, max_value_label, additional_desc);
}

bool CUIMenuWidgets::add_slider(const std::string& label, const char* format, VarInteger* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarInteger>(label, format, var, false, min_value_label, max_value_label, additional_desc);
}

bool CUIMenuWidgets::add_slider_nolabel(const std::string& label, const char* format, VarFloat* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarFloat>(label, format, var, true, min_value_label, max_value_label, additional_desc);
}

bool CUIMenuWidgets::add_slider_nolabel(const std::string& label, const char* format, VarInteger* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarInteger>(label, format, var, true, min_value_label, max_value_label, additional_desc);
}

#pragma warning(pop)

void CUIMenuWidgets::add_pair_textinput(const std::string& label, VarKeyValue* var, const char* first_column_header, const char* second_column_header)
{
	float left_textinput_width = MenuStyle::get_child_width_w_padding() / 1.5f;

	if (g_gui_widgets_i->begin_columns(label.c_str(), 2))
	{
		g_gui_widgets_i->setup_column_fixed_width(left_textinput_width);
		g_gui_widgets_i->goto_next_column();

		if (first_column_header)
		{
			g_gui_widgets_i->add_text(first_column_header);
		}

		bool reclaim_focus_value = false;
		if (g_gui_widgets_i->add_text_input_ex(label + "1", (char*)var->get_value().key().c_str(), var->get_value().key().length(), Vector2D(-1.0f, 0.0f)))
		{
			if (!var->get_value().key().empty())
			{
				var->set_value(KeyValue(var->get_value().key(), var->get_value().value()));
			}
			reclaim_focus_value = true;
		};

		if (reclaim_focus_value)
		{
			g_gui_widgets_i->set_item_default_focus();
			g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
		}

		g_gui_widgets_i->goto_next_column();

		if (first_column_header)
		{
			g_gui_widgets_i->add_text(second_column_header);
		}

		bool reclaim_focus_key = false;
		if (g_gui_widgets_i->add_text_input_ex(label + "2", (char*)var->get_value().value().c_str(), var->get_value().value().length(), Vector2D(-1.0f, 0.0f)))
		{
			if (!var->get_value().value().empty())
			{
				var->set_value(KeyValue(var->get_value().key(), var->get_value().value()));
			}

			reclaim_focus_key = true;
		};

		// Auto-focus on window apparition
		g_gui_widgets_i->set_item_default_focus();
		if (reclaim_focus_key)
		{
			g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
		}

		g_gui_widgets_i->end_columns();
	}
 }

void CUIMenuWidgets::add_listbox(const std::string& label, VarInteger* var, const std::vector<std::string>& items)
{
	assert((items.size() - 1) == var->get_max() &&
		   "The amount of items provided inside a list box must match the amount of options in the variable!");

	int value = var->get_value();
	g_gui_widgets_i->add_simple_listbox(label, &value, items, [&var](int i) { var->set_value(i); });
}

void CUIMenuWidgets::add_description_text(const char* additional_desc, const char* readmore_string, bool no_padding)
{
	if (!additional_desc)
		return;

	g_gui_widgets_i->add_text(additional_desc, TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	if (!readmore_string)
	{
		if (!no_padding)
		{
			g_gui_widgets_i->add_padding({ 0.0f, 3.0f });
		}
	}
	else
	{
		g_gui_widgets_i->add_readmore_on_hover_widget(readmore_string);
	}
}

void CUIMenuWidgets::add_description_text_ex(const char* additional_desc, const std::function<void()>& callback, bool no_padding)
{
	if (additional_desc)
	{
		g_gui_widgets_i->add_text(additional_desc, TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	}

	if (!callback)
	{
		if (!no_padding)
		{
			g_gui_widgets_i->add_padding({ 0.0f, 3.0f });
		}
	}
	else
	{
		g_gui_widgets_i->add_readmore_on_hover_widget_ex(callback);
	}
}

void CUIMenuWidgets::feature_enabled_section(VarBoolean* var_boolean, const std::function<void()>& callback, 
											 const std::string& title, bool see_if_enabled)
{
	add_checkbox(std::format("{}##{}", title, var_boolean->get_name()), var_boolean);
	
	bool enabled = var_boolean->get_value();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->push_disabled();
	}

	callback();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->pop_disabled();
	}
}

void CUIMenuWidgets::feature_enabled_section(VarBoolean* var_boolean, VarColor* colors_var, 
											 const std::function<void()>& callback, bool alpha, const std::string& title, bool see_if_enabled)
{
	add_checkbox_with_color(std::format("{}##{}", title, var_boolean->get_name()), var_boolean, colors_var, alpha);

	bool enabled = var_boolean->get_value();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->push_disabled();
	}

	callback();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->pop_disabled();
	}
}

void CUIMenuWidgets::feature_enabled_section_incommands(BaseInCommand* in_cmd, const std::function<void()>& callback,
														const std::string& title, bool see_if_enabled)
{
	auto toggle_var = in_cmd->get_toggle_var();

	add_checkbox_with_incommand_keypress_button(std::format("{}##{}", title, toggle_var->get_name()), toggle_var, in_cmd->get_name().c_str());

	bool enabled = toggle_var->get_value();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->push_disabled();
	}

	callback();

	if (!enabled && see_if_enabled || enabled && !see_if_enabled)
	{
		g_gui_widgets_i->pop_disabled();
	}
}

void CUIMenuWidgets::section_unavailable_for_builds(const std::function<bool(int current_bn)>& disable_condition, const std::string& avalable, const std::function<void()>& callback)
{
	int current_bn = COxWare::the().get_build_number();

	bool disabled = disable_condition(current_bn);

	if (disabled)
	{
		g_gui_widgets_i->begin_group();

		g_gui_widgets_i->push_disabled();
	}

	callback();

	if (disabled)
	{
		g_gui_widgets_i->pop_disabled();

		g_gui_widgets_i->end_group();

		g_gui_widgets_i->dialog_on_hover_widget(
			[&]()
			{
				g_gui_widgets_i->add_text(
					std::format("Feature unavailable on current engine build.\nOnly avalable for {}, and your build is {}.", avalable, current_bn));
			});
	}
}

void CUIMenuWidgets::handle_widget_hover(BaseVariable* var)
{
	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(var->get_description());
	}
}
