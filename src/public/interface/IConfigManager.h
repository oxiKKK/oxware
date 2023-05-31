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

	virtual bool load_configuration(ECfgType type, const std::string& filename, bool silent = false) = 0;
	virtual bool write_configuration(ECfgType type, const std::string& filename, bool silent = false) = 0;

	virtual VarInteger* get_save_cfg_interval_var() = 0;

	virtual float get_duration_last_saved_sec() const = 0;

	virtual FilePath_t get_config_directory(const FilePath_t& relative = "") = 0;

	virtual void for_each_cfg(const std::function<void(const FilePath_t& cfg_path)>& callback) = 0;

	virtual GenericConfigFile* query_config_file_type(const std::string& id) = 0;
};

extern IConfigManager* g_config_mgr_i;

#define ICONFIGMANAGER_INTERFACEID "IConfigManager"

#endif // ICONFIGMANAGER_H