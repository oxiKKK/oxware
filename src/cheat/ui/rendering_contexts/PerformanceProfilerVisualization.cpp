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

void CUIPerfProfilerVisualization::on_initialize()
{
}

void CUIPerfProfilerVisualization::on_render()
{
#if 0
	g_gui_widgets_i->set_next_window_size({ 400.0f, 600.0f }, ImGuiCond_Once);

	static constexpr auto window_flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoNav;

	g_gui_widgets_i->create_new_window(
		"performace_profiler_visualization", window_flags, [&]()
		{
			g_gui_widgets_i->add_child_with_header(
				"Code profiler", 
				{ -1.0f, -1.0f }, 
				true, 
				ImGuiWindowFlags_None,
				[]()
				{
					if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
					{
						g_gui_widgets_i->goto_next_column();

						if (g_gui_widgets_i->add_button("Perform snapshot", { 120.0f, 0.0f }))
						{
							g_code_perf_profiler_i->create_profile_snapshot();
						}

						bool is_collecting = g_code_perf_profiler_i->is_collecting_data();
						if (g_gui_widgets_i->add_toggle_button("Sampling on/off", { 120.0f, 0.0f }, is_collecting))
						{
							if (is_collecting)
							{
								g_code_perf_profiler_i->stop_sample_collection();
							}
							else
							{
								g_code_perf_profiler_i->resume_sample_collection();
							}
						}

						g_gui_widgets_i->goto_next_column();

						if (g_gui_widgets_i->add_button("Floating mode", { 120.0f, 0.0f }))
						{

						}

						g_gui_widgets_i->end_columns();
					}

					g_gui_widgets_i->add_spacing();
					g_gui_widgets_i->add_separtor_with_text("Collected samples");

					g_gui_widgets_i->add_child(
						"collected_samples",
						{ -1.0f, -1.0f }, false, ImGuiWindowFlags_HorizontalScrollbar,
						[]()
						{
							g_code_perf_profiler_i->for_each_sampled_profile_snapshot(
								[](const ProfSnapshotEntry* e)
								{
									g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
									g_gui_widgets_i->add_text(std::format(
										"{}: avg: {}{:2.3f} ms samples: {}",
										e->m_name, (e->m_avg_time_ms < 0.001f) ? ">" : "", e->m_avg_time_ms, e->m_num_samples, e->m_srcloc.function_name(),
										e->m_srcloc.function_name()));
									g_gui_widgets_i->pop_font();
								});

							g_gui_widgets_i->add_separator();
							g_gui_widgets_i->add_text(std::format("sum avg: {:2.3f} ms (engine: {:2.3f} ms), {:2.1f} % overhead",
																  g_code_perf_profiler_i->get_ms_total(),
																  CLocalState::the().get_engine_frametime() * 1000.0,
																  ((g_code_perf_profiler_i->get_ms_total() / 1000.0) / CLocalState::the().get_engine_frametime()) * 100.0));
						});
				});
		});
#endif
}

void CUIPerfProfilerVisualization::on_destroy()
{
}
