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

bool CSVCFuncDetourMgr::install_hooks()
{
	if (!svc_sound_fn().install_svc(svc_sound_f, "svc_sound", svc_sound)) return false;
	//if (!svc_time_fn().install_svc(svc_time_f, "svc_time", svc_time)) return false;
	if (!svc_sendcvarvalue_fn().install_svc(svc_sendcvarvalue_f, "svc_sendcvarvalue", svc_sendcvarvalue)) return false;
	if (COxWare::the().get_build_number() >= 4554)
	{
		if (!svc_sendcvarvalue2_fn().install_svc(svc_sendcvarvalue2_f, "svc_sendcvarvalue2", svc_sendcvarvalue2)) return false;
	}
	if (!svc_stufftext_fn().install_svc(svc_stufftext_f, "svc_stufftext", svc_stufftext)) return false;

	return true;
}

void CSVCFuncDetourMgr::uninstall_hooks()
{
	svc_sound_fn().uninstall();
	//svc_time_fn().uninstall();
	svc_sendcvarvalue_fn().uninstall();
	if (COxWare::the().get_build_number() >= 4554)
	{
		svc_sendcvarvalue2_fn().uninstall();
	}
	svc_stufftext_fn().uninstall();
}

//----------------------------------------------------------------------------------------------

void CSVCFuncDetourMgr::svc_sound_f()
{
	//if (CHLNetMessageIO::the().is_ready())
	//{
	//	CHLNetMessageIO::the().begin_silent_bit_reading();
	//
	//	int flags = CHLNetMessageIO::the().read_bits(9);
	//
	//	CConsole::the().info("svc_sound: flags {}", flags);
	//
	//	CHLNetMessageIO::the().end_silent_bit_reading();
	//}
	
	CSVCFuncDetourMgr::the().svc_sound_fn().call();
}

void CSVCFuncDetourMgr::svc_time_f()
{
	if (CHLNetMessageIO::the().is_ready())
	{
		CHLNetMessageIO::the().begin_silent_reading();

		CConsole::the().info("svc_time: {}", CHLNetMessageIO::the().read_float());

		CHLNetMessageIO::the().end_silent_reading();
	}

	CSVCFuncDetourMgr::the().svc_time_fn().call();
}

void CSVCFuncDetourMgr::svc_sendcvarvalue_f()
{
	if (CHLNetMessageIO::the().is_ready())
	{
		// we don't actually perform a silent read begin&end operation, because we'll basically reimplement
		// the function completely. "Replace it"

		// servers call this in order to request cvar value from the client.

		char* requested_cvar = CHLNetMessageIO::the().read_string();

		CHLNetMessageIO::the().write_byte(clc_cvarvalue);

		if (cvar_sandbox_monitor_server.get_value())
		{
			CConsole::the().info("Server requested cvar value from '{}' (svc_sendcvarvalue)", requested_cvar);
		}

		if (strlen(requested_cvar) >= 255)
		{
			CHLNetMessageIO::the().write_string("Bad CVAR request");
			return;
		}

		// Send back the cvar type to server
		auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetCvarPointer(requested_cvar);
		if (cvar == NULL)
		{
			CHLNetMessageIO::the().write_string("Bad CVAR request");
			return;
		}

		if (cvar->flags & FCVAR_PRIVILEGED)
		{
			CHLNetMessageIO::the().write_string("CVAR is privileged");
		}
		else if (cvar->flags & FCVAR_SERVER)
		{
			CHLNetMessageIO::the().write_string("CVAR is server-only");
		}
		else if (cvar->flags & FCVAR_PROTECTED)
		{
			CHLNetMessageIO::the().write_string("CVAR is protected");
		}
		else
		{
			const char* value = CCvarSandbox::the().filter_cvarvalue(requested_cvar);
			if (value == nullptr)
			{
				value = cvar->string;
			}

			if (cvar_sandbox_monitor_server.get_value())
			{
				if (stricmp(cvar->string, value))
				{
					CConsole::the().info("Responding to server with.. '{}' (but have '{}')", value, cvar->string);
				}
				else
				{
					CConsole::the().info("Responding to server with.. '{}'", value);
				}
			}

			CHLNetMessageIO::the().write_string(value);
		}
	}
	else
	{
		CSVCFuncDetourMgr::the().svc_sendcvarvalue_fn().call();
	}
}

void CSVCFuncDetourMgr::svc_sendcvarvalue2_f()
{
	if (CHLNetMessageIO::the().is_ready())
	{
		// we don't actually perform a silent read begin&end operation, because we'll basically reimplement
		// the function completely. "Replace it"

		// servers call this in order to request cvar value from the client.

		int request_id = CHLNetMessageIO::the().read_int32();
		char* requested_cvar = CHLNetMessageIO::the().read_string();

		CHLNetMessageIO::the().write_byte(clc_cvarvalue2);
		CHLNetMessageIO::the().write_long(request_id);
		CHLNetMessageIO::the().write_string(requested_cvar);

		if (cvar_sandbox_monitor_server.get_value())
		{
			CConsole::the().info("Server requested cvar value from '{}' (svc_sendcvarvalue2)", requested_cvar);
		}

		if (strlen(requested_cvar) >= 255)
		{
			CHLNetMessageIO::the().write_string("Bad CVAR request");
			return;
		}

		// Send back the cvar type to server
		auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetCvarPointer(requested_cvar);
		if (cvar == NULL)
		{
			CHLNetMessageIO::the().write_string("Bad CVAR request");
			return;
		}

		if (cvar->flags & FCVAR_PRIVILEGED)
		{
			CHLNetMessageIO::the().write_string("CVAR is privileged");
		}
		else if (cvar->flags & FCVAR_SERVER)
		{
			CHLNetMessageIO::the().write_string("CVAR is server-only");
		}
		else if (cvar->flags & FCVAR_PROTECTED)
		{
			CHLNetMessageIO::the().write_string("CVAR is protected");
		}
		else
		{
			const char* value = CCvarSandbox::the().filter_cvarvalue(requested_cvar);
			if (value == nullptr)
			{
				value = cvar->string;
			}

			if (cvar_sandbox_monitor_server.get_value())
			{
				CConsole::the().info("Responding to server with.. '{}' (but have '{}')", value, cvar->string);
			}

			CHLNetMessageIO::the().write_string(value);
		}
	}
	else
	{
		CSVCFuncDetourMgr::the().svc_sendcvarvalue2_fn().call();
	}
}

void CSVCFuncDetourMgr::svc_stufftext_f()
{
	if (CHLNetMessageIO::the().is_ready())
	{
		if (CAntiDemoRecorder::the().allow_demo_to_be_recorded())
		{
			return;
		}

		if (!CStuffCmdFilter::the().allow_command_to_be_executed())
		{
			return;
		}
	}

	CSVCFuncDetourMgr::the().svc_stufftext_fn().call();
}
