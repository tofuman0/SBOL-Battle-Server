#include <Windows.h>
#include "..\server.h"

void ClientPacketAuthentication(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x100:
	{
		client->SendAcceptConnection();
	}
	return;
	case 0x102:
	{
		// Check username and password here
		Server* server = client->server;
		server->managementserver.SendCheckCredentials(client);
	}
	return;
	default:
		client->logger->Log(Logger::LOGTYPE_Client, L"Client sent invalid packet.");
		client->Disconnect();
		return;
	}
	client->Send();
}
