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

#include <nlohmann/json.hpp>

enum ECfgType
{
	CFG_Variables,
	// ...
};

static const char* cfg_type_to_string[] =
{
	/*CFG_Variables*/ "Variables",
};

class BaseCfgFile
{
public:
	BaseCfgFile(ECfgType type, const FilePath_t& full_path) :
		m_cfg_type(type),
		m_full_path(full_path)
	{
	}

	bool write()
	{
		std::ofstream ofs(m_full_path);

		if (!m_silent)
		{
			CConsole::the().info("Writing config '{}' of type {}...", get_relative_to_appdata().string(), cfg_type_to_string[m_cfg_type]);
		}

		if (!ofs.good())
		{
			CConsole::the().error("Couldn't open config file");
			return false;
		}

		if (!m_silent && g_filesystem_i->do_exist(m_full_path))
		{
			CConsole::the().warning("File already exist. An overwrite will be performed.");
		}
		
		write_configuration();

		ofs << std::setw(4) << m_json << std::endl;

		ofs.close();

		return true;
	}

	bool load()
	{
		std::ifstream ifs(m_full_path);

		if (!m_silent)
		{
			CConsole::the().info("Reading config '{}' of type {}...", get_relative_to_appdata().string(), cfg_type_to_string[m_cfg_type]);
		}

		if (!ifs.good())
		{
			CConsole::the().error("Couldn't open config file.");
			return false;
		}

		m_json = nlohmann::json::parse(ifs);

		ifs.close();

		load_configuration();

		return true;
	}

	FilePath_t get_full_path() const
	{
		return m_full_path;
	}

	FilePath_t get_relative_to_appdata() const
	{
		return g_filesystem_i->get_relative_to_appdata(m_full_path);
	}

	void be_silent(bool be) { m_silent = be; }

protected:
	// these MUST be overriden by the class that derives from this.
	virtual void load_configuration() { assert(0 && "You need to override this function! (" __FUNCTION__ ")"); };
	virtual void write_configuration() { assert(0 && "You need to override this function! (" __FUNCTION__ ")"); };

protected:
	nlohmann::json m_json;

	ECfgType m_cfg_type;

	FilePath_t m_full_path;

	// don't emit any console logs
	bool m_silent = false;
};

#endif // CONFIGFILE_H
