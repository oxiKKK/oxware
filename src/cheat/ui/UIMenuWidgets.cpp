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
	bool ret = g_gui_widgets_i->add_checkbox_with_color(label, &value, c, alpha ? ImGuiColorEditFlags_AlphaBar : 0);

	toggle_var->set_value((int)value);
	colors_var->set_value(CColor(c[0], c[1], c[2], c[3]));

	add_description_text(additional_desc);

	handle_widget_hover(toggle_var);

	return ret;
}

bool CUIMenuWidgets::add_color_edit(const std::string& label, VarColor* colors_var, const char* additional_desc)
{
	auto color = colors_var->get_value();
	float c[4] = { color.r, color.g, color.b, color.a };
	bool ret = g_gui_widgets_i->add_color_edit(label, c);

	colors_var->set_value(CColor(c[0], c[1], c[2], c[3]));

	add_description_text(additional_desc);

	handle_widget_hover(colors_var);

	return ret;
}

#pragma warning(push)
#pragma warning(disable : 4244) // warning C4244: 'argument': conversion from 'float' to 'int', possible loss of data

template bool CUIMenuWidgets::add_slider_t<VarFloat>(const std::string&, const char*, VarFloat*, const char*, const char*, const char*);
template bool CUIMenuWidgets::add_slider_t<VarInteger>(const std::string&, const char*, VarInteger*, const char*, const char*, const char*);

template<typename T>
bool CUIMenuWidgets::add_slider_t(const std::string& label, const char* format, T* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
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

	bool ret = g_gui_widgets_i->add_slider(label, &value, &min, &max, format);

	// we have to distinguish in order to call appropriate function
	var->set_value(value);

	add_description_text(additional_desc);

	handle_widget_hover(var);

	return ret;
}

bool CUIMenuWidgets::add_slider(const std::string& label, const char* format, VarFloat* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarFloat>(label, format, var, min_value_label, max_value_label, additional_desc);
}

bool CUIMenuWidgets::add_slider(const std::string& label, const char* format, VarInteger* var, const char* min_value_label, const char* max_value_label, const char* additional_desc)
{
	return add_slider_t<VarInteger>(label, format, var, min_value_label, max_value_label, additional_desc);
}

#pragma warning(pop)

void CUIMenuWidgets::add_pair_textinput(const std::string& label, VarKeyValue* var, const char* first_column_header, const char* second_column_header)
{
	float left_textinput_width = CMenuStyle::get_child_width_w_padding() / 1.5f;

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
	if (label.find("##") == std::string::npos)
	{
		g_gui_widgets_i->add_text(label);
	}

	assert((items.size() - 1) == var->get_max() &&
		   "The amount of items provided inside a list box must match the amount of options in the variable!");

	bool edited = false;
	if (g_gui_widgets_i->begin_listbox(label, items[var->get_value()]))
	{
		g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_imgui_font("segoeui", FONT_MEDIUM, FONTDEC_Regular));
		g_gui_widgets_i->push_stylevar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f});

		for (size_t i = 0; i < items.size(); i++)
		{
			bool selected = (i == var->get_value());
			if (g_gui_widgets_i->add_toggle_button(items[i], Vector2D(-1.0f, 0.0f), selected))
			{
				var->set_value(i);
				g_gui_widgets_i->close_current_popup();
				edited = true;
				break;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (selected)
				g_gui_widgets_i->set_item_default_focus();
		}

		g_gui_widgets_i->pop_stylevar();
		g_gui_widgets_i->pop_font();

		g_gui_widgets_i->end_listbox();
	}

	//if (edited)
	{
		// to close the list box after we select an item.
		g_gui_widgets_i->close_current_popup();
	}
}

void CUIMenuWidgets::add_description_text(const char* additional_desc, const char* readmore_string, bool no_padding)
{
	if (!additional_desc)
		return;

	g_gui_widgets_i->add_text(additional_desc, TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font("segoeui", FONT_SMALL, FONTDEC_Regular));
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
		g_gui_widgets_i->add_text(additional_desc, TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font("segoeui", FONT_SMALL, FONTDEC_Regular));
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

void CUIMenuWidgets::handle_widget_hover(BaseVariable* var)
{
	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(var->get_description());
	}
}
