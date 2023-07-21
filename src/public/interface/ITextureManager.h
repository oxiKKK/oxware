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

#include <BaseTexture.h>

class ITextureManager : public IBaseInterface
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual BaseTexture* register_texture(const char* identifier, const uint8_t* texture_filedata, uint32_t length) = 0;
	virtual void unregister_texture(const char* identifier) = 0;

	virtual BaseTexture* get_texture(const char* identifier) = 0;
};

extern ITextureManager* g_texture_manager_i;

#define ITEXTUREMANAGER_INTERFACEID "ITextureManager"

#endif // ITEXTUREMANAGER_H