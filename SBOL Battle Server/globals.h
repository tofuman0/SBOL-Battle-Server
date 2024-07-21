#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H
// TODO: FIX COMPRESSION
// TODO: FIX ENCRYPTION - Packets get corrupted when not running with debugger.
#define DISABLE_COMPRESSION
#define DISABLE_ENCRYPTION
//#define DISABLE_BATTLE
#ifdef _DEBUG
#define PACKET_OUTPUT
#endif

#define VERSION_STRING			L"0.1"
#define MAX_MESG_LEN			4096
//#define CLIENT_BUFFER_SIZE		64000
#define CLIENT_BUFFER_SIZE		(1024 * 8)
#define TCP_BUFFER_SIZE			65536
#define BLOCK_SIZE				16
#define KEY_SIZE				32

// TODO: add constants to config file
#define CLIENT_TIMEOUT			300
#define PACKET_RESEND			15			
#define SEND_WELCOME_TIME		5
#define COURSE_PACKET_TIME		1
#define COURSE_PROXIMITY		1000.0f
#define PING_TIME				20
#define AUTOSAVE_TIME			(5*60)
#define HEARTBEAT_TIME			30
#define BATTLE_TIMEOUT			60
#define INITIALBATTLE_SP		5000000
#define Rival_AUTOPILOT_SPEED	12
#define POSITION_ACCURACY		10
#define PACKETSEND_LIMIT		10

// Limits
#define LEVEL_CAP				50
#define SERVER_LIMIT			100
#define COURSE_COUNT			9
#define COURSE_PLAYER_LIMIT		200
#define COURSE_NPC_LIMIT		100
#define COURSE_DIMENSIONS		10
#define GARAGE_LIMIT			12
#define ITEMBOX_LIMIT			10

// Costs
#define PAINT_BASE_COST			500
#define GARAGE_COST				3000
#define OVERHAULBASE_COST		50
#define RESALEDIVIDER			4

// Flags
#define PART_LOCKED				0
#define PART_UNLOCKED			1
#define PART_OWNED				2

// Types
#define PARTTYPE_ENGINE			0
#define PARTTYPE_MUFFLER		1
#define PARTTYPE_TRANSMISSION	2
#define PARTTYPE_DIFFERENTIAL	3
#define PARTTYPE_TYREBRAKE		4
#define PARTTYPE_SUSPENSION		6
#define PARTTYPE_BODY			7
#define PARTTYPE_OVERFENDER		0
#define PARTTYPE_FRONTBUMPER	1
#define PARTTYPE_BONNET			2
#define PARTTYPE_MIRRORS		3
#define PARTTYPE_SIDESKIRTS		4
#define PARTTYPE_REARBUMPER		5
#define PARTTYPE_REARSPOILER	6
#define PARTTYPE_GRILL			0
#define PARTTYPE_LIGHTS			1

#endif

#define SWAP_SHORT(l)						   \
            ( ( ((l) >>  8) & 0x00FF ) |       \
              ( ((l) <<  8) & 0xFF00 ) )

#define SWAP_LONG(l)                                \
            ( ( ((l) >> 24) & 0x000000FFL ) |       \
              ( ((l) >>  8) & 0x0000FF00L ) |       \
              ( ((l) <<  8) & 0x00FF0000L ) |       \
              ( ((l) << 24) & 0xFF000000L ) )

#define SWAP_LONGLONG(l)                                     \
            ( ( ((l) >> 56) & 0x00000000000000FFLL ) |       \
              ( ((l) >> 40) & 0x000000000000FF00LL ) |       \
              ( ((l) >> 24) & 0x0000000000FF0000LL ) |       \
              ( ((l) >>  8) & 0x00000000FF000000LL ) |       \
              ( ((l) <<  8) & 0x000000FF00000000LL ) |       \
              ( ((l) << 24) & 0x0000FF0000000000LL ) |       \
              ( ((l) << 40) & 0x00FF000000000000LL ) |       \
              ( ((l) << 56) & 0xFF00000000000000LL ) )

#define TO_FLOAT_COLOUR(l) (float)((1.0f / 255.0f) * ((l) & 0xFF))
#define GET_R_COLOUR(l) (l & 0x0000FF)
#define GET_G_COLOUR(l) ((l & 0x00FF00) >> 8)
#define GET_B_COLOUR(l) ((l & 0xFF0000) >> 16)

extern const wchar_t* LOGFILES[];
extern const char* DISCONNECT_STRINGS[];
extern const char *coursenames[];

