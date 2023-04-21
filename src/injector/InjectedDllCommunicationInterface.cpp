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

bool InjectedDllCommunicationInterface::write_ipc_block(LPVOID where)
{
	if (!write_generic<IPCInterface_t>(
		where, &m_ipc_block))
	{
		return false; // sudden target process crash or mapped dll unload
	}

	return true;
}

bool InjectedDllCommunicationInterface::listen_to_injected_dll(EClient2Injector& code)
{
	IPCInterface_t ipc;
	if (!read_block(&ipc))
	{
		return false;
	}

	// keep in date with the data that is modified by the mapped dll
	m_ipc_block = ipc;

	// send heartbeat to parent client dll
	if (!send_heartbeat())
	{
		return false;
	}

	code = m_ipc_block.m_c2m;
	return true;
}

bool InjectedDllCommunicationInterface::tell_to(EInjector2Client code)
{
	assert(code >= I2C_None && code < I2C_Max);

	if (!write_generic<EInjector2Client>(
		(PVOID)((uint8_t*)get_target_process_ipc_block_addr() + offsetof(IPCInterface_t, m_m2c)),
		&code))
	{
		CConsole::the().error("Couldn't write to target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

	CConsole::the().info("I2C: wrote {}", (int)code);
	return true;
}

bool InjectedDllCommunicationInterface::listen_from(EClient2Injector& code)
{
	IPCInterface_t ipc;
	if (!read_block(&ipc))
	{
		return false; // sudden target process crash or mapped dll unload
	}

	code = ipc.m_c2m;
	return true;
}

bool InjectedDllCommunicationInterface::send_heartbeat()
{
	m_ipc_block.m_heartbeat_sequence++;

	if (!write_generic<size_t>(
		(PVOID)((uint8_t*)get_target_process_ipc_block_addr() + offsetof(IPCInterface_t, m_heartbeat_sequence)),
		&m_ipc_block.m_heartbeat_sequence))
	{
		CConsole::the().error("Couldn't write to target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

//	CConsole::the().info("beat {}", comm.m_heartbeat_sequence);

	return true;
}

bool InjectedDllCommunicationInterface::read_block(IPCInterface_t* ipc)
{
	if (!read_generic<IPCInterface_t>(
		(PVOID)get_target_process_ipc_block_addr(), ipc))
	{
		CConsole::the().error("Couldn't read target process memory while trying to communicate with it.");
		return false; // sudden target process crash or mapped dll unload
	}

	return true;
}
