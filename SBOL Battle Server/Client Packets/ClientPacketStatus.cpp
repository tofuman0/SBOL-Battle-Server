#include <Windows.h>
#include "..\server.h"

void ClientPacketStatus(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1600: // Signs
	{
		client->SendSigns();
	}
	break;
	case 0x1601: // Set sign status
	{
		// Sign at 0x04
		uint32_t sign = client->inbuf.get<uint32_t>(0x04);
		if (sign <= 10 || (sign < 64 && sign >= 11 && client->sign[sign - 11]))
		{
			client->activeSign = sign;
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1600);
			client->outbuf.setSubType(0x1681);
			client->outbuf.append<uint16_t>(client->courseID); // ID
			client->outbuf.append<uint32_t>(sign); // Icon next to name. if player.
			client->SendToCourse(&client->outbuf, true);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to activate sign that they haven't unlocked or that is invalid. Sign %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				sign
			);
			client->Disconnect();
		}
		return;
	}
	break;
	default:
		return;
	}
	client->Send();
}
