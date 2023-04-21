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

#include <set>

ICodePerfProfiler* g_code_perf_profiler_i = nullptr;

class CCodePerfProfiler : public ICodePerfProfiler
{
public:
	CCodePerfProfiler();
	~CCodePerfProfiler();

	void update();

	void register_profile_report(ModuleProfileReport* report);

	bool is_collecting_data();

	void stop_sample_collection();
	void resume_sample_collection();
	void reset_collected_samples();

	void create_profile_snapshot();

	void for_each_sampled_profile_snapshot(const std::function<void(const ProfSnapshotEntry*)>& callback);

	float get_ms_total() const { return m_sum_ms; }

private:
	std::vector<ModuleProfileReport*> m_module_profile_reports;

	// Collected samples from last snapshot
	SampledProfileSnapshots m_profile_snapshot_entries;

	// we have to first collect the data and then sort it separately.
	std::set<ProfSnapshotEntry, ProfSnapshotEntry::less> m_sorted_snapshot_result;

	float m_sum_ms = 0.0f;

	bool m_collection_stopped = false;

	bool m_in_snapshot_creation = false;
};

CCodePerfProfiler g_code_perf_profiler;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CCodePerfProfiler, ICodePerfProfiler, ICODEPERFPROFILER_INTERFACEID, g_code_perf_profiler);

//-------------------------------------------------------------------------------
//
// CCodePerfProfiler implementation
//
//-------------------------------------------------------------------------------

CCodePerfProfiler::CCodePerfProfiler()
{
	g_code_perf_profiler_i = this;
}

CCodePerfProfiler::~CCodePerfProfiler()
{
	g_code_perf_profiler_i = nullptr;
}

void CCodePerfProfiler::update()
{
	static auto last_update = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - last_update).count() < 50)
	{
		return;
	}

	create_profile_snapshot();

	last_update = std::chrono::high_resolution_clock::now();
}

void CCodePerfProfiler::register_profile_report(ModuleProfileReport* report)
{
	CConsole::the().info("Registered {} module for code profiling.", report->get_module());

	m_module_profile_reports.emplace_back(std::move(report));
}

bool CCodePerfProfiler::is_collecting_data()
{
	for (auto r : m_module_profile_reports)
	{
		if (r->is_collecting_data())
		{
			return true;
		}
	}

	return false;
}

void CCodePerfProfiler::stop_sample_collection()
{
	m_collection_stopped = true;

	// inform all modules that are collecting data.
	for (auto r : m_module_profile_reports)
	{
		CConsole::the().info("Stopping sample collection on module {}...", r->get_module());
		r->stop_sample_collection();
	}
}

void CCodePerfProfiler::resume_sample_collection()
{
	m_collection_stopped = false;

	// inform all modules that are collecting data.
	for (auto r : m_module_profile_reports)
	{
		CConsole::the().info("Resuming sample collection on module {}...", r->get_module());
		r->resume_sample_collection();
	}
}

void CCodePerfProfiler::reset_collected_samples()
{
	stop_sample_collection();

	// inform all modules that are collecting data.
	for (auto r : m_module_profile_reports)
	{
		CConsole::the().info("Resetting collected samples for module {}...", r->get_module());
		r->reset_collected_samples();
	}

	resume_sample_collection();
}

void CCodePerfProfiler::for_each_sampled_profile_snapshot(const std::function<void(const ProfSnapshotEntry*)>& callback)
{
	if (m_in_snapshot_creation)
	{
		return;
	}

	for (auto& snap : m_sorted_snapshot_result)
	{
		callback(&snap);

		if (m_in_snapshot_creation)
		{
			return;
		}
	}
}

void CCodePerfProfiler::create_profile_snapshot()
{
	if (m_collection_stopped)
	{
		return;
	}

	m_in_snapshot_creation = true;

	// reset data
	m_sum_ms = 0.0f;
	m_sorted_snapshot_result.clear();

	//CConsole::the().info("-----------------------------");
	
	// loop through all reported code profiles and perform a sample collection and sorting.
	for (auto r : m_module_profile_reports)
	{
		for (auto& [key, profile_reports] : r->m_code_profile_reports)
		{
			size_t num_samples = profile_reports.size();

			SourceLocation loc;

			float avg_ms = 0.0f;
			for (const auto& report : profile_reports)
			{
				float add = report.get_milliseconds();
				avg_ms += add;

				//CConsole::the().info("{}: avg: {}ms", key, add);

				loc = report.sourceloc;
			}

			// insert empty first, then initialize the data, if inserted. If already in the list, just keep updating the data.
			ProfSnapshotEntry snap = {};

			auto[iter, did_insert] = m_profile_snapshot_entries.insert(std::make_pair(key, snap));
			if (did_insert)
			{
				iter->second.m_name = key;
				iter->second.m_module = r->get_module();
				iter->second.m_srcloc = loc;
				iter->second.m_num_samples = num_samples;
				iter->second.m_avg_time_ms = avg_ms / (float)num_samples;
			}
			else
			{
				// aready exist, just update data
				iter->second.m_num_samples += num_samples;
				iter->second.m_avg_time_ms = avg_ms / (float)num_samples;
			}
		}
	}

	// now put them inside of a sorted vector.. I don't think that we can sort the map using data from value rather than from key.
	for (const auto& [key, snap] : m_profile_snapshot_entries)
	{
		m_sum_ms += snap.m_avg_time_ms;
		m_sorted_snapshot_result.insert(snap);
	}

	m_in_snapshot_creation = false;
	
	/*
	for (const auto& snapshot : m_profile_snapshot_entries)
	{
		CConsole::the().info("{}: {} ms avg; {} samples; {}", 
							 snapshot.m_name, snapshot.m_avg_time_ms, snapshot.m_num_samples, snapshot.m_srcloc.function_name());
	}
	*/
}

