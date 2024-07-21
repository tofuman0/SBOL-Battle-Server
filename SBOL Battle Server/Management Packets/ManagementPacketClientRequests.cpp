#include <Windows.h>
#include "..\server.h"

void ManagementPacketClientRequests(Client* client)
{
	Server* server = (Server*)client->server;
	switch (client->serverbuf.getSubType())
	{
	case 0x0000:
	{
	}
	}
}