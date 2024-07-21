#include <Windows.h>
#include "..\server.h"

void ClientPacketRivalDetails(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0xC00:
	{
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0xC00);
		client->outbuf.setSubType(0xC80);
		client->outbuf.append<uint16_t>(client->careerdata.playerWin + client->careerdata.playerLose); // Player total
		client->outbuf.append<uint16_t>(client->careerdata.playerWin); // VS Player Win
		client->outbuf.append<uint16_t>(client->careerdata.playerLose); // VS Player Lose
		client->outbuf.append<uint16_t>(client->careerdata.rivalWin + client->careerdata.rivalLose); // Rival total
		client->outbuf.append<uint16_t>(client->careerdata.rivalWin); // VS Rival Win
		client->outbuf.append<uint16_t>(client->careerdata.rivalLose); // VS Rival Lose
	}
	break;
	case 0xC01:
	{
		int32_t count = static_cast<int32_t>(client->inbuf.get<uint8_t>(0x04));
		if (client->inbuf.getSize() < 5 + (count * sizeof(int32_t)) || count > sizeof(client->careerdata.rivalStatus) / sizeof(RIVAL_STATUS))
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has sent invalid 0xC01 packet.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0xC00);
		client->outbuf.setSubType(0xC81);

		client->outbuf.append<uint8_t>(count); // Rival count should be 0x64 as even removed rival ID's are processed

		uint8_t status_remaps[] = { 3, 0, 1, 2 };
		client->inbuf.addOffset(0x05);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t currentID = client->inbuf.get<uint32_t>();
			if (currentID >= sizeof(client->careerdata.rivalStatus) / sizeof(RIVAL_STATUS))
			{
				client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has sent invalid 0xC01 packet.",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str()
				);
				client->Disconnect();
				return;
			}
			client->outbuf.append<uint32_t>(currentID); // Rival id. Must be loop index. Game is missing some teams.
			int32_t check = 0;
			for (int32_t j = 0; j < 8; j++) check += client->careerdata.rivalStatus[currentID].rivalMember[j] ? 1 : 0;
			client->outbuf.append<uint8_t>(check ? 0x08 : 0x00); // Team count
															// 0 - Seen / Show
															// 1 - Lost
															// 2 - Won
															// 3 - Not Seen / Hide
			if (check)
			{
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x00] % sizeof(status_remaps)]); // Rival 1 // Boss
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x01] % sizeof(status_remaps)]); // Rival 2
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x02] % sizeof(status_remaps)]); // Rival 3
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x03] % sizeof(status_remaps)]); // Rival 4
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x04] % sizeof(status_remaps)]); // Rival 5
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x05] % sizeof(status_remaps)]); // Rival 6
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x06] % sizeof(status_remaps)]); // Rival 7
				client->outbuf.append<uint8_t>(status_remaps[client->careerdata.rivalStatus[currentID].rivalMember[0x07] % sizeof(status_remaps)]); // Rival 8 // Lowest
			}
			//client->outbuf.append<uint32_t>(client->careerdata.rivalStatus[currentID].wins); // Boss Defeats I don't think this should be the wins as there is a limit of a few hundred. Maybe unlocked members?
			client->outbuf.append<uint32_t>(check);
		}
	}
	break;
	default:
		return;
	}
	client->Send();
}
