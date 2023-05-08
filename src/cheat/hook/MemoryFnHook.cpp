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
	if (!R_GetStudioBounds().install()) return false;
	if (!MD5_Hash_File().install()) return false;

	return true;
}

//-----------------------------------------------------------------------------

bool VGuiWrap2_IsConsoleVisible_FnHook_t::install()
{
	initialize("VGuiWrap2_IsConsoleVisible", L"hw.dll");
	return install_using_bytepattern(0);
}

//-----------------------------------------------------------------------------

bool VGuiWrap2_ConPrintf_FnHook_t::install()
{
	initialize("VGuiWrap2_ConPrintf", L"hw.dll");
	return install_using_bytepattern(0);
}

//-----------------------------------------------------------------------------

bool ClearIOStates_FnHook_t::install()
{
	initialize("ClearIOStates", L"hw.dll");
	return install_using_bytepattern(0);
}

//-----------------------------------------------------------------------------

bool R_GetStudioBounds_FnHook_t::install()
{
	initialize("R_GetStudioBounds", L"hw.dll");
	return install_using_bytepattern(0);
}

//-----------------------------------------------------------------------------

bool MD5_Hash_File_FnHook_t::install()
{
	initialize("MD5_Hash_File", L"hw.dll");
	return install_using_bytepattern(0);
}
