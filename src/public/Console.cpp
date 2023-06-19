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
// Console.cpp -- Module-dependent implementation that communicates with the IDeveloperClass interface.
// 
//	Each module links this class and after initialization it uses it to use the developer console. This is 
//	an automatic way for the DeveloperConsole interface to distinguish between which modules are printing msgs.
//

#include "Console.h"

#include "tier/StringTools.h"

#include <assert.h>
#include <format>

bool CConsole::initialize(EOutputModule module, IDeveloperConsole* devcon_i)
{
	m_mutex.lock();

	assert(devcon_i);

	m_devcon_i = devcon_i;
	m_current_module = module;

	// tell the DeveloperConsole interface that we're registering this module
	m_devcon_i->register_module(m_current_module);

	m_initialized = true;

	m_mutex.unlock();

	execute_scheduled_messages();

	return true;
}

void CConsole::destroy()
{
	m_mutex.lock();
	
	if (m_devcon_i)
	{
		m_devcon_i->unregister_module(m_current_module);

		m_devcon_i->shutdown();
	}

	m_initialized = false;

	m_mutex.unlock();
}

void CConsole::print_generic(EOutputCategory category, const std::string& what, bool printed_already, std::chrono::system_clock::time_point time)
{
	m_mutex.lock();

	if (m_initialized)
	{
		// we have the developer console interface pointer, fine
		m_devcon_i->print(m_current_module, category, what, printed_already, time);
	}
	else
	{
		// we don't have it, schedule messages until we have it.
		scheduled_message_t msg = { category, what, time};
		m_scheduled_messages.emplace_back(msg);
		OutputDebugStringA((what + '\n').c_str());
	}

	m_mutex.unlock();
}

void CConsole::execute_scheduled_messages()
{
	assert(m_initialized);

	for (auto& msg : m_scheduled_messages)
	{
		print_generic(msg.category, msg.what, true, msg.m_timestamp);
	}
}
