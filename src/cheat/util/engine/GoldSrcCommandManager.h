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

#ifndef GOLDSRCCOMMANDMANAGER_H
#define GOLDSRCCOMMANDMANAGER_H
#pragma once

class CGoldSrcCommandMgr
{
public:
	DECL_BASIC_CLASS(CGoldSrcCommandMgr);

public:
	void initialize();
	void shutdown();

	hl::cvar_t* get_cvar(const std::string& name);
	hl::cmd_function_t* get_cmd(const std::string& name);

	void register_cvar(const std::string& name, hl::cvar_t* cvar);
	void register_cmd(const std::string& name, hl::cmd_function_t* cmd);

	void for_each_cvar(const std::function<void(hl::cvar_t* cvar)>& callback);
	void for_each_cmd(const std::function<void(hl::cmd_function_t* cvar)>& callback);

	// goes through all tokens inside a tokenized buffer and checks whenever the token coresponds to a cvar or a command.
	std::vector<std::string> find_invalid_tokens_in_tokenized_buffer(const std::vector<std::string>& tokenized_buffer);
	
private:
	std::unordered_map<std::string, hl::cvar_t*> m_cvars;
	std::unordered_map<std::string, hl::cmd_function_t*> m_commands;
};

#endif // GOLDSRCCOMMANDMANAGER_H