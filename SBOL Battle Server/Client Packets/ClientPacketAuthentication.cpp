#include <Windows.h>
#include "..\server.h"

void ClientPacketAuthentication(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x100:
	{
		// Byte at 0x07 is either beginner flag or server number. Initial connections are 0 but connections to beginner is 1
		uint32_t clientVer = client->inbuf.getVer() & 0xFFFFFF00;
		if (clientVer == 0x53427B00 /* new client */)
			//if (clientVer == 0x544D0100 /* Tofuman client */)
		{
			if (client->server->getStatus() == SERVER::SS_RUNNING && client->server->getConnectedToMANAGEMENTSERVER())
			{
				client->sendWelcome = 0;
				if (client->setUsername(std::string((char*)&client->inbuf.buffer[0x08])))
				{
					client->SendAuthError(SERVER::AUTHLIST::AUTH_INVALID_PW);
					return;
				}
				else
				{
					client->SendWelcome(0, std::string("\0\1\2\4\5\6\7"));
					return;
				}
			}
			else
			{
				client->SendAuthError(SERVER::AUTHLIST::AUTH_BUSY);
				return;
			}
		}
		else
		{
			client->SendAuthError(SERVER::AUTHLIST::AUTH_OUTDATED);
			return;
		}
	}
	break;
	case 0x102:
	{
		// Check username and password here
		SERVER* server = client->server;

		server->managementserver.outbuf.clearBuffer();
		server->managementserver.outbuf.setSize(0x06);
		server->managementserver.outbuf.setOffset(0x06);
		server->managementserver.outbuf.setType(0x0001);
		server->managementserver.outbuf.setSubType(0x0000);
		server->managementserver.outbuf.append<int32_t>(client->ClientSocket);
		server->managementserver.outbuf.appendString(client->username, 0x14);
		server->managementserver.outbuf.appendArray((uint8_t*)&client->inbuf.buffer[0x04], client->inbuf.getSize() - 2);
		if (server->managementserver.Send())
		{
			client->SendAuthError(SERVER::AUTHLIST::AUTH_BUSY);
		}
		return;
	}
	break;
	default:
		client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client sent invalid packet.");
		client->Disconnect();
		return;
	}
	client->Send();
}
