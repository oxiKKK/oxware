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

#include "code_performace_profile.h"

#include "Console.h"

#include "tier/GenericUtil.h"

// so that we create an instance per module.
ModuleProfileReport g_module_profile_report;

void ModuleProfileReport::add_code_profile(const std::string& name, const ScopeProfileReport& report)
{
	if (!collect_samples)
	{
		// someone has stopped the collection.
		return;
	}

	auto& report_entry = m_code_profile_reports[name];

	// check if overflowed. we need to keep a count so we don't run out of memory.
	if (report_entry.size() > k_max_code_reports)
	{	
		report_entry.pop_front();
	}

	report_entry.push_back(report);

#if 0 // too verbose
	CConsole::the().info("Profile '{}' ({} ms) inside {}:{}", 
						 name,
						 report.get_milliseconds(),
						 CGenericUtil::the().get_filename_from_path(report.sourceloc.file_name()),
						 report.sourceloc.function_name());
#endif
}
