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

// client.h

#ifndef CLIENT_H
#define CLIENT_H
#pragma once

//=============================================================================
// 
// 
// Client-side constants
// 
// 
//=============================================================================

// stats are integers communicated to the client by the server
#define	MAX_CL_STATS			32
#define	STAT_HEALTH				0
//define STAT_FRAGS				1
#define	STAT_WEAPON				2
#define	STAT_AMMO				3
#define	STAT_ARMOR				4
//define STAT_WEAPONFRAME		5
#define	STAT_SHELLS				6
#define	STAT_NAILS				7
#define	STAT_ROCKETS			8
#define	STAT_CELLS				9
#define	STAT_ACTIVEWEAPON		10
#define	STAT_TOTALSECRETS		11
#define	STAT_TOTALMONSTERS		12
#define	STAT_SECRETS			13		// bumped on client side by svc_foundsecret
#define	STAT_MONSTERS			14		// bumped by svc_killedmonster
#define	STAT_ITEMS				15
//define STAT_VIEWHEIGHT		16

#define	MAX_DLIGHTS				32
#define	MAX_ELIGHTS				64		// entity only point lights

#define	MAX_TEMP_ENTITIES		500		// lightning bolts, etc
#define	MAX_STATIC_ENTITIES		32		// torches, etc

#define	MAX_MAPSTRING			2048
#define	MAX_DEMOS				32
#define	MAX_DEMONAME			16

// The amount of seconds till another connect attempt will be send by client.
#define MAX_PASSIVE_RESEND_TIME	30.0	// Used when cl.passive is on
#define MAX_RESEND_TIME			20.0	// Cap for cvar cl_resend
#define MIN_RESEND_TIME			1.5		// Cap for cvar cl_resend

// The amount of seconds till we send another heartbeat to the server
// so that the server know we're still connected. 
#define HEARTBEAT_RESEND_TIME	5.f		// Now obsolete

// Maximum amount of connect retries before we time out
#define MAX_CONNECT_RETRIES		4

// If we get more than 250 messages in the incoming buffer queue, dump 
// any above this #
#define MAX_INCOMING_MSGS		250

// If the network connection hasn't been active in this many seconds, 
// display text on the screen that the connection was lost.
#define MAX_LAST_RECEIVED_TIME	15.0

// Highest wait tolerance when we're connected and the server changed
// level. If we exceed this amount of time, we will disconnect display-
// ing a message to the user. Processed in the renderer.
#define LVL_CHANGE_WAIT_TOLERANCE 120	// seconds

#define MAX_STARTUP_TIMINGS		32		// Size of g_StartupTimings array.

// Max size needed to contain a steam authentication key (both server 
//	and client)
#define STEAM_AUTH_KEYSIZE		2048

// After # amount of seconds, recalculate packet loss.
#define PACKETLOSS_RECALCTM		1.0		// seconds

// Maximum duration of command. usercmd_t::msec is being capped to this.
#define MAX_COMMAND_DURATION	255		// milliseconds (~3.9 fps)

// cl_updaterate cvar bounds
#define CL_MIN_UPDATERATE		10.0f
#define CL_MAX_UPDATERATE		102.0f

// cl_cmdrate cvar bounds
#define CL_MIN_CMD_RATE			10.0f

// Used as an array size fore oldcmd, which is used when parsing server
// message.
#define CL_SVC_CMD_COUNT		32 // Must be power of two
#define CL_SVC_CMD_MASK			(CL_SVC_CMD_COUNT - 1)

// Holds the value for cl.connect_time after the user has dc'ed
#define CONTIME_AFTER_DC		-99999.0f

// 
// Prediction
// 

// Prediction error correction
#define MIN_CORRECTION_DISTANCE	0.25f	// use smoothing if error is > this
#define PREDICTION_TOLERANCE	0.05f	// complain if error is > this and we have cl_showerror set
#define MAX_PREDICTION_ERROR	64.0f	// above this is assumed to be a teleport, don't smooth, etc.

// Interpolation
#define MIN_INTERPOLATE_MS		50.0f
#define MAX_INTERPOLATE_MS		100.0f
#define MAX_INTERPOLATE_SPEC_MS	200.0f

//=============================================================================
// 
// 
// Client-side structures
// 
// 
//=============================================================================

#define	MAX_STYLESTRING 64
typedef struct
{
	int				length;
	char			map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct
{
	qboolean		bUsed;
	float			fTime;
	int				nBytesRemaining;
} downloadtime_t;

typedef struct
{
	qboolean		doneregistering;
	int				percent;
	qboolean		downloadrequested;

	downloadtime_t	rgStats[t_count];

	int				nCurStat;
	// The total size of resources we need at this moment.
	int				nTotalSize;
	// The total download size.
	int				nTotalToTransfer;
	int				nRemainingToTransfer;

	// Updates every time CL_StartResourceDownloading() is called.
	float			fLastStatusUpdate;

	qboolean		custom;
} incomingtransfer_t;

typedef struct
{
	int				nStartPercent;
	int				nClientSoundFadePercent;

	double			soundFadeStartTime;

	int				soundFadeOutTime;
	int				soundFadeHoldTime;
	int				soundFadeInTime;
} soundfade_t;

// Connection state
typedef enum
{
	ca_dedicated,		// A dedicated server with no ability to start a client
	ca_disconnected,	// Full screen console with no connection
	ca_connecting,		// Challenge requested, waiting for response or to resend connection request.
	ca_connected,		// valid netcon, talking to a server, waiting for server data
	ca_uninitialized,	// valid netcon, autodownloading
	ca_active			// d/l complete, ready game views should be displayed
} cactive_t;

// the client_static_t structure is persistant through an arbitrary number
// of server connections
typedef struct
{
	// Current connection state
	cactive_t	state;

	// Our sequenced channel to the remote server.
	netchan_t	netchan;

	// Unreliable stuff. Gets sent in CL_Move about cl_cmdrate times per second.
	sizebuf_t	datagram; // Sets to MAX_DATAGRAM (4000) in CL_Init
	byte		datagram_buf[MAX_DATAGRAM];

	//
	// Connection to a server
	//

	// How long it took to connect to a server.
	// Doesn't update every frame afterwards.
	// After disconnect, this sets to -99,999.0f
	double		connect_time;

	int			connect_retry;

	int			challenge;

	byte		authprotocol;

	int			userid;

	char		trueaddress[32];

	float		slist_time;

	//
	// connection information
	//

	int			signon;

	char		servername[FILENAME_MAX];	// name of server from original connect
	char		mapstring[MAX_QPATH];

	char		spawnparms[MAX_MAPSTRING];

	// personalization data sent to server	
	char		userinfo[MAX_INFO_STRING];

	// When can we send the next command packet?
	float		nextcmdtime;
	// Sequence number of last outgoing command
	int			lastoutgoingcommand;

	//
	// demo loop control
	//
	int			demonum;						// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];	// when not playing

	// demo recording info must be here, because record is started before
	// entering a map (and clearing client_state_t)
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;		// true when playing a time-based demo

	float		demostarttime;
	int			demostartframe;

	int			forcetrack;			// -1 = use normal cd track

	FileHandle_t demofile;	 // Active demo file
	FileHandle_t demoheader; // demoheader.hdf

	qboolean	demowaiting;
	qboolean	demoappending;

	char		demofilename[FILENAME_MAX];

	int			demoframecount;

	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo

	incomingtransfer_t dl;

	float		packet_loss;				// Packet Loss
	double		packet_loss_recalc_time; // When the packet loss was recalculated

	int			playerbits;

	soundfade_t soundfade;

	char		physinfo[MAX_PHYSINFO_STRING];

	byte		md5_clientdll[16];

	netadr_t	game_stream;
	netadr_t	connect_stream;

	qboolean	passive;
	qboolean	spectator;
	qboolean	director;
	qboolean	fSecureClient;
	qboolean	isVAC2Secure;

	uint64		GameServerSteamID;

	int			build_num;
} client_static_t;

// Client's now store the command they sent to the server and the 
// entire results set of that command. 
typedef struct
{
	// Data received from server
	double			receivedtime;	// Time message from server was received, or -1 (packet was lost)
									//  receivedtime - senttime = latency.
	double			latency;

	qboolean		invalid;		// True if the packet_entities delta was invalid for some reason.
	qboolean		choked;

	entity_state_t	playerstate[MAX_CLIENTS];
	// Server timestamp
	double			time;

	clientdata_t	clientdata;
	weapon_data_t	weapondata[MULTIPLAYER_BACKUP];

	// The packet of entities sent from the server.
	packet_entities_t packet_entities;

	unsigned short	clientbytes;		// Client data ( local player only )
	unsigned short	playerinfobytes;	// # of bytes of message that were occupied by player's data
	unsigned short	packetentitybytes;	// # of bytes that were occupied by packet entities data.
	unsigned short	tentitybytes;		// Tempent bytes
	unsigned short	soundbytes;
	unsigned short	eventbytes;
	unsigned short	usrbytes;			// Usermsg bytes
	unsigned short	voicebytes;
	unsigned short	msgbytes;			// Total size of message
} frame_t;

// cmd_t structure
#include "client_command.h"

#define MAX_LEVELNAME 40

// the client_state_t structure is wiped completely at every server 
// signon
typedef struct
{
	// Set depending on -num_edicts. Calculated inside COM_EntsForPlayerSlots().
	int			max_edicts;

	resource_t	resourcesonhand;
	resource_t	resourcesneeded;
	// Resource download list.
	resource_t	resourcelist[MAX_RESOURCES];
	// Amount of resources inside resourcelist.
	int			num_resources;

	// Set inside CL_ParseConsistencyInfo() and CL_SendConsistencyInfo().
	// TRUE if the server wants us to parse/send consistency information.
	qboolean	need_force_consistency_response;

	// Obsolete. Can be set using 'fullserverinfo' console command.
	char		serverinfo[512];

	// The amount of started servers since .exe launch
	int			servercount;

	// This is the sequence number of the last good world snapshot/update 
	// we got. If this is 0, we can't render a frame yet
	// Set when parsing packet entities.
	int			validsequence;

	int			parsecount;		// server message counter
	int			parsecountmod;	// equals to parsecount & CL_UPDATE_MASK

	// information for local display
	int			stats[MAX_CL_STATS]; // health, etc

	// Filled with clientdata_t::weapons when client data is being parsed.
	int			weapons;

	// last command sent to the server, obtained by the CreateMove client dll function.
	usercmd_t	cmd;

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame. The server sets punchangle when
	// the view is temporarliy offset, and an angle reset commands at the start
	// of each level and after teleporting.
	vec3_t		viewangles;
	vec3_t		punchangle;
	vec3_t		crosshairangle; // Set inside CL_Parse_CrosshairAngle(), then added to ref.viewangles
	
	//
	// Prediction data
	//

	// Simulated origin, velocity & angles on clientside
	vec3_t		simorg, simvel, simangles;
	vec3_t		predicted_origins[MULTIPLAYER_BACKUP];
	// How much error there was when predicting the origin clientside
	// vs the actual origin acknowledged by the server.
	vec3_t		prediction_error;

	// pitch drifting vars. Set inside CL_SetIdealPitch().
	// Affected by cl_idealpitchscale.value
	float		idealpitch;

	// Origin of clients camera set by the client dll.
	vec3_t		viewheight;

	screenfade_t sf;

	// TRUE when we're paused. At this point the 'paused' text
	// is rendered in the center of the screen by Draw_CenterPic()
	// and audio is not played at all.
	qboolean	paused;

	int			onground;
	// TRUE when the delta between current & previous state's X&Y origin is non-null.
	int			moving;
	int			waterlevel; // Set by the client dll
	int			usehull;
	float		maxspeed;	// Set by the client dll
	int			pushmsec;	// Set by the client dll

	// light level at player's position including dlights
	// this is sent back to the server each frame
	// architectually ugly but it works
	int			light_level;// Average color of all three RBG colors

	// don't change view angle, full screen, etc
	int			intermission;

	// timestamps of last two messages. Received by the server when
	// svc_time is issued every frame.
	double		mtime[2];

	//
	// Client time. Updated in CL_ReadPackets().
	//
	
	double		time;		// Client clock
	double		oldtime;	// Old Client clock

	// These are the frames used to generate client side prediction with. 
	// The frames also contain the state of all other entities and players, 
	// so that the visible entities list can be repopulated correctly.
	frame_t		frames[MULTIPLAYER_BACKUP];
	cmd_t		commands[MULTIPLAYER_BACKUP];

	local_state_t predicted_frames[MULTIPLAYER_BACKUP];

	// Acknowledged sequence number
	int			delta_sequence;

	//
	// information that is static for the entire time connected to a server
	//
	int			playernum; // player entity. skips world. Add 1 to get cl_entitites index;

	event_t		event_precache[MAX_EVENTS];

	model_t*	model_precache[MAX_MODELS];
	int			model_precache_count;

	sfx_t*		sound_precache[MAX_SOUNDS];

	consistency_t consistency_list[MAX_CONSISTENCY];
	int			num_consistency;

	// Used to track the highest index for baselines
	int			highentity;

	// for display on solo scoreboard
	char		levelname[MAX_LEVELNAME];
	// Received from the server when svc_serverinfo is issued. Tells
	// How many clients the server accepts.
	int			maxclients;

	// TRUE when coop.value is 0.f and deathmatch.value isn't 0.f.
	// Sent by the server when svc_serverinfo is issued.
	int			gametype;		// deprecated

	// refresh related state
	int			viewentity;		// cl_entitites[cl.viewentity] == player point of view

	model_t*	worldmodel;		// cl_entitites[0].model
	efrag_t*	free_efrags;
	// Number of entities referenced. Incremented each time CL_EntityNum() is called.
	int			num_entities;	// held in cl_entities array
	int			num_statics;	// held in cl_staticentities array. Deprecated.
	// Entity of viewmodel. Processed locally on clientside.
	cl_entity_t viewent;

	// Set when svc_serverinfo is issued. Both set to gGlobalVariables.cdAudioTrack.
	int			cdtrack;
	int			looptrack;

	// To determine if client is playing hacked .map. (entities lump is skipped)
	// Obtained from the server upon issuing svc_serverinfo message.
	CRC32_t		serverCRC;

	// Filled when svc_serverinfo is issued. Filled with server's client dll CRC.
	// Set when svc_serverinfo is issued.
	byte		clientdllmd5[16];

	// Data related to weapon animation.
	float		weaponstarttime;
	int			weaponsequence;

	// For sound precaching. Used inside S_LoadStreamSound().
	qboolean	fPrecaching;

	// Local player flashlight
	dlight_t*	pLight;

	// Player information for self and other players.  Used for client side for prediction.
	player_info_t players[MAX_CLIENTS];

	entity_state_t instanced_baseline[MULTIPLAYER_BACKUP];
	int			instanced_baseline_number;

	CRC32_t		mapCRC;	// client's map CRC value, CL_CheckCRCs() checks this with cl.serverCRC to make sure
						// that the client and server play the same map
	event_state_t events;

	// Server fills this when svc_resourcelocation is issued. Is used as
	// the primary url for resource downloading.
	char		downloadUrl[128];
} client_state_t;

#endif // CLIENT_H