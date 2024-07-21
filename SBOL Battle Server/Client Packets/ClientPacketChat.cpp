#include <Windows.h>
#include "..\server.h"

void ClientPacketChat(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	Server* server = (Server*)client->server;
	switch (pType)
	{
	case 0x601: // Command Chat
	{
		//if (client->privileges)
		{
			// Command being sent
			std::vector<std::string> command = server->split(client->inbuf.getString(0x14), std::string(" "));
			if (command.size() == 0) return;

			if (command[0].compare("ping") == 0)
			{
				client->pokes = 0;
				client->SendPing();
				client->SendAnnounceMessage(std::string("Sent Ping Command!"), RGB(50, 100, 250), client->driverslicense);
				return;
			}
			else if (command[0].compare("clearsigns") == 0)
			{
				ZeroMemory(&client->sign[0], sizeof(client->sign));
				client->SendSigns();
				client->SendAnnounceMessage(std::string("Signs Cleared!"), RGB(50, 100, 250), client->driverslicense);
				return;
			}
			else if (command[0].compare("test") == 0 && client->privileges)
			{
				client->outbuf.clearBuffer();
				client->outbuf.setSize(0x06);
				client->outbuf.setOffset(0x06);
				client->outbuf.setType(0x400);
				client->outbuf.setSubType(0x483);
				client->outbuf.append<unsigned char>(1);
				client->outbuf.append<char>(1);
				client->outbuf.append<uint8_t>(1);
				client->outbuf.append<int8_t>(1);
				client->outbuf.append<uint16_t>(1);
				client->outbuf.append<int16_t>(1);
				client->outbuf.append<short>(1);
				client->outbuf.append<uint32_t>(1);
				client->outbuf.append<int32_t>(1);
				client->outbuf.append<int>(1);
				client->outbuf.append<uint64_t>(1);
				client->outbuf.append<int64_t>(1);
				client->outbuf.append<long long>(1);
				client->outbuf.append<float>(1.0f);
				return;
			}
			else if (command[0].compare("logposition") == 0 && client->privileges)
			{
				if (command.size() > 1)
				{
					if (command[1].compare("on") == 0)
						client->logPosition = true;
					else
						client->logPosition = false;
					(client->logPosition) ? client->SendAnnounceMessage(std::string("Now logging position"), RGB(50, 100, 250), client->driverslicense) : client->SendAnnounceMessage(std::string("No longer logging position"), RGB(50, 100, 250), client->driverslicense);
					return;
				}
				client->SendAnnounceMessage(std::string("Usage: !logposition on / !logposition off"), RGB(50, 100, 250), client->driverslicense);
				return;
			}
			//else if (command[0].compare("test") == 0)// || client->test1 == 0)
			//{
			//	client->test1 = 1;
			//	client->outbuf.clearBuffer();
			//	client->outbuf.setSize(0x06);
			//	client->outbuf.setOffset(0x06);
			//	client->outbuf.setType(0x400);
			//	client->outbuf.setSubType(0x480);

			//	client->outbuf.append<uint16_t>(tempValue1 + 1); // ID

			//	client->outbuf.appendString("Rival Test", 0x10); // Player Name

			//	client->outbuf.append<uint32_t>(tempValue1); // ????

			//	// 0x01 byte array
			//	client->outbuf.append<uint8_t>(1); // Level

			//	// 0x6C byte array - Car Data
			//	// TESTING
			//	
			//	client->outbuf.append<uint32_t>(tempValue1, false); // ????
			//	client->outbuf.append<uint32_t>(client->garagedata.activeCar->carID, false); // Car ID
			//	client->outbuf.appendArray((uint8_t*)&client->garagedata.activeCar->carMods, sizeof(client->garagedata.activeCar->carMods));
			//	client->outbuf.append<uint32_t>(tempValue1, false); // ????
			//	// TESTING

			//	// 0x0E byte array
			//	// Positioning here somewhere
			//	client->outbuf.append<uint16_t>(client->startJunction, false); // Junction
			//	client->outbuf.append<uint16_t>(client->startDistance + (0x100 * tempValue1), false); // ?? Distance ? 0xFF01 working
			//	client->outbuf.append<uint16_t>(0x1FFF, false); // ?? Speed?
			//	client->outbuf.append<uint16_t>(0xFFFF, false); // Set 0xFFFF to auto place in area
			//	
			//	client->outbuf.append<uint8_t>(1); // npc stuff must be below 0x20. 0x00 = Self, 0x01 = Player, 0x02 = NPC
			//	client->outbuf.append<uint8_t>(0x00); // 1st bit arrow is flashin (if player), 7th bit arrow is green, 8th bit arrow is red
			//	client->outbuf.append<uint8_t>(0x00); // 1st bit car is in safe mode, When 3rd bit set car is transparent
			//	client->outbuf.append<uint8_t>(0x00); // ????
			//	client->outbuf.append<uint8_t>(0x00); // ???? 
			//	client->outbuf.append<uint8_t>(0x00); // ????

			//	// 0x6C byte array - Team Data
			//	TEAMDATA tempteam = { 0 };
			//	tempteam.teamID = 0xffffffff; // no team
			//	//strcpy(&tempteam.name[0], "Tofu Delivery");
			//	client->outbuf.appendArray((uint8_t*)&tempteam, sizeof(TEAMDATA));
			//	
			//	client->outbuf.append<uint8_t>(1); // If set with player notification of player entering is displayed.
			//	client->outbuf.append<uint16_t>(0); // Count for something requires the shorts as many as these
			//	//for (int32_t i = 0; i < 150; i++) client->outbuf.append<uint16_t>(0x01);
			//	//client->outbuf.append<uint16_t>(0x21);
			//	//client->outbuf.append<uint16_t>(11);
			//	//client->outbuf.append<uint16_t>(12);
			//	//client->outbuf.append<uint16_t>(13);
			//	//client->outbuf.append<uint16_t>(14);
			//	client->outbuf.append<uint32_t>(0); // Icon next to name. if player.
			//	client->Send();

			//	stringstream ss;
			//	ss << "Command data: " << tempValue1 << ", " << tempValue2;
			//	tempValue1 += 0x01;// 0x80;
			//	client->SendChatMessage(CHATTYPE_NOTIFICATION, "NOTIFICATION", ss.str(), client->driverslicense);
			//	return;
			//}
			//else if (command[0].compare("add") == 0)
			//{
			//	tempValue1++;
			//	return;
			//}
			else if (command[0].compare("set") == 0)
			{
				if (command.size() > 1)
				{
					if (command[1].compare("condition") == 0)
					{
						uint8_t condition = 100;
						if (command.size() > 2) condition = static_cast<uint8_t>(atol(command[2].c_str()));
						client->garagedata.activeCar->engineCondition = condition * 100;
						std::stringstream ss;
						ss << "Set engine condition to " << static_cast<uint32_t>(condition) << "%";
						client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
						client->SendCarData(client->getActiveCar());
						return;
					}
					else if (command[1].compare("car") == 0)
					{
						if (command.size() > 2)
						{
							int16_t car = min((int16_t)atoi(command[2].c_str()), 164);
							if (client->isValidCar(car) == false)
							{
								client->SendAnnounceMessage(std::string("Invalid Car ID"), RGB(50, 100, 250), client->driverslicense);
								return;
							}
							client->garagedata.activeCar->carID = car;
							std::stringstream ss;
							ss << "Car changed to " << car;
							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
						}
						return;
					}
					else if (command[1].compare("wheel") == 0)
					{
						if (command.size() > 2)
						{
							int16_t wheel = min((int16_t)atoi(command[2].c_str()), 399);
							client->garagedata.activeCar->carMods.wheels = wheel;
							std::stringstream ss;
							ss << "Wheels changed to " << wheel;
							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
						}
						return;
					}
					else if (command[1].compare("sticker") == 0)
					{
						if (command.size() > 2)
						{
							int16_t sticker = (int16_t)atoi(command[2].c_str());
							client->garagedata.activeCar->carMods.stickers = sticker;
							std::stringstream ss;
							ss << "Sticker changed to " << sticker;
							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
							return;
						}
						else
						{
							client->SendAnnounceMessage(std::string("setsticker requires value : !setsticker 1"), RGB(50, 100, 250), client->driverslicense);
							return;
						}
					}
					else if (command[1].compare("level") == 0 && client->privileges)
					{
						if (command.size() > 2)
						{
							int32_t level = atoi(command[2].c_str());
							std::stringstream ss;
							client->setLevel((uint8_t)level);
							ss << "Level changed to " << level;
							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
							client->SendPlayerStats();
							return;
						}
					}
					else if (command[1].compare("xp") == 0 && client->privileges)
					{
						if (command.size() > 2)
						{
							int32_t xp = 0;
							std::stringstream ss;
							if (command[2][0] == 0x2B) // +
							{
								xp = atoi((char*)&(command[2].c_str()[1]));
								client->addExp(xp);
								ss << xp << " XP added. Now at " << client->careerdata.experiencePoints;
							}
							else if (command[2][0] == 0x2D) // -
							{
								xp = atoi((char*)&(command[2].c_str()[1]));
								client->takeExp(xp);
								ss << xp << " XP taken. Now at " << client->careerdata.experiencePoints;
							}
							else
							{
								xp = atoi(command[2].c_str());
								client->setExp(xp);
								ss << "XP set to " << client->careerdata.experiencePoints;
							}

							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
							client->SendPlayerStats();
							return;
						}
					}
					else if (command[1].compare("maintenance") == 0 && client->privileges == 255)
					{
						if (command.size() == 3)
						{
							if (command[2].compare("on") == 0)
							{
								client->server->setStatus(Server::SERVERSTATUS::SS_MAINTENANCE);
								client->SendAnnounceMessage(std::string("SERVER SET IN MAINTENANCE"), RGB(250, 100, 10), client->driverslicense);
								client->server->SendAnnounceMessage(std::string("Server is now in maintenance. Please log out."), RGB(250, 100, 10));
								return;
							}
							else if (command[2].compare("off") == 0)
							{
								client->server->setStatus(Server::SERVERSTATUS::SS_MAINTENANCE);
								client->SendAnnounceMessage(std::string("SERVER MAINTENANCE CLEARED"), RGB(250, 100, 10), client->driverslicense);
								return;
							}
							else
							{
								client->SendAnnounceMessage(std::string("set maintenance requires on or off argument"), RGB(50, 100, 250), client->driverslicense);
								return;
							}
						}
						else
						{
							client->SendAnnounceMessage(std::string("set maintenance syntax error: !set maintenance on"), RGB(50, 100, 250), client->driverslicense);
							return;
						}

					}
				}
			}
			else if (command[0].compare("dc") == 0 && client->privileges == 255)
			{
				if (command.size() == 2)
				{
					if (command[1].compare("all") == 0)
					{
						client->server->SendAnnounceMessage(std::string("You have been disconnected by an admin."), RGB(250, 100, 10));
						client->server->disconnectAllUsers(client->driverslicense);
						return;
					}
					else
					{
						Client* foundClient = client->server->findUser(command[1]);
						if (foundClient != nullptr)
						{
							client->SendAnnounceMessage(std::string("You have been disconnected by an admin."), RGB(250, 100, 10), foundClient->driverslicense);
							foundClient->Disconnect();
							return;
						}
					}
				}
				else
				{
					client->SendAnnounceMessage(std::string("dc syntax error: !dc all or !dc handle"), RGB(50, 100, 250), client->driverslicense);
					return;
				}
			}
			else if (command[0].compare("give") == 0)
			{
				if (command.size() > 1)
				{
					if (command[1].compare("cp") == 0)
					{
						uint64_t cp = 1000;
						if (command.size() > 2) cp = atoll(command[2].c_str());
						client->giveCP(cp);
						std::stringstream ss;
						ss << "Added " << cp << " CP";
						client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
						client->SendPlayerStats();
						return;
					}
					else if (command[1].compare("item") == 0)
					{
						if (client->itembox.size() < ITEMBOX_LIMIT)
						{
							uint16_t itemID = 0x840;
							if (command.size() > 2) itemID = (uint16_t)atoi(command[2].c_str());
							if (client->isValidItem(itemID) == false)
							{
								client->SendAnnounceMessage(std::string("Invalid item"), RGB(50, 100, 250), client->driverslicense);
								return;
							}
							client->addItem(itemID);
							std::stringstream ss;
							ss << "Given item " << itemID;
							client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
							return;
						}
						else
						{
							client->SendAnnounceMessage(std::string("Itembox full!"), RGB(50, 100, 250), client->driverslicense);
							return;
						}
					}
				}
			}
			else if (command[0].compare("unlock") == 0)
			{
				if (command.size() > 1)
				{
					if (command[1].compare("all") == 0)
					{
						for (int32_t i = 0; i < sizeof(PARTS::engine); i++) client->garagedata.activeCar->parts.engine[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::muffler); i++) client->garagedata.activeCar->parts.muffler[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::transmission); i++) client->garagedata.activeCar->parts.transmission[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::differential); i++) client->garagedata.activeCar->parts.differential[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::tyreBrakes); i++) client->garagedata.activeCar->parts.tyreBrakes[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::suspension); i++) client->garagedata.activeCar->parts.suspension[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::body); i++) client->garagedata.activeCar->parts.body[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::overFenders); i++) client->garagedata.activeCar->parts.overFenders[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::frontBumper); i++) client->garagedata.activeCar->parts.frontBumper[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::bonnet); i++) client->garagedata.activeCar->parts.bonnet[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::mirrors); i++) client->garagedata.activeCar->parts.mirrors[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::sideSkirts); i++) client->garagedata.activeCar->parts.sideSkirts[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::rearBumper); i++) client->garagedata.activeCar->parts.rearBumper[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::rearSpoiler); i++) client->garagedata.activeCar->parts.rearSpoiler[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::grill); i++) client->garagedata.activeCar->parts.grill[i] |= PART_UNLOCKED;
						for (int32_t i = 0; i < sizeof(PARTS::lights); i++) client->garagedata.activeCar->parts.lights[i] |= PART_UNLOCKED;
						client->SendAnnounceMessage(std::string("All parts unlocked"), RGB(50, 100, 250), client->driverslicense);
					}
					else
					{
						client->SendAnnounceMessage(std::string("Unknown unlock type. Valid types: all, engine, muffler, trans, dif, tirebrake, sus, body, frontbumper, bonnet, mirrors, sideskirts, rearbumper, rearspoiler, grill, lights"), RGB(50, 100, 250), client->driverslicense);
					}
				}
				else
				{
					client->SendAnnounceMessage(std::string("Syntax error. Example: !unlock all"), RGB(50, 100, 250), client->driverslicense);
				}
				return;
			}
			else if (command[0].compare("quit") == 0)
			{
				client->Disconnect();
				return;
			}
		}
	}
	break;
	case 0x602: // Private Chat Offline
	{	
		int32_t offset = 0x24;
		client->SendChatMessage(Client::CHATTYPE::CHATTYPE_PRIVATE, client->inbuf.getString(0x04, 0x10), client->inbuf.getString(offset, 0x4E), client->driverslicense, client->handle);
		client->server->SendOfflineChatMessage(Server::CHATTYPE::CHATTYPE_PRIVATE, client->handle, client->inbuf.getString(0x04, 0x10), client->inbuf.getString(offset, 0x4E));
		return;
	}
	break;
	case 0x603: // Team Chat
	{	
		// Name @ 0x04
		// Message @ 0x14
		// Team ID @ 0x62
		uint32_t teamID = client->inbuf.get<uint32_t>(0x62);
		if (teamID != client->teamdata.teamID || client->inTeam() == false)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to team chat but is not in a team or in the specified team. Team ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				teamID
			);
			client->Disconnect();
			return;
		}
		client->server->SendTeamChatMessage(client->handle, client->teamdata.teamID, client->inbuf.getString(0x14, 0x4E));
		return;
	}
	return;
	case 0x608: // Private Chat Online
	{	
		int32_t offset = 0x16;
		Client* findClient = client->server->findUser(client->inbuf.getString(0x06, 0x10));
		if (findClient != nullptr) client->SendChatMessage(Client::CHATTYPE::CHATTYPE_PRIVATE, client->inbuf.getString(0x06, 0x10), client->inbuf.getString(offset, 0x4E), findClient->driverslicense, client->handle);
		client->SendChatMessage(Client::CHATTYPE::CHATTYPE_PRIVATE, client->inbuf.getString(0x06, 0x10), client->inbuf.getString(offset, 0x4E), client->driverslicense, client->handle);
		return;
	}
	break;
	case 0x609:
	{	// Chat
		int32_t offset = (client->inbuf.get<uint16_t>(0x14) * 4) + 0x16;
		if (client->privileges == 0xFF) client->SendChatMessage(Client::CHATTYPE::CHATTYPE_ADMIN, client->handle, client->inbuf.getString(offset, 0x4E), 0);
		else client->SendChatMessage(Client::CHATTYPE::CHATTYPE_NORMAL, client->handle, client->inbuf.getString(offset, 0x4E), 0);
		return;
	}
	break;
	default:
		return;
	}
	client->Send();
}
