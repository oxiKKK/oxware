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

bool CMemoryFnHookMgr::install_hooks()
{
	// let's install individual hooks.
	if (!VGuiWrap2_IsConsoleVisible().install()) return false;
	if (!VGuiWrap2_ConPrintf().install()) return false;
	if (!ClearIOStates().install()) return false;

	return true;
}

//-----------------------------------------------------------------------------

bool VGuiWrap2_IsConsoleVisibleFnHook_t::install()
{
	initialize("VGuiWrap2_IsConsoleVisible", L"hw.dll");
	return generic_bytepattern_installer({ "\x8B\x00\x00\x00\x00\x00\x85\xC9\x74\x00\x8B\x01\xFF\x50\x20\x8B\x00\x00\x00\x00\x00\x85\xC9\x74\x00\x8B\x11\xFF\x62\x28" });
}

//-----------------------------------------------------------------------------

bool VGuiWrap2_ConPrintfFnHook_t::install()
{
	initialize("VGuiWrap2_ConPrintf", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x85\xC0\x74\x14\x8B\x55\x08\x8B\x08\x52\x68\x00\x00\x00\x00\x50\xFF\x51\x1C\x83\xC4\x0C\x5D\xC3\x53\x56\x8B\x75\x08\x57\x8B\xFE\x83\xC9\xFF\x33\xC0\xF2\xAE\xA1\x00\x00\x00\x00\xF7\xD1\x49\x8B\xD8\x8B\xF9\x74\x49\x8B" });
}

//-----------------------------------------------------------------------------

bool ClearIOStatesFnHook_t::install()
{
	initialize("ClearIOStates", L"hw.dll");
	return generic_bytepattern_installer({ "\x56\x33\xF6\x6A\x00\x56\xE8\x00\x00\x00\x00\x83\xC4\x08\x46\x81\xFE" });
}

//-----------------------------------------------------------------------------
