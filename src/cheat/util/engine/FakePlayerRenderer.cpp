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

void CFakePlayerRenderer::render_player(hl::cl_entity_t& ent, EEntityIdentifier id)
{
	// we use this _unused_ member of cl_entity_t as our identifier.
	// depending on this identifier we then act accordingly inside our chams code.
	ent.curstate.iuser4 = id;

	render_internal(STUDIO_RENDER, ent);
}

void CFakePlayerRenderer::render_internal(int flags, hl::cl_entity_t& ent)
{
	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();

	hl::cl_entity_t** currententity = CMemoryHookMgr::the().currententity().get();

	// set current entity to our entity, since the engine will use this data in studio code.
	*currententity = &ent;

	// setup the renderer for the studio code and then render the player. we render
	// this model without engine actually knowing it, so clean everything up afterwards.
	engine_studio_api->SetupRenderer(hl::kRenderNormal);
	CMemoryFnDetourMgr::the().R_StudioDrawPlayer().call(flags, &ent.curstate);
	engine_studio_api->RestoreRenderer();
}
