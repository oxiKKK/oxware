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

//------------------------------------------------------------------------------------------------------
// Texture loaders
// 

class BaseTextureLoaderFromFile
{
public:
	virtual bool load_texture_and_upload_to_gpu(BaseTexture& texture_to_be_loaded, uint8_t* file_data, uint32_t length) = 0;
};

class BaseTextureLoaderFromPixelBuffer
{
public:
	virtual bool load_texture_and_upload_to_gpu(BaseTexture& texture_to_be_loaded, uint8_t* pixel_data, const Vector2D& resolution) = 0;
};

// abstract builder class
template<typename T>
class TextureLoader : public virtual T
{
protected:
	static GLuint upload_texture_to_gpu(const Vector2D& texture_resolution, uint8_t* texture_pixel_data)
	{
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

		GLuint new_texture_id;
		glGenTextures(1, &new_texture_id);
		glBindTexture(GL_TEXTURE_2D, new_texture_id);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)texture_resolution.x, (GLsizei)texture_resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_pixel_data);

		glBindTexture(GL_TEXTURE_2D, last_texture);

		return new_texture_id;
	}
};

// texture loader implementation from a file buffer
class TextureLoaderFromFileData : public virtual TextureLoader<BaseTextureLoaderFromFile>
{
public:
	bool load_texture_and_upload_to_gpu(BaseTexture& texture_to_be_loaded, uint8_t* file_data, uint32_t length) override
	{
		assert(file_data);
		assert(length);

		int width, height;
		uint8_t* image_pixel_data = SOIL_load_image_from_memory(file_data, length, &width, &height, NULL, SOIL_LOAD_RGBA);

		if (!image_pixel_data)
		{
			CConsole::the().error("SOIL error: {}", SOIL_last_result());
			return false;
		}

		Vector2D image_resolution = { (float)width, (float)height };

		GLuint new_texture_id = upload_texture_to_gpu(image_resolution, image_pixel_data);

		// free soil buffer
		SOIL_free_image_data(image_pixel_data);

		// fill texture data
		texture_to_be_loaded.create(image_resolution, new_texture_id);
		return true;
	}
};

// texture loader implementation from a raw pixel data
class TextureLoaderFromPixelData : public virtual TextureLoader<BaseTextureLoaderFromPixelBuffer>
{
public:
	bool load_texture_and_upload_to_gpu(BaseTexture& texture_to_be_loaded, uint8_t* pixel_data, const Vector2D& resolution) override
	{
		assert(pixel_data);

		GLuint new_texture_id = upload_texture_to_gpu(resolution, pixel_data);

		// fill texture data
		texture_to_be_loaded.create(resolution, new_texture_id);
		return true;
	}
};

//------------------------------------------------------------------------------------------------------

ITextureManager* g_texture_manager_i = nullptr;

class CTextureManager : public ITextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	void initialize();
	void shutdown();

	const BaseTexture& create_and_register_new_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length);
	const BaseTexture& create_and_register_new_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution);
	bool destroy_and_unregister_texture(const char* identifier);

	const BaseTexture& reupload_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length);
	const BaseTexture& reupload_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution);

	const BaseTexture& get_invalid_texture();

	const BaseTexture& get_texture(const char* identifier);

private:
	void on_texture_register_success(const char* identifier, BaseTexture& new_texture);
	void on_texture_register_failure(const char* identifier);

	void release_texture_from_gpu(BaseTexture& texture);

private:
	std::unordered_map<std::string, BaseTexture> m_precached_textures;

	TextureLoaderFromFileData* m_filedata_texture_loader;
	TextureLoaderFromPixelData* m_pixeldata_texture_loader;
};

CTextureManager g_texture_manager;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CTextureManager, ITextureManager, ITEXTUREMANAGER_INTERFACEID, g_texture_manager);

//-------------------------------------------------------------------------------
//
// CTextureManager implementation
//
//-------------------------------------------------------------------------------

CTextureManager::CTextureManager()
{
	g_texture_manager_i = this;
}

CTextureManager::~CTextureManager()
{
	g_texture_manager_i = nullptr;
}

void CTextureManager::initialize()
{
	// texture loaders
	m_filedata_texture_loader = new TextureLoaderFromFileData();
	m_pixeldata_texture_loader = new TextureLoaderFromPixelData();

	create_and_register_new_texture_from_filedata("invalid", g_checkerboard, g_checkerboard_size);

	CConsole::the().info("TextureManager initialized. Registered {} textures.", m_precached_textures.size());
}

void CTextureManager::shutdown()
{
	delete m_filedata_texture_loader;
	delete m_pixeldata_texture_loader;

	m_precached_textures.clear();

	CConsole::the().info("Shutting down TextureManager...");
}

const BaseTexture& CTextureManager::create_and_register_new_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length)
{
	// create the texture here by using the array operator
	auto& new_texture = m_precached_textures[identifier];

	// upload the texture data to gpu, and if we fail, use the checkerboard texture as invalid texture.
	if (!m_filedata_texture_loader->load_texture_and_upload_to_gpu(new_texture, file_data, length))
	{
		on_texture_register_failure(identifier);

		return get_invalid_texture();
	}

	on_texture_register_success(identifier, new_texture);

	return new_texture;
}

const BaseTexture& CTextureManager::create_and_register_new_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution)
{
	// create the texture here by using the array operator
	auto& new_texture = m_precached_textures[identifier];

	// upload the texture data to gpu, and if we fail, use the checkerboard texture as invalid texture.
	if (!m_pixeldata_texture_loader->load_texture_and_upload_to_gpu(new_texture, pixel_data, resolution))
	{
		on_texture_register_failure(identifier);

		return get_invalid_texture();
	}

	on_texture_register_success(identifier, new_texture);

	return new_texture;
}

bool CTextureManager::destroy_and_unregister_texture(const char* identifier)
{
	if (m_precached_textures.find(identifier) == m_precached_textures.end())
	{
		CConsole::the().error("Cannot unregister non-existent texture '{}'.", identifier);
		return false;
	}

	// unload from gpu
	release_texture_from_gpu(m_precached_textures[identifier]);

	m_precached_textures.erase(identifier);

	CConsole::the().info("Unregistered texture: {}", identifier);
	return true;
}

const BaseTexture& CTextureManager::reupload_texture_from_filedata(const char* identifier, uint8_t* file_data, uint32_t length)
{
	// destroy if it exists, if not, proceed.
	if (!destroy_and_unregister_texture(identifier))
	{
//		CConsole::the().dwarning("Couldn't destroy texture '{}' when re-uploading.", identifier);
	}

	return create_and_register_new_texture_from_filedata(identifier, file_data, length);
}

const BaseTexture& CTextureManager::reupload_texture_from_pixeldata(const char* identifier, uint8_t* pixel_data, const Vector2D& resolution)
{
	// destroy if it exists, if not, proceed.
	if (!destroy_and_unregister_texture(identifier))
	{
//		CConsole::the().dwarning("Couldn't destroy texture '{}' when re-uploading.", identifier);
	}

	return create_and_register_new_texture_from_pixeldata(identifier, pixel_data, resolution);
}

const BaseTexture& CTextureManager::get_invalid_texture()
{
	return m_precached_textures["invalid"];
}

const BaseTexture& CTextureManager::get_texture(const char* identifier)
{
	try
	{
		return m_precached_textures.at(identifier);
	}
	catch (...)
	{
		CConsole::the().derror("Texture not found: '{}'", identifier);
		return get_invalid_texture();
	}
}

void CTextureManager::on_texture_register_failure(const char * identifier)
{
	CConsole::the().error("Couldn't register texture: '{}'", identifier);
}

void CTextureManager::release_texture_from_gpu(BaseTexture& texture)
{
	auto id = texture.get_gl_id();

	if (!id)
	{
		return;
	}

	glDeleteTextures(1, &id);
}

void CTextureManager::on_texture_register_success(const char* identifier, BaseTexture& new_texture)
{
	CConsole::the().info("Registered texture: '{}', {}x{}, GL id {}",
						 identifier,
						 new_texture.get_resolution().x, new_texture.get_resolution().y,
						 new_texture.get_gl_id());
}
