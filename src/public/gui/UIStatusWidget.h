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

#ifndef UI_STATUSWIDGET_H
#define UI_STATUSWIDGET_H
#pragma once

#define k_StatusWidgetStandardLifeDur 2500

class UIStatusWidget
{
public:
	UIStatusWidget()
	{
	}

	explicit UIStatusWidget(uint32_t status_life_duration)
		: m_status_life_duration(status_life_duration)
	{
	}

	enum Type
	{
		Info,		// white color
		Success,	// green color
		Error,		// red color
		Warning,	// orange color
	};

	void update_status(const std::string& new_status, Type type)
	{
		assert(type >= Info && type <= Warning);
		m_status = { new_status, type };

		m_last_updated = GetTickCount();
	}

	const auto& get_string() const { return m_status.st; }
	CColor get_color() const
	{
		switch (m_status.type)
		{
			case Info:		return CColor(230, 230, 230, 170);
			case Success:	return CColor(0, 112, 0, 170);
			case Error:		return CColor(112, 0, 0, 170);
			case Warning:	return CColor(252, 186, 3, 170);
		}

		assert(0);
		return CColor{};
	}

	bool is_alive() const
	{
		return (GetTickCount() - m_status_life_duration) < m_last_updated;
	}

private:
	struct status_t
	{
		std::string st;
		Type		type;
	};

	status_t m_status;

	uint32_t m_status_life_duration, m_last_updated;
};

#endif