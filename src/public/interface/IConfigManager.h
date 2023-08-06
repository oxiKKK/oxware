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

#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H
#pragma once

// use this base class on classes that need to provide load&write callbacks.
// this is not needed, but it's just a common convention that is shared between interfaces that implements this.
class IConfigIOOperations
{
public:
	virtual ~IConfigIOOperations() { }

	// provide a load&export callback to the config file that is associated with this interface.
	virtual void provide_cfg_load_export_callbacks() = 0;
};

#include "interface.h"

#include "ConfigFile.h"

class VarInteger;

class IConfigManager : public IBaseInterface
{
public:
	virtual bool initialize() = 0;
	virtual void shutdown() = 0;

	// should by called by only one thread.
	virtual void update() = 0;

	// load and write configuration by type from/to the config/ directory.
	virtual bool load_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent = false) = 0;
	virtual bool write_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent = false) = 0;

	// returns the variable that controls the cheat settings config interval.
	virtual VarInteger* get_save_cfg_interval_var() = 0;

	// returns amount of seconds since the cheat settings config was last saved.
	virtual float get_duration_last_saved_sec() const = 0;

	// returns "%appdata%\\oxware\\config\\" + the relative path/filename specified.
	virtual std::filesystem::path get_config_directory(const std::filesystem::path& relative = "") = 0;

	// iterate through all configs inside the config\\ directory or additional subdirectory, if specified
	// function only goes through files with ".json" extension.
	virtual void for_each_cfg(const std::function<void(const std::filesystem::path& relative_cfg_path)>& callback, 
							  const std::filesystem::path& subdirectory = "") = 0;

	// get config file by id.
	virtual std::optional<GenericConfigFile*> query_config_file_type(const std::string& id) = 0;

	// replaces the extension with ".json" if the extension is missing or is not json.
	virtual void fix_config_file_extension(std::filesystem::path& path) = 0;

	// provide callback that will be ran each config save interval.
	virtual void provide_on_save_callback(const std::function<void()>& callback) = 0;
};

extern IConfigManager* g_config_mgr_i;

#define ICONFIGMANAGER_INTERFACEID "IConfigManager"

#endif // ICONFIGMANAGER_H