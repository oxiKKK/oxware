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

std::string CGenericUtil::get_filename_from_path(const std::string& path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

std::string CGenericUtil::remove_extension_from_path(const std::string& path)
{
	size_t lastindex = path.find_last_of(".");
	return path.substr(0, lastindex);
}

void CGenericUtil::open_link_inside_browser(const std::string& link)
{
	ShellExecuteA(0, 0, link.c_str(), 0, 0, SW_SHOW);
}

void CGenericUtil::open_folder_inside_explorer(const std::string & folder_path)
{
	ShellExecuteA(NULL, "open", folder_path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

// https://stackoverflow.com/questions/36543301/detecting-windows-10-version/36545162#36545162
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
RTL_OSVERSIONINFOW CGenericUtil::get_os_version()
{
	HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
	if (hMod)
	{
		RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
		if (fxPtr)
		{
			RTL_OSVERSIONINFOW rovi = { 0 };
			rovi.dwOSVersionInfoSize = sizeof(rovi);
			if (STATUS_SUCCESS == fxPtr(&rovi))
			{
				return rovi;
			}
		}
	}
	RTL_OSVERSIONINFOW rovi = { 0 };
	return rovi;
}

std::string CGenericUtil::get_os_version_str()
{
	auto osver = CGenericUtil::the().get_os_version();

	return std::format("{}.{} build {}", osver.dwMajorVersion, osver.dwMinorVersion, osver.dwBuildNumber);
}

// https://stackoverflow.com/questions/940707/how-do-i-programmatically-get-the-version-of-a-dll-or-exe-file
file_version_t CGenericUtil::get_file_version(const std::string& path)
{
	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSizeA(path.c_str(), &verHandle);

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfoA(path.c_str(), verHandle, verSize, verData))
		{
			if (VerQueryValueA(verData, "\\", (VOID FAR * FAR*)&lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						// This returns product version, not file version. For more info see:
						// https://stackoverflow.com/questions/38068477/why-does-getfileversioninfo-on-kernel32-dll-in-windows-10-return-version-6-2
						file_version_t v = 
						{ 
							(verInfo->dwProductVersionMS >> 16) & 0xffff,
							(verInfo->dwProductVersionMS >> 0) & 0xffff, 
							(verInfo->dwProductVersionLS >> 16) & 0xffff, 
							(verInfo->dwProductVersionLS >> 0) & 0xffff
						};
						delete[] verData;
						return v;
					}
				}
			}
		}

		delete[] verData;
	}

	CConsole::the().error("Couldn't get file version of '{}'. (last err: {})", path, get_last_win_error());
	return {};
}

std::filesystem::path CGenericUtil::get_windows_directory(const std::filesystem::path& subdir)
{
	// NOTE: Duplicate inside LibraryLoader code

	uintptr_t* volatile pNtSystemRoot = (uintptr_t*)0x7FFE0030;
	return std::filesystem::path((const wchar_t*)pNtSystemRoot) / subdir; // cool, isn't it?

}

std::filesystem::path CGenericUtil::get_system_directory(const std::filesystem::path& subdir)
{
	if (running_32_bit_windows())
	{
		return get_windows_directory("system32") / subdir;
	}
	else
	{
		return get_windows_directory("SysWOW64") / subdir;
	}
}

bool CGenericUtil::running_32_bit_windows()
{
	BOOL running = FALSE;
	BOOL ret = IsWow64Process(get_current_process_handle(), &running);
	if (!ret)
	{
		CConsole::the().error("IsWow64Process failed with {} error code.", get_last_win_error());
	}
	return running == FALSE;
}

bool CGenericUtil::does_cmdline_parameter_exist(const std::string& substring)
{
	if (strstr(GetCommandLineA(), substring.c_str()))
	{
		return true;
	}

	return false;
}

std::string CGenericUtil::fix_directory_separators(const std::string& directory)
{
	std::filesystem::path p = directory;
	return p.make_preferred().string();
}

void CGenericUtil::push_page_protection(uintptr_t address, size_t size, uint32_t page_protection_flags)
{
	assert(m_page_protection_patch_address == NULL && m_page_protection_patch_size == NULL && "Didn't pop previously pushed page protection!");

	BOOL success = VirtualProtect((LPVOID)address, size, page_protection_flags, (PDWORD)&m_old_page_protection_flags) != FALSE;
	if (success)
	{
		m_page_protection_patch_address = address;
		m_page_protection_patch_size = size;
	}
	else
	{
		CConsole::the().error("Failed to push '{}' page protection flags to memory region: 0x{:08X} of size 0x{:08X}", page_protection_flags, address, size);
	}
}

void CGenericUtil::pop_page_protection()
{
	assert(m_page_protection_patch_address != NULL && m_page_protection_patch_size != NULL && "Tried to pop non-pushed page protection!");

	DWORD dummy; // the function fails if we don't provde it this.
	BOOL success = VirtualProtect((LPVOID)m_page_protection_patch_address, m_page_protection_patch_size, m_old_page_protection_flags, &dummy) != FALSE;
	if (success)
	{
		// call this because we modified the memory
		FlushInstructionCache(get_current_process_handle(), (LPCVOID)m_page_protection_patch_address, m_page_protection_patch_size);
	}
	else
	{
		CConsole::the().error("Failed to restore '{}' page protection flags to memory region: 0x{:08X} of size 0x{:08X}", 
							  m_old_page_protection_flags, m_page_protection_patch_address, m_page_protection_patch_size);
	}

	reset_page_protection_data();
}

Vector2D CGenericUtil::get_window_pos(HWND hwnd)
{
	Vector2D v{};
	RECT rect = { NULL };
	if (GetWindowRect(hwnd, &rect))
	{
		v.x = rect.left;
		v.y = rect.top;
	}
	return v;
}
