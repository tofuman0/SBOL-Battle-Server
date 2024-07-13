#include <Windows.h>
#include "..\managementserver.h"
#include "..\server.h"
#include "..\structures.h"

void ClientOperations(MANAGEMENTSERVER* managementserver)
{
	SERVER* server = managementserver->server;
	managementserver->inbuf.setOffset(0x06);

	switch (managementserver->inbuf.getSubType())
	{
	case 0x0000: // Offline Chat
	{
		uint8_t type = managementserver->inbuf.get<uint8_t>();
		std::string fromHandle = managementserver->inbuf.getStringA(0x10);
		std::string toHandle = managementserver->inbuf.getStringA(0x10);
		std::string message = managementserver->inbuf.getStringA(0x4E);
		switch (type)
		{
		case CLIENT::CHATTYPE::CHATTYPE_ANNOUNCE:
		case CLIENT::CHATTYPE::CHATTYPE_NOTIFICATION:
		case CLIENT::CHATTYPE::CHATTYPE_PRIVATE:
		case CLIENT::CHATTYPE::CHATTYPE_EVENT:
		case CLIENT::CHATTYPE::CHATTYPE_NORMAL:
		case CLIENT::CHATTYPE::CHATTYPE_ADMIN:
		{
			CLIENT* findClient = server->findUser(toHandle);
			if (findClient != nullptr) findClient->SendChatMessage(static_cast<CLIENT::CHATTYPE>(type), fromHandle, message, findClient->driverslicense, fromHandle);
		}
		return;
		}
	}
	break;
	case 0x0001: // Inform Client
	{
		std::string toHandle = managementserver->inbuf.getStringA(0x10);
		std::string message = managementserver->inbuf.getStringA(0x4E);
		uint32_t colour = managementserver->inbuf.get<uint32_t>();
		CLIENT* findClient = server->findUser(toHandle);
		if (findClient != nullptr) findClient->SendAnnounceMessage(message, colour, findClient->driverslicense);
	}
	break;
	case 0x0002: // Team Chat
	{
		std::string fromHandle = managementserver->inbuf.getStringA(0x10);
		std::string message = managementserver->inbuf.getStringA(0x4E);
		uint32_t teamID = managementserver->inbuf.get<uint32_t>();
		for (auto& c : server->connections)
		{
			if (c->isClient == true && c->teamdata.teamID == teamID)
			{
				c->SendTeamChatMessage(fromHandle, message, teamID);
			}
		}
	}
	return;
	}
}
