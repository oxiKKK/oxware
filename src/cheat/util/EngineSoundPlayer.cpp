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

void CEngineSoundPlayer::play_engine_sound(int ent, const Vector& origin, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	//
	// use EventAPI because unlike the function from pmove, it doesn't use CL_DemoQueueSound(), so the
	// sound record won't be stored inside the demo.
	//

	cl_enginefuncs->pEventAPI->EV_PlaySound(ent, (Vector&)origin, channel, sample, volume, attenuation, fFlags, pitch);
}

void CEngineSoundPlayer::play_ambient_sound_unique(int ent, const Vector& origin, const char* sample, float volume, EAmbientSoundSpawnFlags radius, bool repeat)
{
	float attenuation = ATTN_STATIC;
	switch (radius)
	{
		case AMBIENT_EVERYWHERE: attenuation = ATTN_NONE; break;
		case AMBIENT_SMALL_RADIUS: attenuation = ATTN_IDLE; break;
		case AMBIENT_MEDIUM_RADIUS: attenuation = ATTN_STATIC; break;
		case AMBIENT_LARGE_RADIUS: attenuation = ATTN_NORM; break;
	}

	play_engine_sound(ent, origin, CHAN_STATIC, sample, volume, attenuation, SND_SPAWNING, PITCH_NORM);

	m_sounds.insert(sample);
}

void CEngineSoundPlayer::stop_sound(const std::string& sample)
{
	decltype(m_sounds)::iterator remove = m_sounds.end();
	for (auto it = m_sounds.begin(); it != m_sounds.end(); it++)
	{
		if (*it == sample)
		{
			remove = it;
			play_engine_sound(0, Vector(0, 0, 0), CHAN_STATIC, it->c_str(), 0.0f, 0.0f, SND_STOP, 0);
		}
	}

	if (remove != m_sounds.end())
	{
		m_sounds.erase(remove);
	}
}

void CEngineSoundPlayer::stop_all_ambient_sounds()
{
	for (const auto& s : m_sounds)
	{
		play_engine_sound(0, Vector(0, 0, 0), CHAN_STATIC, s.c_str(), 0.0f, 0.0f, SND_STOP, 0);
	}
}
