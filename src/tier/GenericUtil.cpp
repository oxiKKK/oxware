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

HANDLE CGenericUtil::get_current_process_handle()
{
	return (HANDLE)-1;
}

HANDLE CGenericUtil::get_current_thread_handle()
{
	return (HANDLE)-2; // yes really, don't ask why.
}

DWORD CGenericUtil::get_last_win_error()
{
	return NtCurrentTeb()->LastErrorValue;
}

std::string CGenericUtil::virtual_key_to_string(int virtual_key)
{
	unsigned code = MapVirtualKeyA(virtual_key, MAPVK_VK_TO_VSC);

	char key_name[128];
	switch (virtual_key)
	{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			code |= KF_EXTENDED;
		default:
			GetKeyNameTextA(code << 16, key_name, sizeof(key_name));
	}
	return key_name;
}

void CGenericUtil::copy_to_clipboard(const std::string& text)
{
	if (OpenClipboard(NULL))
	{
		if (EmptyClipboard())
		{
			HGLOBAL hglbCopy;
			LPTSTR  tstrCopy;

			hglbCopy = GlobalAlloc(GMEM_DDESHARE, text.size() + sizeof(TCHAR));

			if (hglbCopy != NULL)
			{
				tstrCopy = (LPTSTR)GlobalLock(hglbCopy);
				strcpy(tstrCopy, (LPCTSTR)text.c_str());
				GlobalUnlock(hglbCopy);

				SetClipboardData(CF_TEXT, hglbCopy);
			}
		}
		CloseClipboard();
	}
}

void CGenericUtil::simulate_crash()
{
	*(int*)0 = 0xDEADBEEF;
}

bool CGenericUtil::is_alphabetical_or_number(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

std::string CGenericUtil::get_filename_from_path(const std::string & path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}
