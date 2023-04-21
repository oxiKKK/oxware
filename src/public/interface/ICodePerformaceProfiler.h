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

#ifndef ICODEPERFORMANCEPROFILER_H
#define ICODEPERFORMANCEPROFILER_H
#pragma once

#include "code_performace_profile.h"

// data about the snapshot that we then display, sorted by number of samples.
struct ProfSnapshotEntry
{
	ProfSnapshotEntry() = default;
	ProfSnapshotEntry(const ProfSnapshotEntry&) = default;
	ProfSnapshotEntry(ProfSnapshotEntry&&) = default;

	ProfSnapshotEntry& operator=(const ProfSnapshotEntry&) = default;

	// sorting function
	struct less
	{
		bool operator()(const ProfSnapshotEntry& lhs, const ProfSnapshotEntry& rhs) const
		{
#if 0
			return lhs.m_num_samples > rhs.m_num_samples;
#else
			return lhs.m_avg_time_ms > rhs.m_avg_time_ms;
#endif
		}
	};

	std::string m_name, m_module;

	SourceLocation m_srcloc;

	size_t m_num_samples = 0;

	float m_avg_time_ms = 0.0f;
};

using SampledProfileSnapshots = std::unordered_map<std::string, ProfSnapshotEntry>;

class ICodePerfProfiler : public IBaseInterface
{
public:
	virtual void update() = 0;

	virtual void register_profile_report(ModuleProfileReport* report) = 0;

	virtual bool is_collecting_data() = 0;

	virtual void stop_sample_collection() = 0;
	virtual void resume_sample_collection() = 0;
	virtual void reset_collected_samples() = 0;

	virtual void create_profile_snapshot() = 0;

	virtual void for_each_sampled_profile_snapshot(const std::function<void(const ProfSnapshotEntry*)>& callback) = 0;

	virtual float get_ms_total() const = 0;
};

extern ICodePerfProfiler* g_code_perf_profiler_i;

#define ICODEPERFPROFILER_INTERFACEID "ICodePerfProfiler"

#endif // ICODEPERFORMANCEPROFILER_H