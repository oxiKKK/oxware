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

#ifndef STUFFCMDFILTER_H
#define STUFFCMDFILTER_H
#pragma once

extern VarBoolean cmdfilter_enable;
extern VarBoolean cmdfilter_print_blocked;
extern VarBoolean cmdfilter_print_every;
extern VarBoolean cmdfilter_filter_all;
extern VarString cmdfilter_filtered_commands;

class CStuffCmdFilter
{
public:
	DECL_BASIC_CLASS(CStuffCmdFilter);

public:
	bool allow_command_to_be_executed();

	void render_ui();

private:
	bool is_buffer_valid(const char* buffer);

	bool approve_server_cmdlist(const std::string& server_cmd);

	std::vector<std::string> m_filtered_cmds_tokenized;

	// command that caused the block
	std::string m_block_victim;

	// ui
	static UIStatusWidget m_status_widget;
};

#endif // STUFFCMDFILTER_H