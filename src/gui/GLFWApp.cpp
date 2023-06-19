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

IGLFWApp* g_glfw_app_i = nullptr;

class CGLFWApp : public IGLFWApp
{
public:
	CGLFWApp();
	~CGLFWApp();

	bool create_window(int width, int height, const std::string& title, const std::function<void()>& renderer_callback);
	void destroy_window();

	bool on_tick();

	void update_window_title(const std::string& title)
	{
		m_app_title = title;
	}

	HWND get_window_handle()
	{
		return glfwGetWin32Window(m_glfw_window);
	}

	Vector2D get_app_resolution();

private:
	void glfw_update();

	bool is_alive();

private:
	std::string m_app_title;
	int			m_app_width, m_app_height;

	GLFWwindow* m_glfw_window;
};

CGLFWApp g_glfw_app;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGLFWApp, IGLFWApp, IGLFWAPP_INTERFACEID, g_glfw_app);

//-------------------------------------------------------------------------------
//
// CGLFWApp implementation
// 
//-------------------------------------------------------------------------------

static void glfw_error_callback(int error, const char* description)
{
	CMessageBox::display_error("GLFW error occured:\n\n{}", description);
}

CGLFWApp::CGLFWApp()
{
	g_glfw_app_i = this;
}

CGLFWApp::~CGLFWApp()
{
	g_glfw_app_i = nullptr;
}

bool CGLFWApp::create_window(int width, int height, const std::string& title, const std::function<void()>& renderer_callback)
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		CConsole::the().error("Failed to initialize GLFW.");
		return false;
	}

	// GL 3.0 + GLSL 130
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// other hints
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create window with graphics context
	m_app_title = title;
	m_app_width = width;
	m_app_height = height;

	CConsole::the().info("Creating new window... {}x{} - '{}'", m_app_width, m_app_height, m_app_title);

	m_glfw_window = glfwCreateWindow(
		m_app_width, m_app_height,
		m_app_title.c_str(),
		nullptr, nullptr);

	if (m_glfw_window == NULL)
	{
		CConsole::the().error("Couldn't create GLFW window.");
		return false;
	}

	glfwMakeContextCurrent(m_glfw_window);
	glfwSwapInterval(1); // Enable vsync

	if (!g_imgui_platform_layer_i->create_new_layer(IMPLATFORM_glfw, (void*)m_glfw_window, renderer_callback))
	{
		return false;
	}

	return true;
}

void CGLFWApp::destroy_window()
{
	g_imgui_platform_layer_i->destroy_layer();

	glfwDestroyWindow(m_glfw_window);
	glfwTerminate();

	m_glfw_window = nullptr;
}

bool CGLFWApp::on_tick()
{
	// User has requested a window close or the window just
	// has to be closed now.
	if (is_alive() == false)
		return false;

	// GUI renderer inside here
	glfw_update();

	return true;
}

Vector2D CGLFWApp::get_app_resolution()
{
	return { (float)m_app_width, (float)m_app_height };
}

void CGLFWApp::glfw_update()
{
	glfwPollEvents();

	// GLFW code goes here...
	glfwSetWindowTitle(m_glfw_window, m_app_title.c_str());

	// always keep the window at the same size. There was some bug where the window would randomly
	// resize due to CS resolution change or whatever, just random things. prevent that from happening.
	int w, h;
	glfwGetWindowSize(m_glfw_window, &w, &h);
	if (w != m_app_width || h != m_app_height)
	{
		glfwSetWindowSize(m_glfw_window, m_app_width, m_app_height);
	}

	// Renderer code goes here...
	g_imgui_platform_layer_i->render();

	glfwMakeContextCurrent(m_glfw_window);
	glfwSwapBuffers(m_glfw_window);
}

bool CGLFWApp::is_alive()
{
	return !glfwWindowShouldClose(m_glfw_window);
}
