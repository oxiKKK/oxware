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

#ifndef IGLFWAPP_H
#define IGLFWAPP_H
#pragma once

#include <functional>

#include <vector.h>

struct GLFWwindow;

class IGLFWApp : public IBaseInterface
{
public:
	virtual bool create_window(int width, int height, const std::string& title, const std::function<void()>& renderer_callback) = 0;
	virtual void destroy_window() = 0;
	virtual bool on_tick() = 0;

	virtual void update_window_title(const std::string& title) = 0;

	virtual HWND get_window_handle() = 0;

	virtual Vector2D get_app_resolution() = 0;
};

extern IGLFWApp* g_glfw_app_i;

#define IGLFWAPP_INTERFACEID "IGLFWApp"

#endif // IGLFWAPP_H