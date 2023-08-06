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

VarInteger save_cfg_interval_sec("save_cfg_interval_sec", "Iterval in seconds for how often current settings should be saved.", 30, 9, 60);

// writes a cheat settings config file from the configs/ directory.
BaseCommand export_config(
	"export_config", "<config name>", "Exports a json config file",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 2)
		{
			cmd->print_usage();
			return;
		}

		if (g_config_mgr_i->write_configuration(CFG_CheatSettings, args.get(1)))
		{
			CConsole::the().info("Exported config '{}'.", args.get(1));
		}
	}
);

// loads a cheat settings config file from the configs/ directory.
BaseCommand load_config(
	"load_config", "<config name>", "Loads a json config file",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 2)
		{
			cmd->print_usage();
			return;
		}

		if (g_config_mgr_i->load_configuration(CFG_CheatSettings, args.get(1)))
		{
			CConsole::the().info("Loaded config '{}'.", args.get(1));
		}
	}
);

//-------------------------------------------------------------------------------------------------

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

	bool load_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent = false);
	bool write_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent = false);

	VarInteger* get_save_cfg_interval_var();

	float get_duration_last_saved_sec() const;

	std::filesystem::path get_config_directory(const std::filesystem::path& relative = "");

	void for_each_cfg(const std::function<void(const std::filesystem::path& relative_cfg_path)>& callback, 
					  const std::filesystem::path& subdirectory);

	std::optional<GenericConfigFile*> query_config_file_type(const std::string& id);

	void fix_config_file_extension(std::filesystem::path& path);

	void provide_on_save_callback(const std::function<void()>& callback);

private:
	std::chrono::high_resolution_clock::time_point m_last_saved;

	CfgFile_CheatSettings m_cheat_settings_cfg;
	CfgFile_CheatTheme m_cheat_theme_cfg;

	// just to access it through query from external code.
	std::unordered_map<std::string, GenericConfigFile*> m_registered_config_types;

	void init_config_types()
	{
		// NOTE: that this is called in ctor.

		m_registered_config_types["cheat_settings"] = &m_cheat_settings_cfg;
		m_registered_config_types["cheat_theme"] = &m_cheat_theme_cfg;

		// DON'T FORGET TO ADD NEW ONES AS YOU ADD THEM AS CLASS MEMBERS.
	}

	std::vector<std::function<void()>> m_on_save_callbacks;
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

	init_config_types();
}

CConfigManager::~CConfigManager()
{
	g_config_mgr_i = nullptr;
}

bool CConfigManager::initialize()
{
	// default config is the one that gets filled with default values each time we start our cheat
	// saved config is the one that holds current configuration.
	write_configuration(CFG_CheatSettings, "default.json");
	load_configuration(CFG_CheatSettings, "saved.json");

	m_last_saved = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Config Manager initialized");
	return true;
}

void CConfigManager::shutdown()
{
	CConsole::the().info("Shutting down Config Manager...");

	// write changed settings
	write_configuration(CFG_CheatSettings, "saved.json");

	for (auto& callback : m_on_save_callbacks)
	{
		callback();
	}
}

void CConfigManager::update()
{
	// see if enabled
	if (save_cfg_interval_sec.get_value() == save_cfg_interval_sec.get_min())
	{
		return;
	}

	if (get_duration_last_saved_sec() > save_cfg_interval_sec.get_value())
	{
		write_configuration(CFG_CheatSettings, "saved.json", true);

		// execute on-save callbacks.
		for (auto& callback : m_on_save_callbacks)
		{
			callback();
		}

		m_last_saved = std::chrono::high_resolution_clock::now();
	}
}

bool CConfigManager::load_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent)
{
	auto full_path = get_config_directory(relative_cfg_path);
	fix_config_file_extension(full_path);

	bool success = false;
	switch (type)
	{
		case CFG_CheatSettings:
		{
			m_cheat_settings_cfg.be_silent(silent);
			success = m_cheat_settings_cfg.load(full_path);
			break;
		}
		case CFG_CheatTheme:
		{
			m_cheat_theme_cfg.be_silent(silent);
			success = m_cheat_theme_cfg.load(full_path);
			break;
		}
		default:
		{
			assert(0 && "Called CConfigManager::load_configuration with invalid config type!");
			break;
		}
	}

	return success;
}

bool CConfigManager::write_configuration(ECfgType type, const std::string& relative_cfg_path, bool silent)
{
	auto full_path = get_config_directory(relative_cfg_path);
	fix_config_file_extension(full_path);

	bool success = false;
	switch (type)
	{
		case CFG_CheatSettings:
		{
			m_cheat_settings_cfg.be_silent(silent);
			success = m_cheat_settings_cfg.write(full_path);
			break;
		}
		case CFG_CheatTheme:
		{
			m_cheat_theme_cfg.be_silent(silent);
			success = m_cheat_theme_cfg.write(full_path);
			break;
		}
		default:
		{
			assert(0 && "Called CConfigManager::write_configuration with invalid config type!");
			break;
		}
	}

	return success;
}

VarInteger* CConfigManager::get_save_cfg_interval_var()
{
	return &save_cfg_interval_sec;
}

float CConfigManager::get_duration_last_saved_sec() const
{
	return std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - m_last_saved).count();
}

std::filesystem::path CConfigManager::get_config_directory(const std::filesystem::path& relative)
{
	return g_appdata_mgr_i->get_known("config\\") / relative;
}

void CConfigManager::for_each_cfg(
	const std::function<void(const std::filesystem::path& relative_cfg_path)>& callback, 
	const std::filesystem::path& subdirectory)
{
	g_filesystem_i->iterate_through_files(
		get_config_directory() / subdirectory, false,
		[&](const std::filesystem::path& filepath)
		{
			if (filepath.extension() == ".json")
			{
				callback(filepath);
			}
		}
	);
}

std::optional<GenericConfigFile*> CConfigManager::query_config_file_type(const std::string& id)
{
	try
	{
		return m_registered_config_types.at(id);
	}
	catch (...)
	{
		assert(0 && "Failed config type query. Did you add your config type to the init_config_types() function?");
		return std::nullopt;
	}
}

void CConfigManager::fix_config_file_extension(std::filesystem::path& path)
{
	if (!path.has_extension() || path.extension() != ".json")
	{
		path.replace_extension("json");
	}
}

void CConfigManager::provide_on_save_callback(const std::function<void()>& callback)
{
	m_on_save_callbacks.push_back(callback);
}

