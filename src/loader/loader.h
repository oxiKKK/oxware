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
// loader.h -- Entirety of the loader.
//

#ifndef LOADER_H
#define LOADER_H
#pragma once

class CMainLoader
{
public:
	DECL_BASIC_CLASS(CMainLoader);

public:
	int run(HINSTANCE hinst);

	inline auto get_injector() const { return m_selected_injector; }

private:
	bool on_window_invoke();
	void on_window_destroy();

	bool check_supported_os_version();

	void decide_injection_type();

	bool run_frame();

	void set_window_icon();

	bool is_already_launched();

	void release_handles();

	inline double get_frametime() const
	{
		return std::chrono::duration<double>(frame_end - frame_start).count();
	}

	inline double get_fps() const
	{
		return 1.0 / get_frametime();
	}

	bool load_and_initialize_dependencies();
	void unload_dependencies();

	// function responsible for rendering the loader gui
	void on_render();

private:
	IBaseInjector* m_selected_injector = nullptr;

	// For frametime & fps calculations
	std::chrono::high_resolution_clock::time_point frame_start, frame_end;
	double m_avg_fps = 1.0; // Uses moving-average algorithm

	HINSTANCE m_instance_handle;

	HANDLE m_loader_mutex;
};

#endif // LOADER_H