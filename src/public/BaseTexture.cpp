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

#include "platform.h"

#include <soil/soil.h>

#include "BaseTexture.h"
#include "Console.h"

bool BaseTexture::load()
{
	int width, height;
	uint8_t* ptr_loaded_texture = SOIL_load_image_from_memory(m_texture_filedata.first, m_texture_filedata.second, &width, &height, NULL, SOIL_LOAD_RGBA);

	if (!ptr_loaded_texture)
	{
		CConsole::the().error("SOIL error: {}", SOIL_last_result());
		return false;
	}

	m_resolution = { (float)width, (float)height };

	// upload texture to opengl
	upload_to_gpu(ptr_loaded_texture);

	SOIL_free_image_data(ptr_loaded_texture);

	return true;
}

void BaseTexture::unload()
{
	if (m_texture_id)
	{
		glDeleteTextures(1, &m_texture_id);
	}
}

void BaseTexture::upload_to_gpu(uint8_t* ptr_loaded_texture)
{
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_resolution.x, (GLsizei)m_resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr_loaded_texture);

	glBindTexture(GL_TEXTURE_2D, last_texture);
}
