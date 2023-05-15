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

VarInteger save_cfg_interval_sec("save_cfg_interval_sec", "Iterval in seconds for how often current settings should be saved.", 30, 10, 60);

BaseCommand export_config(
	"export_config", "<config name>", "Exports a json config file",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 2)
		{
			cmd->print_usage();
			return;
		}

		if (g_config_mgr_i->write_configuration(CFG_Variables, args.get(1)))
		{
			CConsole::the().info("Exported config '{}'.", args.get(1));
		}
	}
);

BaseCommand load_config(
	"load_config", "<config name>", "Loads a json config file",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 2)
		{
			cmd->print_usage();
			return;
		}

		if (g_config_mgr_i->load_configuration(CFG_Variables, args.get(1)))
		{
			CConsole::the().info("Loaded config '{}'.", args.get(1));
		}
	}
);

IConfigManager* g_config_mgr_i = nullptr;

class CConfigManager : public IConfigManager
{
public:
	CConfigManager();
	~CConfigManager();

	bool initialize();
	void shutdown();

	// should by called by only one thread.
	void update();

	bool load_configuration(ECfgType type, const std::string& filename, bool silent = false);
	bool write_configuration(ECfgType type, const std::string& filename, bool silent = false);

	VarInteger* get_save_cfg_interval_var() { return &save_cfg_interval_sec; }

	float get_duration_last_saved_sec() const { return std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - m_last_saved).count(); }

	FilePath_t get_config_directory(const FilePath_t& relative = "");

	void for_each_cfg(const std::function<void(const FilePath_t& cfg_path)>& callback);

private:
	std::chrono::high_resolution_clock::time_point m_last_saved;
};

CConfigManager g_config_mgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CConfigManager, IConfigManager, ICONFIGMANAGER_INTERFACEID, g_config_mgr);

//-------------------------------------------------------------------------------
//
// CConfigManager implementation
//
//-------------------------------------------------------------------------------

CConfigManager::CConfigManager()
{
	g_config_mgr_i = this;
}

CConfigManager::~CConfigManager()
{
	g_config_mgr_i = nullptr;
}

bool CConfigManager::initialize()
{
	// default config is the one that gets filled with default values each time we start our cheat
	// saved config is the one that holds current configuration.
	write_configuration(CFG_Variables, "default.json");
	load_configuration(CFG_Variables, "saved.json");

	m_last_saved = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Config Manager initialized");
	return true;
}

void CConfigManager::shutdown()
{
	// write changed settings
	write_configuration(CFG_Variables, "saved.json");
}

void CConfigManager::update()
{
	if (get_duration_last_saved_sec() > save_cfg_interval_sec.get_value())
	{
		write_configuration(CFG_Variables, "saved.json", true);
		m_last_saved = std::chrono::high_resolution_clock::now();
	}
}

bool CConfigManager::load_configuration(ECfgType type, const std::string& filename, bool silent)
{
	auto full_path = get_config_directory(filename);

	if (!full_path.has_extension() || full_path.extension() != ".json")
	{
		full_path.replace_extension("json");
	}

	bool success = false;

	switch (type)
	{
		case CFG_Variables:
		{
			CfgFileVariables cfg(full_path);
			cfg.be_silent(silent);
			success = cfg.load();
			break;
		}
		default:
		{
			assert(0 && "Called " __FUNCTION__ " with invalid config type!");
			break;
		}
	}

	return success;
}

bool CConfigManager::write_configuration(ECfgType type, const std::string& filename, bool silent)
{
	auto full_path = get_config_directory(filename);

	if (!full_path.has_extension() || full_path.extension() != ".json")
	{
		full_path.replace_extension("json");
	}

	bool success = false;

	switch (type)
	{
		case CFG_Variables:
		{
			CfgFileVariables cfg(full_path);
			cfg.be_silent(silent);
			success = cfg.write();
			break;
		}
		default:
		{
			assert(0 && "Called " __FUNCTION__ " with invalid config type!");
			break;
		}
	}

	return success;
}

FilePath_t CConfigManager::get_config_directory(const FilePath_t& relative)
{
	return g_appdata_mgr_i->get_known("config\\") / relative;
}

void CConfigManager::for_each_cfg(const std::function<void(const FilePath_t& cfg_path)>& callback)
{
	g_filesystem_i->iterate_through_files(
		get_config_directory(), true, 
		[&callback](const FilePath_t& filepath)
		{
			if (filepath.extension() == ".json")
			{
				callback(filepath);
			}
		}
	);
}

