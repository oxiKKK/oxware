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

void CInjectedDllIPCLayerClient::establish_communication(IPCInterface_t* ipc)
{
	m_ipc = ipc;

	write_code(C2I_None);
}

IPCLayerStatus CInjectedDllIPCLayerClient::dispatch()
{	
	assert(m_ipc);

	// injector stopped communication with us, it probably suddenly crashed or exited.
	if (!heart_beating())
	{
		return k_IPCLayerStatus_InjectorStopBeat;
	}

	// reading injector codes...
	switch (read_code())
	{
		case I2C_None:
		{
			break;
		}
		case I2C_RequestUnload:
		{
			// stuff
			CConsole::the().info("Injector requested unload... destroying cheat...");
			return k_IPCLayerStatus_Unloading;
		}
		default:
		{
			assert(0);
			break;
			// shouldn't happen
		}
	}

	return k_IPCLayerStatus_Ok;
}

void CInjectedDllIPCLayerClient::write_code(EClient2Injector code)
{
	assert(m_ipc);
	assert(code >= C2I_None && code < C2I_Max);

	m_ipc->m_c2m = code;
}

EInjector2Client CInjectedDllIPCLayerClient::read_code() const
{
	assert(m_ipc);

	return m_ipc->m_m2c;
}

bool CInjectedDllIPCLayerClient::heart_beating()
{
	static size_t last_sequence = 0;
	static uint32_t last_heartbeat = GetTickCount();
	if (m_ipc->m_heartbeat_sequence == last_sequence)
	{
		if (GetTickCount() - last_heartbeat > k_havent_received_hbeat_in_seconds * 1000)
		{
			CConsole::the().info("Fatal - Injector stopped beating. Haven't received heart beat in {} seconds. Destroying...", k_havent_received_hbeat_in_seconds);
			return false;
		}
	}
	else
	{
		// injector sent new one
		last_sequence = m_ipc->m_heartbeat_sequence;
		last_heartbeat = GetTickCount();
	}

	return true;
}
