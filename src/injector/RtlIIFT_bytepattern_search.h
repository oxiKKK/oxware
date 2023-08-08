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

#ifndef RTLIIFT_BYTEPATTERN_SEARCH_H
#define RTLIIFT_BYTEPATTERN_SEARCH_H
#pragma once

// this function isn't exported by the ntdll.dll module and its byte code varies from windows versions... bad for us, hehe
// RtlInsertInvertedFunctionTable and RtlRemoveInvertedFunctionTable is the function.
class RtlIIFT_BytePattern_Search
{
public:
	DECL_BASIC_CLASS(RtlIIFT_BytePattern_Search);

public:
	bool resolve_bytepatterns();
	std::string m_RtlIIFT_bytepattern, m_RtlIIFT_bytepattern_mask;
	std::string m_RtlRIFT_bytepattern, m_RtlRIFT_bytepattern_mask;

private:
	bool resolve_iift(); // RtlInsertInvertedFunctionTable
	bool resolve_rift(); // RtlRemoveInvertedFunctionTable

private:
	bool try_to_find_function_in_ntdll(const CBytePatternWithLengthConstexpr<k_max_mmapper_pattern_length>& pattern);
};

#endif // RTLIIFT_BYTEPATTERN_SEARCH_H