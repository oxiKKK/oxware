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

#ifndef CONSOLE_H
#define CONSOLE_H
#pragma once

#include "commondefs.h"
#include "interface/IDeveloperConsole.h"

#include <string>
#include <mutex>
#include <vector>
#include <format>

class CConsole
{
public:
	DECL_BASIC_CLASS(CConsole);

public:
	bool initialize(EOutputModule module, IDeveloperConsole* devcon_i);
	void destroy();

	template <class... _Types>
	void info(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		print_generic(EOutputCategory::INFO, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... _Types>
	void dinfo(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
#ifdef _DEBUG
		print_generic(EOutputCategory::INFO, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
#endif
	}

	template <class... _Types>
	void error(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		print_generic(EOutputCategory::ERROR, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
	}
	
	template <class... _Types>
	void derror(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
#ifdef _DEBUG
		print_generic(EOutputCategory::ERROR, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
#endif
	}

	template <class... _Types>
	void warning(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		print_generic(EOutputCategory::WARNING, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... _Types>
	void dwarning(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
#ifdef _DEBUG
		print_generic(EOutputCategory::WARNING, std::vformat(_Fmt.get(), std::make_format_args(_Args...)));
#endif
	}

	inline bool is_initialized() const { return m_initialized; };

private:
	void print_generic(EOutputCategory category, const std::string& what, bool printed_already = false, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

	void execute_scheduled_messages();

private:
	IDeveloperConsole* m_devcon_i = nullptr;

	EOutputModule m_current_module = EOutputModule::UNSPECIFIED;

	std::mutex m_mutex;

	bool m_initialized = false;

	// message that has been printed before the console has been initialized.
	struct scheduled_message_t
	{
		EOutputCategory category;
		std::string what;
		std::chrono::system_clock::time_point m_timestamp;
	};

	std::vector<scheduled_message_t> m_scheduled_messages;
};

#endif // CONSOLE_H