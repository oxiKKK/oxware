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

//
// NetchanSequenceHistory.cpp -- A simple sequence counter with timestamps, so that
//								 we can backtrack into the past while fakelagging.
//

#include "precompiled.h"

VarBoolean fake_latency("fake_latency", "Simulate fake laggy latency", false);
VarInteger fake_latency_amount("fake_latency_amount", "Latency to generate", 200, 50, 500);

void CNetchanSequenceHistory::update()
{
	if (!fake_latency.get_value())
	{
		return;
	}

	if (m_sequences.size() > k_max_sequences)
	{
		m_sequences.pop_back();
	}

	int incoming_sequence = CMemoryHookMgr::the().cls()->netchan.incoming_sequence;

	if (incoming_sequence > m_last_incoming)
	{
		float realtime = CMemoryHookMgr::the().cl().get()->time;

		m_sequences.emplace_front(incoming_sequence, realtime);

//		CConsole::the().dinfo("sequence: #{}, time {}", incoming_sequence, realtime);

		m_last_incoming = incoming_sequence;
	}

	generate_fake_latency();
}

void CNetchanSequenceHistory::flush()
{
	m_sequences.clear();
	m_last_incoming = 0;
}

void CNetchanSequenceHistory::generate_fake_latency()
{
	// dig into past and find the right entry that suits our settings
	auto entry = find_sequence_entry();
	if (!entry.has_value())
	{
		return; // didn't find any yet, need to wait a little bit so that we generate enough history records.
	}

	CMemoryHookMgr::the().cls()->netchan.incoming_sequence = entry.value().seq;
}

std::optional<netseq_t> CNetchanSequenceHistory::find_sequence_entry()
{
	float realtime = CMemoryHookMgr::the().cl().get()->time;

	// point in time to look for
	float desired_latency = (float)fake_latency_amount.get_value() / 1000.0f;

	// linear search.. ouch

	for (auto& seq : m_sequences)
	{
		float delta = realtime - seq.timestamp;

		if (delta >= desired_latency)
		{
			m_desired_fake_latency = delta;

			// found it
			return seq;
		}
	}

	return std::nullopt;
}
