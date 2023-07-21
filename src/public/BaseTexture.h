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

#ifndef BASETEXTURE_H
#define BASETEXTURE_H
#pragma once

#include <gl/GL.h>

class BaseTexture
{
public:
	BaseTexture()
	{
	}

	BaseTexture(const uint8_t* texture_filedata, uint32_t length)
	{
		assert(texture_filedata);

		m_texture_filedata = std::make_pair(texture_filedata, length);
	}

	// just in case that we don't unload it..
	~BaseTexture()
	{
		unload();
	}

	// load and upload from gpu
	bool load();
	void unload();

	inline auto get_resolution() { return m_resolution; }
	inline GLuint get_gl_id() { return m_texture_id; }

private:
	void upload_to_gpu(uint8_t* ptr_loaded_texture);

private:
	Vector2D m_resolution;
	GLuint m_texture_id;

	std::pair<const uint8_t*, uint32_t> m_texture_filedata;
};

#endif // BASETEXTURE_H