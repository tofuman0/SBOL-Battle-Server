#include "rival.h"
#include "TicketTables.h"
#include "RouteTables.h"
#include "RivalTables.h"
#include <iostream>

RIVAL::RIVAL()
{
	Initialize();
	client = nullptr;
}

RIVAL::RIVAL(CLIENT* Client, int32_t RivalID)
{
	// TODO: To load Rival table and set data from table
	Initialize();
	client = Client;
	Rival(RivalID);
}

RIVAL::~RIVAL()
{
}

void RIVAL::Random(int32_t Difficulty)
{
	// TODO: Randomly generate a random rival based of difficulty
	memcpy(&settings, &RandomRivals[0], sizeof(RIVALDATA));
}

void RIVAL::Name(std::string& Name)
{
	// Rival name must not match a players handle so add a hidden character to start and the end
	size_t nameLength = min(16, Name.length());
	settings.name[0] = '\t';
	memcpy(&settings.name[1], Name.c_str(), nameLength);
	settings.name[nameLength + 1] = '\t';
}

bool RIVAL::Rival(int32_t RivalID)
{
	if (!client || !client->server)
		return false;

	settings.rivalID = RivalID;

	RIVALDATA* rd = client->server->GetRivalData(RivalID);
	if (rd == nullptr)
		return false;

	memcpy(&settings, rd, sizeof(settings));
	return true;
}

void RIVAL::SpaceTick(uint32_t part, uint32_t total)
{
	uint32_t routeTableSize = 0;
	switch (settings.routeTable)
	{
	case 0:
		routeTableSize = sizeof(NPC_Position_C1_Outer) / 6;
		break;
	case 1:
		routeTableSize = sizeof(NPC_Position_C1_Inner) / 6;
		break;
	case 2:
		routeTableSize = sizeof(NPC_Position_Loop1_Outer) / 6;
		break;
	case 3:
		routeTableSize = sizeof(NPC_Position_Loop1_Inner) / 6;
		break;
	case 4:
		routeTableSize = sizeof(NPC_Position_Loop2_Outer) / 6;
		break;
	case 5:
		routeTableSize = sizeof(NPC_Position_Loop2_Inner) / 6;
		break;
	case 6:
		routeTableSize = sizeof(NPC_Position_Loop3_Outer) / 6;
		break;
	case 7:
		routeTableSize = sizeof(NPC_Position_Loop3_Inner) / 6;
		break;
	default:
		routeTableSize = 0;
		break;
	}

	tick = (routeTableSize / total) * part;
}

void RIVAL::Tick()
{
	uint32_t routeTableSize = 0;
	int16_t* routeTablePtr = nullptr;
	switch (settings.routeTable)
	{
	case 0:
		routeTableSize = sizeof(NPC_Position_C1_Outer) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_C1_Outer;
		break;
	case 1:
		routeTableSize = sizeof(NPC_Position_C1_Inner) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_C1_Inner;
		break;
	case 2:
		routeTableSize = sizeof(NPC_Position_Loop1_Outer) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop1_Outer;
		break;
	case 3:
		routeTableSize = sizeof(NPC_Position_Loop1_Inner) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop1_Inner;
		break;
	case 4:
		routeTableSize = sizeof(NPC_Position_Loop2_Outer) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop2_Outer;
		break;
	case 5:
		routeTableSize = sizeof(NPC_Position_Loop2_Inner) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop2_Inner;
		break;
	case 6:
		routeTableSize = sizeof(NPC_Position_Loop3_Outer) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop3_Outer;
		break;
	case 7:
		routeTableSize = sizeof(NPC_Position_Loop3_Inner) / 6;
		routeTablePtr = (int16_t*)&NPC_Position_Loop3_Inner;
		break;
	default:
		routeTableSize = 0;
		break;
	}
	if (routeTablePtr == nullptr) return;
	tick += RIVAL_AUTOPILOT_SPEED;
	tick %= routeTableSize;
	position.location1 = routeTablePtr[(tick * 3) + 0];
	position.location2 = routeTablePtr[(tick * 3) + 1];
	position.location3 = routeTablePtr[(tick * 3) + 2];
	position.time = timeGetTime();
}

uint32_t RIVAL::WinCP(float distance, bool firsttime, float boost)
{
	uint32_t CP = settings.cp * (firsttime ? 4 : 3);
	CP += (uint32_t)(distance / 1000.0f) * 10;
	CP = (uint32_t)((float)CP * (boost + 1.0f));
	return CP;
}

uint32_t RIVAL::LoseCP(float distance, bool firsttime, float boost)
{
	uint32_t CP = 0;
	CP += (uint32_t)(distance / 1000.0f) * 10;
	CP = (uint32_t)((float)CP * (boost + 1.0f));
	return CP;
}

int16_t RIVAL::WinReward(float boost)
{
	// TODO: Get reward for win
	int16_t Reward = -1;
	float chance = 1.0f + boost;
	return Reward;
}

int16_t RIVAL::LoseReward(float boost)
{
	// No reward for losers
	return -1;
}

void RIVAL::Initialize()
{
	ready = false;
	ZeroMemory(&settings, sizeof(settings));
	settings.rivalID = -1;
	ZeroMemory(&position, sizeof(position));
	reward = 0;
	tick = 0;
}

int16_t RIVAL::CarTicket()
{
	// Does rivals car have a ticket that can be obtained?

	if (settings.carID < (sizeof(CARTICKET_LOOKUP) / sizeof(int16_t)))
	{
		return CARTICKET_LOOKUP[settings.carID];
	}

	// No requirments met
	return -1;
}
