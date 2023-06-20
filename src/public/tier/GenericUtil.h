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

#ifndef GENERICUTIL_H
#define GENERICUTIL_H
#pragma once

struct file_version_t
{
	// X.X.X.X
	uint32_t w1, w2, w3, w4;

	std::string to_string() { return std::format("{}.{}.{}.{}", w1, w2, w3, w4); }
};

class CGenericUtil
{
public:
	DECL_BASIC_CLASS(CGenericUtil);

public:
	HANDLE get_current_process_handle();
	HANDLE get_current_thread_handle();

	// synonym for GetLastError
	DWORD get_last_win_error();

	std::string virtual_key_to_string(int virtual_key);

	void copy_to_clipboard(const std::string& text);

	// invalid address access
	void simulate_crash();

	// returns true if character is a number or a letter.
	bool is_alphabetical_or_number(char c);

	std::string get_filename_from_path(const std::string& path);
	std::string remove_extension_from_path(const std::string& path);

	void open_link_inside_browser(const std::string& link);
	void open_folder_inside_explorer(const std::string& folder_path);

	RTL_OSVERSIONINFOW get_os_version();

	// "{}.{} build {}" format
	// e.g.
	// ""
	std::string get_os_version_str();

	file_version_t get_file_version(const std::string& path);

	// get C:\Windows directory
	std::filesystem::path get_windows_directory(const std::filesystem::path& subdir);

	// get "C:\Windows\system32" for x86 systems and "C:\Windows\SysWOW64" for x64 systems
	std::filesystem::path get_system_directory(const std::filesystem::path& subdir);

	// true if current process is running under WOW64 (is 32-bit)
	bool running_32_bit_windows();

	bool does_cmdline_parameter_exist(const std::string& substring);

	std::string fix_directory_separators(const std::string& directory);

	void push_page_protection(uintptr_t address, size_t size, uint32_t page_protection_flags);
	void pop_page_protection();

	Vector2D get_window_pos(HWND hwnd);

private:
	uint32_t m_old_page_protection_flags;
	uint32_t m_page_protection_patch_address;
	size_t m_page_protection_patch_size;
	void reset_page_protection_data()
	{
		m_old_page_protection_flags = m_page_protection_patch_address = m_page_protection_patch_size = 0;
	}
};

#endif // GENERICUTIL_H