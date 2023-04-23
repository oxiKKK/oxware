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

// a communicative dll is a dll that exposes this routine and provides communication functionality with the injector.
extern "C" DLLEXPORT bool CommunicativeDllEntryPoint(injector_information_package_t* ifp)
{
	CConsole::the().info(__FUNCTION__ " start.");

	bool ret = CoXWARE::the().run(ifp);
	if (!ret)
	{
		CInjectedDllIPCLayerClient::the().write_code(C2I_Init_Failed);
	}

	CConsole::the().info(__FUNCTION__ " end.");
	return ret;
}

//-------------------------------------------------------------------------------

#include "tier/MessageBox.h"

bool insert_inverted_function_table(uintptr_t* module_base)
{
	// byte pattern for RtlInsertInvertedFunctionTable inside ntdll.
	CBytePattern RltIIFT_pattern("\x8B\xFF\x55\x8B\xEC\x83\xEC\x0C\x53\x56\x57\x8D\x45\xF8\x8B\xFA");

	DWORD ntdll_base = (DWORD)GetModuleHandleA("ntdll.dll");
	DWORD ntdll_base_code = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)ntdll_base + ((PIMAGE_DOS_HEADER)ntdll_base)->e_lfanew))->OptionalHeader.BaseOfCode;
	DWORD size_of_ntdll_image_code = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)ntdll_base + ((PIMAGE_DOS_HEADER)ntdll_base)->e_lfanew))->OptionalHeader.SizeOfCode;
	DWORD size_of_ntdll_image = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)ntdll_base + ((PIMAGE_DOS_HEADER)ntdll_base)->e_lfanew))->OptionalHeader.SizeOfImage;

	// Note that this function's declaration changes rapidly through various windows versions.
	// On windows 7, this function has three parameters, but on windows 10 it has only two.
	// The byte pattern for this function may change often, too...
	//
	// This function is normally called by the internal native loader api when loading a dll.
	// Without this function call, we aren't able to use C++ exceptions inside of our code.
	void(__fastcall * RtlInsertInvertedFunctionTable)(DWORD ImageBase, DWORD SizeOfImage);
	RtlInsertInvertedFunctionTable = (decltype(RtlInsertInvertedFunctionTable))RltIIFT_pattern.search_in_loaded_address_space(ntdll_base, ntdll_base + size_of_ntdll_image);

	if (RtlInsertInvertedFunctionTable)
	{
		DWORD size_of_image_current = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)module_base + ((PIMAGE_DOS_HEADER)module_base)->e_lfanew))->OptionalHeader.SizeOfImage;
		RtlInsertInvertedFunctionTable((DWORD)module_base, size_of_image_current);
	}
	else
	{
		CMessageBox::display_error("Couldn't find RtlInsertInvertedFunctionTable function. This function is mandatory. Aborting injection...");
		return false;
	}

	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved)
{
	//
	// Enable C++ exceptions on x86 module
	//
	if (!insert_inverted_function_table((uintptr_t*)hModule))
	{
		return false;
	}

#if 0
	try
	{
		throw std::invalid_argument("received negative value");
	}
	catch (...)
	{
		CMessageBox::display_error("error");
	}
#endif

	// TODO: Implement also removing exception function table RtlRemoveInvertedFunctionTable.

	return TRUE;
}
