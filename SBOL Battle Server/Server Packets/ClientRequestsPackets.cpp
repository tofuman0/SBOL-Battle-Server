#include <Windows.h>
#include "..\managementserver.h"
#include "..\server.h"
#include "..\structures.h"

void ClientRequests(ManagementServer* managementserver)
{
	Server* server = managementserver->server;
	managementserver->inbuf.setOffset(0x06);
	switch (managementserver->inbuf.getSubType())
	{
	case 0x0000: // Check Create Team Name
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		uint32_t teamid = managementserver->inbuf.get<uint32_t>();
		uint8_t result = managementserver->inbuf.get<uint8_t>();
		std::string name = managementserver->inbuf.getStringA(0x10);
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			if (result == 0)
			{
				findClient->initializeTeam();
				findClient->teamdata.teamID = teamid;
				CopyMemory(&findClient->teamdata.leaderName[0], findClient->handle.c_str(), findClient->handle.length());
				CopyMemory(&findClient->teamdata.name[0], name.c_str(), name.length());
				findClient->teamdata.memberCount = 1;
				findClient->GetTeamData();
			}
			else if (result == 2)
			{
				findClient->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to create a team but is already in a team. Team ID %u.",
					findClient->logger->toWide(findClient->handle).c_str(),
					findClient->driverslicense,
					findClient->logger->toWide((char*)&findClient->IP_Address).c_str(),
					findClient->teamdata.teamID
				);
				findClient->Disconnect();
				return;
			}
			findClient->outbuf.clearBuffer();
			findClient->outbuf.setSize(0x06);
			findClient->outbuf.setOffset(0x06);
			findClient->outbuf.setType(0x1200);
			findClient->outbuf.setSubType(0x1281);
			findClient->outbuf.append<uint8_t>(result); // Result
			findClient->outbuf.append<uint32_t>(teamid); // Team ID
			findClient->Send();
		}
	}
	return;
	case 0x0001: // Update Team Invite Status
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		uint32_t teamid = managementserver->inbuf.get<uint32_t>();
		uint8_t result = managementserver->inbuf.get<uint8_t>();
		uint8_t inviteStatus = managementserver->inbuf.get<uint8_t>();
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			if (result == 0)
			{
				findClient->teamdata.inviteOnly = inviteStatus;
			}
			else if (result == 2)
			{
				findClient->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to update team comment but is not in team. Team ID %u.",
					findClient->logger->toWide(findClient->handle).c_str(),
					findClient->driverslicense,
					findClient->logger->toWide((char*)&findClient->IP_Address).c_str(),
					teamid
				);
				findClient->Disconnect();
				return;
			}
			else if (result == 3)
			{
				findClient->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to update team comment but is not the team leader. Team ID %u.",
					findClient->logger->toWide(findClient->handle).c_str(),
					findClient->driverslicense,
					findClient->logger->toWide((char*)&findClient->IP_Address).c_str(),
					teamid
				);
				findClient->Disconnect();
				return;
			}
			findClient->outbuf.clearBuffer();
			findClient->outbuf.setSize(0x06);
			findClient->outbuf.setOffset(0x06);
			findClient->outbuf.setType(0x1200);
			findClient->outbuf.setSubType(0x128B);
			findClient->outbuf.append<uint8_t>(result); // Result - Doesn't seem to matter what the value is here.
			findClient->Send();
		}
	}
	return;
	case 0x0002: // Update Team Comment
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		uint32_t teamid = managementserver->inbuf.get<uint32_t>();
		uint8_t result = managementserver->inbuf.get<uint8_t>();
		std::string comment = managementserver->inbuf.getStringA(0x28);
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			if (result == 0)
			{
				ZeroMemory(&findClient->teamdata.comment[0], sizeof(findClient->teamdata.comment));
				CopyMemory(&findClient->teamdata.comment[0], comment.c_str(), comment.length());
			}
			else if (result == 2 || result == 3)
			{
				findClient->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to update team comment but is not in team or is not team leader. Team ID %u.",
					findClient->logger->toWide(findClient->handle).c_str(),
					findClient->driverslicense,
					findClient->logger->toWide((char*)&findClient->IP_Address).c_str(),
					teamid
				);
				findClient->Disconnect();
				return;
			}
			findClient->outbuf.clearBuffer();
			findClient->outbuf.setSize(0x06);
			findClient->outbuf.setOffset(0x06);
			findClient->outbuf.setType(0x1200);
			findClient->outbuf.setSubType(0x128B);
			findClient->outbuf.append<uint8_t>(result); // Result - Doesn't seem to matter what the value is here.
			findClient->Send();
		}
	}
	return;
	case 0x0003: // Delete team
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		uint32_t teamid = managementserver->inbuf.get<uint32_t>();
		uint8_t result = managementserver->inbuf.get<uint8_t>();
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			if (result == 0)
			{
				ZeroMemory(&findClient->teamdata, sizeof(findClient->teamdata));
				findClient->initializeTeam();
			}
			else if (result == 2 || result == 3)
			{
				findClient->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to delete team comment but is not in team or team leader. Team ID %u.",
					findClient->logger->toWide(findClient->handle).c_str(),
					findClient->driverslicense,
					findClient->logger->toWide((char*)&findClient->IP_Address).c_str(),
					teamid
				);
				findClient->Disconnect();
				return;
			}
			findClient->outbuf.clearBuffer();
			findClient->outbuf.setSize(0x06);
			findClient->outbuf.setOffset(0x06);
			findClient->outbuf.setType(0x1200);
			findClient->outbuf.setSubType(0x1282);
			findClient->outbuf.append<uint8_t>(result);
			findClient->Send();
		}
	}
	return;
	case 0x0004: // Get Team Data
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			uint32_t teamID = managementserver->inbuf.get<uint32_t>();
			uint32_t teamLeaderID = managementserver->inbuf.get<uint32_t>();
			std::string teamLeaderName = managementserver->inbuf.getStringA(0x10);
			std::string teamName = managementserver->inbuf.getStringA(0x10);
			std::string teamComment = managementserver->inbuf.getStringA(0x28);
			uint32_t teamSurvivalWin = managementserver->inbuf.get<uint32_t>();
			uint32_t teamSurvivalLose = managementserver->inbuf.get<uint32_t>();
			uint32_t teamCount = managementserver->inbuf.get<uint32_t>();
			uint32_t teamFlag = managementserver->inbuf.get<uint32_t>();

			findClient->teamdata.teamID = teamID;
			ZeroMemory(&findClient->teamdata.leaderName[0], sizeof(findClient->teamdata.leaderName));
			CopyMemory(&findClient->teamdata.leaderName[0], teamLeaderName.c_str(), teamLeaderName.length());
			ZeroMemory(&findClient->teamdata.name[0], sizeof(findClient->teamdata.name));
			CopyMemory(&findClient->teamdata.name[0], teamName.c_str(), teamName.length());
			ZeroMemory(&findClient->teamdata.comment[0], sizeof(findClient->teamdata.comment));
			CopyMemory(&findClient->teamdata.comment[0], teamComment.c_str(), teamComment.length());
			findClient->teamdata.survivalWins = teamSurvivalWin;
			findClient->teamdata.survivalLoses = teamSurvivalLose;
			findClient->teamdata.memberCount = teamCount;
			findClient->teamdata.inviteOnly = teamFlag & 0x01 ? 1 : 0;
			
			ZeroMemory(&findClient->teammembers, sizeof(TEAMMEMBERS));
			for (uint32_t i = 0; i < teamCount; i++)
			{
				findClient->teammembers[i].license = managementserver->inbuf.get<uint32_t>();
				managementserver->inbuf.getArray((uint8_t*)&findClient->teammembers[i].name[0], 0x10);
				findClient->teammembers[i].rank = managementserver->inbuf.get<uint32_t>();
				findClient->teammembers[i].leader = managementserver->inbuf.get<uint8_t>();
				findClient->teammembers[i].area = managementserver->inbuf.get<uint8_t>();
			}
			uint8_t teamarea = managementserver->inbuf.get<uint8_t>();
			findClient->teamareaaccess = teamarea;
		}
	}
	return;
	case 0x0005:
	{
		uint32_t license = managementserver->inbuf.get<uint32_t>();
		uint8_t result = managementserver->inbuf.get<uint8_t>();
		Client* findClient = server->findUser(license);
		if (findClient != nullptr)
		{
			findClient->outbuf.clearBuffer();
			findClient->outbuf.setSize(0x06);
			findClient->outbuf.setOffset(0x06);
			findClient->outbuf.setType(0x1200);
			findClient->outbuf.setSubType(0x1292);
			findClient->outbuf.append<uint8_t>(result); // Result
			findClient->Send();
		}
	}
	return;
	}
}
