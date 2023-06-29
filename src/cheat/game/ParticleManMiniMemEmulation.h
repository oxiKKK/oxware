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

#ifndef PARTICLEMANMINIMEMEMULATION_H
#define PARTICLEMANMINIMEMEMULATION_H
#pragma once

//------------------------------------------------------------------------

class CMiniMemEmulator
{
private:
	// Main memory pool.  Array is fine, but vectors are easier. :)
	static hl::VectorOfMemoryBlocks m_memory_pool;
	// Size of memory blocks in pool.
	static long m_memory_block_size;
	static long m_max_blocks;
	static long m_memory_pool_size;

	hl::visibleparticles_t* m_allocated_visible_particles;

	// find a free block and mark it as "in use". Return NULL if no free blocks found.
	char* alloc_free_block();

public:
	CMiniMemEmulator(long memory_pool_size, long max_block_size);
	virtual ~CMiniMemEmulator();

	int m_total_parts;
	int m_parts_drawn;

	// return a pointer to usable block of memory.
	char* new_block();

	// mark a target memory item as no longer "in use".
	void delete_block(hl::MemoryBlock* p);

	// return the remaining capacity of the memory pool as a percent.
	long percent_used();

	void process_all();

	// clears memory, setting all particles to not used.
	void reset();

	long max_block_size();

	int apply_force(Vector vOrigin, Vector vDirection, float flRadius, float flStrength);
};

static_assert(sizeof(CMiniMemEmulator) == sizeof(hl::CMiniMem));

//------------------------------------------------------------------------

//
// functions that need to be detoured:
// 
// Important:
//	-	CMiniMem::Instance or 
//		 CMiniMem::_instance		-> so that we can modify CMiniMem members
//	-	CMiniMem::newBlock			-> called at every new operator
//	-	CMiniMem::ProcessAll		-> main func
//	-	CMiniMem::ApplyForce		-> main func
//	-	CMiniMem::Reset				-> resets all parts
// 
// Not so important:
//	-	CMiniMem::MaxBlockSize		-> called inside CCoreTriangleEffect::CheckSize(), not sure if this is being called anywere, e.g. in cs
//	-	CMiniMem::PercentUsed		-> used in cl_pmanstats
//

// CMiniMem* CMiniMem::_instance;
struct CMiniMem___instance_MemoryHook final : public GenericMemoryHook<hl::CMiniMem*>
{
	bool install() override;
	void test_hook() override;
};

// float g_flOldTime;
struct g_flOldTime_MemoryHook final : public GenericMemoryHook<float>
{
	bool install() override;
	void test_hook() override;
};

// char* CMiniMem::newBlock();
struct CMiniMem__newBlock_FnDetour_t final : public GenericMemoryFnDetour_cdecl<char*>
{
	bool install();
	static char* CMiniMem__newBlock();
};

// void CMiniMem::ProcessAll();
struct CMiniMem__ProcessAll_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void CMiniMem__ProcessAll();
};

// int CMiniMem::ApplyForce(Vector vOrigin, Vector vDirection, float flRadius, float flStrength);
struct CMiniMem__ApplyForce_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, Vector, Vector, float, float>
{
	bool install();
	static int CMiniMem__ApplyForce(Vector vOrigin, Vector vDirection, float flRadius, float flStrength);
};

// void CMiniMem::Reset();
struct CMiniMem__Reset_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
	bool install();
	static void CMiniMem__Reset();
};

// long CMiniMem::PercentUsed();
struct CMiniMem__PercentUsed_FnDetour_t final : public GenericMemoryFnDetour_cdecl<long>
{
	bool install();
	static long CMiniMem__PercentUsed();
};

//------------------------------------------------------------------------

class CParticlemanMiniMemEmulation
{
public:
	DECL_BASIC_CLASS(CParticlemanMiniMemEmulation);

public:
	bool emulate();
	void restore_from_emulation();

	// hooks
	inline auto& CMiniMem___instance() { static CMiniMem___instance_MemoryHook fnhook; return fnhook; }
	inline auto& g_flOldTime() { static g_flOldTime_MemoryHook fnhook; return fnhook; }

	inline auto& CMiniMem__newBlock() { static CMiniMem__newBlock_FnDetour_t fnhook; return fnhook; }
	inline auto& CMiniMem__ProcessAll() { static CMiniMem__ProcessAll_FnDetour_t fnhook; return fnhook; }
	inline auto& CMiniMem__ApplyForce() { static CMiniMem__ApplyForce_FnDetour_t fnhook; return fnhook; }
	inline auto& CMiniMem__Reset() { static CMiniMem__Reset_FnDetour_t fnhook; return fnhook; }
	inline auto& CMiniMem__PercentUsed() { static CMiniMem__PercentUsed_FnDetour_t fnhook; return fnhook; }

	static CMiniMemEmulator* instance();

	// true if successfully emulated
	bool m_emulated = false;

	// if emulating for the first time
	bool m_first_time = true;

private:
	inline static CMiniMemEmulator* m_instance;
};

#endif // PARTICLEMANMINIMEMEMULATION_H