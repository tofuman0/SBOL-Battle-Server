#include <Windows.h>
#include "..\server.h"

void ClientPacketKeepAlive(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x0002:
		// Hello?
		//client->outbuf.clearBuffer();
		//client->outbuf.setSize(0x06);
		//client->outbuf.setOffset(0x06);
		//client->outbuf.setType(0x000);
		//client->outbuf.setSubType(0x080);
		//client->Send();
		client->packetResend = time(NULL) + PACKET_RESEND;
		return;
	default:
		return;
	}
}
