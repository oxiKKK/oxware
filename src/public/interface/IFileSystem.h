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

#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H
#pragma once

#include "commondefs.h"

#include <filesystem>
#include <fstream>
#include <functional>

enum class ECopyOptions
{
	none = 0, 
	skip_existing = BIT(0),			// keep the existing file
	overwrite_existing = BIT(1),	// replace the existing file
	recursive = BIT(2),				// recursive directory search
	only_directories = BIT(3),		// only copy folders, not files
};
DEFINE_ENUM_BITWISE_OPERATORS(ECopyOptions);

class IFileSystem : public IBaseInterface
{
public:
	//
	// File system operations
	//

	virtual bool is_directory(const std::filesystem::path& path) = 0;
	virtual bool is_file(const std::filesystem::path& path) = 0;
	virtual bool do_exist(const std::filesystem::path& path) = 0;

	virtual uintmax_t file_size(const std::filesystem::path& path) = 0;
	virtual uintmax_t directory_size(const std::filesystem::path& path) = 0;

	virtual bool copy(const std::filesystem::path& from, const std::filesystem::path& to, ECopyOptions options = ECopyOptions::none) = 0;
	virtual bool rename(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
	virtual bool move(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
	virtual bool replace(const std::filesystem::path& what, const std::filesystem::path& with) = 0;
	virtual bool remove(const std::filesystem::path& path) = 0;
	virtual bool remove_all(const std::filesystem::path& directory) = 0;
	virtual bool create_file(const std::filesystem::path& path) = 0;
	virtual bool create_directory(const std::filesystem::path& path) = 0;

	virtual void iterate_through_files(const std::filesystem::path& directory, bool recursive, const std::function<void(const std::filesystem::path& file)>& callback) = 0;

	virtual std::filesystem::path locate_halflife_dir() = 0;
	virtual std::filesystem::path path_relative_to_hl_dir(const std::filesystem::path& relative_path = "") = 0;
	virtual std::filesystem::path get_loader_exe_filepath(const std::filesystem::path& relative_path = "") = 0;
	virtual std::filesystem::path get_appdata_path(const std::filesystem::path& relative_path = "") = 0;
	virtual std::filesystem::path get_relative_to_appdata(const std::filesystem::path& full_path) = 0;
	virtual std::filesystem::path get_relative_to_appdata_ex(const std::filesystem::path& additional_path, const std::filesystem::path& full_path) = 0;

	virtual std::error_code get_last_error() const = 0;
};

extern IFileSystem* g_filesystem_i;

#define IFILESYSTEM_INTERFACEID "IFileSystem"

#endif // IFILESYSTEM_H