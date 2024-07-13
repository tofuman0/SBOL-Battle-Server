#pragma once
#include "server.h"

typedef void(PacketFunction)(CLIENT* client);

void ClientPacketDoNothing(CLIENT* client) { }
void ClientPacketKeepAlive(CLIENT* client);
void ClientPacketAuthentication(CLIENT* client);
void ClientPacketCar(CLIENT* client);
void ClientPacketCourseDetails(CLIENT* client);
void ClientPacketNotifications(CLIENT* client);
void ClientPacketBattle(CLIENT* client);
void ClientPacketChat(CLIENT* client);
void ClientPacketPositioning(CLIENT* client);
void ClientPacketShop(CLIENT* client);
void ClientPacketPing(CLIENT* client);
void ClientPacketRivalDetails(CLIENT* client);
void ClientPacketPlayerCreation(CLIENT* client);
void ClientPacketPlayerDetails(CLIENT* client);
void ClientPacketItem(CLIENT* client);
void ClientPacketTeam(CLIENT* client);
void ClientPacket15(CLIENT* client);
void ClientPacketStatus(CLIENT* client);
void ClientPacketGarage(CLIENT* client);

PacketFunction* MainPacketFunctions[] =
{
	&ClientPacketKeepAlive,			// 0x0000
	&ClientPacketAuthentication,	// 0x0100
	&ClientPacketCar,				// 0x0200
	&ClientPacketCourseDetails,		// 0x0300
	&ClientPacketNotifications,		// 0x0400
	&ClientPacketBattle,			// 0x0500
	&ClientPacketChat,				// 0x0600
	&ClientPacketPositioning,		// 0x0700
	&ClientPacketDoNothing,			// 0x0800
	&ClientPacketShop,				// 0x0900
	&ClientPacketPing,				// 0x0A00
	&ClientPacketDoNothing,			// 0x0B00
	&ClientPacketRivalDetails,		// 0x0C00
	&ClientPacketDoNothing,			// 0x0D00
	&ClientPacketPlayerCreation,	// 0x0E00
	&ClientPacketDoNothing,			// 0x0F00
	&ClientPacketPlayerDetails,		// 0x1000
	&ClientPacketItem,				// 0x1100
	&ClientPacketTeam,				// 0x1200
	&ClientPacketDoNothing,			// 0x1300
	&ClientPacketDoNothing,			// 0x1400
	&ClientPacket15,				// 0x1500
	&ClientPacketStatus,			// 0x1600
	&ClientPacketGarage,			// 0x1700 - Garage purchases
	&ClientPacketDoNothing,			// 0x1800
	&ClientPacketDoNothing,			// 0x1900
};