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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#pragma once

#include "interface/IFileSystem.h"

#include "Console.h"

#include <json.h>

enum ECfgType
{
	CFG_CheatSettings,	// under variables there're also binds
	CFG_CheatTheme,		// theme
};

static const char* cfg_type_to_string[] =
{
	/*CFG_CheatSettings*/	"Cheat settings",
	/*CFG_CheatTheme*/		"Cheat theme",
};

using fnLoadExportCallback = void(*)(nh::json& json);

class GenericConfigFile
{
public:
	GenericConfigFile() = default;
	GenericConfigFile(ECfgType type) : m_cfg_type(type)
	{
	}

	bool write(const std::filesystem::path& full_path)
	{
		std::ofstream ofs(full_path);

		if (!m_silent)
		{
			CConsole::the().info("Writing config '{}' of type '{}'...",
								 g_filesystem_i->get_relative_to_appdata(full_path),
								 cfg_type_to_string[m_cfg_type]);
		}

		if (!ofs.good())
		{
			CConsole::the().error("Couldn't open config file.");
			return false;
		}

		if (!m_silent && g_filesystem_i->do_exist(full_path))
		{
			CConsole::the().warning("File already exist. An overwrite will be performed.");
		}

		m_json.clear(); // erase first
		
		for (auto& fn : m_export_fns)
		{
			fn(m_json);
		}

		ofs << std::setw(4) << m_json << std::endl;

		ofs.close();

		if (!m_silent)
		{
			CConsole::the().info("Wrote {}.", cfg_type_to_string[m_cfg_type]);
		}

		return true;
	}

	bool load(const std::filesystem::path& full_path)
	{
		std::ifstream ifs(full_path);

		if (!m_silent)
		{
			CConsole::the().info("Reading config '{}' of type '{}'...",
								 g_filesystem_i->get_relative_to_appdata(full_path),
								 cfg_type_to_string[m_cfg_type]);
		}

		if (!ifs.good())
		{
			CConsole::the().error("Couldn't open config file.");
			return false;
		}

		try
		{
			m_json = nh::json::parse(ifs);
		}
		catch (const nh::json::exception& e)
		{
			CConsole::the().error("JSON error: {}", e.what());
		}

		ifs.close();

		for (auto& fn : m_load_fns)
		{
			fn(m_json);
		}

		if (!m_silent)
		{
			CConsole::the().info("Loaded {}.", cfg_type_to_string[m_cfg_type]);
		}

		return true;
	}

	void be_silent(bool be) { m_silent = be; }
	inline ECfgType get_type() const { return m_cfg_type; }

	void provide_load_fn(fnLoadExportCallback load_fn)
	{
		m_load_fns.push_back(load_fn);
	}

	void provide_export_fn(fnLoadExportCallback export_fn)
	{
		m_export_fns.push_back(export_fn);
	}

protected:
	nh::json m_json;

	ECfgType m_cfg_type;

	// don't emit any console logs
	bool m_silent = false;

	std::vector<fnLoadExportCallback> m_load_fns, m_export_fns;
};

//----------------------------------------------------------------
//
//
// Following classes are just syntax sugar in order to distinguish
// between configuration types.
//
//
//----------------------------------------------------------------

// cheat settings -- stores variables, binds, incommands
class CfgFile_CheatSettings : public GenericConfigFile
{
public:
	CfgFile_CheatSettings()
		: GenericConfigFile(CFG_CheatSettings)
	{
	}
};

// theme container -- stores colors
class CfgFile_CheatTheme : public GenericConfigFile
{
public:
	CfgFile_CheatTheme()
		: GenericConfigFile(CFG_CheatTheme)
	{
	}
};


#endif // CONFIGFILE_H
