#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include "server.h"
//#include <openssl/rand.h>

typedef void(PacketFunction)(Client* client);

void ManagementPacketDoNothing(Client* client) { }
void ManagementPacketClientAuth(Client* client);
void ManagementPacketClientRequests(Client* client);

PacketFunction* ManagementPacketFunctions[] =
{
	&ManagementPacketDoNothing,			// 0x0000 - TBC
	&ManagementPacketClientAuth,		// 0x0001 - Client Authentication
	&ManagementPacketClientRequests,	// 0x0002 - Client Requests: Car, Team
	&ManagementPacketDoNothing,			// 0x0003 - TBC
	&ManagementPacketDoNothing,			// 0x0004 - TBC
	&ManagementPacketDoNothing,			// 0x0005 - TBC
};