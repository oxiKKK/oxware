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

#ifndef GENERICENT_H
#define GENERICENT_H
#pragma once

class CGenericEntity
{
public:
	CGenericEntity() :
		m_ent(nullptr)
	{
	}

	virtual void update(hl::cl_entity_t* ent);

	virtual bool is_valid() const { return m_ent != nullptr; }

	// getters
	hl::cl_entity_t* cl_entity() const { return m_ent; }
	const CGenericMdl& associated_model() const { return m_model; }

	Vector get_bounding_box_min() const { return m_ent->curstate.mins; }
	Vector get_bounding_box_max() const { return m_ent->curstate.maxs; }

	// true if entity didn't receive any state updates for an interval.
	bool is_out_of_update_for(float interval) const;

protected:
	hl::cl_entity_t* m_ent = nullptr; // can't be null when iterated over the list

	CGenericMdl m_model;
};

#endif // GENERICENT_H