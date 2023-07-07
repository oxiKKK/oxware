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

#ifndef GOLDSRCMUNGIFICATIONALGORITHMS_H
#define GOLDSRCMUNGIFICATIONALGORITHMS_H
#pragma once

class GoldSrcMungificationAlgorithms
{
public:
	DECL_BASIC_CLASS(GoldSrcMungificationAlgorithms);

public:
	void enmungify(unsigned char* data, int length, int seq);
	void enmungify2(unsigned char* data, int length, int seq);
	void enmungify3(unsigned char* data, int length, int seq);

	// does the opposite of munge
	void demungify(unsigned char* data, int length, int seq);
	void demungify2(unsigned char* data, int length, int seq);
	void demungify3(unsigned char* data, int length, int seq);

private:
	void enmungify_internal(const unsigned char* table, unsigned char* data, int length, int seq);
	void demungify_internal(const unsigned char* table, unsigned char* data, int length, int seq);
};

#endif // GOLDSRCMUNGIFICATIONALGORITHMS_H