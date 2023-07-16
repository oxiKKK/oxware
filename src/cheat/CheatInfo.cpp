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

ICheatInfo* g_cheat_info_i = nullptr;

class CCheatInfo : public ICheatInfo
{
public:
	CCheatInfo();
	~CCheatInfo();

	bool is_cheat_ui_running();

	bool is_game_ui_running();

	bool is_in_messagemode();
};

EXPOSE_SINGLE_INTERFACE(CCheatInfo, ICheatInfo, ICHEATINFO_INTERFACEID);

//-------------------------------------------------------------------------------
//
// CCheatInfo implementation
//
//-------------------------------------------------------------------------------

CCheatInfo::CCheatInfo()
{
	g_cheat_info_i = this;
}

CCheatInfo::~CCheatInfo()
{
	g_cheat_info_i = nullptr;
}

bool CCheatInfo::is_cheat_ui_running()
{
	return COxWareUI::the().is_any_ui_rendering();
}

bool CCheatInfo::is_game_ui_running()
{
	return CEngineInput::the().is_gameui_rendering();
}

bool CCheatInfo::is_in_messagemode()
{
	return CLocalState::the().is_in_messagemode();
}
