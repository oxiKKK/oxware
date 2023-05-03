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

#ifndef IBYTEPATTERNBANK_H
#define IBYTEPATTERNBANK_H
#pragma once

#include "BytePattern.h"

struct PatternRecord
{
	std::string module;
	std::string function;
	CBytePattern pattern;
};

using BytePatternContainer = std::unordered_map<std::string, PatternRecord>;

class IBytePatternBank : public IBaseInterface
{
public:
	virtual bool initialize(int build_number) = 0;
	
	virtual bool is_build_supported(int build_number) = 0;

	virtual std::string supported_builds_as_str() = 0;

	virtual CBytePattern get_pattern(const std::string& hook_name) const = 0;
	virtual const PatternRecord* get_full(const std::string& hook_name) const = 0;
};

extern IBytePatternBank* g_bytepattern_bank_i;

#define IBYTEPATTERNBANK_INTERFACEID "IBytePatternBank"

#endif // IGUIWIDGETS_H