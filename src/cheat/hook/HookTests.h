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

#ifndef HOOKTESTS_H
#define HOOKTESTS_H
#pragma once

// define this to automatically start the testing after the cheat is injected. The testing can be done in two ways:
//	1) when you inject to a fresh game (ie right after startup)
//	2) when ingame, some tests have special cases and these cases can be only executed when yo're in game.
//
//	HOW TO TEST HOOKS:
//	a) always have a fresh game (after launch)
//	b) don't test after you have disconnected from a server. Some internal engine data still remains in the memory and this could make the
//	   the results of testing inaccurate!
#ifdef _DEBUG
//#	define OX_ENABLE_HOOK_TESTING
#endif

// use this for memory hooks
class TestableHook
{
public:
	virtual void test_hook() { };

	virtual void default_test_hook() { };
};

class CHookTests
{
public:
	DECL_BASIC_CLASS(CHookTests);

public:
	void execute_tests();

	void run_seh_protected_block(const std::string& name, const std::function<bool()>& code_to_be_ran, bool is_default = false);

public:
	void add_for_testing(const std::string& hook_type, TestableHook* hook)
	{
		m_testable_hooks[hook_type].push_back(hook);
	}

private:
	std::unordered_map<std::string, std::vector<TestableHook*>> m_testable_hooks;
};

#endif // HOOKTESTS_H