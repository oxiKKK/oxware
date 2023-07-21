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

ITextureManager* g_texture_manager_i = nullptr;

class CTextureManager : public ITextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	void initialize();
	void shutdown();

	BaseTexture* register_texture(const char* identifier, const uint8_t* texture_filedata, uint32_t length);
	void unregister_texture(const char* identifier);

	BaseTexture* get_texture(const char* identifier);

private:
	std::unordered_map<std::string, BaseTexture> m_precached_textures;
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
	register_texture("unknown_pfp", g_unknown_pfp, g_unknown_pfp_size);

	CConsole::the().info("TextureManager initialized. Registered {} textures.", m_precached_textures.size());
}

void CTextureManager::shutdown()
{
	for (auto& [id, texture] : m_precached_textures)
	{
		texture.unload();
	}

	m_precached_textures.clear();

	CConsole::the().info("Shutting down TextureManager...");
}

BaseTexture* CTextureManager::register_texture(const char* identifier, const uint8_t* texture_filedata, uint32_t length)
{
	// create new texture entry
	m_precached_textures[identifier] = { texture_filedata, length };

	// now update the texture data to gpu
	if (!m_precached_textures[identifier].load())
	{
		CConsole::the().error("Couldn't register texture: '{}'", identifier);
		return nullptr;
	}

	CConsole::the().info("Registered texture: '{}', {}x{}, id {}", 
						 identifier, 
						 m_precached_textures[identifier].get_resolution().x, m_precached_textures[identifier].get_resolution().y, 
						 m_precached_textures[identifier].get_gl_id());

	return &m_precached_textures[identifier];
}

void CTextureManager::unregister_texture(const char* identifier)
{
	if (m_precached_textures.find(identifier) == m_precached_textures.end())
	{
		CConsole::the().error("Cannot unregister non-existent texture '{}'.", identifier);
		return;
	}

	m_precached_textures.erase(identifier);

	CConsole::the().info("Unregistered texture: {}", identifier);
}

BaseTexture* CTextureManager::get_texture(const char* identifier)
{
	try
	{
		return &m_precached_textures.at(identifier);
	}
	catch (...)
	{
		return nullptr;
	}
}
