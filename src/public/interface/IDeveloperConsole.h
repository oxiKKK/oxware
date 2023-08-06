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

#ifndef IDEVELOPERCONSOLE_H
#define IDEVELOPERCONSOLE_H
#pragma once

#include "interface.h"
#include "color.h"

#include "IFileSystem.h"

#include <string>
#include <assert.h>

#ifdef ERROR
#undef ERROR
#endif

enum class EOutputCategory : unsigned int
{
	INFO,
	ERROR,
	WARNING,

	COUNT
};
constexpr unsigned k_num_output_categories = (unsigned)EOutputCategory::COUNT;

// Modules that want to use the console output must be added here.
// !!! As well as at the color palette.
enum class EOutputModule : unsigned int
{
	UNSPECIFIED, // when the module hasn't been set yet

	GUI,
	LOADER,
	MMAPPER,
	CHEAT,
	UTIL,

	// used by input inside the console 
	CONSOLE,

	COUNT
};
constexpr unsigned k_num_output_modules = (unsigned)EOutputModule::COUNT;

class IDeveloperConsole : public IBaseInterface
{
public:
	virtual void initialize(const std::string& module_name) = 0;
	virtual void shutdown() = 0;
	virtual void render() = 0;

	virtual void print(EOutputModule module, EOutputCategory category, const std::string& what, bool printed_already = false, std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) = 0;

	virtual void flush_screen() = 0;

	virtual void register_module(EOutputModule which) = 0;
	virtual void unregister_module(EOutputModule which) = 0;

	virtual std::filesystem::path get_logfile_path() = 0;
	virtual std::filesystem::path get_logfile_path_fancy() = 0; // with %appdata% instead of the actual directory

	virtual void enable_tooltip(bool enable) = 0;

protected:
	template<typename T>
	struct print_color_t
	{
		T				which;
		CColor			color;
	};

	using category_print_color_t = print_color_t<EOutputCategory>;
	using module_print_color_t = print_color_t<EOutputModule>;

	// Predefined colors for each category
	inline static category_print_color_t s_category_color_table[(int)EOutputCategory::COUNT] =
	{
		{ EOutputCategory::INFO,		{  44,  44,  44, 255 }}, // Should be in sync with GUICLR_Text*
		{ EOutputCategory::ERROR,		{ 220,  20,  60, 255 }},
		{ EOutputCategory::WARNING,		{ 255, 145,   0, 255 }},
	};

	// and for each module
	inline static module_print_color_t s_module_color_table[k_num_output_modules] =
	{
		{ EOutputModule::UNSPECIFIED,	{ 220, 220, 220, 255 }},
		{ EOutputModule::GUI,			{ 230, 40, 120, 255 }},
		{ EOutputModule::LOADER,		{ 0, 100, 167, 255 }},
		{ EOutputModule::MMAPPER,		{ 187, 245, 0, 255 }},
		{ EOutputModule::CHEAT,			{ 120, 50, 165, 255 }},
		{ EOutputModule::UTIL,			{ 68, 218, 73, 255 }},
		{ EOutputModule::CONSOLE,		{ 100, 56, 176, 255 }},
	};

	static const char* outputcategory_as_string(EOutputCategory m)
	{
		switch (m)
		{
			case EOutputCategory::INFO:		return "Info";
			case EOutputCategory::ERROR:	return "Error";
			case EOutputCategory::WARNING:	return "Warning";
		}

		assert(0);
		return "idk";
	}

	static const char* outputmodule_as_string(EOutputModule m)
	{
		switch (m)
		{
			case EOutputModule::UNSPECIFIED:	return "Unspecified";
			case EOutputModule::GUI:			return "GUI";
			case EOutputModule::LOADER:			return "Loader";
			case EOutputModule::MMAPPER:		return "MMapper";
			case EOutputModule::CHEAT:			return "Cheat";
			case EOutputModule::UTIL:			return "Util";
			case EOutputModule::CONSOLE:		return "Console";
		}

		assert(0);
		return "idk";
	}
};

extern IDeveloperConsole* g_devconsole_i;

#define IDEVELOPERCONSOLE_INTERFACEID "IDeveloperConsole"

#endif // IDEVELOPERCONSOLE_H