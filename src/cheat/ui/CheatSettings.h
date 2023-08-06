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

#ifndef CHEATSETTINGS_H
#define CHEATSETTINGS_H
#pragma once

class CUICheatSettings
{
public:
	DECL_BASIC_CLASS(CUICheatSettings);

public:
	void render_ui();

private:
	void search_for_configs();

	void render_actions();
	void render_options_on_selected();
	void render_file_list();

	void options_load();
	void options_rename();
	void options_delete();

	void actions_export_as();
	void actions_open_config_dir();
	void actions_save_current();
	void actions_restore_defaults();
	void autosave();

	std::vector<std::filesystem::path> m_configs;

	static UIStatusWidget m_status_widget;

	std::filesystem::path m_selected_cfg = {};
};

#endif // CHEATSETTINGS_H