#pragma once
#include "managementserver.h"

typedef void(PacketFunction)(ManagementServer* managementserver);

void ServerAuth(ManagementServer* managementserver);
void ClientAuth(ManagementServer* managementserver);
void ClientRequests(ManagementServer* managementserver);
void ClientOperations(ManagementServer* managementserver);
void ServerOperations(ManagementServer* managementserver);

void ClientPacketDoNothing(ManagementServer* managementserver) { }

PacketFunction* PacketFunctions[] =
{
	&ServerAuth,				// 0x0000 - Handshake: Verify keys and encryption
	&ClientAuth,				// 0x0100 - Verify client login: Check login. Including if banned and reason.
	&ClientRequests,			// 0x0200 - Request client data: Retrieve car, garage and team information.
	&ClientPacketDoNothing,		// 0x0300 - TBC
	&ClientOperations,			// 0x0400 - Client operations: Kick, ban message clients
	&ServerOperations,			// 0x0500 - Get Server Stats: Total player count, races in progress ....
};