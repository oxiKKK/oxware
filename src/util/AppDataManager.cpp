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

IAppDataManager* g_appdata_mgr_i = nullptr;

class CAppDataManager : public IAppDataManager
{
public:
	CAppDataManager();
	~CAppDataManager();

	bool initialize_directory_hirearchy();

	bool register_and_create_directory(const std::string& relative_dir);
	std::filesystem::path get_known(const std::string& relative_dir);

private:
	inline std::filesystem::path get_full(const std::filesystem::path& relative_dir = "") const { return g_filesystem_i->get_appdata_path(relative_dir); }

public:
	// Holds relative directories as keys, absolute directories as values.
	std::map<std::string, std::filesystem::path> m_directories;
};

CAppDataManager g_appdata_mgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CAppDataManager, IAppDataManager, IAPPDATAMANAGER_INTERFACEID, g_appdata_mgr);

//-------------------------------------------------------------------------------
//
// CAppDataManager implementation
//
//-------------------------------------------------------------------------------

CAppDataManager::CAppDataManager()
{
	g_appdata_mgr_i = this;
}

CAppDataManager::~CAppDataManager()
{
	g_appdata_mgr_i = nullptr;
}

bool CAppDataManager::initialize_directory_hirearchy()
{
	// create base
	if (!g_filesystem_i->create_directory(g_filesystem_i->get_appdata_path()))
	{
		CConsole::the().error("Couldn't create base directory for the cheat. ('{}')",
							  g_filesystem_i->get_last_error().message());
		return false;
	}

	register_and_create_directory("config\\");		/* oxware\\config\\			*/
	register_and_create_directory("config\\theme");	/* oxware\\config\\theme\\	*/
	register_and_create_directory("log\\");			/* oxware\\log\\			*/
	// add more here if you wish to...

	CConsole::the().info("Created directory hirearchy.");
	return true;
}

bool CAppDataManager::register_and_create_directory(const std::string& relative_dir)
{
	m_directories[relative_dir] = get_full(relative_dir);

	if (!g_filesystem_i->create_directory(m_directories[relative_dir]))
	{
		CConsole::the().error("Failed to create a directory: '{}' ('{}')",
							  m_directories[relative_dir].string(), g_filesystem_i->get_last_error().message());
		return false;
	}

	return true;
}

std::filesystem::path CAppDataManager::get_known(const std::string& relative_dir)
{
	return m_directories[relative_dir];
}
