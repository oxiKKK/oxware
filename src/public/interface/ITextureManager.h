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

#ifndef ITEXTUREMANAGER_H
#define ITEXTUREMANAGER_H
#pragma once

#include <gl/GL.h>

// base texture container. contains the uploaded texture data. the uploading and unloading is done 
// by the builder class -> TextureManager.
class BaseTexture
{
public:
	BaseTexture()
	{
	}

	void create(const Vector2D& res, GLuint id)
	{
		m_resolution = res;
		m_texture_id = id;
	}

	inline auto get_resolution() const { return m_resolution; }
	inline GLuint get_gl_id() const { return m_texture_id; }

	inline bool is_loaded() const
	{
		return m_texture_id != 0 && m_resolution.IsZero() == false;
	}

private:
	Vector2D m_resolution;
	GLuint m_texture_id;
};

class ITextureManager : public IBaseInterface
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	// creates and uploads new texture to the gpu. on success, a pointer to the texture object is returned. you can register texture either from
	// 1# an image file buffer (raw JPG, PNG, BMP file contents) or 2# using raw pixel data of the image.
	virtual const BaseTexture& create_and_register_new_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length) = 0;
	virtual const BaseTexture& create_and_register_new_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution) = 0;

	// destroys the original texture and then re-uploads it back to the gpu and registers it to the list. on the upload front, the function
	// behaves the same as create_and_register_new_texture_*. the texture not being registered isn't determined as an error.
	virtual const BaseTexture& reupload_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length) = 0;
	virtual const BaseTexture& reupload_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution) = 0;

	// destroy and remove the texture from the list and gpu. returns false if the texture wasn't registered.
	virtual bool destroy_and_unregister_texture(const char* identifier) = 0;

	// returns checkerboard texture. this one is always loaded after texture manager is initialized and is never unloaded.
	virtual const BaseTexture& get_invalid_texture() = 0;

	// get already registered texture. if no such texture is found, invalid "checkerboard" texture is returned.
	virtual const BaseTexture& get_texture(const char* identifier) = 0;
};

extern ITextureManager* g_texture_manager_i;

#define ITEXTUREMANAGER_INTERFACEID "ITextureManager"

#endif // ITEXTUREMANAGER_H