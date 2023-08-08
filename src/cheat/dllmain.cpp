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
// dllmain.cpp -- Entry point for the injected cheat dll.
//

#include "precompiled.h"

// called before DllMain
C_DLLEXPORT bool PreDllLoad()
{	
	// preserved for future use.
	return true;
}

//-------------------------------------------------------------------------------

// a communicative dll is a dll that exposes this routine and provides communication functionality with the injector.
C_DLLEXPORT bool CommunicativeDllEntryPoint(injector_information_package_t* ifp)
{
	CConsole::the().info(__FUNCTION__ " start.");

	bool ret = COxWare::the().run(ifp);
	if (!ret)
	{
		CInjectedDllIPCLayerClient::the().write_code(C2I_Init_Failed);
	}

	CConsole::the().info(__FUNCTION__ " end.");
	return ret;
}

//-------------------------------------------------------------------------------

uintptr_t g_cheat_dll_base;

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved)
{
	// so that we don't have to add new members to the information package that is provided to the cheat...
	g_cheat_dll_base = (uintptr_t)hModule;

#if 0
	try
	{
		throw std::invalid_argument("received negative value");
	}
	catch (const std::invalid_argument& arg)
	{
		CMessageBox::display_error("error: {}", arg.what());
	}
#endif

	return TRUE;
}
