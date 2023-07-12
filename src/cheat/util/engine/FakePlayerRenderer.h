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

#ifndef FAKEPLAYERRENDERER_H
#define FAKEPLAYERRENDERER_H
#pragma once

// useful when creating an entity
enum EEntityIdentifier
{
	ENT_ID_NONE = 0x00000000,
	ENT_ID_AA = 0xC0000000,
	ENT_ID_BACKTRACK = 0x0C000000,
};

class CFakePlayerRenderer
{
public:
	DECL_BASIC_CLASS(CFakePlayerRenderer);
	
public:
	// schedule entity for later rendering
	void render_player(hl::cl_entity_t& ent, EEntityIdentifier id);

private:
	void render_internal(int flags, hl::cl_entity_t& ent);
};

#endif // FAKEPLAYERRENDERER_H