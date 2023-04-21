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

void CGenericMdl::update(hl::model_t* mdl)
{
	m_model = mdl;

	if (m_model)
	{
		switch (m_model->type)
		{
			case hl::mod_studio:
			{
				m_studiomodel.update((hl::studiohdr_t*)m_model->cache.data);
			}
			case hl::mod_sprite:
			{
				m_studiomodel.update((hl::studiohdr_t*)m_model->cache.data);
			}
			case hl::mod_bad:
			case hl::mod_brush:
			case hl::mod_alias:
			default:
			{
				// these ones don't have any header. (except for alias models.. but who cares about them.. xd)
				break;
			}
		}
	}
}

std::pair<Vector, Vector> CGenericMdl::ideal_movement_hullsize() const
{
	assert(m_model->type == hl::mod_studio && "trying to get bounds from non-studio model!");

	if (!m_studiomodel.is_valid())
	{
		return {};
	}

	return m_studiomodel.get_movement_bounds();
}
