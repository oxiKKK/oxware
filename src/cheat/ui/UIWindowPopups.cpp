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

void UIBasePopupContext::render()
{
	// a popup must have uid, therefore it can be destroyed properly.
	assert(!m_uid.empty() && "popup without uid!");

	render_background();

	g_gui_widgets_i->center_next_window_pos(ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_rounding(MenuStyle::rounding_factor, ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft);
	g_gui_widgets_i->set_next_window_size(m_window_size, ImGuiCond_Appearing);

	g_gui_thememgr_i->push_color(GUICLR_ChildBackground, CColor());

	auto window_flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

	window_flags |= m_window_flags;

	bool closing_popup = false;

	g_gui_widgets_i->create_new_window(
		"popup_window", window_flags,
		[&]()
		{
			// derived classes can do whatever they want here
			if (m_popup_custom_contents_fn)
			{
				if (m_popup_custom_contents_fn())
				{
					closing_popup = true;
				}
			}
			else
			{
				// if derived class didn't provide any contents rendering callback, it's our turn to render it.
				if (m_contents_fn)
				{
					if (m_contents_fn())
					{
						closing_popup = true; // user wants to close callback.
					}
				}
			}

			g_gui_widgets_i->add_spacing();

			bool has_both = m_on_cancel_fn && m_on_okay_fn;

			assert((m_on_cancel_fn || m_on_okay_fn) && "You have to specify either on_cancel or on_okay popup callbacks!");

			if (g_gui_widgets_i->begin_columns("popup_window_column", has_both ? 3 : 2, ImGuiTableFlags_SizingStretchSame))
			{
				g_gui_widgets_i->goto_next_column();

				if (m_on_cancel_fn)
				{
					g_gui_widgets_i->goto_next_column();
					
					if (g_gui_widgets_i->add_button("Cancel", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						closing_popup = true;
						m_on_cancel_fn();
					}
				}

				if (m_on_okay_fn)
				{
					g_gui_widgets_i->goto_next_column();
					
					if (g_gui_widgets_i->add_button("Okay", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						closing_popup = true;
						m_on_okay_fn();
					}
				}

				g_gui_widgets_i->end_columns();
			}
		});

	g_gui_thememgr_i->pop_color(); // child background

	if (closing_popup)
	{
		// tell the master that we're exiting this popup.
		CUIWindowPopups::the().signal_popup_close(m_uid);
	}
}

void UIBasePopupContext::render_background()
{
	static constexpr auto window_flags_bg =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

	g_gui_widgets_i->set_next_window_pos({}, ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_size(g_imgui_platform_layer_i->get_screen_size(), ImGuiCond_Always);

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, Vector2D());

	g_gui_widgets_i->create_new_window(
		"popup_background", window_flags_bg,
		[this]()
		{
			g_gui_window_rendering_i->render_box(
				g_gui_window_rendering_i->get_current_drawlist(),
				{ 0, 0 },
				g_imgui_platform_layer_i->get_screen_size(),
				{ 0.0f, 0.0f, 0.0f, 0.5f });
		});

	g_gui_widgets_i->pop_stylevar(); // window padding
}

//-----------------------------------------------------------------------------------------------

bool UIDecoratedPopup::popup_window_contents()
{
	bool ret = false;

	g_gui_widgets_i->add_child(
		"popup_window_child",
		Vector2D(-1, -1 - 20), true, ImGuiWindowFlags_None,
		[&]()
		{
			if (m_contents_fn)
			{
				ret = m_contents_fn();
			}
		});

	return ret;
}

//-----------------------------------------------------------------------------------------------

bool UIDecoratedWithTitlePopup::popup_window_contents()
{
	if (!m_title.empty())
	{
		auto title_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_27px, FDC_Regular);
		g_gui_widgets_i->add_text(m_title, TEXTPROP_None, title_font);
	}

	// render child window
	return UIDecoratedPopup::popup_window_contents();
}

//-----------------------------------------------------------------------------------------------

void CUIWindowPopups::pre_render()
{
	// welcoming popup dialog, but only if the window is focused.
	static bool once = false;
	if (!once && (COxWare::the().at_least_once_focused() || GetFocus() == COxWareUI::the().get_window_handle()))
	{
		// see registry if we already shown the "welcome dialog"
		bool show_welcome_popup = g_registry_i->read_int(REG_OXWARE, "already_shown_welcome_popup") == 0;
		if (show_welcome_popup)
		{
			create_welcome_popup();
		}

		once = true; // okay, don't repeat next time.
	}
}

void CUIWindowPopups::render()
{
	handle_popup_close();

	if (is_in_popup_dialog())
	{
		g_gui_widgets_i->push_executing_popup_code();
	}
	
	for (auto& popup : m_popup_stack)
	{
		popup->render();
	}

	if (is_in_popup_dialog())
	{
		g_gui_widgets_i->pop_executing_popup_code();
	}
}

void CUIWindowPopups::schedule_popup(UIBasePopupContext* popup)
{
	CConsole::the().dinfo("Scheduling popup {}...", popup->get_uid());

	m_popup_stack.push_back(popup);
}

void CUIWindowPopups::create_welcome_popup()
{
	auto welcome_popup = create_popup_context<UIDecoratedWithTitlePopup>("about_popup");

	welcome_popup->provide_window_title("About");
	welcome_popup->provide_window_size(Vector2D(350, 420));
	welcome_popup->provide_on_cancel_fn(
		[]()
		{
			g_registry_i->write_int(REG_OXWARE, "already_shown_welcome_popup", 1);
		});

	welcome_popup->provide_contents_fn(
		[]()
		{
			g_gui_widgets_i->add_separtor_with_text("Introduction");
			g_gui_widgets_i->add_text("Welcome to oxWARE " OXVER_STRING "🥳! A public open-source cheat for CS 1.6.", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_spacing();
			g_gui_widgets_i->add_text("Please, note that this cheat is still in development! It's nowhere near finished! 😉", TEXTPROP_Wrapped);

			g_gui_widgets_i->add_spacing();

			g_gui_widgets_i->add_text("Press INSERT to begin the cheating experience! 😁 Or simply re-bind the key to whatever you like using the Bind manager!", TEXTPROP_Wrapped);

			float sections_padding = 10.0f;

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Project GitHub repository");

			g_gui_widgets_i->add_text("Please, don't forget to visit the project repository on GitHub! You can find there information such as:", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_bullet_text("TODO list");
			g_gui_widgets_i->add_bullet_text("ChangeLog for each version");
			g_gui_widgets_i->add_bullet_text("Feature list");
			g_gui_widgets_i->add_bullet_text("Non-steam support");
			g_gui_widgets_i->add_bullet_text("And more!");

			g_gui_widgets_i->add_spacing();
			if (g_gui_widgets_i->add_url_text("github.com/oxiKKK/oxware"))
			{
				CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK/oxware");
			}

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Availability");

			g_gui_widgets_i->add_text(std::format("The cheat is currently available on following builds: {}", g_bytepattern_bank_i->supported_builds_as_str()), TEXTPROP_Wrapped);

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Bugs / feature requests");
			g_gui_widgets_i->add_text("🐞 If you find any bugs or you just want something to be added, please, create an issue here:", TEXTPROP_Wrapped);
			if (g_gui_widgets_i->add_url_text("github.com/oxiKKK/oxware/issues"))
			{
				CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK/oxware/issues");
			}

			g_gui_widgets_i->add_text("Please, note that I develop this hack based on community suggestions and recommendations, so feel free to contact me about anything you want!", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_text("So that we can make together the best CS 1.6 cheat ever! 😊", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_text("You can always find me for example on discord. Links below.", TEXTPROP_Wrapped);

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Contact on the author");

			if (g_gui_widgets_i->begin_columns("contact", 2))
			{
				g_gui_widgets_i->setup_column_fixed_width(90);
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("Discord");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("__ox__");
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("Steam");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_url_text("steamcommunity.com/id/oxiKKK"))
				{
					CGenericUtil::the().open_link_inside_browser("https://steamcommunity.com/id/oxiKKK");
				}
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("YouTube");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_url_text("www.youtube.com/@OX666"))
				{
					CGenericUtil::the().open_link_inside_browser("https://steamcommunity.com/id/oxiKKK");
				}
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("GitHub");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_url_text("github.com/oxiKKK"))
				{
					CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK");
				}

				g_gui_widgets_i->end_columns();
			}

			g_gui_widgets_i->add_padding({ 0.0f, 20.0f });

			COxWareUI::the().internal_force_interactible_rendering();

			return false;
		});

	schedule_popup(welcome_popup);
}

void CUIWindowPopups::handle_popup_close()
{
	if (m_popup_to_be_deleted.empty() == false)
	{
		size_t index_to_be_deleted = -1;
		for (size_t i = 0; i < m_popup_stack.size(); i++)
		{
			if (m_popup_stack[i]->get_uid() == m_popup_to_be_deleted)
			{
				CConsole::the().dinfo("Closing popup {}...", m_popup_stack[i]->get_uid());
				index_to_be_deleted = i;
			}
		}

		assert(index_to_be_deleted != -1 && "shouldn't happen");
		m_popup_stack.erase(m_popup_stack.begin() + index_to_be_deleted);
		
		m_popup_to_be_deleted.clear();
	}
}

