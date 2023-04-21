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
// net.h -- Half-Life's interface to the networking layer
//
// For banning IP addresses (or allowing private games)
//

#ifndef NET_H
#define NET_H
#pragma once

// Use this to pick apart the network stream, must be packed
#pragma pack(1)
struct SPLITPACKET
{
	int		netID;			// SPLITPACKET_HEADER
	int		sequenceNumber;
	byte	packetID;
};
#pragma pack()

#ifndef PAD_NUMBER
// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER( num, boundary )	((( num ) + (( boundary ) - 1 )) / ( boundary )) * ( boundary )
#endif

// Flow control bytes per second limits
#define	MAX_RATE				100000.0f
#define	MIN_RATE				  1000.0f
#define DEFAULT_RATE			 30000.0f

// NETWORKING INFO //

// The maximum size for a packet that was split
#define MIN_ROUTEABLE_PACKET	  16
#define MAX_ROUTEABLE_PACKET	1400 // Ethernet 1518 - ( CRC + IP + UDP header)
#define SPLIT_SIZE				(MAX_ROUTEABLE_PACKET - sizeof(SPLITPACKET)) // 1,391

// This is the packet payload without any header bytes (which are attached for actual sending)
#define MAX_POSSIBLE_MSG		0x10000   // 65,536

#define NET_MAX_RELIABLE_PAYLOAD 1200

#define NET_WS_MAX_FRAGMENTS	5

// Limit of fragment size not depending on who -> who
#define FRAGMENT_MAX_SIZE		(1 << 10) //  1,024

#define FRAGMENT_C2S_MIN_SIZE	(1 << 4)  //     16 - client -> server min
#define FRAGMENT_S2C_MIN_SIZE	(1 << 8)  //    256 - server -> client min
#define FRAGMENT_S2C_MAX_SIZE	(1 << 10) //  1,024 - server -> client max

#define CL_FRAGSIZE_ON_CONNECT	(1 << 7)  //    128
#define CUSTOMIZATION_MAX_SIZE	0x5000    // 20,480

enum // Stream types
{
	FRAG_NORMAL_STREAM = 0,
	FRAG_FILE_STREAM,
};

// 0 == regular, 1 == file stream
#define MAX_STREAMS				2

// Client sends normal fragments only while connecting
#define MAX_NORMAL_FRAGMENTS	(MAX_POSSIBLE_MSG / CL_FRAGSIZE_ON_CONNECT)

// While client is connecting it sending fragments with minimal size, also it transfers sprays with minimal fragments...
// But with sv_delayed_spray_upload it sends with cl_dlmax fragment size
#define MAX_FILE_FRAGMENTS		(CUSTOMIZATION_MAX_SIZE / FRAGMENT_C2S_MIN_SIZE)

// Max length for a message not counting padding byte and packet header
#define MAX_DATAGRAM			4000

// Max length of a reliable message
#define MAX_MSGLEN				(MAX_DATAGRAM - (sizeof(SPLITPACKET) + 1)) // 3990

// Max size of udp packet payload
#define MAX_UDP_PACKET			(MAX_DATAGRAM + (sizeof(SPLITPACKET) + 1)) // 4010

// The size of packet header for every stream
// Calucalted by 4-byte aligned size of SPLITPACKTET (12) + one byte alignment, 
// this all for every streams (times MAX_STREAMS) and finally 4 bytes of incomming and outgoing sequence.
#define HEADER_BYTES			((PAD_NUMBER(sizeof(SPLITPACKET), 4) + 1) * MAX_STREAMS + (sizeof(int) * 2)) // 34

// Pad this to next higher 16-byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
// bytes will be stripped by the networking channel layer
#define NET_MAX_MESSAGE			PAD_NUMBER((MAX_MSGLEN + HEADER_BYTES), MIN_ROUTEABLE_PACKET) + NET_WS_MAX_FRAGMENTS // 4037

// Data flows
#define MAX_FLOWS				2
#define FLOW_OUTGOING			0
#define FLOW_INCOMING			1

// if address is 127.0.0.1
#define NET_IS_LOCALHOST_IP(ip) (ip[0] == 127 && ip[1] == 0 && ip[2] == 0 && ip[3] == 1)

// 224.0.0.0 to 239.255.255.255
#define NET_IS_D_IP_RANGE(ip)	((ip[0] >= 224 && ip[0] <= 239) ? TRUE : FALSE)

#define RESOURCE_MIN_DL_SIZE	(1 << 10)			// 1,024
#define RESOURCE_MAX_DL_SIZE	((1 << 11) * 10)	// 20,480

//-----------------------------------------------------------------------------
// Purpose: Address type
//-----------------------------------------------------------------------------
enum netadrtype_t
{
	NA_NULL,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX,
};

//-----------------------------------------------------------------------------
// Purpose: Representation of an IPv4 address
//-----------------------------------------------------------------------------
struct netadr_t
{
	netadrtype_t	type;
	unsigned char	ip[4];		// XXX.XXX.XXX.XXX
	unsigned char	ipx[10];	// XXXXXXXX:XXXXXXXXXXXX:XXXXX
	unsigned short	port;
};

//-----------------------------------------------------------------------------
// Purpose: Socket source
//-----------------------------------------------------------------------------
enum netsrc_t
{
	NS_CLIENT,
	NS_SERVER,
	NS_MULTICAST,	// xxxMO
	NS_COUNT
};

// Network API that we expose to client dlls and others.
// net_api.c
//#include "net_api.h"

//=============================================================================
//
// 
// NETCHAN constants
//
//	net_chan.c
// 
//=============================================================================

#define MAX_FRAGMENTS			25000

#define MAX_FRAGMENT_LENGTH		(1 << 11) //  2,048
#define MAX_FRAGMENT_OFFSET		(1 << 14) // 16,384

#define MAKE_FRAGID(id,count)	(((id & 0xffff) << 16) | (count & 0xffff))
#define FRAG_GETID(fragid)		((fragid >> 16) & 0xffff)
#define FRAG_GETCOUNT(fragid)	(fragid & 0xffff)

#define UDP_HEADER_SIZE			(20 + 8) // IP = 20, UDP = 8

// How fast to converge flow estimates
#define FLOW_AVG				(2.0 / 3.0)

#define	MAX_LATENT				32

//-----------------------------------------------------------------------------
// Purpose: Maitained message data.
//-----------------------------------------------------------------------------
struct flowstats_t
{
	// Size of message sent/received
	int		size;
	// Time that message was sent/received
	double	time;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
struct flow_t
{
	// Data for last MAX_LATENT messages
	flowstats_t stats[MAX_LATENT];
	// Current message position
	int			current;
	// Time when we should recompute k/sec data
	double		nextcompute;
	// Average data
	float		kbytespersec;
	// Average calculated using The movement average algorithm
	// https://en.wikipedia.org/wiki/Moving_average
	float		avgkbytespersec;
};

// Size of fragmentation buffer internal buffers
#define FRAGMENT_SIZE 1400

#include "netbuf.h"

//-----------------------------------------------------------------------------
// Purpose: Generic fragment structure
//-----------------------------------------------------------------------------
struct fragbuf_t
{
	// Next buffer in chain
	fragbuf_t*			next;
	// Id of this buffer (this is resolved as a cumulative counter)
	int					bufferid;
	// Message buffer where raw data is stored
	sizebuf_t			frag_message;
	// The actual data sits here
	byte				frag_message_buf[FRAGMENT_SIZE];
	// Is this a file buffer?
	qboolean			isfile;
	// Is this file buffer from memory ( custom decal, etc. ).
	qboolean			isbuffer;

	qboolean			iscompressed;
	// Name of the file to save out on remote host
	char				filename[MAX_PATH];
	// Offset in file from which to read data
	int					foffset;
	// Size of data to read at that offset
	int					size;
};

//-----------------------------------------------------------------------------
// Purpose: Waiting list of fragbuf chains
//-----------------------------------------------------------------------------
struct fragbufwaiting_t
{
	// Next chain in waiting list
	fragbufwaiting_t*	next;
	// Number of buffers in this chain
	int					fragbufcount;
	// The actual buffers
	fragbuf_t*			fragbufs;
};

//-----------------------------------------------------------------------------
// Purpose: Network Connection Channel. Holds data used in communication between
//			the client (us) and the remote using the UDP internet protocol.
//-----------------------------------------------------------------------------
struct netchan_t
{
	// NS_SERVER, NS_CLIENT or NS_MULTICAST, depending on channel.
	netsrc_t	sock;

	// Address this channel is talking to.
	netadr_t	remote_address;

	// Unused
	int			player_slot;

	// For timeouts. Time last message was received.
	float		last_received;

	// Time when channel was connected, i.e. at setup.
	float		connect_time;

	// 
	// Bandwidth choke
	//

	// Bytes per second
	double		rate;
	// If realtime > cleartime, free to send next packet.
	double		cleartime;

	//
	// Sequencing variables
	//
	
	// Increasing count of sequence numbers. Increases every time remote transfers.
	int			incoming_sequence;
	// # of last outgoing message that has been ack'd.
	int			incoming_acknowledged;
	// Toggles T/F as reliable messages are received.
	qboolean	incoming_reliable_acknowledged;
	// single bit, maintained local
	qboolean	incoming_reliable_sequence;
	// Message we are sending to remote. Increases every time client transfers.
	int			outgoing_sequence;
	// Whether the message contains reliable payload, single bit
	qboolean	reliable_sequence;
	// Outgoing sequence number of last send that had reliable data
	int			last_reliable_sequence;

	void*		connection_status; // cls.state

	int			(*pfnNetchan_Blocksize) (void*);

	// Staging and holding areas
	sizebuf_t	message;
	byte		message_buf[MAX_MSGLEN];

	// Reliable message buffer. We keep adding to it until reliable is acknowledged. Then we clear it.
	int			reliable_length;
	byte		reliable_buf[MAX_MSGLEN]; // unacked reliable message

	//
	// Fragmentation
	//

	// Waiting list of buffered fragments to go onto queue.
	// Multiple outgoing buffers can be queued in succession
	fragbufwaiting_t* waitlist[MAX_STREAMS];

	// Is reliable waiting buf a fragment?
	int			reliable_fragment[MAX_STREAMS];
	// Buffer id for each waiting fragment
	unsigned int reliable_fragid[MAX_STREAMS];

	// The current fragment being sent
	fragbuf_t*	fragbufs[MAX_STREAMS];
	// The total number of fragments in this stream
	int			fragbufcount[MAX_STREAMS];

	// Position in outgoing buffer where frag data starts
	short		frag_startpos[MAX_STREAMS];
	// Length of frag data in the buffer
	short		frag_length[MAX_STREAMS];

	// Incoming fragments are stored here
	fragbuf_t*	incomingbufs[MAX_STREAMS];
	// Set to true when we have processed all incomming fragments
	qboolean	incomingready[MAX_STREAMS];

	// Only referenced by the FRAG_FILE_STREAM component
	// Name of file being downloaded
	char		incomingfilename[MAX_PATH];

	// Compressed currently downloaded file (temporary, starts with !)
	void*		tempbuffer;
	int			tempbuffersize;

	// Incoming and outgoing flow metrics
	flow_t		flow[MAX_FLOWS];
};

#endif // NET_H