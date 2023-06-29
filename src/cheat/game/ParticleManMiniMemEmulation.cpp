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
// ParticleManMiniMemEmulation.cpp -- Emulator for particleman.dll module CMiniMem class.
// 
//	The idea behind emulation is basically that it redirects all of the important function
//	calls from the CMiniMem class into our artifical implementation of it. Using this tech-
//	nique, we can mirror the same functionality while improving some of the limitations that
//	the original class have, such as being slow or having low limits.
// 
// Why do we have to take the hard way? - detour so many things in order to accomplish that?
// 
//	Good question. Well, since the class is a singleton, almost all of the functions are not
//	bound to a specific object, therefore these are treated as regular non-class functions.
//	They're also not virtual, so we cannot just modify the vtable of that class. Therefore, 
//	we need to take this complicated approach of hooking important class member functions and
//	re-implementing them inside our own code.
//

#include "precompiled.h"

// 30k memory - 1,875 particles for use
#define VANILLA_PART_MEM	(30 * 1000)
#define VANILLA_PART_LIMIT	((VANILLA_PART_MEM) / sizeof(hl::MemoryBlock))

// 1M memory - 62,500 particles for use
#define NEW_PART_MEM		(1000 * 1000)
#define NEW_PART_LIMIT		((NEW_PART_MEM) / sizeof(hl::MemoryBlock))

// static definitions
hl::VectorOfMemoryBlocks CMiniMemEmulator::m_memory_pool;
long CMiniMemEmulator::m_memory_block_size = 0;
long CMiniMemEmulator::m_max_blocks = 0;
long CMiniMemEmulator::m_memory_pool_size = 0;

static hl::MemList g_free_mem, g_active_mem;

CMiniMemEmulator::CMiniMemEmulator(long memory_pool_size, long max_block_size)
{
	m_memory_pool_size = memory_pool_size;
	m_memory_block_size = max_block_size;
	m_max_blocks = memory_pool_size / sizeof(hl::MemoryBlock);

	// allocate all blocks
	for (int i = 0; i < m_max_blocks; i++)
	{
		hl::MemoryBlock* pNewBlock = new hl::MemoryBlock(m_memory_block_size);
		m_memory_pool.push_back(pNewBlock);
		g_free_mem.Push(pNewBlock);
	}

	m_allocated_visible_particles = new hl::visibleparticles_t[m_max_blocks];
}

CMiniMemEmulator::~CMiniMemEmulator()
{
	hl::MemoryBlock *pFreeBlock = g_free_mem.Front();
	while (pFreeBlock)
	{
		hl::MemoryBlock *pNext = pFreeBlock->next;
		g_free_mem.Delete(pFreeBlock);
		delete pFreeBlock;
		pFreeBlock = pNext;
	}

	m_memory_pool.clear();

	delete[] m_allocated_visible_particles;
	m_allocated_visible_particles = NULL;
}

char* CMiniMemEmulator::alloc_free_block()
{
	hl::MemoryBlock* pNewBlock = g_free_mem.Pop();
	if (!pNewBlock)
	{
		return NULL;
	}

	g_active_mem.Push(pNewBlock);
	return pNewBlock->Memory();
}

char* CMiniMemEmulator::new_block()
{
	char* p = alloc_free_block();
	if (!p)
	{
//		gEngfuncs.pfnCon_DPrintf("Particleman is out of memory, too many particles");
		return NULL;
	}

	return p;
}

void CMiniMemEmulator::delete_block(hl::MemoryBlock* p)
{
	g_active_mem.Delete(p);
	g_free_mem.Push(p);
}

long CMiniMemEmulator::percent_used()
{
	int iFreeNum = 0;

	hl::MemoryBlock* pFreeBlock = g_free_mem.Front();
	while (pFreeBlock)
	{
		iFreeNum++;
		pFreeBlock = pFreeBlock->next;
	}

	iFreeNum *= 100;
	iFreeNum /= m_max_blocks;

	return iFreeNum;
}

long CMiniMemEmulator::max_block_size()
{
	return m_memory_block_size;
}

static int particle_sort_fn(const void* p1, const void* p2)
{
	hl::CCoreTriangleEffect* pParticle1 = ((hl::visibleparticles_t*)p1)->pVisibleParticle;
	hl::CCoreTriangleEffect* pParticle2 = ((hl::visibleparticles_t*)p2)->pVisibleParticle;

	// bigger distance
	if (pParticle1->GetPlayerDistance() > pParticle2->GetPlayerDistance())
	{
		return -1;
	}
	// equal distance
	else if (pParticle1->GetPlayerDistance() == pParticle2->GetPlayerDistance())
	{
		return 0;
	}
	// different distance
	else
	{
		return 1;
	}
}

void CMiniMemEmulator::process_all()
{
	float				time;
	hl::MemoryBlock*	currentBlock, *nextBlock;
	Vector				vPlayerOrigin;
	hl::CCoreTriangleEffect* pEffect;
	float				flDistance, flPOrigin;
	int					j;
	hl::cl_entity_t*	localEntity;

	time = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetClientTime();

	m_total_parts = m_parts_drawn = 0;

	memset(m_allocated_visible_particles, NULL, m_max_blocks * sizeof(hl::visibleparticles_t*));

	float* g_flOldTime = CParticlemanMiniMemEmulation::the().g_flOldTime().get();

	currentBlock = g_active_mem.Front();
	while (currentBlock)
	{
		nextBlock = currentBlock->next; // can get deleted, so save into a variable

		pEffect = (hl::CCoreTriangleEffect*)currentBlock->Memory();
		if (pEffect)
		{
			m_total_parts++;

			if (pEffect->CheckVisibility())
			{
				localEntity = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetLocalPlayer();
				vPlayerOrigin = localEntity->origin;

				flDistance = 0.0f;
				for (j = 0; j < 3; j++)
				{
					flPOrigin = vPlayerOrigin[j] - pEffect->m_vOrigin[j];
					flDistance += (flPOrigin * flPOrigin);
				}

				pEffect->SetPlayerDistance(flDistance);

				m_allocated_visible_particles[m_parts_drawn++].pVisibleParticle = pEffect;
			}

			if (CMemoryHookMgr::the().cl_enginefuncs()->pfnGetClientTime() != *g_flOldTime)
			{
				pEffect->Think(time);
			}

			// check if dead
			if (pEffect->m_flDieTime > 0.0f && pEffect->m_flDieTime <= time)
			{
				pEffect->Die();
				delete_block(currentBlock);
			}
		}

		currentBlock = nextBlock;
	}

	// Sort it by distance between particle and player
	qsort(m_allocated_visible_particles, m_parts_drawn, sizeof(hl::visibleparticles_t*), particle_sort_fn);

	for (j = 0; j < m_parts_drawn; j++)
	{
		m_allocated_visible_particles[j].pVisibleParticle->Draw();
	}

	*g_flOldTime = time;
}

int CMiniMemEmulator::apply_force(Vector vOrigin, Vector vDirection, float flRadius, float flStrength)
{
	hl::MemoryBlock*currentBlock;
	float			flAdjustedStrength, falloff, size;
	float			flPOrign, flAccel;
	float			flSQRDistance;
	float			flRadiusSquared;
	hl::CCoreTriangleEffect* pEffect;
	Vector			mins, maxs;
	Vector			direction, velocity;

	flRadiusSquared = flRadius * flRadius;

	currentBlock = g_active_mem.Front();
	while (currentBlock)
	{
		pEffect = (hl::CCoreTriangleEffect*)currentBlock->Memory();
		if (pEffect)
		{
			// don't apply any force on this one
			if (pEffect->m_bAffectedByForce)
			{
				size = pEffect->m_flSize / 5.0f;
				mins = pEffect->m_vOrigin - Vector(size, size, size);
				maxs = pEffect->m_vOrigin + Vector(size, size, size);

				//
				// compute squared distance
				//
				flSQRDistance = 0.0f;
				for (int i = 0; i < 3; i++)
				{
					if (vOrigin[i] < mins[i])
					{
						flPOrign = mins[i];
					}
					else if (vOrigin[i] > maxs[i])
					{
						flPOrign = maxs[i];
					}
					else
					{
						continue;
					}

					flSQRDistance += (vOrigin[i] - flPOrign) * (vOrigin[i] - flPOrign);
				}

				if (flSQRDistance > flRadiusSquared)
				{
					continue;
				}

				falloff = flStrength / (flRadiusSquared * 0.5f);
				flAdjustedStrength = flStrength - ((vOrigin - pEffect->m_vOrigin).Length() * falloff);
				if (flAdjustedStrength < 0)
					flAdjustedStrength = 0;

				if (vDirection.IsZero())
				{
					flAccel = flAdjustedStrength / pEffect->m_flMass;
					flAccel *= -1.0f;

					direction = (vOrigin - pEffect->m_vOrigin).Normalize();
					velocity = pEffect->m_vVelocity.Normalize();
				}
				else
				{
					flAccel = flAdjustedStrength / pEffect->m_flMass;

					direction = vDirection.Normalize();
					velocity = pEffect->m_vVelocity.Normalize();
				}

				pEffect->m_vVelocity = flAccel * (direction + velocity);

				pEffect->Force();
			}
		}

		currentBlock = currentBlock->next;
	}

	return 1;
}

void CMiniMemEmulator::reset()
{
	g_active_mem.Reset();
	g_free_mem.Reset();

	m_parts_drawn = m_total_parts = 0;

	for (hl::MemoryBlockIterator i = m_memory_pool.begin(); i != m_memory_pool.end(); i++)
	{
		g_free_mem.Push(*i);
	}
}

//--------------------------------------------------------------------------------

bool CParticlemanMiniMemEmulation::emulate()
{
	if (m_emulated)
	{
		return true;
	}

	if (m_first_time)
	{
		CConsole::the().info("Starting to emulate particleman MiniMem class...");
	}

	if (!CMiniMem___instance().install()) return false;
	if (!g_flOldTime().install()) return false;

	if (!CMiniMem__newBlock().install()) return false;
	if (!CMiniMem__ProcessAll().install()) return false;
	if (!CMiniMem__ApplyForce().install()) return false;
	if (!CMiniMem__Reset().install()) return false;
	if (!CMiniMem__PercentUsed().install()) return false;

	m_emulated = true;
	return true;
}

void CParticlemanMiniMemEmulation::restore_from_emulation()
{
	if (!m_emulated)
	{
		// don't restore emulation if we didn't establish it
		return;
	}

	if (m_first_time)
	{
		CConsole::the().info("Removing particleman MiniMem emulation...");
	}

	// free memory
	delete m_instance;
	m_instance = nullptr;

	CMiniMem__newBlock().uninstall();
	CMiniMem__ProcessAll().uninstall();
	CMiniMem__ApplyForce().uninstall();
	CMiniMem__Reset().uninstall();
	CMiniMem__PercentUsed().uninstall();

	m_emulated = false;

	// already emulated/restored from it for the first time.
	m_first_time = false;
}

CMiniMemEmulator* CParticlemanMiniMemEmulation::instance()
{
	// create a singleton instance
	if (!m_instance)
	{
		m_instance = new CMiniMemEmulator(NEW_PART_MEM, sizeof(hl::CCoreTriangleEffect) * 2); // this should be enough for any cstrike particle.
	}

	return m_instance;
}

//--------------------------------------------------------------------------------

bool CMiniMem___instance_MemoryHook::install()
{
	initialize("CMiniMem::_instance", L"particleman.dll");
	return install_using_bytepattern(1);
}

void CMiniMem___instance_MemoryHook::test_hook()
{
	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return true;
		});
}

//--------------------------------------------------------------------------------

bool g_flOldTime_MemoryHook::install()
{
	initialize("g_flOldTime", L"particleman.dll");
	return install_using_bytepattern(1);
}

void g_flOldTime_MemoryHook::test_hook()
{
	auto p = get();
	
	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return *p != 0.0f;
		});
}

//--------------------------------------------------------------------------------

bool CMiniMem__newBlock_FnDetour_t::install()
{
	initialize("CMiniMem::newBlock", L"particleman.dll");
	return detour_using_bytepattern((uintptr_t*)CMiniMem__newBlock);
}

char* CMiniMem__newBlock_FnDetour_t::CMiniMem__newBlock()
{
	return CParticlemanMiniMemEmulation::the().instance()->new_block();
}

//--------------------------------------------------------------------------------

bool CMiniMem__ProcessAll_FnDetour_t::install()
{
	initialize("CMiniMem::ProcessAll", L"particleman.dll");
	return detour_using_bytepattern((uintptr_t*)CMiniMem__ProcessAll);
}

void CMiniMem__ProcessAll_FnDetour_t::CMiniMem__ProcessAll()
{
	CParticlemanMiniMemEmulation::the().instance()->process_all();

	// redirect the original instance to our own. This takes care of all of the CMiniMem members, 
	// such as m_iTotalParticles or m_iParticlesDrawn, and therefore that automatically takes care
	// or member functions: GetTotalParticles() and GetDrawnParticles(), because they return this data.
	// Hence, we don't need to detour them.
	(*CParticlemanMiniMemEmulation::the().CMiniMem___instance().get())->m_iParticlesDrawn = CParticlemanMiniMemEmulation::the().instance()->m_parts_drawn;
	(*CParticlemanMiniMemEmulation::the().CMiniMem___instance().get())->m_iTotalParticles = CParticlemanMiniMemEmulation::the().instance()->m_total_parts;
}

//--------------------------------------------------------------------------------

bool CMiniMem__ApplyForce_FnDetour_t::install()
{
	initialize("CMiniMem::ApplyForce", L"particleman.dll");
	return detour_using_bytepattern((uintptr_t*)CMiniMem__ApplyForce);
}

int CMiniMem__ApplyForce_FnDetour_t::CMiniMem__ApplyForce(Vector vOrigin, Vector vDirection, float flRadius, float flStrength)
{
	return CParticlemanMiniMemEmulation::the().instance()->apply_force(vOrigin, vDirection, flRadius, flStrength);
}

//--------------------------------------------------------------------------------

bool CMiniMem__Reset_FnDetour_t::install()
{
	initialize("CMiniMem::Reset", L"particleman.dll");
	return detour_using_bytepattern((uintptr_t*)CMiniMem__Reset);
}

void CMiniMem__Reset_FnDetour_t::CMiniMem__Reset()
{
	CParticlemanMiniMemEmulation::the().instance()->reset();
}

//--------------------------------------------------------------------------------

bool CMiniMem__PercentUsed_FnDetour_t::install()
{
	initialize("CMiniMem::PercentUsed", L"particleman.dll");
	return detour_using_bytepattern((uintptr_t*)CMiniMem__PercentUsed);
}

long CMiniMem__PercentUsed_FnDetour_t::CMiniMem__PercentUsed()
{
	return CParticlemanMiniMemEmulation::the().instance()->percent_used();
}

//--------------------------------------------------------------------------------