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

#ifndef ENGINESOUNDPLAYER_H
#define ENGINESOUNDPLAYER_H
#pragma once

class CEngineSoundPlayer
{
public:
	DECL_BASIC_CLASS(CEngineSoundPlayer);

public:
	void play_engine_sound(int ent, const Vector& origin, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch);

	// plays ambient sound just like ambient_generic
	void play_ambient_sound_unique(int ent, const Vector& origin, const char* sample, float volume, EAmbientSoundSpawnFlags radius, bool repeat);

	void stop_sound(const std::string& sample);
	void stop_all_ambient_sounds();

private:
	std::unordered_set<std::string> m_sounds;
};

#endif // ENGINESOUNDPLAYER_H