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

#ifndef OXWAREUI_H
#define OXWAREUI_H
#pragma once

class COxWareUI
{
public:
	DECL_BASIC_CLASS(COxWareUI);

public:
	void swapbuffers_detour(HDC hdc);
	void destroy();

	// if some interactive window is up, disable ingame input.
	bool should_disable_ingame_input() const { return m_is_any_interactible_rendering_context_active; }

	IRenderingContext* find_context(const std::string& id) const;

	bool is_in_popup_dialog() const { return m_popup_callback != nullptr; }

	void create_popup(const std::function<void()>& contents, const std::function<void()>& on_close_callback);
	bool was_popup_ok_button_pressed() const
	{
		return m_popup_has_been_closed;
	}

	void add_background_rendering_constrain(const std::function<bool()>& callback)
	{
		ctx_BackgroundRendering->add_render_constain(callback);
	}

	void add_background_rendering_code(const std::function<void()>& callback)
	{
		ctx_BackgroundRendering->add_render_code(callback);
	}

private:
	void initialize(HWND wnd);

	void run_ui();
	void render_imgui();
	void post_render();

	void create_welcome_popup();

	void instantiate_rendering_contexts();
	void destroy_rendering_contexts();

	template<typename T>
	void instantiate_single_rendering_context(T** to_be_instantiated, IRenderingContext* ctx);

	void handle_ingame_mouseevents();

private:
	bool m_is_any_interactible_rendering_context_active = false;

	HDC m_hdc;
	HGLRC m_cheat_context, m_original_context;

	std::vector<IRenderingContext*> m_rendering_contexts;
	CUIMenu						*ctx_Menu = nullptr;
	CUIFeatureList				*ctx_FeatureList = nullptr;
	CUIConsole					*ctx_Console = nullptr;
	CUIPerfProfilerVisualization*ctx_PerfProfilerVisualization = nullptr;
	CUIMenuBackground			*ctx_MenuBackground = nullptr;
	CUIBackgroundRendering		*ctx_BackgroundRendering = nullptr;

	std::vector<IRenderingContext*> m_contexts_to_be_rendered;

	std::function<void()> m_popup_callback;
	std::function<void()> m_on_close_callback;
	bool m_popup_has_been_closed = false;

	bool m_should_render_welcome_popup = false;

	void render_popup();
};

template<typename T>
void COxWareUI::instantiate_single_rendering_context(T** to_be_instantiated, IRenderingContext* ctx)
{
	CConsole::the().info("Initializting {} rendering context...", ctx->get_id());

	*to_be_instantiated = reinterpret_cast<T*>(ctx);
	ctx->on_initialize();
	m_rendering_contexts.push_back(ctx);
}

#endif // OXWAREUI_H