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

#ifndef CVARSANDBOX_H
#define CVARSANDBOX_H
#pragma once

extern VarBoolean cvar_sandbox_enable;
extern VarBoolean cvar_sandbox_monitor_server;

extern VarKeyValue cvar_sandbox_fps_max;
extern VarKeyValue cvar_sandbox_fps_override;

extern VarKeyValue fps_max;
extern VarKeyValue fps_override;
extern VarKeyValue gl_vsync;
extern VarKeyValue developer;

// movement
extern VarKeyValue cl_movespeedkey;
extern VarKeyValue cl_forwardspeed;
extern VarKeyValue cl_sidespeed;
extern VarKeyValue cl_upspeed;
extern VarKeyValue cl_yawspeed;
extern VarKeyValue cl_pitchspeed;

// prediction
extern VarKeyValue cl_lc;
extern VarKeyValue cl_lw;

// other
extern VarKeyValue cl_minmodels;

// networking
extern VarKeyValue cl_cmdrate;

class CCvarSandbox
{
public:
	DECL_BASIC_CLASS(CCvarSandbox);

public:
	// returns fake cvar value
	const char* filter_cvarvalue(const char* cvar_name);

	void render_ui();

private:
	void initialize();

	void render_cvar_sandbox_slot(VarKeyValue* var);

	// ik that this seems really dumb and out of place however, I don't really know how to properly
	// work with input text buffers... xD
	inline static constexpr size_t k_max_filtered_cvars = 32;
	std::unordered_map<std::string, size_t> m_filtered_cvars;
	char m_input_buffers[k_max_filtered_cvars][256];

	bool keep_status_alive() const { return (GetTickCount() - m_status_update_ms) < 5000; }

	uint32_t m_status_update_ms;
	std::string m_current_status;

	std::vector<VarKeyValue*> m_variables;
};

#endif // CVARSANDBOX_H