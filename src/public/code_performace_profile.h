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

#ifndef CODEPERFORMANCEPROFILE_H
#define CODEPERFORMANCEPROFILE_H
#pragma once

#if !defined(_RETAIL)
//#	define OX_ENABLE_CODE_PROFILE
#endif

#include "commondefs.h"
#include "source_location.h"

#include <string>
#include <deque>
#include <unordered_map>
#include <utility>

// container that starts the timer after constructed and stops it when destructed. Also, while being destructed, 
// a new entry is added to the global list for this particular name.
class AutomatedScopeProfile
{
public:
	AutomatedScopeProfile()
	{

	}

	explicit AutomatedScopeProfile(const char* name, const SourceLocation& current = SourceLocation::current())
	{
		m_name = name;
		m_sourceloc = current;

		m_start = std::chrono::high_resolution_clock::now();
	}

	AutomatedScopeProfile(const AutomatedScopeProfile& other) = delete;
	AutomatedScopeProfile(AutomatedScopeProfile&&) = delete;

	~AutomatedScopeProfile()
	{
		m_end = std::chrono::high_resolution_clock::now();

		add_to_list();
	}

	void add_to_list();

	std::string m_name;

	std::chrono::high_resolution_clock::time_point m_start, m_end;

	SourceLocation m_sourceloc;
};

// data about scope measurement that had been done that is transfered over to the module profile report.
struct ScopeProfileReport
{
	inline float get_seconds() const { return std::chrono::duration<float, std::ratio<1, 1>>(end - start).count(); }
	inline float get_milliseconds() const { return std::chrono::duration<float, std::milli>(end - start).count(); }
	inline float get_microseconds() const { return std::chrono::duration<float, std::micro>(end - start).count(); }
	inline float get_nanoseconds() const { return std::chrono::duration<float, std::nano>(end - start).count(); }

	std::string name;
	SourceLocation sourceloc;
	std::chrono::high_resolution_clock::time_point start, end;
};

// we do rough 0-overhead collection here and then we sort all results and display them with fashion..
class ModuleProfileReport
{
public:
	// we have to have some limit, otherwise we would run out of memory.
	inline static size_t k_max_code_reports = 1024 * 8;

	ModuleProfileReport()
	{
		m_module = CURRENT_MODULE;
	}

	void add_code_profile(const std::string& name, const ScopeProfileReport& prof);

	std::unordered_map<std::string, std::deque<ScopeProfileReport>> m_code_profile_reports;

	bool is_collecting_data() { return collect_samples; }

	void stop_sample_collection() { collect_samples = false; }
	void resume_sample_collection() { collect_samples = true; }
	void reset_collected_samples() { m_code_profile_reports.clear(); }

	const char* get_module() const { return m_module.c_str(); };

private:
	bool collect_samples = true;

	std::string m_module;
};

extern ModuleProfileReport g_module_profile_report;

inline void AutomatedScopeProfile::add_to_list()
{
	g_module_profile_report.add_code_profile(m_name, ScopeProfileReport(m_name, m_sourceloc, m_start, m_end));
}

#ifdef OX_ENABLE_CODE_PROFILE

#define OX_PROFILE_SCOPE(name) AutomatedScopeProfile scope_profile(name)

#else

#define OX_PROFILE_SCOPE(name) 

#endif // OX_ENABLE_CODE_PROFILE

#endif // CODEPERFORMANCEPROFILE_H