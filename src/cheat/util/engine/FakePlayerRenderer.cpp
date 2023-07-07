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

void CFakePlayerRenderer::schedule_entity(hl::cl_entity_t& ent, EEntityIdentifier id)
{
	// we use this _unused_ member of cl_entity_t as our identifier.
	// depending on this identifier we then act accordingly inside our chams code.
	ent.curstate.iuser4 = id;

	m_entities.push_back(ent);
}

void CFakePlayerRenderer::create_entities()
{
	for (auto& ent : m_entities)
	{
		CMemoryHookMgr::the().cl_enginefuncs()->pfnCL_CreateVisibleEntity(ET_PLAYER, &ent);
	}

	flush();
}

void CFakePlayerRenderer::flush()
{
	// clear the list after we render them
	m_entities.clear();
}
