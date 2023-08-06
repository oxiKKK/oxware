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

#include "precompiled.h"

std::array<CCSWeapon, MAX_WEAPONS> CWeapons::s_weapons =
{{
	{ },
	// Weapon index			Class						Ammo Type			Bullet type						Max Clip			Weapon weight			Weapon price		Weapon Primary Damage			Weapon Secondary Damage			Weapon Max Speed		Reload	Wall Penetration (+ 1)	Bullet Max Distance			RangeMod	NAttck	Sniper	Pistol	Shotgun	MG		SMG		Rifle	knife	gun		nade	c4
	{ WEAPON_P228,			WEAPONCLASS_PISTOL,			AMMO_357SIG,		BULLET_PLAYER_357SIG,			P228_MAX_CLIP,		P228_WEIGHT,			P228_PRICE,			P228_PRIMARY_DAMAGE,			P228_SECONDARY_DAMAGE,			P228_MAX_SPEED,			2.7f,	WALL_PENETRATION_1,		P228_MAX_DISTANCE,			0.800f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_GLOCK,			WEAPONCLASS_PISTOL,			AMMO_9MM,			BULLET_NONE,					GLOCK18_MAX_CLIP,	GLOCK18_WEIGHT,			GLOCK18_PRICE,		GLOCK_PRIMARY_DAMAGE,			GLOCK_SECONDARY_DAMAGE,			GLOCK_MAX_SPEED,		0.0f,	WALL_PENETRATION_NONE,	GLOCK_MAX_DISTANCE,			0.000f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_SCOUT,			WEAPONCLASS_SNIPERRIFLE,	AMMO_762NATO,		BULLET_PLAYER_762MM,			SCOUT_MAX_CLIP,		SCOUT_WEIGHT,			SCOUT_PRICE,		SCOUT_PRIMARY_DAMAGE,			SCOUT_SECONDARY_DAMAGE,			SCOUT_MAX_SPEED,		2.0f,	WALL_PENETRATION_3,		SCOUT_MAX_DISTANCE,			0.980f,		false,	true,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_HEGRENADE,		WEAPONCLASS_GRENADE,		AMMO_HEGRENADE,		BULLET_NONE,					MAX_CLIP_NONE,		HEGRENADE_WEIGHT,		HEGRENADE_PRICE,	HEGRENADE_PRIMARY_DAMAGE,		HEGRENADE_SECONDARY_DAMAGE,		HEGRENADE_MAX_SPEED,	0.0f,	WALL_PENETRATION_NONE,	HEGRENADE_MAX_DISTANCE,		0.000f,		true,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	},
	{ WEAPON_XM1014,		WEAPONCLASS_SHOTGUN,		AMMO_BUCKSHOT,		BULLET_PLAYER_BUCKSHOT,			XM1014_MAX_CLIP,	XM1014_WEIGHT,			XM1014_PRICE,		XM1014_PRIMARY_DAMAGE,			XM1014_SECONDARY_DAMAGE,		XM1014_MAX_SPEED,		0.0f,	WALL_PENETRATION_NONE,	XM1014_MAX_DISTANCE,		0.000f,		false,	false,	false,	true,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_C4,			WEAPONCLASS_GRENADE,		AMMO_C4,			BULLET_NONE,					MAX_CLIP_NONE,		C4_WEIGHT,				PRICE_FREE,			C4_PRIMARY_DAMAGE,				C4_SECONDARY_DAMAGE,			C4_MAX_SPEED,			0.0f,	WALL_PENETRATION_NONE,	C4_MAX_DISTANCE,			0.000f,		true,	false,	false,	false,	false,	false,	false,	false,	false,	true,	true,	},
	{ WEAPON_MAC10,			WEAPONCLASS_SUBMACHINEGUN,	AMMO_45ACP,			BULLET_PLAYER_45ACP,			MAC10_MAX_CLIP,		MAC10_WEIGHT,			MAC10_PRICE,		MAC10_PRIMARY_DAMAGE,			MAC10_SECONDARY_DAMAGE,			MAC10_MAX_SPEED,		3.15f,	WALL_PENETRATION_1,		MAC10_MAX_DISTANCE,			0.820f,		false,	false,	false,	false,	false,	true,	false,	false,	true,	false,	false,	},
	{ WEAPON_AUG,			WEAPONCLASS_RIFLE,			AMMO_556NATO,		BULLET_PLAYER_556MM,			AUG_MAX_CLIP,		AUG_WEIGHT,				AUG_PRICE,			AUG_PRIMARY_DAMAGE,				AUG_SECONDARY_DAMAGE,			AUG_MAX_SPEED,			3.3f,	WALL_PENETRATION_2,		AUG_MAX_DISTANCE,			0.960f,		false,	false,	false,	false,	false,	false,	true,	false,	true,	false,	false,	},
	{ WEAPON_SMOKEGRENADE,	WEAPONCLASS_GRENADE,		AMMO_SMOKEGRENADE,	BULLET_NONE,					MAX_CLIP_NONE,		SMOKEGRENADE_WEIGHT,	SMOKEGRENADE_PRICE, SMOKEGRENADE_PRIMARY_DAMAGE,	SMOKEGRENADE_SECONDARY_DAMAGE,	SMOKEGRENADE_MAX_SPEED, 0.0f,	WALL_PENETRATION_NONE,	SMOKEGRENADE_MAX_DISTANCE,	0.000f,		true,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	},
	{ WEAPON_ELITE,			WEAPONCLASS_PISTOL,			AMMO_9MM,			BULLET_PLAYER_9MM,				ELITE_MAX_CLIP,		ELITE_WEIGHT,			ELITE_PRICE,		ELITE_PRIMARY_DAMAGE,			ELITE_SECONDARY_DAMAGE,			ELITE_MAX_SPEED,		4.5f,	WALL_PENETRATION_1,		ELITE_MAX_DISTANCE,			0.750f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_FIVESEVEN,		WEAPONCLASS_PISTOL,			AMMO_57MM,			BULLET_PLAYER_357SIG,			FIVESEVEN_MAX_CLIP, FIVESEVEN_WEIGHT,		FIVESEVEN_PRICE,	FIVESEVEN_PRIMARY_DAMAGE,		FIVESEVEN_SECONDARY_DAMAGE,		FIVESEVEN_MAX_SPEED,	2.7f,	WALL_PENETRATION_1,		FIVESEVEN_MAX_DISTANCE,		0.885f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_UMP45,			WEAPONCLASS_SUBMACHINEGUN,	AMMO_45ACP,			BULLET_PLAYER_45ACP,			UMP45_MAX_CLIP,		UMP45_WEIGHT,			UMP45_PRICE,		UMP45_PRIMARY_DAMAGE,			UMP45_SECONDARY_DAMAGE,			UMP45_MAX_SPEED,		3.5f,	WALL_PENETRATION_1,		UMP45_MAX_DISTANCE,			0.820f,		false,	false,	false,	false,	false,	true,	false,	false,	true,	false,	false,	},
	{ WEAPON_SG550,			WEAPONCLASS_SNIPERRIFLE,	AMMO_556NATO,		BULLET_PLAYER_556MM,			SG550_MAX_CLIP,		SG550_WEIGHT,			SG550_PRICE,		SG550_PRIMARY_DAMAGE,			SG550_SECONDARY_DAMAGE,			SG550_MAX_SPEED,		3.35f,	WALL_PENETRATION_2,		SG550_MAX_DISTANCE,			0.980f,		false,	true,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_GALIL,			WEAPONCLASS_RIFLE,			AMMO_556NATO,		BULLET_PLAYER_556MM,			GALIL_MAX_CLIP,		GALIL_WEIGHT,			GALIL_PRICE,		GALIL_PRIMARY_DAMAGE,			GALIL_SECONDARY_DAMAGE,			GALIL_MAX_SPEED,		2.45f,	WALL_PENETRATION_2,		GALIL_MAX_DISTANCE,			0.980f,		false,	false,	false,	false,	false,	false,	true,	false,	true,	false,	false,	},
	{ WEAPON_FAMAS,			WEAPONCLASS_RIFLE,			AMMO_556NATO,		BULLET_PLAYER_556MM,			FAMAS_MAX_CLIP,		FAMAS_WEIGHT,			FAMAS_PRICE,		FAMAS_PRIMARY_DAMAGE,			FAMAS_SECONDARY_DAMAGE,			FAMAS_MAX_SPEED,		3.3f,	WALL_PENETRATION_2,		FAMAS_MAX_DISTANCE,			0.960f,		false,	false,	false,	false,	false,	false,	true,	false,	true,	false,	false,	},
	{ WEAPON_USP,			WEAPONCLASS_PISTOL,			AMMO_45ACP,			BULLET_PLAYER_45ACP,			USP_MAX_CLIP,		USP_WEIGHT,				USP_PRICE,			USP_PRIMARY_DAMAGE,				USP_SECONDARY_DAMAGE,			USP_MAX_SPEED,			2.7f,	WALL_PENETRATION_1,		USP_MAX_DISTANCE,			0.790f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_GLOCK18,		WEAPONCLASS_PISTOL,			AMMO_9MM,			BULLET_PLAYER_9MM,				GLOCK18_MAX_CLIP,	GLOCK18_WEIGHT,			GLOCK18_PRICE,		GLOCK18_PRIMARY_DAMAGE,			GLOCK18_SECONDARY_DAMAGE,		GLOCK18_MAX_SPEED,		2.2f,	WALL_PENETRATION_1,		GLOCK18_MAX_DISTANCE,		0.750f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_AWP,			WEAPONCLASS_SNIPERRIFLE,	AMMO_338MAGNUM,		BULLET_PLAYER_338MAG,			AWP_MAX_CLIP,		AWP_WEIGHT,				AWP_PRICE,			AWP_PRIMARY_DAMAGE,				AWP_SECONDARY_DAMAGE,			AWP_MAX_SPEED,			2.5f,	WALL_PENETRATION_3,		AWP_MAX_DISTANCE,			0.990f,		false,	true,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_MP5N,			WEAPONCLASS_SUBMACHINEGUN,	AMMO_9MM,			BULLET_PLAYER_9MM,				MP5N_MAX_CLIP,		MP5NAVY_WEIGHT,			MP5NAVY_PRICE,		MP5N_PRIMARY_DAMAGE,			MP5N_SECONDARY_DAMAGE,			MP5N_MAX_SPEED,			2.63f,	WALL_PENETRATION_1,		MP5N_MAX_DISTANCE,			0.840f,		false,	false,	false,	false,	false,	true,	false,	false,	true,	false,	false,	},
	{ WEAPON_M249,			WEAPONCLASS_MACHINEGUN,		AMMO_556NATOBOX,	BULLET_PLAYER_556MM,			M249_MAX_CLIP,		M249_WEIGHT,			M249_PRICE,			M249_PRIMARY_DAMAGE,			M249_SECONDARY_DAMAGE,			M249_MAX_SPEED,			4.7f,	WALL_PENETRATION_2,		M249_MAX_DISTANCE,			0.970f,		false,	false,	false,	false,	true,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_M3,			WEAPONCLASS_SHOTGUN,		AMMO_BUCKSHOT,		BULLET_PLAYER_BUCKSHOT,			M3_MAX_CLIP,		M3_WEIGHT,				M3_PRICE,			M3_PRIMARY_DAMAGE,				M3_SECONDARY_DAMAGE,			M3_MAX_SPEED,			0.0f,	WALL_PENETRATION_NONE,	M3_MAX_DISTANCE,			0.000f,		false,	false,	false,	true,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_M4A1,			WEAPONCLASS_RIFLE,			AMMO_556NATO,		BULLET_PLAYER_556MM,			M4A1_MAX_CLIP,		M4A1_WEIGHT,			M4A1_PRICE,			M4A1_PRIMARY_DAMAGE,			M4A1_SECONDARY_DAMAGE,			M4A1_MAX_SPEED,			3.05f,	WALL_PENETRATION_2,		M4A1_MAX_DISTANCE,			0.970f,		false,	true,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_TMP,			WEAPONCLASS_SUBMACHINEGUN,	AMMO_9MM,			BULLET_PLAYER_9MM,				TMP_MAX_CLIP,		TMP_WEIGHT,				TMP_PRICE,			TMP_PRIMARY_DAMAGE,				TMP_SECONDARY_DAMAGE,			TMP_MAX_SPEED,			2.12f,	WALL_PENETRATION_1,		TMP_MAX_DISTANCE,			0.850f,		false,	false,	false,	false,	false,	false,	true,	false,	true,	false,	false,	},
	{ WEAPON_G3SG1,			WEAPONCLASS_SNIPERRIFLE,	AMMO_762NATO,		BULLET_PLAYER_762MM,			G3SG1_MAX_CLIP,		G3SG1_WEIGHT,			G3SG1_PRICE,		G3SG1_PRIMARY_DAMAGE,			G3SG1_SECONDARY_DAMAGE,			G3SG1_MAX_SPEED,		3.5f,	WALL_PENETRATION_2,		G3SG1_MAX_DISTANCE,			0.980f,		false,	false,	true,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_FLASHBANG,		WEAPONCLASS_GRENADE,		AMMO_FLASHBANG,		BULLET_NONE,					MAX_CLIP_NONE,		FLASHBANG_WEIGHT,		FLASHBANG_PRICE,	FLASHBANG_PRIMARY_DAMAGE,		FLASHBANG_SECONDARY_DAMAGE,		FLASHBANG_MAX_SPEED,	0.0f,	WALL_PENETRATION_NONE,	FLASHBANG_MAX_DISTANCE,		0.000f,		true,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	},
	{ WEAPON_DEAGLE,		WEAPONCLASS_PISTOL,			AMMO_50AE,			BULLET_PLAYER_50AE,				DEAGLE_MAX_CLIP,	DEAGLE_WEIGHT,			DEAGLE_PRICE,		DEAGLE_PRIMARY_DAMAGE,			DEAGLE_SECONDARY_DAMAGE,		DEAGLE_MAX_SPEED,		2.2f,	WALL_PENETRATION_2,		DEAGLE_MAX_DISTANCE,		0.810f,		false,	false,	false,	true,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_SG552,			WEAPONCLASS_RIFLE,			AMMO_556NATO,		BULLET_PLAYER_556MM,			SG552_MAX_CLIP,		SG552_WEIGHT,			SG552_PRICE,		SG552_PRIMARY_DAMAGE,			SG552_SECONDARY_DAMAGE,			SG552_MAX_SPEED,		3.0f,	WALL_PENETRATION_2,		SG552_MAX_DISTANCE,			0.955f,		false,	true,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_AK47,			WEAPONCLASS_RIFLE,			AMMO_762NATO,		BULLET_PLAYER_762MM,			AK47_MAX_CLIP,		AK47_WEIGHT,			AK47_PRICE,			AK47_PRIMARY_DAMAGE,			AK47_SECONDARY_DAMAGE,			AK47_MAX_SPEED,			2.45f,	WALL_PENETRATION_2,		AK47_MAX_DISTANCE,			0.980f,		false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	},
	{ WEAPON_KNIFE,			WEAPONCLASS_KNIFE,			AMMO_NONE,			BULLET_NONE,					MAX_CLIP_NONE,		KNIFE_WEIGHT,			PRICE_FREE,			KNIFE_PRIMARY_DAMAGE,			KNIFE_SECONDARY_DAMAGE,			KNIFE_MAX_SPEED,		0.0f,	WALL_PENETRATION_NONE,	KNIFE_MAX_DISTANCE,			0.000f,		false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	},
	{ WEAPON_P90,			WEAPONCLASS_SUBMACHINEGUN,	AMMO_57MM,			BULLET_PLAYER_57MM,				P90_MAX_CLIP,		P90_WEIGHT,				P90_PRICE,			P90_PRIMARY_DAMAGE,				P90_SECONDARY_DAMAGE,			P90_MAX_SPEED,			3.4f,	WALL_PENETRATION_1,		P90_MAX_DISTANCE,			0.885f,		false,	false,	false,	false,	false,	false,	true,	false,	true,	false,	false,	},
}};

CCSWeapon::CCSWeapon(WeaponIdType weapon_id, EWeaponClassType weapon_class, EAmmoType ammo, EBulletType bullet_type,
					 int clip_size, int weight, int price,
					 int primary_damage, int secondary_damage,
					 float max_speed, float reload_time, EWallPenetrationStrength wall_pen_strength,
					 float max_distance, float range_modifier,

					 // properties
					 bool is_nonattack, bool is_sniper, bool is_pistol, bool is_shotgun, bool is_machinegun,
					 bool is_submachinegun, bool is_rifle, bool is_knife, bool is_gun, bool is_nade, bool is_c4)
{
	m_weapon_id = weapon_id;
	m_weapon_class = weapon_class;
	m_ammo_type = ammo;
	m_bullet_type = bullet_type;
	m_clip_size = clip_size;
	m_weight = weight;
	m_price = price;
	m_primary_damage = primary_damage;
	m_secondary_damage = secondary_damage;
	m_max_speed = max_speed;
	m_reload_time = reload_time;
	m_wall_penetration_strength = wall_pen_strength;
	m_max_distance = max_distance;
	m_range_modifier = range_modifier;

	m_is_nonattack = is_nonattack;
	m_is_sniper = is_sniper;
	m_is_pistol = is_pistol;
	m_is_shotgun = is_shotgun;
	m_is_machinegun = is_machinegun;
	m_is_submachinegun = is_submachinegun;
	m_is_rifle = is_sniper;
	m_is_knife = is_knife;
	m_is_gun = is_gun;
	m_is_nade = is_nade;
	m_is_c4 = is_c4;

	m_name = g_weapon_names[weapon_id].name;
	m_model_name = g_weapon_names[weapon_id].model_name;
}

void CCSWeapon::update(hl::local_state_t* state, hl::usercmd_t* cmd)
{
	m_local_state = state;
	m_cmd = cmd;
	m_weapon_data = &state->weapondata[state->client.m_iId];
	if (CMemoryHookCBaseStuff::the().ClientWeapons().is_installed())
	{
		m_cs_weapon = (*CMemoryHookCBaseStuff::the().ClientWeapons().get())[state->client.m_iId];

		if (m_cs_weapon != nullptr)
		{
			calc_spread();
		}
	}
}

bool CCSWeapon::is_silenced() const
{
	if (m_weapon_id == WEAPON_M4A1 && state_present(WPNSTATE_M4A1_SILENCED))
		return true;

	if (m_weapon_id == WEAPON_USP && state_present(WPNSTATE_USP_SILENCED))
		return true;

	return false;
}

bool CCSWeapon::is_in_burst() const
{
	if (m_weapon_id == WEAPON_GLOCK18 && state_present(WPNSTATE_GLOCK18_BURST_MODE))
		return true;

	if (m_weapon_id == WEAPON_FAMAS && state_present(WPNSTATE_FAMAS_BURST_MODE))
		return true;

	return false;
}

bool CCSWeapon::can_attack() const
{
	return next_primary_attack_delay() <= 0.0f/* &&
			!IsReloading() &&
			!g_LocalPlayer.IsInFreezePeriod() &&
			g_LocalPlayer.CanShoot() &&
			g_LocalPlayer.Alive()*/;
}

bool CCSWeapon::has_secondary_attack() const
{
	if (CLocalState::the().player_holding_shield())
	{
		return true;
	}

	switch (m_weapon_id)
	{
		case WEAPON_AK47:
		case WEAPON_XM1014:
		case WEAPON_MAC10:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_MP5N:
		case WEAPON_UMP45:
		case WEAPON_M249:
		case WEAPON_M3:
		case WEAPON_TMP:
		case WEAPON_DEAGLE:
		case WEAPON_P228:
		case WEAPON_P90:
		case WEAPON_C4:
		case WEAPON_GALIL:
			return false;
		default:
			break;
	}

	return true;
}

float CCSWeapon::time_since_last_fired() const
{
	return CMemoryHookMgr::the().cl().get()->time - m_last_time_fired;
}

bool CCSWeapon::is_useable() const
{
	if (is_out_of_ammo() && m_clip_size != WEAPON_NO_CLIP)
	{
		// clip is empty (or nonexistant) and the player has no more ammo of this type.
		return false;
	}

	return true;
}

EWeaponAccuracyFlags CCSWeapon::calc_wpn_accuracy_flags() const
{
	uint32_t flags = WPNACCFLAG_NONE;

	switch (m_weapon_id)
	{
		case WEAPON_AUG:
		case WEAPON_GALIL:
		case WEAPON_M249:
		case WEAPON_SG552:
		case WEAPON_AK47:
		case WEAPON_P90:
			flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED;

			break;
		case WEAPON_P228:
		case WEAPON_FIVESEVEN:
		case WEAPON_DEAGLE:
			flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_DUCK;

			break;
		case WEAPON_GLOCK18:
			if (state_present(WPNSTATE_GLOCK18_BURST_MODE))
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_DUCK;
			else
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_DUCK | WPNACCFLAG_MULTIPLY_BY_14_2;

			break;
		case WEAPON_MAC10:
		case WEAPON_UMP45:
		case WEAPON_MP5N:
		case WEAPON_TMP:
			flags = WPNACCFLAG_AIR;

			break;
		case WEAPON_M4A1:
			if (state_present(WPNSTATE_USP_SILENCED))
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED;
			else
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_MULTIPLY_BY_14;

			break;
		case WEAPON_FAMAS:
			if (state_present(WPNSTATE_FAMAS_BURST_MODE))
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED;
			else
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_MULTIPLY_BY_14_2;

			break;
		case WEAPON_USP:
			if (state_present(WPNSTATE_USP_SILENCED))
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_DUCK;
			else
				flags = WPNACCFLAG_AIR | WPNACCFLAG_SPEED | WPNACCFLAG_DUCK | WPNACCFLAG_MULTIPLY_BY_14;

			break;
	}

	return (EWeaponAccuracyFlags)flags;
}

void CCSWeapon::calc_spread()
{
	int playermove_flags = CLocalState::the().get_player_flags();
	float vel_2d = CLocalState::the().get_local_velocity_2d();
	float fov = CLocalState::the().get_fov();

	const float accuracy = get_accuracy();

	switch (m_weapon_id)
	{
		case WEAPON_NONE:
			break;

		case WEAPON_P228:
			if (get_shots_fired() > 1)
				return;

			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 1.5f * (1.0f - accuracy);

			else if (vel_2d > 0)
				m_spread = 0.255f * (1.0f - accuracy);
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.075f * (1.0f - accuracy);
			else
				m_spread = 0.15f * (1.0f - accuracy);

			break;

		case WEAPON_GLOCK:
			break;

		case WEAPON_SCOUT:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.2f;
			else if (vel_2d > 170)
				m_spread = 0.075f;
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.0f;
			else
				m_spread = 0.007f;

			if (fov == 90)
				m_spread += 0.025f;

			break;

		case WEAPON_HEGRENADE:
			break;

		case WEAPON_XM1014:
			break;

		case WEAPON_C4:
			break;

		case WEAPON_MAC10:
			if (FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.03f * accuracy;
			else
				m_spread = 0.375f * accuracy;

			break;

		case WEAPON_AUG:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.4f + 0.035f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.07f + 0.035f;
			else
				m_spread = accuracy * 0.02f;

			break;

		case WEAPON_SMOKEGRENADE:
			break;

		case WEAPON_ELITE:
			if (get_shots_fired() > 1)
				return;

			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 1.3f * (1.0f - accuracy);
			else if (vel_2d > 0)
				m_spread = 0.175f * (1.0f - accuracy);
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.08f * (1.0f - accuracy);
			else
				m_spread = 0.1f * (1.0f - accuracy);

			break;

		case WEAPON_FIVESEVEN:
			if (get_shots_fired() > 1)
				return;

			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 1.5f * (1.0f - accuracy);
			else if (vel_2d > 0)
				m_spread = 0.255f * (1.0f - accuracy);
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.075f * (1.0f - accuracy);
			else
				m_spread = 0.15f * (1.0f - accuracy);

			break;

		case WEAPON_UMP45:
			if (FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.04f * accuracy;
			else
				m_spread = 0.24f * accuracy;

			break;

		case WEAPON_SG550:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.45f * (1.0f - accuracy);
			else if (vel_2d > 0)
				m_spread = 0.15f;
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.04f * (1.0f - accuracy);
			else m_spread = 0.05f * (1.0f - accuracy);

			if (fov == 90)
				m_spread += 0.025f;

			break;

		case WEAPON_GALIL:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.3f + 0.04f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.07f + 0.04f;
			else
				m_spread = accuracy * 0.0375f;

			break;

		case WEAPON_FAMAS:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.3f + 0.030f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.07f + 0.030f;
			else
				m_spread = accuracy * 0.02f;

			if (!is_in_burst())
				m_spread += 0.01f;

			break;

		case WEAPON_USP:
			if (get_shots_fired() > 1)
				return;

			if (is_silenced())
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = 1.3f * (1.0f - accuracy);
				else if (vel_2d > 0)
					m_spread = 0.25f * (1.0f - accuracy);
				else if (FBitSet(playermove_flags, FL_DUCKING))
					m_spread = 0.125f * (1.0f - accuracy);
				else
					m_spread = 0.15f * (1.0f - accuracy);
			}
			else
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = 1.2f * (1.0f - accuracy);
				else if (vel_2d > 0)
					m_spread = 0.225 * (1.0f - accuracy);
				else if (FBitSet(playermove_flags, FL_DUCKING))
					m_spread = 0.08f * (1.0f - accuracy);
				else
					m_spread = 0.1f * (1.0f - accuracy);
			}

			break;

		case WEAPON_GLOCK18:
			if (!is_in_burst() && get_shots_fired() > 1)
				return;

			if (is_in_burst())
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = 1.2f * (1.0f - accuracy);
				else if (vel_2d > 0)
					m_spread = 0.185f * (1.0f - accuracy);
				else if (FBitSet(playermove_flags, FL_DUCKING))
					m_spread = 0.095f * (1.0f - accuracy);
				else
					m_spread = 0.3f * (1.0f - accuracy);
			}
			else
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = 1.0f * (1.0f - accuracy);
				else if (vel_2d > 0)
					m_spread = 0.165f * (1.0f - accuracy);
				else if (FBitSet(playermove_flags, FL_DUCKING))
					m_spread = 0.075f * (1.0f - accuracy);
				else
					m_spread = 0.1f * (1.0f - accuracy);
			}

			break;

		case WEAPON_AWP:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.85f;
			else if (vel_2d > 140)
				m_spread = 0.25f;
			else if (vel_2d > 10)
				m_spread = 0.1f;

			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.0f;
			else m_spread = 0.001f;

			if (fov == 90)
				m_spread += 0.08f;

			break;

		case WEAPON_MP5N:
			if (FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.04f * accuracy;
			else
				m_spread = 0.2f * accuracy;

			break;

		case WEAPON_M249:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.5 + 0.045f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.095f + 0.045f;
			else
				m_spread = accuracy * 0.03f;

			break;

		case WEAPON_M3:
			break;

		case WEAPON_M4A1:
			if (is_silenced())
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = accuracy * 0.4f + 0.035f;
				else if (vel_2d > 140)
					m_spread = accuracy * 0.07f + 0.035f;
				else
					m_spread = accuracy * 0.025f;
			}
			else
			{
				if (!FBitSet(playermove_flags, FL_ONGROUND))
					m_spread = accuracy * 0.4f + 0.035f;
				else if (vel_2d > 140)
					m_spread = accuracy * 0.07f + 0.035f;
				else
					m_spread = accuracy * 0.02f;
			}

			break;

		case WEAPON_TMP:
			if (FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.03f * accuracy;
			else
				m_spread = 0.25f * accuracy;

			break;

		case WEAPON_G3SG1:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 0.45f;
			else if (vel_2d > 0)
				m_spread = 0.15f;
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.035f;
			else
				m_spread = 0.055f;

			if (fov == 90)
				m_spread += 0.025f;

			break;

		case WEAPON_FLASHBANG:
			break;

		case WEAPON_DEAGLE:
			if (get_shots_fired() > 1)
				return;

			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = 1.5f * (1.0f - accuracy);
			else if (vel_2d > 0)
				m_spread = 0.25f * (1.0f - accuracy);
			else if (FBitSet(playermove_flags, FL_DUCKING))
				m_spread = 0.115f * (1.0f - accuracy);
			else
				m_spread = 0.13f * (1.0f - accuracy);

			break;

		case WEAPON_SG552:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.45 + 0.035f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.075 + 0.035f;
			else
				m_spread = accuracy * 0.02f;

			break;

		case WEAPON_AK47:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.4 + 0.04f;
			else if (vel_2d > 140)
				m_spread = accuracy * 0.07 + 0.04f;
			else
				m_spread = accuracy * 0.0275f;

			break;

		case WEAPON_KNIFE:
			break;

		case WEAPON_P90:
			if (!FBitSet(playermove_flags, FL_ONGROUND))
				m_spread = accuracy * 0.3f;
			else if (vel_2d > 170)
				m_spread = accuracy * 0.115f;
			else
				m_spread = accuracy * 0.045f;

			break;
	}
}

void CWeapons::update(hl::local_state_t* state, hl::usercmd_t* cmd)
{
	m_current_weapon_id = (WeaponIdType)state->client.m_iId;
	s_weapons[m_current_weapon_id].update(state, cmd);
}
