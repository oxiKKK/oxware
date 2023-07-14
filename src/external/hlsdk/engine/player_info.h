/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/
#ifndef PLAYERINFO_H
#define PLAYERINFO_H
#pragma once

#include "custom.h"

#define	MAX_INFO_STRING			256
#define	MAX_SCOREBOARDNAME		32

// info representing player that is playing on the server
struct player_info_t
{
	// User id on server
	int				userid;

	// User info string
	char			userinfo[MAX_INFO_STRING];

	// Name
	char			name[MAX_SCOREBOARDNAME];

	// Spectator or not, unused
	int				spectator;

	int				ping;
	int				packet_loss;

	// skin information
	char			model[MAX_QPATH];
	int				topcolor;
	int				bottomcolor;

	// last frame rendered
	int				renderframe;

	// Gait frame estimation
	int				gaitsequence;
	float			gaitframe;
	float			gaityaw;
	vec3_t			prevgaitorigin;

	customization_t customdata;

	char			hashedcdkey[16];
	uint64			m_nSteamID;
};

struct player_info3266_t
{
	// User id on server
	int				userid;

	// User info string
	char			userinfo[MAX_INFO_STRING];

	// Name
	char			name[MAX_SCOREBOARDNAME];

	// Spectator or not, unused
	int				spectator;

	int				ping;
	int				packet_loss;

	// skin information
	char			model[MAX_QPATH];
	int				topcolor;
	int				bottomcolor;

	// last frame rendered
	int				renderframe;

	// Gait frame estimation
	int				gaitsequence;
	float			gaitframe;
	float			gaityaw;
	vec3_t			prevgaitorigin;

	customization_t customdata;

	char			hashedcdkey[16];
	uint32			someting_most_likely_just_alignment_generated_by_the_compiler;
};


struct extra_player_info_t
{
	short		frags, deaths, team_id;
	int			has_c4, vip;
	Vector		origin;
	float		radarflash;
	int			radarflashon, radarflashes;
	short		playerclass, teamnumber;
	char		teamname[16];
	bool		dead;
	float		showhealth;
	int			health;
	char		location[32];
	int			sb_health, sb_account;
	int			has_defuse_kit;
};

// before 8684
struct extra_player_info_old_t
{
	short frags;
	short deaths;
	short team_id;
	int has_c4;
	int vip;
	Vector origin;
	float radarflash;
	int radarflashon;
	int radarflashes;
	short playerclass;
	short teamnumber;
	char teamname[16];
	bool dead;
	float showhealth;
	int health;
	char location[32];
};;

#endif // PLAYERINFO_H