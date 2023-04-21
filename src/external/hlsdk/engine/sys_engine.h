/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

class CEngine
{
	void*			__vtable;

	int				m_nQuitting;
	int				m_nDLLState;
	int				m_nSubState;

	double			m_fCurTime;
	double			m_fFrameTime;
	double			m_fOldTime;

	bool			m_bTrapMode;
	bool			m_bDoneTrapping;
	int				m_nTrapKey;
	int				m_nTrapButtons;
};
