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

#ifndef CLIENTMOVEMENTPACKET_H
#define CLIENTMOVEMENTPACKET_H
#pragma once

class CClientMovementPacket
{
public:
	DECL_BASIC_CLASS(CClientMovementPacket);

public:
	void update_msg_writeusercmd(hl::usercmd_t* to);
	void update_clientmove(hl::usercmd_t* cmd);

	void set_current_cmd_for_manipulation(hl::usercmd_t* cmd)
	{
		m_current_cmd = cmd;
		m_original_cmd = *cmd;
	}

	bool set_button_bit_atomic(unsigned short button);
	void set_button_bit(unsigned short button, bool set);

	bool was_in(unsigned short in_action) { return m_previous_buttons_state & in_action; }
	bool is_in(unsigned short in_action) { return m_original_cmd.buttons & in_action; }
	bool is_in_modified(unsigned short in_action) { return m_current_cmd->buttons & in_action; }

	// obviously based on IN_* buttons, not on speed or anything similar
	bool is_moving();

	void svside_movement_speed_factor(uint8_t msec, bool override_previous = false);

	hl::usercmd_t* get_cmd() { return m_current_cmd; }

private:
	hl::usercmd_t* m_current_cmd = nullptr, m_original_cmd;

	unsigned short m_previous_buttons_state = 0;

	// true if modified command frametime.
	bool m_ft_modified = false;
	int m_new_ft = -1; // -1 denotes unset

	void reset_ft_state();
};

//---------------------------------------------------------------------------------------------------

struct MPVisualDataEntry
{
	MPVisualDataEntry(float v, float m)
	{
		value = v;
		max = m;
		is_bool = false;
	}
	MPVisualDataEntry(bool v)
	{
		value = v;
		is_bool = true;
		max = 0;
	}

	float value, max;

	bool is_bool = false;	
};

struct MPVisualDataContainer
{
	std::deque<MPVisualDataEntry> entries;
	CColor color;
};

extern VarBoolean movement_plot;
extern VarInteger movement_plot_row_height;
extern VarInteger movement_plot_scale;
extern VarBoolean movement_plot_stop;

class CClientMovementPacketPlot
{
public:
	DECL_BASIC_CLASS(CClientMovementPacketPlot);

public:
	void initialize_gui();

	void on_render();

	void feed_by_name(const std::string& name, const CColor& color, const MPVisualDataEntry& data);

private:
	std::map<std::string, MPVisualDataContainer> m_data;

	// absolute zero of the entire plot
	Vector2D m_plot_base;

	float m_max_x_data = 500.0f;
};

#endif // BUNNYHOP_H