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

#ifndef REMOVALS_H
#define REMOVALS_H
#pragma once

extern VarBoolean remove_screenshake;
extern VarBoolean remove_viewmodel;

extern VarInteger remove_players_team;

extern VarBoolean remove_hud_enable;
extern VarBoolean remove_hud_crosshair;
extern VarBoolean remove_hud_weapons;
extern VarBoolean remove_hud_flashlight;
extern VarBoolean remove_hud_health;
extern VarBoolean remove_hud_timer;
extern VarBoolean remove_hud_money;

extern VarBoolean remove_motd;

extern VarBoolean remove_sniper_scope;

class CRemovals
{
public:
	DECL_BASIC_CLASS(CRemovals);

public:
	bool remove_screenshake();
	bool remove_viewmodel();
	bool remove_player(int id);

	void remove_hud_modifier();

	bool remove_motd();

	bool remove_sniper_scope();
};

#endif // REMOVALS_H