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

//
// protocol.h -- communications protocols
//

#ifndef PROTOCOL_H
#define PROTOCOL_H
#pragma once

#define	PROTOCOL_VERSION			48

#define PROTOCOL_INVALID			-1		// invalid protocol value. Defaulted to CD key.
#define PROTOCOL_AUTHCERTIFICATE	0x01	// Connection from client is using a WON authenticated certificate
#define PROTOCOL_HASHEDCDKEY		0x02	// Connection from client is using hashed CD key because WON comm. channel was unreachable
#define PROTOCOL_STEAM				0x03	// Steam certificates
// one unknown protocol here?
#define PROTOCOL_LASTVALID			0x04	// Last valid protocol

#define SIGNED_GUID_LEN				32		// Hashed CD Key (32 hex alphabetic chars + 0 terminator )

// game types sent by serverinfo
// these determine which intermission screen plays
#define	GAME_COOP					0
#define	GAME_DEATHMATCH				1

// Framerate constants (used in Host_FilterTime() & server timings)
#define MIN_FPS						0.5		// Minimal possible framerate - one frame per two seconds
#define MAX_FPS						100.0	// Maximum fps when fps_override is off
#define CONNECTING_FPS				31.0	// Constant fps when connecting to a server
#define MIN_ACTIVE_FPS				20.0	// Minimal framerate when connected to a online server

// Framerate caps
#define MAX_HOST_FRAMETIME			(1.0 / 4.0)		// 0.25 is the maximal frametime (4 fps)
#define MIN_HOST_FRAMETIME			(1.0 / 1000.0)	// 1k fps, highest framerate possible

// Stages in server connection process
#define PROGRESS_CHANGELEVEL		1	// client setups connection with the server
#define PROGRESS_SPAWNSERVER		2	// after the connection has been established
#define PROGRESS_LOADWORLDMODEL		3	// svc_serverinfo, resource precaching begins
#define PROGRESS_CRCCLIENTDLL		4	// client crc-checks client dll against server ones
#define PROGRESS_SECURITYMODULE		5	// VAC1 security modules, obsolete
#define PROGRESS_CRCMAP				6	// client crc-checks bsp map against server ones
#define PROGRESS_PRECACHE			7	// resource precaching
#define PROGRESS_REGISTER			8	// resource registering
#define PROGRESS_PRECACHEWORLD		9	// level precaching
#define PROGRESS_UPLOAD				10	// register upload
#define PROGRESS_BASELINE			11	// created entity baseline
#define PROGRESS_BEGINCONNECT		12	// connection begins, transition
#define PROGRESS_STARTSERVER		24	// starting a server (leave space for server-related types)

// Maximum length in bytes of a user msg name.
#define MAX_USERMSG_NAME			16

// Maximum theoretical user msg size and invalid usermsg size.
#define MAX_USERMSG_SIZE			255
#define MAX_USERMSG_DATA_LEN		192		// Max user message data size
#define USERMSG_INVALID_SIZE		-1		// Invalid size

// This is to check for WeaponLists larger than this value and prevent
// them from executing, if needed.
#define MAX_WEAPONLIST_USERMSG_SIZE 123

// Distance to check over when players seems to "teleport" over frames.
#define ENTITY_TELEPORT_DELTA		128.f

// Used to check if player entity has teleported between frames.
// This is used both on client and server side
#define DID_ENTITY_TELEPORT(a, b)				\
(fabs(a[0] - b[0]) > ENTITY_TELEPORT_DELTA ||	\
 fabs(a[1] - b[1]) > ENTITY_TELEPORT_DELTA ||	\
 fabs(a[2] - b[2]) > ENTITY_TELEPORT_DELTA)

// Largest # of commands to send in a movement packet by CL_Move().
#define CMD_MAXBACKUP_OLD			16		// Old build 5971
#define CMD_MAXBACKUP				64

// max amount of dropped commands that will be sent again
#define CMDDROP_LIMIT				24

// if the client/host have low framerate (0.05s frametime, i.e. 20 frames per second), 
// we try to split the movement packet into smaller parts so we can keep up.
#define MOVECOMMAND_TOOBIG			50

//============================================================================
//
// 
// Server to client commands
//
// 
//=============================================================================

#define	svc_bad						0
#define	svc_nop						1
#define	svc_disconnect				2
#define	svc_event					3	// 
#define	svc_version					4	// [long] server version
#define	svc_setview					5	// [short] entity number
#define	svc_sound					6	// <see code>
#define	svc_time					7	// [float] server time
#define	svc_print					8	// [string] null terminated string
#define	svc_stufftext				9	// [string] stuffed into client's console buffer
										// the string should be \n terminated
#define	svc_setangle				10	// [angle3] set the view angle to this absolute value
#define	svc_serverinfo				11	// [long] version
										// [string] signon string
										// [string]..[0]model cache
										// [string]...[0]sounds cache
#define	svc_lightstyle				12	// [byte] [string]
#define	svc_updateuserinfo			13	// [byte] slot [long] uid [string] userinfo
#define svc_deltadescription		14
#define	svc_clientdata				15	// <shortbits + data>
#define	svc_stopsound				16	// <see code>
#define	svc_pings					17
#define	svc_particle				18	// [vec3] <variable>
#define	svc_damage					19
#define	svc_spawnstatic				20
#define	svc_event_reliable			21
#define svc_spawnbaseline			22
#define svc_tempentity				23
#define	svc_setpause				24	// [byte] on / off
#define	svc_signonnum				25	// [byte]  used for the signon sequence
#define	svc_centerprint				26	// [string] to put in center of the screen
#define	svc_killedmonster			27
#define	svc_foundsecret				28
#define	svc_spawnstaticsound		29	// [coord3] [byte] samp [byte] vol [byte] aten
#define	svc_intermission			30	// [string] music
#define	svc_finale					31	// [string] music [string] text
#define	svc_cdtrack					32	// [byte] track [byte] looptrack
#define svc_restore					33
#define svc_cutscene				34
#define svc_weaponanim				35
#define svc_decalname				36	// [byte] index [string] name
#define	svc_roomtype				37	// [byte] roomtype (dsp effect)
#define	svc_addangle				38	// [angle3] set the view angle to this absolute value
#define svc_newusermsg				39
#define	svc_packetentities			40	// [...]  Non-delta compressed entities
#define	svc_deltapacketentities		41	// [...]  Delta compressed entities
#define	svc_choke   				42	// # of packets held back on channel because too much data was flowing.
#define svc_resourcelist			43
#define svc_newmovevars				44
#define svc_resourcerequest			45
#define svc_customization			46
#define	svc_crosshairangle			47	// [char] pitch * 5 [char] yaw * 5
#define svc_soundfade				48	// char percent, char holdtime, char fadeouttime, char fadeintime
#define svc_filetxferfailed			49
#define svc_hltv					50
#define svc_director				51
#define svc_voiceinit				52	// Initialize voice stuff.
#define svc_voicedata				53	// Voicestream data from the server
#define	svc_sendextrainfo			54
#define svc_timescale				55
#define svc_resourcelocation		56
#define svc_sendcvarvalue			57
#define svc_sendcvarvalue2			58
#define svc_exec					59
#define SVC_LASTMSG					59

// Used as a index into msg_buckets & last_data to store usermsg data.
#define SVC_BUCKET_USERMSG			(MAX_SVC_MESSAGES - 1)

// Theoretical limit of a command number
#define SVC_END_OF_THE_LIST			255

// Limit of how much svc messages the engine can handle atm.
// The theoretical limit is still SVC_END_OF_THE_LIST(255)
#define MAX_SVC_MESSAGES			64

//=============================================================================
//
// 
// Client to server commands
//
// 
//=============================================================================

#define	clc_bad						0
#define	clc_nop 					1
#define	clc_move					2		// [usercmd_t]
#define	clc_stringcmd				3		// [string] message
#define	clc_delta					4
#define	clc_resourcelist			5
#define	clc_tmove					6
#define	clc_fileconsistency			7
#define	clc_voicedata				8
#define	clc_hltv					9
#define	clc_cvarvalue				10
#define	clc_cvarvalue2				11
#define clc_endoflist				255		// the last one

// TODO: use these across code and perhaps rename/change depending on the situation. These afrom 07 leak and can differ...
#define SIGNONSTATE_NONE			0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE		1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED		2	// client is connected to server, netchans ready

//=============================================================================
//
// 
// Backup values
//
// 
//=============================================================================

#define MULTIPLAYER_BACKUP			(1<<6) // How many data slots to use when in multiplayer (must be power of 2)
#define SINGLEPLAYER_BACKUP			(1<<3) // Same for single player

extern int SV_UPDATE_BACKUP; // Copies of entity_state_t to keep buffered
extern int SV_UPDATE_MASK;	 // must be power of two.  Increasing this number costs a lot of RAM.

extern int CL_UPDATE_BACKUP;
extern int CL_UPDATE_MASK;

//=============================================================================
//
// 
// Address ports
//
// 
//=============================================================================

#define	PORT_MASTER					27010   // Default master port
#define PORT_MODMASTER				27011	// Default mod-master port
#define PORT_SECUREMASTER			27012	// Master port for security modules
#define PORT_CLIENT					"27005" // The default port for client
#define PORT_SERVER					"27015" // The default port for server
#define PORT_STEAM					"26900" // Steam default port
#define PORT_MULTICAST				"27025" // Default multicast port
#define PORT_RDKF					"4242"  // Rag Doll Kung Fu, used in SB server filter
#define PORT_LOSTPLANET				"27215" // used in SB server filter

// This is used, unless overridden in the registry
#define VALVE_MASTER_ADDRESS		"half-life.east.won.net:27010"
#define VALVE_SECUREMASTER_ADDRESS	"gridmaster.valvesoftware.com:27012"

//=============================================================================
//
// 
// Out of band message id bytes
//
// These are parsed inside CL_ConnectionlessPacket().
//
// 	M = master, S = server, C = client, A = any
// 	the second character will always be \n if the message isn't a single
// 	byte long (?? not true anymore?)
//
// 
//=============================================================================

// Server is sending heartbeat to Server Master
#define	S2M_HEARTBEAT				'a'	// + challeange + sequence + active + #channels + channels
// Server is ending current game
#define	S2M_SHUTDOWN				'b' // no params

// Requesting for full server list from Server Master
#define	A2M_GET_SERVERS				'c'	// no params

// Master response with full server list
#define	M2A_SERVERS					'd'	// + 6 byte IP/Port list.

// Request for full server list from Server Master done in batches
#define A2M_GET_SERVERS_BATCH		'e' // + in532 uniqueID ( -1 for first batch )

// Master response with server list for channel
#define M2A_SERVER_BATCH			'f' // + int32 next uniqueID( -1 for last batch ) + 6 byte IP/Port list.

// Request for MOTD from Server Master  (Message of the Day)
#define	A2M_GET_MOTD				'g'	// no params

// MOTD response Server Master
#define	M2A_MOTD					'h'	// + string 

// Generic Ping Request
#define	A2A_PING					'i'	// respond with an A2A_ACK

// Generic Ack
#define	A2A_ACK						'j'	// general acknowledgement without info

// Generic Nack with comment
#define	A2A_NACK					'k'	// [+ comment] general failure

// Print to client console.
#define	A2C_PRINT					'l'	// print a message on client

// Client is sending it's CD Key to Client Master
#define S2A_INFO_DETAILED			'm'	// New Query protocol, returns dedicated or not, + other performance info.

// Basic information about the server
#define A2S_INFO					'T'

// Details about each player on the server
#define A2S_PLAYER					'U'

// The rules the server is using
#define A2S_RULES					'V'

#define S2A_INFO_SRC				'I' // new source response (Address, hostname, map, gamedir, gamedescription, active players, maxplayers, protocol)

// Batches for MODS
#define A2M_GETMODLIST				'n'
#define M2A_MODLIST					'o'
#define A2M_SELECTMOD				'p'	// Unused

// Another user is requesting a challenge value from this machine
#define A2A_GETCHALLENGE			'q'	// Request challenge # from another machine
// Challenge response from master.
#define M2A_CHALLENGE				's'	// + challenge value 

// A user is requesting the list of master servers, auth servers, and titan dir servers from the Client Master server
#define A2M_GETMASTERSERVERS		'v' // + byte (type of request, TYPE_CLIENT_MASTER or TYPE_SERVER_MASTER)

// Master server list response
#define M2A_MASTERSERVERS			'w'	// + byte type + 6 byte IP/Port List

#define A2M_GETACTIVEMODS			'x' // + string Request to master to provide mod statistics ( current usage ).  "1" for first mod.

#define M2A_ACTIVEMODS				'y' // response:  modname\r\nusers\r\nservers

#define M2M_MSG						'z' // Master peering message

// SERVER TO CLIENT/ANY

// Client connection is initiated by requesting a challenge value
//  the server sends this value back
#define S2C_CHALLENGE				'A' // + challenge value

// Server notification to client to commence signon process using challenge value.
#define	S2C_CONNECTION				'B' // no params

// Response to server info requests
#define S2A_INFO					'C' // + Address, hostname, map, gamedir, gamedescription, active players, maxplayers, protocol
#define S2A_PLAYER					'D' // + Playernum, name, frags, /*deaths*/, time on server

// Request for detailed server/rule information.
#define S2A_RULES					'E' // + number of rules + string key and string value pairs

#define M2A_ACTIVEMODS2				'F' // response:  modname\r\key\value\r\end\\

// Proxy sends multicast IP or 0.0.0.0 if he's not broadcasting the game
#define S2C_LISTEN					'G'	// + IP x.x.x.x:port

#define S2A_INFOSTRING				'H' // + key value set
// MASTER TO MASTER

#define S2M_GETFILE					'J'	// request module from master
#define M2S_SENDFILE				'K'	// send module to server

#define S2C_REDIRECT				'L'	// + IP x.x.x.x:port, redirect client to other server/proxy 

#define	C2M_CHECKMD5				'M'	// player client asks secure master if Module MD5 is valid
#define M2C_ISVALIDMD5				'N'	// secure servers answer to C2M_CHECKMD5

// MASTER TO SERVER
#define M2S_REQUESTRESTART			'O' // HLMaster rejected a server's connection because the server needs to be updated

#define M2A_ACTIVEMODS3				'P' // response:  keyvalues struct of mods
#define A2M_GETACTIVEMODS3			'Q' // get a list of mods and the stats about them

#define S2A_LOGSTRING				'R'	// send a log string
#define S2A_LOGKEY					'S'	// send a log event as key value

#define S2M_HEARTBEAT2				'0' // New style heartbeat

#define A2M_GET_SERVERS_BATCH2		'1' // New style server query

#define A2M_GETACTIVEMODS2			'2' // New style mod info query

#define C2S_AUTHREQUEST1			'3' // 
#define S2C_AUTHCHALLENGE1			'4' //
#define C2S_AUTHCHALLENGE2			'5' //
#define S2C_AUTHCOMPLETE			'6'
#define C2S_AUTHCONNECT				'7'  // Unused, signals that the client has
										 // authenticated the server
#define S2C_BADPASSWORD				'8' // Special protocol for bad passwords.

#define S2C_CONNREJECT				'9'  // Special protocol for rejected connections.

#define S2C_CONNREJECT				'9'  // Special protocol for rejected connections.

// Required postfix to a A2S_INFO query
#define A2S_KEY_STRING				"Source Engine Query"

// All OOB packet start with this sequence. Written by Netchan_OutOfBand().
#define CONNECTIONLESS_HEADER		0xFFFFFFFF // -1

// Used when split packet is being transfered.
#define SPLITPACKET_HEADER			0xFFFFFFFE // -2

// The new S2A_INFO_SRC packet has a byte at the end that has these bits in it, telling
// which data follows.
#define S2A_EXTRA_DATA_HAS_GAME_PORT		0x80	// Next 2 bytes include the game port.
#define S2A_EXTRA_DATA_HAS_SPECTATOR_DATA	0x40	// Next 2 bytes include the spectator port, then the spectator server name.
#define S2A_EXTRA_DATA_HAS_GAMETAG_DATA		0x20	// Next bytes are the game tag string
#define S2A_EXTRA_DATA_HAS_STEAMID			0x10	// Next 8 bytes are the steamID
#define S2A_EXTRA_DATA_GAMEID				0x01	// Next 8 bytes are the gameID of the server

#endif // PROTOCOL_H