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

extern VarBoolean ui_background_rain;
extern VarBoolean ui_background_fade;
extern VarBoolean ui_render_feature_list;

class COxWareUI
{
public:
	DECL_BASIC_CLASS(COxWareUI);

public:
	void swapbuffers_detour(HDC hdc);
	void destroy();

	// if some interactive window is up, disable ingame input.
	bool should_disable_ingame_input() const { return is_any_ui_rendering(); }

	// applies only for interactible UI rendering contexts
	bool is_any_ui_rendering() const { return m_is_any_interactible_rendering_context_active; }

	IRenderingContext* find_context(const std::string& id) const;

	void add_background_rendering_constrain(const std::function<bool()>& callback)
	{
		ctx_BackgroundRendering->add_render_constain(callback);
	}

	void add_background_rendering_code(const std::function<void()>& callback)
	{
		ctx_BackgroundRendering->add_render_code(callback);
	}

	inline HWND get_window_handle() const { return m_hwnd; }

	// forces interactible context rendering. don't use this.
	void internal_force_interactible_rendering()
	{
		m_is_any_interactible_rendering_context_active = true;
	}

private:
	void initialize(HWND wnd);

	void run_ui();
	void render_imgui();
	void post_render();

	void initialize_textures();

	void instantiate_rendering_contexts();
	void destroy_rendering_contexts();

	template<typename T>
	void instantiate_single_rendering_context(T** to_be_instantiated, IRenderingContext* ctx);

	void handle_ingame_mouseevents();

	void lock_interaction_on_all_contexts();
	void unlock_interaction_on_all_contexts();

	void handle_popups_inputs();

private:
	bool m_is_any_interactible_rendering_context_active = false;

	bool m_initialized = false;

	HWND m_hwnd;
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