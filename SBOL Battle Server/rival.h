#pragma once
#include "structures.h"
#include "client.h"
#include "server.h"
#include "Logger.h"
#include <vector>

class Rival
{
public:
	Rival();
	Rival(Client* client, int32_t RivalID = -1);
	~Rival();
	const enum RIVALSTATUS {
		RS_HIDDEN,
		RS_SHOW,
		RS_LOST,
		RS_WON
	};
	void SetClient(Client* client) { m_Client = client; }
	void Random(int32_t Difficulty = 0);
	void SetID(int16_t ID) { id = ID; }
	uint16_t GetID() { return id; }
	void SetReady(bool Ready) { ready = Ready; }
	bool GetReady() { return ready; }
	void SetCustom(bool Custom) { settings.customTeam = Custom; }
	bool GetCustom() { return settings.customTeam; }
	void SetName(std::string& Name);
	char* GetName() { return settings.name; }
	uint8_t GetLevel() { return settings.level; }
	void SetReward(uint32_t Reward) { reward = Reward; }
	uint32_t GetReward() { return reward; }
	void SetCP(uint32_t CP) { settings.cp = CP; }
	uint32_t GetCP() { return settings.cp; }
	void SetPosition(POSITION Position) { position = Position; }
	POSITION GetPosition() { return position; }
	void SetLeader(bool Leader) { settings.leader = Leader; }
	bool GetLeader() { return settings.leader; }
	void SetCar(int16_t CarID) { settings.carID = CarID; }
	int16_t GetCar() { return settings.carID; }
	void SetCarMods(CARMODS CarMods) { settings.carMods = CarMods; }
	CARMODS GetCarMods() { return settings.carMods; }
	CARMODS* GetCarModsPtr() { return &settings.carMods; }
	CARSETTINGS GetCarSettings() { return settings.carSettings; }
	CARSETTINGS* GetCarSettingsPtr() { return &settings.carSettings; }
	void SetTeamData(TEAMDATA TeamData) { settings.teamData = TeamData; }
	TEAMDATA GetTeamData() { return settings.teamData; }
	TEAMDATA* GetTeamDataPtr() { return &settings.teamData; }
	RIVALDIFFICULTY GetRivalDifficulty() { return settings.difficulty; }
	RIVALDIFFICULTY* GetRivalDifficultyPtr() { return &settings.difficulty; }
	bool SetRivalID(int32_t RivalID);
	int32_t GetRivalID() { return settings.rivalID; }
	void SetRouteTable(int32_t RouteTable) { settings.routeTable = RouteTable; }
	int32_t GetRouteTable() { return settings.routeTable; }
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
	Client* m_Client;
	
	void Initialize();
	int16_t CarTicket();
};

