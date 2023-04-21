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
// ImGUILayerGLFW.cpp -- Implementation of ImGUI layer for GLFW
//

#include "precompiled.h"

bool CImGUILayerGLFW::initialize()
{
	assert(m_window_handle);

	// Setup Dear ImGui context
	if (!ImGui::CreateContext())
	{
		CConsole::the().error("Coudln't create ImGui context.");
		return false;
	}

	auto& io = ImGui::GetIO();

	// Disable generating imgui.ini
	io.IniFilename = io.LogFilename = nullptr;

	g_gui_fontmgr_i->initialize();

	ImGui::StyleColorsDark();
	g_gui_thememgr_i->initialize();

	// Setup Platform/Renderer backends
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(as_glfw_window(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

void CImGUILayerGLFW::destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_window_handle = nullptr;
}

void CImGUILayerGLFW::on_render(const std::function<void()>& post_render_callback)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	g_gui_fontmgr_i->push_default_font(); // has to be called after ImGui::NewFrame

	if (m_render_callback) // can be null if client didn't specify
	{
		m_render_callback();
	}

	if (post_render_callback)
	{
		post_render_callback();
	}

	g_gui_fontmgr_i->pop_default_font();

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
