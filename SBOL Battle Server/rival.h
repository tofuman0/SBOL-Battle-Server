#pragma once
#include "structures.h"
#include "client.h"
#include "server.h"
#include "Logger.h"
#include <vector>

class RIVAL
{
public:
	RIVAL();
	RIVAL(CLIENT* Client, int32_t RivalID = -1);
	~RIVAL();
	const enum RIVALSTATUS {
		RS_HIDDEN,
		RS_SHOW,
		RS_LOST,
		RS_WON
	};
	void Client(CLIENT* Client) { client = Client; }
	void Random(int32_t Difficulty = 0);
	void ID(int16_t ID) { id = ID; }
	uint16_t ID() { return id; }
	void Ready(bool Ready) { ready = Ready; }
	bool Ready() { return ready; }
	void Custom(bool Custom) { settings.customTeam = Custom; }
	bool Custom() { return settings.customTeam; }
	void Name(std::string& Name);
	char* Name() { return settings.name; }
	uint8_t Level() { return settings.level; }
	void Reward(uint32_t Reward) { reward = Reward; }
	uint32_t Reward() { return reward; }
	void CP(uint32_t CP) { settings.cp = CP; }
	uint32_t CP() { return settings.cp; }
	void Position(POSITION Position) { position = Position; }
	POSITION Position() { return position; }
	void Leader(bool Leader) { settings.leader = Leader; }
	bool Leader() { return settings.leader; }
	void Car(int16_t CarID) { settings.carID = CarID; }
	int16_t Car() { return settings.carID; }
	void CarMods(CARMODS CarMods) { settings.carMods = CarMods; }
	CARMODS CarMods() { return settings.carMods; }
	CARMODS* CarModsPtr() { return &settings.carMods; }
	CARSETTINGS CarSettings() { return settings.carSettings; }
	CARSETTINGS* CarSettingsPtr() { return &settings.carSettings; }
	void TeamData(TEAMDATA TeamData) { settings.teamData = TeamData; }
	TEAMDATA TeamData() { return settings.teamData; }
	TEAMDATA* TeamDataPtr() { return &settings.teamData; }
	RIVALDIFFICULTY RivalDifficulty() { return settings.difficulty; }
	RIVALDIFFICULTY* RivalDifficultyPtr() { return &settings.difficulty; }
	bool Rival(int32_t RivalID);
	int32_t Rival() { return settings.rivalID; }
	int32_t RouteTable() { return settings.routeTable; }
	void RouteTable(int32_t RouteTable) { settings.routeTable = RouteTable; }
	uint32_t WinCP(float distance = 0.0f, bool firsttime = false, float boost = 0.0f);
	uint32_t LoseCP(float distance = 0.0f, bool firsttime = false, float boost = 0.0f);
	uint32_t WinXP() { return 0; }
	uint32_t LoseXP() { return 0; }
	int16_t WinReward(float boost = 0.0f);
	int16_t LoseReward(float boost = 0.0f);
	
	void SpaceTick(uint32_t part, uint32_t total);
	void Tick();
	
private:
	uint16_t id;
	RIVALDATA settings;
	bool ready;
	POSITION position;
	int32_t reward;
	uint32_t tick;
	CLIENT* client;
	
	void Initialize();
	int16_t CarTicket();
};

