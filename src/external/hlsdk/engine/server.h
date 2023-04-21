//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================

#ifndef SERVER_H
#define SERVER_H
#pragma once

#define DEFAULT_SOUND_PACKET_VOLUME			255
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0f
#define DEFAULT_SOUND_PACKET_PITCH			100

// Sound flags
#define SND_FL_VOLUME			(1 << 0)	// send volume
#define SND_FL_ATTENUATION		(1 << 1)	// send attenuation
#define SND_FL_LARGE_INDEX		(1 << 2)	// send sound number as short instead of byte
#define SND_FL_PITCH			(1 << 3)	// send pitch
#define SND_FL_SENTENCE			(1 << 4)	// set if sound num is actually a sentence num
#define SND_FL_STOP				(1 << 5)	// stop the sound
#define SND_FL_CHANGE_VOL		(1 << 6)	// change sound vol
#define SND_FL_CHANGE_PITCH		(1 << 7)	// change sound pitch
#define SND_FL_SPAWNING			(1 << 8)	// we're spawning, used in some cases for ambients (not sent across network)

// Message send destination flags
#define MSG_FL_NONE				0			// No flags
#define MSG_FL_BROADCAST		(1 << 0)	// Broadcast?
#define MSG_FL_PVS				(1 << 1)	// Send to PVS
#define MSG_FL_PAS				(1 << 2)	// Send to PAS
#define MSG_FL_ONE				(1 << 7)	// Send to single client

#define MAX_SIGNON				32768

#define MAX_MULTICAST			1024 // Max length of a reliable message

// Max # of master servers this server can be associated with
typedef enum
{
	ss_dead,	// no server at all
	ss_loading,	// spawning a server
	ss_active	// client is fully in game
} server_state_t;

// Mod Gameplay Type
typedef enum
{
	BOTH = 0,
	SINGLEPLAYER_ONLY,
	MULTIPLAYER_ONLY
} MOD_GAMEPLAY_TYPE_E;

#define MODINFO_VERSION 1

// Mod information
typedef struct
{
	qboolean		bIsMod;

	char			szInfo[256];
	char			szDL[256];
	char			szHLVersion[32];

	int				version;
	int				size;

	qboolean		svonly;
	qboolean		cldll;
	qboolean		secure;

	MOD_GAMEPLAY_TYPE_E type;

	int				num_edicts;

	qboolean		clientcrccheck;
} modinfo_t;

void SV_ResetModInfo();

// received from client
typedef struct
{
	// Time world sample was sent to client.
	double			senttime;
	// Realtime when client ack'd the update.
	float			ping_time;

	clientdata_t	clientdata;
	weapon_data_t	weapondata[MAX_CLIENT_WEAPONS];

	// State of entities this frame from the POV of the client.
	packet_entities_t entities;
} client_frame_t;

// representation of single client entity connected to a server
typedef struct
{
	qboolean		active;					// false = client is free
	qboolean		spawned;				// false = don't send datagrams
	qboolean		fully_connected;		// true = client has fully connected, set after sendents command is received
	qboolean		connected;				// Has been assigned to a client_t, but not in game yet
	qboolean		uploading;				// true = client uploading custom resources
	qboolean		hasusrmsgs;				// Whether this client has received the list of user messages
	qboolean		has_force_unmodified;	// true = mp_consistency is set and at least one file is forced to be consistent

	//===== NETWORK ============
	netchan_t		netchan;				// The client's net connection.
	int				chokecount;				// amount of choke since last client message

	int				delta_sequence;			// -1 = no compression.  This is where the server is creating the
											// compressed info from.

	qboolean		fakeclient;				// Bot
	qboolean		proxy;					// HLTV proxy

	usercmd_t		lastcmd;				// for filling in big drops and partial predictions

	double			connecttime;			// Time at which client connected, this is the time after "spawn" is sent, not initial connection
	double			cmdtime;				// Time since connecttime that last usercmd was received
	double			ignorecmdtime;			// Time until which usercmds are ignored

	float			latency;				// Average latency
	float			packet_loss;			// Packet loss suffered by this client

	double			localtime;				// of last message
	double			nextping;				// next time to recalculate ping for this client
	double			svtimebase;				// Server timebase for the client when running movement

	// the datagram is written to after every frame, but only cleared
	// when it is sent out to the client.  overflow is tolerated.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];	

	double			connection_started;		// or time of disconnect for zombies TODO verify that zombies still exist
	double			next_messagetime;		// Earliest time to send another message
	double			next_messageinterval;	// Minimum interval between messages

	qboolean		send_message;			// set on frames a datagram arived on
	qboolean		skip_message;			// Skip sending message next frame

	client_frame_t* frames;					// updates can be deltad from here

	event_state_t	events;

	edict_t*		edict;					// EDICT_NUM(clientnum+1)
	const edict_t*	pViewEntity;			// View entity, equal to edict if not overridden

	int				userid;					// identifying number

	USERID_t		network_userid;

	char			userinfo[MAX_INFO_STRING];// infostring
	qboolean		sendinfo;				// at end of frame, send info to all
											// this prevents malicious multiple broadcasts
	float			sendinfo_time;			// Time when userinfo was sent

	char			hashedcdkey[64];		// Hashed cd key from user. Really the user's IP address in IPv4 form
	char			name[32];				// for printing to other people
											// extracted from userinfo

	int				topcolor;				// top color for model
	int				bottomcolor;			// bottom color for model

	int				entityId;				// unused TODO verify

	resource_t		resourcesonhand;		// Head of resources accounted for list
	resource_t		resourcesneeded;		// Head of resources to download list

	FileHandle_t	upload;					// Handle of file being uploaded

	qboolean		uploaddoneregistering;	// If client files have finished uploading

	customization_t customdata;				// Head of custom client data list

	int				crcValue;				// checksum for calcs

	int				lw;						// If user is predicting weapons locally (cl_lw)
	int				lc;						// If user is lag compensating (cl_lc)

	char			physinfo[MAX_PHYSINFO_STRING];// Physics info string

	qboolean		m_bLoopback;			// Does this client want to hear his own voice?

	uint32			m_VoiceStreams[2];		// Bit mask for whether client is listening to other client TODO 64 clients? 
	double			m_lastvoicetime;		// Last time client voice data was processed on server

	int				m_sendrescount;			// Count of times resources sent to client

	qboolean		m_bSentNewResponse;
} client_t;

// log messages are used so that fraglog processes can get stats
typedef struct
{
	qboolean		active;					// Is the log file active?
	qboolean		net_log;				// Are we logging to a remote address?

	netadr_t		net_address;			// Remote address to log to

	void*			file;					// Handle to the log file
} server_log_t;

// server statistics
typedef struct
{
	int				num_samples;			// Total number of samples taken over server lifetime
	int				at_capacity;			// Number of samples where server was filled to capacity (numclients == maxclients)
	int				at_empty;				// Number of samples where server was empty (numclients <= 1, singleplayer counts as empty)

	float			capacity_percent;		// Percentage of time that server was at capacity
	float			empty_percent;			// Percentage of time that server was empty

	int				minusers;				// Lowest number of players on server at any time
	int				maxusers;				// Highest number of players on server at any time

	float			cumulative_occupancy;	// Cumulative occupancy level over time
	float			occupancy;				// Average occupancy

	int				num_sessions;			// Number of client sessions (clients that joined and left, and were on server for more than a minute)

	float			cumulative_sessiontime;	// Total amount of time spent on server by all clients with recorded session
	float			average_session_len;	// Average length of a single client session
	float			cumulative_latency;		// Cumulation of average latency for all clients per sample
	float			average_latency;		// Average latency for all clients over server lifetime
} server_stats_t;

// persistant server info
typedef struct
{
	qboolean		dll_initialized;		// Have we loaded the game dll.

	client_t*		clients;				// array of up to [maxclients] client slots

	int				maxclients;				// Current max #
	int				maxclientslimit;		// Max allowed on server.
	int				spawncount;				// Number of servers spawned since start,
											// used to check late spawns (e.g., when d/l'ing lots of
											// data)

	int				serverflags;

	server_log_t	log;

	double			next_cleartime;			// Next time to clear stats
	double			next_sampletime;		// Next time to gather stat samples

	server_stats_t	stats;					// Server statistics

	qboolean		isSecure;
} server_static_t;

typedef struct
{
	qboolean		active;					// false if only a net client
	qboolean		paused;
	qboolean		loadgame;				// handle connections specially

	double			time;
	double			oldtime;

	int				lastcheck;
	double			lastchecktime;

	char			name[MAX_QPATH];		// map name
	char			oldname[MAX_QPATH];
	char			startspot[MAX_QPATH];
	char			modelname[MAX_QPATH];	// maps/<name>.bsp, for model_precache[0]

	model_t*		worldmodel;
	CRC32_t			worldmapCRC;
	byte			clientdllmd5[16];

	resource_t		resourcelist[MAX_RESOURCES];// Resource download list
	int				num_resources;

	consistency_t	consistency_list[MAX_CONSISTENCY];
	int				num_consistency;

	char*			model_precache[MAX_MODELS];// List of models that have been precached
	model_t*		models[MAX_MODELS];
	byte			model_precache_flags[MAX_MODELS];

	event_t			event_precache[MAX_EVENTS];

	char*			sound_precache[MAX_SOUNDS];
	short			sound_precache_hashedlookup[SOUNDS_HASHLOOKUP_SIZE];
	qboolean		sound_precache_hashedlookup_built;

	char*			generic_precache[MAX_GENERIC];
	char			generic_precache_names[MAX_GENERIC][MAX_QPATH];

	int				num_generic_names;

	char*			lightstyles[MAX_LIGHTSTYLES];
	int				num_edicts;
	int				max_edicts;
	edict_t*		edicts;					// Can array index now, edict_t is fixed

	entity_state_t*		baselines;			// parallel array to sv.edicts, storing entity baselines
	extra_baselines_t*	instance_baselines;
	server_state_t		state;

	// Unreliable data to send to clients.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];

	// Reliable data to send to clients.
	sizebuf_t		reliable_datagram;		// copied to all clients at end of frame
	byte			reliable_datagram_buf[MAX_DATAGRAM];

	sizebuf_t		multicast;
	byte			multicast_buf[MAX_MULTICAST]; // Longest multicast message

	sizebuf_t		spectator;
	byte			spectator_buf[MAX_MULTICAST];

	sizebuf_t		signon;
	byte			signon_data[MAX_SIGNON];

} server_t;

//=============================================================================

// console.cpp
enum redirect_t
{
	RD_NONE = 0,
	RD_CLIENT,
	RD_PACKET
};

enum sv_delta_t
{
	sv_packet_nodelta,
	sv_packet_delta,
};

#endif // SERVER_H