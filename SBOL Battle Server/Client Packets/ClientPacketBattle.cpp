#include <Windows.h>
#include "..\server.h"

void ClientPacketBattle(Client* client)
{
	uint16_t pType = client->inbuf.getType();
	uint16_t clientID;
	if (client->course == nullptr) return;
	switch (pType)
	{
#ifdef DISABLE_BATTLE
	case 0x500:
	{
		client->SendBattleAbort(0);
	}
	break;
#else
	case 0x500:
	{	// Initiate battle : Only send to the opponent
		// Client ID @ 0x04
		clientID = client->inbuf.get<uint16_t>(0x04);
		client->battle.challenger = client->course->getClient(clientID);

		if (client->battle.challenger != nullptr && client->battle.challenger->battle.challenger == nullptr && client->battle.status == Client::BATTLESTATUS::BS_NOT_IN_BATTLE && !(client->battle.challenger->status & Client::PLAYERSTATUS::PS_SAFEMODE))
		{
			client->SendBattleChallenge(client->courseID, clientID);
			client->SendBattleChallengeToOpponent(client->courseID, clientID);
		}
		else
		{
			client->battle.status = Client::BATTLESTATUS::BS_NOT_IN_BATTLE;
			client->SendBattleAbort(0);
		}
		client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u is challenging player ID %u",
			client->logger->toWide(client->handle).c_str(),
			client->driverslicense,
			client->logger->toWide((char*)&client->IP_Address).c_str(),
			client->courseID,
			clientID);
		return;
	}
	break;
	case 0x501:
	{	// End Battle
		uint8_t result = client->inbuf.get<uint8_t>(0x04);
		if (client->battle.challenger != nullptr && client->battle.status != Client::BATTLESTATUS::BS_NOT_IN_BATTLE && client->battle.status != Client::BATTLESTATUS::BS_IN_BATTLE)
		{
			client->battle.timeout = 0;
			client->battle.status = Client::BATTLESTATUS::BS_NOT_IN_BATTLE;
			client->battle.challenger->battle.timeout = 0;
			client->battle.challenger->battle.status = Client::BATTLESTATUS::BS_NOT_IN_BATTLE;
			client->battle.challenger->SendBattleAbort(result);
			client->SendBattleAbort(result);
			client->battle.challenger->battle.challenger = nullptr;
			client->battle.challenger = nullptr;
		}
		else if (client->battle.challenger != nullptr && client->battle.status == Client::BATTLESTATUS::BS_IN_BATTLE)
		{ // Race end - 0 Normal loss, 1 different direction loss
			client->battle.status = Client::BATTLESTATUS::BS_LOST;
			client->battle.challenger->battle.status = Client::BATTLESTATUS::BS_WON;
			client->SendBattleCheckStatus();
		}
		return;
	}
	break;
	case 0x502:
	{	// Start Battle
		if (client->battle.challenger != nullptr && client->battle.status == Client::BATTLESTATUS::BS_INIT_BATTLE)
		{
			client->battle.timeout = 0;
			if (client->battle.challenger->battle.status == Client::BATTLESTATUS::BS_WAITING)
			{
				client->SendBattleStart();
				client->battle.challenger->SendBattleStart();
				return;
			}
			client->battle.status = Client::BATTLESTATUS::BS_WAITING;
		}
		return;
	}
	break;
	case 0x503:
	{	// During Battle
		if (client->battle.challenger != nullptr && client->battle.status != Client::BATTLESTATUS::BS_NOT_IN_BATTLE && client->battle.status != Client::BATTLESTATUS::BS_WAITING)
		{	// TODO: check for cheating
			client->battle.lastSP = client->battle.SP;
			client->battle.SP = client->inbuf.get<uint32_t>(0x06);
			client->battle.spCount++;
			if (client->battle.lastSP != INITIALBATTLE_SP && (client->battle.SP >= client->battle.lastSP || (client->battle.lastSP - client->battle.SP) > 400000))
			{
				client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u may have altered the battle SP. %u SP taken since last SP",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str(),
					client->courseID,
					(client->battle.lastSP - client->battle.SP)
				);
			}
			client->SendBattleCheckStatus();
		}
		return;
	}
	break;
	case 0x504:
	{	// Initiate battle : With NPC
		// Rival ID @ 0x04
		clientID = client->inbuf.get<uint16_t>(0x04);
		if (clientID < COURSE_NPC_LIMIT)
		{	// Is Rival ID
			client->SendBattleChallengeNPC(clientID, 100000);
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u is challenging NPC ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				client->courseID,
				clientID);
		}
		else
		{
			client->battle.status = Client::BATTLESTATUS::BS_NOT_IN_BATTLE;
			client->SendBattleAbort(0);
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u is challenging invalid NPC ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				client->courseID,
				clientID);
		}
		return;
	}
	break;
	case 0x505:
	{	// Finish battle : With NPC
		// Distance? @ 0x04
		// Distance? @ 0x06
		// Remaining NPC SP @ 0x0A
		// Remaining Player SP @ 0x0E
		if (client->battle.status == Client::BATTLESTATUS::BS_IN_BATTLE)
		{
			uint16_t _1 = client->inbuf.get<uint16_t>(0x04);
			uint32_t _2 = client->inbuf.get<uint32_t>(0x06);
			uint32_t _3 = client->inbuf.get<uint32_t>(0x0A);
			uint32_t _4 = client->inbuf.get<uint32_t>(0x0E);
			client->logger->Log(Logger::LOGTYPE_NONE, L"Client %s (%u / %s) with ID %u has finshed battle. %u, %u, %u, %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				client->courseID,
				_1, _2, _3, _4);
			if (_3 < _4) client->battle.status = Client::BATTLESTATUS::BS_WON;
			else client->battle.status = Client::BATTLESTATUS::BS_LOST;
			client->SendBattleNPCFinish();
			return;
		}
		else
		{
			client->SendBattleNPCAbort(7);
			return;
		}
	}
	break;
#endif
	default:
		return;
	}
	client->Send();
}
