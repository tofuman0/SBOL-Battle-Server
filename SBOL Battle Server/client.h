#pragma once
#include <mutex>
#include <vector>
#include <queue>
#include "serverpacket.h"
#include "packet.h"
#include "globals.h"
#include "structures.h"
#include "Logger.h"
#include "rival.h"

class COURSE;
class SERVER;
class RIVAL;

class CLIENT
{
private:
	std::queue<SEND_QUEUE> sendQueue;
	std::mutex _muClient;
	uint8_t packetAllow[0x18];
public:
	CLIENT();
	~CLIENT();
	const enum PLAYERSTATUS {
		PS_READY = 0,
		PS_SAFEMODE = 1,
		PS_IN_RACE = 2,
		PS_IN_SHOP = 4,
		PS_AUTOPILOT = 8,
		PS_IN_GARAGE = 16
	};
	const enum BATTLESTATUS {
		BS_NOT_IN_BATTLE,
		BS_INIT_BATTLE,
		BS_WAITING,
		BS_IN_BATTLE,
		BS_WON,
		BS_LOST,
		BS_DRAW
	};
	const enum COURSETYPE {
		COURSE_MAIN,
		COURSE_SHOP,
		COURSE_PARTS,
		COURSE_FREEWAYA,
		COURSE_FREEWAYB,
		COURSE_SURVIVAL,
		COURSE_TIMEATTACKA,
		COURSE_TIMEATTACKB
	};
	const enum CHATTYPE {
		CHATTYPE_ANNOUNCE,
		CHATTYPE_NOTIFICATION,
		CHATTYPE_PRIVATE,
		CHATTYPE_EVENT = 0x03,
		CHATTYPE_NORMAL,
		CHATTYPE_ADMIN = 0xE0,
	};
	const enum ITEMTYPES {
		ITEMTYPE_CAR,
		ITEMTYPE_PART,
		ITEMTYPE_ITEM
	};
	const enum ITEMUSETYPES {
		USETYPE_COFFEE,
		USETYPE_CHEWINGGUM,
		USETYPE_COOKIE,
		USETYPE_U1,
		USETYPE_U2,
		USETYPE_U3,
		USETYPE_U4,
		USETYPE_CAKE,
		USETYPE_PUDDING,
		USETYPE_MOUSSE,
		USETYPE_MOVE,
		USETYPE_CP,
		USETYPE_USER,
		USETYPE_EVENT,
		USETYPE_ICON
	};
	const enum CARCLASS {
		CARCLASS_A,
		CARCLASS_B,
		CARCLASS_C
	};
	typedef struct st_battle {
		time_t timeout;
		BATTLESTATUS status;
		uint32_t SP;
		uint32_t lastSP;
		CLIENT* challenger;
		bool initiator;
		bool isNPC;
		float KMs;
		int32_t spCount;
	} BATTLE;
#pragma region Client Class Pointers
	LOGGER* logger;
	SERVER* server;
	COURSE* course;
	BATTLE battle;
#pragma endregion
#pragma region Client rival data
	std::vector<RIVAL> rivals;
	RIVAL* currentRival;
#pragma endregion
#pragma region Client Connection Variables
	int32_t ClientSocket;
	uint8_t rcvbuf[CLIENT_BUFFER_SIZE];
	uint8_t sndbuf[CLIENT_BUFFER_SIZE];
	uint16_t rcvread;
	uint32_t snddata;
	uint16_t packetsize;
	uint16_t packetoffset;
	bool hassize;
	bool packetready;
	uint8_t IP_Address[16]; // Text version
	uint8_t ipaddr[4]; // Binary version
	PACKET inbuf;
	PACKET outbuf;
	PACKET coursebuf;
	SERVERPACKET serverbuf;
#pragma endregion
#pragma region Client Variables
	int32_t test1;
	int32_t test2;
	time_t packetResend;
	bool todc;
	bool isClient;
	bool canSave;
	time_t lastSave;
	bool notBeginner;
	uint16_t status;
	uint32_t currentCourse;
	uint32_t driverslicense;
	uint16_t startJunction;
	uint16_t startDistance;
	POSITION position;
	uint8_t positionCount;
	uint16_t courseID;
	bool inCourse;
	bool hasPlayers; // Set when client has received other players in course
	uint8_t privileges;
	bool logPosition; // Used to create route tables
	time_t timeoutCount;
	time_t sendWelcome;
	time_t ping;
	uint32_t pokes;
	std::string handle;
	std::string username;
	uint32_t activeSign;
	uint8_t sign[54];
	TEAMDATA teamdata;
	TEAMMEMBERS teammembers[5];
	uint8_t teamareaaccess;
	std::vector<TEAMJOINREQUESTS> teamRequests;
	GARAGE garagedata;
	CAREERDATA careerdata;
	std::vector<ITEMBOX_ITEM> itembox;
	std::vector<uint32_t> shopCars;
	uint8_t unlockableCars[88];
#pragma endregion
#pragma region Client Functions
	void initialize();
	void initializeGarage();
	void initializeTeam();
	int32_t joinCourse();
	int8_t getCarCount();
	int32_t getEmptyBay();
	bool addCar(int32_t carID, uint32_t bay, float r1, float g1, float b1, float r2, float g2, float b2);
	bool removeCar(uint32_t bay);
	int32_t setActiveCar(uint32_t bay);
	int32_t getActiveCar() { return garagedata.activeCarBay; };
	int32_t getCarID(uint32_t bay);
	int32_t getCarManufacturer(uint32_t bay);
	int32_t setHandle(std::string& in);
	int32_t setUsername(std::string& in);
	int64_t getCP() { return careerdata.CP; };
	bool enoughCP(int64_t price);
	void takeCP(int64_t price);
	void giveCP(int64_t cp);
	uint32_t calculateOverhaul(uint32_t bay);
	uint16_t getShopPartPrice(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID);
	uint16_t getShopPartPriceFromID(uint32_t carID, uint8_t itemCategory, uint8_t itemType, uint32_t itemID);
	bool purchasePart(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID, bool override = false);
	bool equipPart(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID);
	bool confirmClass(uint32_t bay, CARCLASS carClass);
	uint8_t getCarClass(int32_t bay = -1);
	void addItem(int16_t itemID);
	bool removeItem(uint16_t itemID);
	bool hasItem(uint16_t itemID);
	bool isValidItem(uint16_t itemID);
	bool isValidCar(uint32_t carID);
	int16_t getItemPrice(uint16_t itemID);
	int16_t getItemResalePrice(uint16_t itemID);
	int16_t getShopCarPrice(uint32_t carID);
	int16_t getShopCarResalePrice(uint32_t carID);
	uint32_t getCarSalePrice(uint32_t bay);
	ITEMDATA_ENTRY getItem(uint16_t itemID);
	int32_t isInCarShop(uint32_t carID);
	void processBattleWin();
	void processBattleLose();
	void adjustEngineCondition(int32_t damage1, int32_t damage2, int32_t damage3, int32_t damage4);
	void addExp(uint32_t exp);
	void takeExp(uint32_t exp);
	void setExp(uint32_t exp);
	uint8_t getLevel();
	void setLevel(uint8_t level);
	void clearBattle();
	RIVAL* getRival(uint32_t RivalID);
	void getRivals();
	void clearRivals();
	void setRivalStatus(uint32_t TeamID, uint8_t MemberID, uint8_t Status);
	int32_t getSign(uint16_t id);
	void enableSign(uint16_t id);
	void disableSign(uint16_t id);
	void CreateTeam(std::string& teamname);
	void DeleteTeam();
	void RemoveTeamMember(uint32_t memberID);
	void UpdateTeamInviteOnly(uint8_t invitestatus);
	void UpdateTeamComment(std::string& comment);
	void UpdateTeamMemberAreaAccess(uint32_t memberID, uint8_t access);
	void GetTeamData();
	bool inTeam();
	bool isTeamLeader();
	void Disconnect();
#pragma endregion
#pragma region Client Packets
	void SendPing();
	void SendWelcome(uint8_t type, std::string& name);
	void SendAuthError(std::string& cmd);
	void SendAuthError(uint8_t cmd);
	void SendCourseJoin(uint8_t notify = 1);
	void SendRivalJoin();
	void SendRivalPosition();
	void SendRemoveRivals();
	void SendPositionBrief();
	void SendPosition();
	void SendPositionToOpponent();
	void SendChatMessage(CHATTYPE type, std::string& handle, std::string& message, uint32_t license = NULL, std::string& fromHandle = std::string(""));
	void SendTeamChatMessage(std::string& fromHandle, std::string& message, uint32_t teamID = 0);
	void SendAnnounceMessage(std::string& message, uint32_t colour = 0xFFFFFF, uint32_t license = NULL);
	void SendPlayerStats();
	void SendItems();
	void SendSigns();
	void SendCarData();
	void SendCarData(uint32_t bay);
	void SendBayDetails();
	void SendBattleChallengeNPC(uint16_t RivalID, uint32_t _time = 1000);
	void SendBattleChallenge(uint16_t challengeID, uint16_t clientID, uint32_t _time = 1000);
	void SendBattleChallengeToOpponent(uint16_t challengeID, uint16_t clientID, uint32_t _time = 1000);
	void SendBattleAbort(uint8_t res);
	void SendBattleNPCAbort(uint8_t res);
	void SendBattleStart();
	void SendBattleCheckStatus();
	void SendBattleNPCFinish();
#pragma endregion
#pragma region Client Packet Transmission
	void addToSendQueue(PACKET* src);
	void addToSendQueue(SEND_QUEUE* src);
	SEND_QUEUE getFromSendQueue();
	uint32_t messagesInSendQueue();
	void clearSendQueue();
	void Send(PACKET* src = nullptr);
	void SendToCourse(PACKET* src = nullptr, bool exclude = false);
	void SendToProximity(float x, float y, PACKET* src = nullptr, bool exclude = false);
	void SendToOpponent(PACKET* src = nullptr);
	void packetEnable(uint32_t packetType);
	void packetDisable(uint32_t packetType);
	void enableStandardPackets();
	void enableShopPackets();
	void enableCoursePackets();
	void enableAllPackets();
	bool CanSendPackets(uint32_t packetType);
	void ProcessPacket();
	void ProcessManagementPacket();
#pragma endregion
};

