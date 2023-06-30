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
// BytePatternBank.cpp -- Provides api around cs/engine builds & byte patterns.
//

#include "precompiled.h"

#include "bytepattern_containers.cpp"

#include <set>

IBytePatternBank* g_bytepattern_bank_i = nullptr;

class CBytePatternBank : public IBytePatternBank
{
public:
	CBytePatternBank();
	~CBytePatternBank();

	bool initialize(int build_number);

	bool is_build_supported(int build_number)
	{
		for (auto build : m_supported_builds)
		{
			if (build == build_number)
				return true;
		}

		return false;
	}

	std::string supported_builds_as_str()
	{
		std::string s;
		int n = 0;
		for (auto build : m_supported_builds)
		{
			s += std::to_string(build);
			if (n != m_supported_builds.size() - 1)
				s += ", ";

			n++;
		}

		return s;
	}

	CBytePattern get_pattern(const std::string& hook_name) const;
	const PatternRecord* get_full(const std::string& hook_name) const;

private:
	void decide_on_build_container();

	int get_closest_build_number_supported();

	std::set<int> m_supported_builds;

	// picked depending on the situation (e.g. current build)
	const BytePatternContainer* m_active_bytepattern_container = nullptr;

	int m_current_build = -1;
};

CBytePatternBank g_bytepattern_bank;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CBytePatternBank, IBytePatternBank, IBYTEPATTERNBANK_INTERFACEID, g_bytepattern_bank);

//-------------------------------------------------------------------------------
//
// CBytePatternBank implementation
//
//-------------------------------------------------------------------------------

CBytePatternBank::CBytePatternBank()
{
	g_bytepattern_bank_i = this;

	// If you add a new build, you need to register it here!!!
	m_supported_builds.insert(8684);
	m_supported_builds.insert(4554);
	m_supported_builds.insert(3266);
}

CBytePatternBank::~CBytePatternBank()
{
	g_bytepattern_bank_i = nullptr;
}

bool CBytePatternBank::initialize(int build_number)
{
	CConsole::the().info("Initializing BytePatternBank...");
	CConsole::the().info("Your game build is: {}", build_number);

	m_current_build = build_number;

	// try to pick the best build.
	decide_on_build_container();

	CConsole::the().info("BytePatternBank initialized.");
	return true;
}

CBytePattern CBytePatternBank::get_pattern(const std::string& hook_name)const 
{
	auto full = get_full(hook_name);
	if (!full)
	{
		return {};
	}

	return full->pattern;
}

const PatternRecord* CBytePatternBank::get_full(const std::string& hook_name) const
{
	assert(m_active_bytepattern_container && "Called before initialization! m_active_bytepattern_container is null!");

	try
	{
		auto pattern_map = m_active_bytepattern_container;
		try
		{
			auto& record = pattern_map->at(hook_name);
			return &record;
		}
		catch (...)
		{
			CConsole::the().error("Couldn't get byte pattern for hook '{}'! ({} build)", hook_name, m_current_build);
		}
	}
	catch (...)
	{
		CConsole::the().error("Couldn't get hook '{}' because the build '{}' is unsupported!", hook_name, m_current_build);
	}

	return nullptr;
}

void CBytePatternBank::decide_on_build_container()
{
	bool need_to_pick_closest = false;

	if (!is_build_supported(m_current_build))
	{
		need_to_pick_closest = true;
	}

	if (!need_to_pick_closest)
	{
		// we have the exact build available, choose it
		switch (m_current_build)
		{
			case 8684: m_active_bytepattern_container = &bytepattern_container_8684; break;
			case 4554: m_active_bytepattern_container = &bytepattern_container_4554; break;
			case 3266: m_active_bytepattern_container = &bytepattern_container_3266; break;
		}
	}
	else
	{
		// user uses build that we don't have available. Pick the closest one to the ones we have.

		int closest_build = get_closest_build_number_supported();

		CMessageBox::display_error("Your current engine build isn't supported yet: {}"
			   "\n\nFor now, the cheat will try to continue with the following build that is supported: {}."
			   "\nIf you experience crashes, random behaviour, or something not working, please contact developers of this cheat so that we can add support for your CS build.",
			   m_current_build,
			   closest_build);

		m_current_build = closest_build;
		decide_on_build_container(); // call recursively.
	}
}

int CBytePatternBank::get_closest_build_number_supported()
{
	int c = 0;
	int l = 0, h = 99999;
	int d = m_current_build;
	for (int b : m_supported_builds) // find closest smaller
	{
		if (l < b && b < d)
			l = b;
	}

	for (int b : m_supported_builds) // find closest bigger
	{
		if (h > b && b > d)
			h = b;
	}

	//
	// find who is closer
	//

	if (h == l) // equal, doesn't matter who
	{
		c = l;
	}
	// lean to the bigger version, if we're using the one from 2013. These versions received
	// huge update to the surface code and it make more sense to choose the higher builds for 
	// these, because the byte patterns from <6000 would not work.
	else if (d >= FIRST_SDL_BUILD)
	{
		c = h;
	}
	else if (d - l < h - d) // lower is closer
	{
		c = l;
	}
	else if (h - d < d - l) // higher is closer
	{
		c = h;
	}
	else if (d - l == h - d) // equaly close, select higher
	{
		c = h;
	}

	return c;
}
