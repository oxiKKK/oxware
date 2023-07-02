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

#ifndef NETCHANSEQUENCEHISTORY_H
#define NETCHANSEQUENCEHISTORY_H
#pragma once

extern VarBoolean fake_latency;
extern VarInteger fake_latency_amount;

// sequence record
struct netseq_t
{
	int		seq;		// sequence number
	float	timestamp;	// sequence arrival time
};

class CNetchanSequenceHistory
{
public:
	DECL_BASIC_CLASS(CNetchanSequenceHistory);

public:
	void update();
	void flush();

	// in seconds
	inline float get_desired_fake_latency() const { return m_desired_fake_latency; }

private:
	void generate_fake_latency();

	std::optional<netseq_t> find_sequence_entry();

private:
	std::deque<netseq_t> m_sequences;

	int m_last_incoming;
	
	float m_desired_fake_latency;

	inline static constexpr int k_max_sequences = 1000; // 1 s to the past
};

#endif // NETCHANSEQUENCEHISTORY_H