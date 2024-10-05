#include <Windows.h>
#include "..\server.h"

void ClientPacketTeam(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1201: // Team Creation
	{	
		// Name @ 0x04
		if (client->getLevel() < 10 || client->inTeam())
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to create a team but doesn\'t meet the required level of 10 or is already in a team. Level %u, Team ID %d.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				client->getLevel(),
				client->teamdata.teamID
			);
			client->Disconnect();
			return;
		}
		std::string teamName = client->inbuf.getString(0x04, 0x10);
		client->CreateTeam(teamName);
	}
	return;
	case 0x1202: // Team Deletion
	{	
		// Team ID @ 0x04
		client->inbuf.setOffset(0x04);
		uint32_t teamid = client->inbuf.get<uint32_t>();
		if (client->teamdata.teamID != teamid || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to delete a team but is isn't in the team or the leader. Team ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				teamid
			);
			client->Disconnect();
			return;
		}
		client->DeleteTeam();
	}
	return;
	case 0x1204: // When entering garage and highway
	{   
		client->SendCarData();
		//client->GetTeamData();
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1284);

		uint8_t teamAreaAccess = 0x00;
		uint8_t memberType = 0x00;
		if (client->inTeam() == false)
		{
			teamAreaAccess = 0xFF;
			memberType = 0x11;
		}
		else if (client->isTeamLeader() == false)
		{
			memberType = 0x01;
		}

		if (client->teamareaaccess)
		{
			teamAreaAccess = 0x01;
		}

		client->outbuf.append<uint8_t>(memberType);
		client->outbuf.append<uint8_t>(teamAreaAccess); 
	}
	break;
	case 0x1205: // Accept Member Join Request
	{	
		// TODO: Implement
		// 0x04: Request ID
		client->inbuf.setOffset(0x04);
		uint32_t requestID = client->inbuf.get<uint32_t>();
		if (client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to accept a team join request but is isn't the team leader. Request ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				requestID
			);
			client->Disconnect();
			return;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1285);
		client->outbuf.append<uint8_t>(0x00); // Result
		// Result
		//	0xFF: move 0 to 0x6F55E8 and return
		//	0x00 : inc 0x6F51A0 by 1, move 1 to 0x6F5569 and return
		//	0x01 : move 1 to 0x6F55E8 and return
		//	0x02 : move 2 to 0x6F55E8 and return
		//	0x03 : move 3 to 0x6F55E8 and return
		//	0x09 : move 5 to 0x6F55E8 and return
		//	0x0a : move 6 to 0x6F55E8 and return
		//	default: return
	}
	break;
	case 0x120A: // Update Join Status
	{	
		// ID @ 0x04
		// Join Status @ 0x08
		uint32_t teamid = client->inbuf.get<uint32_t>(0x04);
		uint8_t joinStatus = client->inbuf.get<uint8_t>(0x08);
		if (client->teamdata.teamID != teamid)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to update team join status but is not in team. Team ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				teamid
			);
			client->Disconnect();
			return;
		}
		client->UpdateTeamInviteOnly(joinStatus);
	}
	return;
	case 0x120B: // Update Comment
	{	
		// ID @ 0x04
		// Comment @ 0x08
		uint32_t teamid = client->inbuf.get<uint32_t>(0x04);
		std::string teamComment = client->inbuf.getString(0x08, 0x28);
		if (client->teamdata.teamID != teamid || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to update team comment but is not in team or team leader. Team ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				teamid
			);
			client->Disconnect();
			return;
		}
		client->UpdateTeamComment(teamComment);
	}
	return;
	case 0x120C: // Team Data
	{	
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x128C);
		if (client->inTeam())
		{
			client->outbuf.append<uint8_t>(0x00);
			client->outbuf.appendArray((uint8_t*)&client->teamdata, sizeof(client->teamdata));
		}
		else
			client->outbuf.append<uint8_t>(0x01);
	}
	break;
	case 0x120D: // Team List
	{	
		// TODO: Implement
		// Page @ 0x04
		// Request Count @ 0x08
		// Sort Column (0: Name, 1: Invite, 2: Members) @ 0x0C
		// Sort Asc (0) /Desc (1) @ 0x0D
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x128D);
		// TODO: Populate this data from team_data
		client->outbuf.append<uint32_t>(10);
		client->outbuf.append<uint16_t>(1);
		for (uint32_t i = 0; i < 1; i++)
		{
			client->outbuf.append<uint32_t>(i + 1); // Team ID
			client->outbuf.appendString(std::string("Team Name"), 0x10);
			client->outbuf.appendString(std::string("Team Leader"), 0x10);
			client->outbuf.append<uint8_t>(0); // Invite Only
			client->outbuf.appendString(std::string("Team Comment"), 0x28);
			for (int32_t j = 0; j < 10; j++) client->outbuf.append<uint8_t>(j);
			client->outbuf.append<uint8_t>(3); // Members
		}
	}
	break;
	case 0x120E: // Team Members
	{	
		// ID @ 0x04
		// Page Int @ 0x08
		// Count Int @ 0x0C is 0x05 in course and 0x14 in Team Center
		// Sort Column (0: Name, 1: Rank, 2: Team Area Access) @ 0x10
		// Sort Asc (0) /Desc (1) @ 0x11
		client->inbuf.setOffset(0x04);
		uint32_t teamid = client->inbuf.get<uint32_t>();
		uint32_t page = client->inbuf.get<uint32_t>();
		uint32_t count = client->inbuf.get<uint32_t>();
		uint8_t sortColumn = client->inbuf.get<uint8_t>();
		uint8_t sortOrder = client->inbuf.get<uint8_t>();
		
		if (client->teamdata.teamID != teamid)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to list a team members' but is not in the team. Team ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				teamid
			);
			client->Disconnect();
			return;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x128E);
		client->outbuf.append<uint32_t>(client->teamdata.memberCount); // number of members out of limit
		client->outbuf.append<uint16_t>(client->teamdata.memberCount); // Number of members
		for (auto& tm : client->teammembers)
		{
			if (tm.license != 0)
			{
				client->outbuf.append<uint32_t>(tm.license); // Team Member ID
				client->outbuf.appendArray((uint8_t*)&tm.name[0], 0x10);
				client->outbuf.append<uint32_t>(tm.rank); // Rank
				client->outbuf.append<uint8_t>(tm.leader); // Team Leader
				client->outbuf.append<uint8_t>(tm.area); // Team Area Access
			}
		}
	}
	break;
	case 0x120F: // Team Join Requests
	{	
		// TODO: Implement
		// ? Int @ 0x04 - Page Number
		// ? Int @ 0x08 - Number request usually 10
		// Sort Column (0: Name, 1: Rank) @ 0x0C
		// Sort Asc (0) /Desc (1) @ 0x0D
		// TODO: Grab table from DB
		uint32_t page = client->inbuf.get<uint32_t>(0x04);
		uint32_t requests = client->inbuf.get<uint32_t>(0x04);

		if (client->inTeam() == false || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to list a team join requests but is not in a team or the team leader.",
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
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x128F);
		client->outbuf.append<uint32_t>(20); // Number of requests?
		client->outbuf.append<uint16_t>(10); // Number of members
		for (uint32_t i = 0; i < 10; i++)
		{
			client->outbuf.append<uint32_t>((page * requests) + (i + 1)); // Request ID
			client->outbuf.appendString(std::string("Pick ME!"), 0x10);
			client->outbuf.append<uint32_t>(rand() % 1000 + 1000); // Rank
		}
	}
	break;
	case 0x1210: // Remove Team Join Requests
	{	
		// TODO: Implement
		// Request ID @ 0x04
		client->inbuf.setOffset(0x04);
		uint32_t requestID = client->inbuf.get<uint32_t>();
		uint8_t result = 0;

		if (client->inTeam() == false || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to remove team join request ID but is not in a team or leader. Request ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				requestID
			);
			client->Disconnect();
			return;
		}

		if (client->isTeamLeader())
		{
			result = 0x00;
		}
		else
		{
			result = 0xFF;
		}
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1290);
		client->outbuf.append<uint8_t>(result);
	}
	break;
	case 0x1211: // Team Data
	{	
		// TODO: Implement
		uint8_t type = 0;
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1291);
		client->outbuf.append<uint8_t>(type);
		client->outbuf.appendArray((uint8_t*)&client->teamdata, sizeof(client->teamdata));
		// switched on type.
		//   0x00: Return
		//   0xFF: Return
		//   0x05: Return
		//   0x07: Return
		//   0x0D: Set 0x6F5513 to 0 - Possibly the correct setting when leader
		//   Default: Return
	}
	break;
	case 0x1212: // Team Member Remove
	{	
		// TODO: Implement
		// Team Member ID @ 0x04
		client->inbuf.setOffset(0x04);
		uint32_t memberID = client->inbuf.get<uint32_t>();
		uint8_t result = 0;

		if (client->inTeam() == false || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to remove team memberbut is not in a team or leader. Member ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				memberID
			);
			client->Disconnect();
			return;
		}

		client->RemoveTeamMember(memberID);
		for (auto& tm : client->teammembers)
		{
			if (tm.license == memberID)
				client->server->SendInformMessage(std::string(tm.name), std::string("You have been removed from your team."), RGB(100, 50, 250));
			if (tm.license)
				client->server->managementserver.TeamGetData(tm.license);
		}
	}
	return;
	case 0x1214: // ???
	{	
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1294);
		client->outbuf.append<uint8_t>(0);
	}
	break;
	case 0x1215: // Team Area Access Adjust
	{	
		// Team Member ID Int @ 0x04
		// Access Byte @ 0x08
		// TODO: Update access
		client->inbuf.setOffset(0x04);
		uint32_t memberID = client->inbuf.get<uint32_t>();
		uint8_t access = client->inbuf.get<uint8_t>();
		uint8_t result = 0xFF;

		if (client->inTeam() == false || client->isTeamLeader() == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to adjust team member team area access but is not in a team or the team leader. Member ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				memberID
			);
			client->Disconnect();
			return;
		}

		for (auto& tm : client->teammembers)
		{
			if (tm.license == memberID && tm.leader == 0)
			{
				tm.area = access;
				result = 0;
				client->server->SendInformMessage(std::string(tm.name), std::string("Your access to the team area has been changed."), RGB(100, 50, 250));
				client->UpdateTeamMemberAreaAccess(memberID, access);
				client->server->managementserver.TeamGetData(memberID);
				break;
			}
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1295);
		client->outbuf.append<uint8_t>(result);
	}
	break;
	case 0x1216: // Team Locker
	{	
		// TODO: Check access to team locker
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1296);
		client->outbuf.append<uint8_t>(0x00); // Item Count
	}
	break;
	case 0x1219: // Team Garage Take
	{ 
		// TODO: Implement
		return;
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x1299);
		client->outbuf.append<uint32_t>(client->getActiveCar() + 1); // Active car slot from e80 on new accounts and 182 on existing
		client->outbuf.append<uint32_t>(client->garagedata.activeCar->carID); // Car ID
		client->outbuf.append<uint32_t>((uint32_t)(client->garagedata.activeCar->KMs / 100.0f)); // KMs
		client->outbuf.append<uint8_t>(0);
		client->outbuf.append<uint16_t>(static_cast<uint16_t>(client->garagedata.activeCar->engineCondition / 100)); // Engine Condition
		client->outbuf.append<uint8_t>(0);
		client->outbuf.appendArray((uint8_t*)&client->garagedata.activeCar->carMods, sizeof(client->garagedata.activeCar->carMods));
		client->outbuf.append<uint16_t>(0); // ????
	}
	break;
	case 0x121C: // Looks like it contains car details similar structure to 0281 Packet
	{ 
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1200);
		client->outbuf.setSubType(0x129C);
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x281);
		client->outbuf.append<uint32_t>(client->getActiveCar() + 1); // Active car slot from e80 on new accounts and 182 on existing
		client->outbuf.append<uint32_t>(client->garagedata.activeCar->carID); // Car ID
		client->outbuf.append<uint32_t>((uint32_t)(client->garagedata.activeCar->KMs / 100.0f)); // KMs
		client->outbuf.append<uint8_t>(0);
		client->outbuf.append<uint16_t>(static_cast<uint16_t>(client->garagedata.activeCar->engineCondition / 100)); // Engine Condition
		client->outbuf.append<uint8_t>(0);
		client->outbuf.appendArray((uint8_t*)&client->garagedata.activeCar->carMods, sizeof(client->garagedata.activeCar->carMods));
		client->outbuf.append<uint16_t>(0); // ????
	}
	break;
	default:
		return;
	}
	client->Send();
}
