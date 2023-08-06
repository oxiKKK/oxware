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

IFileSystem* g_filesystem_i = nullptr;

class CFileSystem : public IFileSystem
{
public:
	CFileSystem();
	~CFileSystem();

	//
	// File system operations
	//

	bool is_directory(const std::filesystem::path& path);
	bool is_file(const std::filesystem::path& path);
	bool do_exist(const std::filesystem::path& path);

	uintmax_t file_size(const std::filesystem::path& path);
	uintmax_t directory_size(const std::filesystem::path& path);

	bool copy(const std::filesystem::path& from, const std::filesystem::path& to, ECopyOptions options = ECopyOptions::none);
	bool rename(const std::filesystem::path& from, const std::filesystem::path& to);
	bool move(const std::filesystem::path& from, const std::filesystem::path& to);
	bool replace(const std::filesystem::path& what, const std::filesystem::path& with);
	bool remove(const std::filesystem::path& path);
	bool remove_all(const std::filesystem::path& directory);
	bool create_file(const std::filesystem::path& path); 
	bool create_directory(const std::filesystem::path& path); 

	void iterate_through_files(const std::filesystem::path& directory, bool recursive, const std::function<void(const std::filesystem::path& file)>& callback);

	std::filesystem::path locate_halflife_dir();
	std::filesystem::path path_relative_to_hl_dir(const std::filesystem::path& relative_path = "");
	std::filesystem::path get_loader_exe_filepath(const std::filesystem::path& relative_path = "");
	std::filesystem::path get_appdata_path(const std::filesystem::path& relative_path = "");
	std::filesystem::path get_relative_to_appdata(const std::filesystem::path& full_path);
	std::filesystem::path get_relative_to_appdata_ex(const std::filesystem::path& additional_path, const std::filesystem::path& full_path);

	//
	// Other
	//

	std::error_code get_last_error() const
	{
		return m_last_error;
	}

private:
	std::error_code m_last_error;
};

CFileSystem g_filesystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFileSystem, IFileSystem, IFILESYSTEM_INTERFACEID, g_filesystem);

//-------------------------------------------------------------------------------
//
// CFileSystem implementation
//
//-------------------------------------------------------------------------------

CFileSystem::CFileSystem()
{
	g_filesystem_i = this;

	m_last_error = {};
}

CFileSystem::~CFileSystem()
{
	g_filesystem_i = nullptr;

	m_last_error = {};
}

bool CFileSystem::is_directory(const std::filesystem::path& path)
{
	std::error_code errc;
	bool ret = std::filesystem::is_directory(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}
	return ret;
}

bool CFileSystem::is_file(const std::filesystem::path& path)
{
	std::error_code errc;
	bool ret = std::filesystem::is_regular_file(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}
	return ret;
}

bool CFileSystem::do_exist(const std::filesystem::path& path)
{
	std::error_code errc;
	bool ret = std::filesystem::exists(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}
	return ret;
}

uintmax_t CFileSystem::file_size(const std::filesystem::path& path)
{
	std::error_code errc;
	uintmax_t size = std::filesystem::file_size(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}
	return size;
}

uintmax_t CFileSystem::directory_size(const std::filesystem::path& path)
{
	std::error_code errc;
	uintmax_t size = 0;

	iterate_through_files(
		path, true, 
		[&](const std::filesystem::path& directory)
		{
			if (!is_file(directory))
			{
				return;
			}

			size += file_size(directory);
		});
	
	if (errc)
	{
		m_last_error = errc;
		return false;
	}
	return size;
}

bool CFileSystem::copy(const std::filesystem::path& from, const std::filesystem::path& to, ECopyOptions options)
{
	std::filesystem::copy_options cpyo = std::filesystem::copy_options::none;

	if ((options & ECopyOptions::skip_existing) != ECopyOptions::none)
	{
		cpyo |= std::filesystem::copy_options::skip_existing;
	}
	if ((options & ECopyOptions::overwrite_existing) != ECopyOptions::none)
	{
		cpyo |= std::filesystem::copy_options::overwrite_existing;
	}
	if ((options & ECopyOptions::recursive) != ECopyOptions::none)
	{
		cpyo |= std::filesystem::copy_options::recursive;
	}
	if ((options & ECopyOptions::only_directories) != ECopyOptions::none)
	{
		cpyo |= std::filesystem::copy_options::directories_only;
	}

	std::error_code errc;
	std::filesystem::copy(from, to, cpyo, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}

	return true;
}

bool CFileSystem::rename(const std::filesystem::path& from, const std::filesystem::path& to)
{
	std::error_code errc;
	std::filesystem::rename(from, to, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}

	return true;
}

bool CFileSystem::move(const std::filesystem::path& from, const std::filesystem::path& to)
{
	// overwrite the destination with source and remove the source

	if (!copy(from, to, ECopyOptions::overwrite_existing))
	{
		return false;
	}

	if (!remove(from))
	{
		return false;
	}

	return true;
}

bool CFileSystem::replace(const std::filesystem::path& what, const std::filesystem::path& with)
{
	// overwrite the destination with source

	if (!copy(what, with, ECopyOptions::overwrite_existing))
	{
		return false;
	}

	return true;
}

bool CFileSystem::remove(const std::filesystem::path& path)
{
	std::error_code errc;
	bool ret = std::filesystem::remove(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}

	return true;
}

bool CFileSystem::remove_all(const std::filesystem::path& path)
{
	std::error_code errc;
	std::filesystem::remove_all(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}

	return true;
}

bool CFileSystem::create_file(const std::filesystem::path& path)
{
	std::ofstream ofs(path);
	if (!ofs.bad())
	{
		return false;
	}

	return true;
}

bool CFileSystem::create_directory(const std::filesystem::path& path)
{
	std::error_code errc;
	std::filesystem::create_directory(path, errc);
	if (errc)
	{
		m_last_error = errc;
		return false;
	}

	return true;
}

void CFileSystem::iterate_through_files(const std::filesystem::path& directory, bool recursive, const std::function<void(const std::filesystem::path&directory)>& callback)
{
	std::error_code errc;
	if (recursive)
	{
		for (const auto& file : std::filesystem::recursive_directory_iterator(directory, errc))
		{
			callback(file.path());
		}
	}
	else
	{
		for (const auto& file : std::filesystem::directory_iterator(directory, errc))
		{
			callback(file.path());
		}
	}

	if (errc)
	{
		m_last_error = errc;
	}
}

std::filesystem::path CFileSystem::locate_halflife_dir()
{
	uintptr_t base;
	PLDR_DATA_TABLE_ENTRY ldr_data;

	ldr_data = g_libloader_i->get_target_loaded_dll_data_table_entry(L"hl.exe");
	if (ldr_data == NULL)
	{
		ldr_data = g_libloader_i->get_target_loaded_dll_data_table_entry(L"cstrike.exe");
	}

	std::filesystem::path halflife_dir;

	if (ldr_data)
	{
		halflife_dir = CStringTools::the().utf16_to_utf8(ldr_data->FullDllName.Buffer);
		halflife_dir = halflife_dir.parent_path();
	}
	else
	{
		// locate it using steam
		const char* path = g_registry_i->read_string(REG_STEAM, "SteamPath");
		if (!path)
		{
			assert(0);
			return {};
		}

		// TODO: Parse 'libraryfolders.vdf' inside 'steamapps/'. There are directories to 
		//		 more steamapps/ directories, if any.

		halflife_dir = std::filesystem::path(path + std::string("\\steamapps\\common\\Half-Life\\"));
	}

	return halflife_dir;
}

std::filesystem::path CFileSystem::path_relative_to_hl_dir(const std::filesystem::path& relative_path)
{
	return locate_halflife_dir().string() + relative_path.string();
}

std::filesystem::path CFileSystem::get_loader_exe_filepath(const std::filesystem::path& relative_path)
{
	WCHAR path[MAX_PATH];
	GetCurrentDirectoryW(sizeof(path), path);
	return std::filesystem::path(path) / relative_path;
}

std::filesystem::path CFileSystem::get_appdata_path(const std::filesystem::path& relative_path)
{
	PWSTR pwstr_appdata_directory;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &pwstr_appdata_directory);
	assert(SUCCEEDED(result));
	std::filesystem::path ret = pwstr_appdata_directory;
	CoTaskMemFree(pwstr_appdata_directory);
	return ret / "oxware" / relative_path;
}

std::filesystem::path CFileSystem::get_relative_to_appdata(const std::filesystem::path& full_path)
{
	return full_path.lexically_relative(get_appdata_path());
}

std::filesystem::path CFileSystem::get_relative_to_appdata_ex(const std::filesystem::path& additional_path, const std::filesystem::path& full_path)
{
	return full_path.lexically_relative(get_appdata_path(additional_path));
}
