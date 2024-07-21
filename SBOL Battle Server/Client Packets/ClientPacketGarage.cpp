#include <Windows.h>
#include "..\server.h"

void ClientPacketGarage(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1700: // Garage Purchase
	{
		// Garage ID at 0x04
		// Cost at 0x08
		uint32_t garageID = client->inbuf.get<uint32_t>(0x04);
		uint32_t cost = client->inbuf.get<uint32_t>(0x08);

		if (cost != GARAGE_COST)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has altered garage cost. Cost: %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				cost
			);
			client->Disconnect();
			return;
		}
		if (client->enoughCP(cost))
		{
			client->takeCP(cost);
			client->garagedata.garageCount = 2;
			client->garagedata.garageType.push_back(0);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1700);
			client->outbuf.setSubType(0x1780);
			client->outbuf.append<uint8_t>(0);
			client->outbuf.append<uint32_t>(cost);
		}
		else
		{
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1700);
			client->outbuf.setSubType(0x1780);
			client->outbuf.append<uint8_t>(1);
			client->outbuf.append<uint32_t>(0);
		}
	}
	break;
	case 0x1701: // Garage Style Purchase
	{
		// Garage ID at 0x04
		// Style at 0x08
		// Cost at 0x0C
		uint32_t garageID = client->inbuf.get<uint32_t>(0x04);
		uint32_t style = client->inbuf.get<uint32_t>(0x08);
		uint32_t cost = client->inbuf.get<uint32_t>(0x0C);

		uint32_t prices[] = {
			10,
			160,
			100,
			500,
			300,
		};

		if (client->enoughCP(cost) && client->garagedata.garageCount > 1 && style < (sizeof(prices) / 4) && garageID == 2 && cost == prices[style])
		{
			client->takeCP(cost);
			client->garagedata.garageType[1] = static_cast<uint8_t>(style);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1700);
			client->outbuf.setSubType(0x1781);
			client->outbuf.append<uint8_t>(0);
			client->outbuf.append<uint32_t>(cost);
		}
		else
		{
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1700);
			client->outbuf.setSubType(0x1781);
			client->outbuf.append<uint8_t>(1);
			client->outbuf.append<uint32_t>(0);
		}
	}
	break;
	default:
		return;
	}
	client->Send();
}
