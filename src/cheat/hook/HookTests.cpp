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

void CHookTests::execute_tests()
{
	CConsole::the().info("Executing tests...");

	CConsole::the().info("{:<32}  {:<3} {:<12}", "hook name", "status", "default or custom");
	CConsole::the().info("-----------------------------------------------------------------");
	
	for (const auto& [type, hook] : m_testable_hooks)
	{
		CConsole::the().info("---------- {} ----------", type);

		for (const auto& test : hook)
		{
			test->default_test_hook(); // common for every hook
			test->test_hook();
		}
	}

	CConsole::the().info("-----------------------------------------------------------------");
	CConsole::the().info("Executing tests finished.");
}

void CHookTests::run_seh_protected_block(const std::string& name, const std::function<bool()>& code_to_be_ran, bool is_default)
{
	bool success = false;
	__try
	{
		success = code_to_be_ran();
	}
	__except (1)
	{
		CMessageBox::display_error("Crashed while trying to test {} hook.", name);
	}

	if (success)
	{
		CConsole::the().info("{:<32}: {:<6} {:<12}", name, "OK", (is_default ? "default" : "custom"));
		return;
	}
	else
	{
		CMessageBox::display_error("Got unexpected results while trying to test {}.", name);
	}

	CConsole::the().info("{:<32}: {:<6} {:<12}", name, "BAD", (is_default ? "default" : "custom"));
}
