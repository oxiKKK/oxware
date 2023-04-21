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
#ifndef CONSISTENCY_H
#define CONSISTENCY_H
#pragma once

// Used for consistency_list[MAX_CONSISTENCY]. Use for resources
struct consistency_t
{
	char*	filename;		// File name of the resource

	int		issound;		// True if the resource type is t_sound
	int		orig_index;		// Index for resource
	int		value;			// 

	int		check_type;		// consistency check type, see FORCE_TYPE in eiface.h

	// For force_model_specifybounds and force_model_specifybounds_if_avail
	float	mins[3];
	float	maxs[3];
};

#endif // CONSISTENCY_H
