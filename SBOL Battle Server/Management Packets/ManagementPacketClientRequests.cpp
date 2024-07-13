#include <Windows.h>
#include "..\server.h"

void ManagementPacketClientRequests(CLIENT* client)
{
	SERVER* server = (SERVER*)client->server;
	switch (client->serverbuf.getSubType())
	{
	case 0x0000:
	{
	}
	}
}