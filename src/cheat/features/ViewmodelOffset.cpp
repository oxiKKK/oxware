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

VarBoolean viewmodel_offset_enable("viewmodel_offset_enable", "Enables shifted viewmodel", false);
VarFloat viewmodel_offset_value("viewmodel_offset_value", "Shifts viewmodel back from the camera origin", 0.0f, -5.0f, 10.0f);

void CViewmodelOffset::update()
{
	if (!viewmodel_offset_enable.get_value())
	{
		return;
	}

	auto local = CLocalState::the().local_player();
	if (!local)
		return;

	if (CGameUtil::the().is_spectator())
	{
		return;
	}

	auto cl = CMemoryHookMgr::the().cl().get();
	auto vm = &cl->viewent;

	float offset = viewmodel_offset_value.get_value();

	Vector forward;
	CMath::the().angle_vectors(cl->viewangles, &forward, nullptr, nullptr);

	vm->origin += forward * offset;
}
