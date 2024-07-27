#pragma once
#include "globals.h"
#include <stdint.h>
#include <vector>
#include <Windows.h>

#ifndef SHARED_STRUCTS
#define SHARED_STRUCTS
class Client;

typedef struct st_sendqueue {
	uint8_t sndbuf[CLIENT_BUFFER_SIZE];
} SEND_QUEUE;
typedef struct st_coursequeue {
	int32_t exclude;
	uint8_t sndbuf[CLIENT_BUFFER_SIZE];
} COURSE_QUEUE;
typedef struct st_messagequeue {
	int32_t socket;
	uint8_t buffer[CLIENT_BUFFER_SIZE];
} MESSAGE_QUEUE;
typedef struct st_sendmanagementqueue {
	uint8_t sndbuf[CLIENT_BUFFER_SIZE];
} SEND_MANAGEMENT_QUEUE;
typedef uint16_t ITEMBOX_ITEM;
typedef struct st_rivalstatus {
	uint8_t rivalMember[8];
	uint32_t wins;
} RIVAL_STATUS;
typedef struct st_careerdata {
	uint8_t level;
	uint8_t experiencePercent;
	uint32_t experiencePoints;
	uint32_t recentExperiancePoints;
	uint32_t ranking;
	uint16_t rivalWin;
	uint16_t rivalLose;
	uint16_t playerWin;
	uint16_t playerLose;
	int64_t CP;
	RIVAL_STATUS rivalStatus[100];
} CAREERDATA;
typedef struct st_teamdata {
	uint32_t teamID;
	uint8_t unknown1[4];
	uint8_t memberID;
	char name[18];
	char comment[41];
	uint8_t memberCount;
	uint8_t inviteOnly; // unknown3;
	uint8_t unknown4;
	uint8_t unknown5;
	uint32_t survivalWins;
	uint32_t survivalLoses;
	char leaderName[16];
	uint8_t unknown2[12]; // One of these is the team member id
} TEAMDATA;
typedef struct st_teammembers {
	uint32_t license;
	char name[0x10];
	uint32_t rank;
	uint8_t leader;
	uint8_t area;
} TEAMMEMBERS;
typedef struct st_teamjoinrequests {
	uint32_t requestID;
	char name[16];
	uint32_t rank;
} TEAMJOINREQUESTS;
typedef struct st_colour2 {
	float R1;
	float G1;
	float B1;
	float R2;
	float G2;
	float B2;
} COLOUR2;
typedef struct st_carmods {
	COLOUR2 carcolour;
	uint8_t engine;
	uint8_t muffler;
	uint8_t transmission;
	uint8_t differential;
	uint8_t tyrebrakes;
	uint8_t unknown1;
	uint8_t suspension;
	uint8_t body;
	uint8_t overfenders;
	uint8_t frontbumper;
	uint8_t bonnet;
	uint8_t mirrors;
	uint8_t sideskirts;
	uint8_t rearbumper;
	uint8_t rearspoiler;
	uint8_t grill;
	uint8_t lights;
	uint8_t unknown2[5];
	uint16_t wheels;
	uint8_t colourwheels;
	uint8_t unknown3[3];
	COLOUR2 wheelcolour;
	uint32_t stickers;
	uint8_t tuner[16]; // Tuner name
} CARMODS;
typedef struct st_carsettings {
	int8_t handle;
	int8_t acceleration;
	int8_t brake;
	int8_t brakeBalance;
	int8_t springRateFront;
	int8_t springRateRear;
	int8_t damperRateFront;
	int8_t damperRateRear;
	int8_t gear1;
	int8_t gear2;
	int8_t gear3;
	int8_t gear4;
	int8_t gear5;
	int8_t gear6;
	int8_t rideHeightFront;
	int8_t rideHeightRear;
	int8_t finalDrive;
	int8_t boostPressure; // 1.5bar = 0,
} CARSETTINGS;
typedef struct st_parts {
	uint8_t unknown1[4];
	uint8_t engine[8];
	uint8_t muffler[9];
	uint8_t transmission[5];
	uint8_t differential[5];
	uint8_t tyreBrakes[10];
	uint8_t suspension[6];
	uint8_t body[6];
	uint8_t overFenders[2];
	uint8_t frontBumper[6];
	uint8_t rearBumper[6];
	uint8_t bonnet[3];
	uint8_t mirrors[3];
	uint8_t sideSkirts[6];
	uint8_t rearSpoiler[6];
	uint8_t grill[3];
	uint8_t lights[3];
} PARTS;
typedef struct st_car {
	uint32_t carID;
	uint16_t status;
	float KMs; // Divided by 10 by the game. Stored in database as meters so divide by 100 before sending
	uint32_t engineCondition;
	CARMODS carMods;
	CARSETTINGS carSettings;
	PARTS parts;
} CAR;
typedef struct st_garage {
	CAR* activeCar;
	uint32_t activeCarBay;
	std::vector<CAR> car;
	uint8_t garageCount;
	std::vector<uint8_t> garageType;
} GARAGE;
typedef struct st_position {
	uint16_t accuracy;
	uint16_t location1;
	uint16_t location2;
	uint16_t location3;
	float posX1;
	float posY1;
	float posX2;
	float posY2;
	float posZ1;
	float posZ2;
	int32_t time;
} POSITION;
#pragma region pDAT structure
typedef struct st_pDAT_EN {
	uint16_t price[3]; // Each class
	int32_t unknown1;
	int16_t powerRPMAdjust;
	int16_t torqueRPMAdjust;
	int16_t weightAdjust;
	int16_t unknown2;
	int16_t unknown3;
	int16_t unknown4;
	int16_t unknown5;
	int16_t unknown6;
} P_DAT_ENGINE;
typedef struct st_pDAT_MF {
	uint16_t price[3]; // Each class
	int16_t unknown1;
	int16_t unknown2;
	int16_t weightAdjust;
	int16_t unknown3;
	int16_t unknown4;
	int16_t unknown5;
	int16_t unknown6;
	int16_t unknown7;
	int16_t unknown8;
	int16_t unknown9;
	int16_t unknown10;
	int16_t unknown11;
	int16_t unknown12;
} P_DAT_MUFFLER;
typedef struct st_pDAT_TN {
	uint16_t price[3]; // Each class
	int16_t unknown1;
	float gear1Adjust;
	float gear2Adjust;
	float gear3Adjust;
	float gear4Adjust;
	float gear5Adjust;
	float gear6Adjust;
	float finalDriveAdjust;
	int16_t weightAdjust;
	int16_t unknown2;
} P_DAT_TRANSMISSION;
typedef struct st_pDAT_DF {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
	float unknown1;
} P_DAT_DIFFERENTIAL;
typedef struct st_pDAT_SU {
	uint16_t price[3]; // Each class
	int16_t weightAdjust1;
	int16_t weightAdjust2;
	int16_t unknown1;
	int16_t unknown2;
	int16_t unknown3;
	int16_t unknown4;
	int16_t unknown5;
	int16_t unknown6;
	int16_t unknown7;
	int16_t unknown8;
	int16_t unknown9;
	float unknown10;
	float unknown11;
} P_DAT_SUSPENSION;
typedef struct st_pDAT_BO {
	uint16_t price[3]; // Each class
	int16_t weightAdjust1;
	int16_t weightAdjust2;
	int16_t unknown1;
} P_DAT_BODY;
typedef struct st_pDAT_BM {
	uint16_t price[3]; // Each class
	int16_t unknown1;
	float unknown2;
	int16_t weightAdjust1;
	int16_t weightAdjust2;
	float unknown3;
	float unknown4;
} P_DAT_BUMPER;
typedef struct st_pDAT_BN {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
} P_DAT_BONNET;
typedef struct st_pDAT_OF {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
} P_DAT_OVERFENDER;
typedef struct st_pDAT_MR {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
	float unknown1;
} P_DAT_MIRROR;
typedef struct st_pDAT_SS {
	uint16_t price[3]; // Each class
	int16_t unknown1;
	float unknown2;
	int16_t weightAdjust1;
	int16_t weightAdjust2;
	float unknown3;
	float unknown4;
	float unknown5;
} P_DAT_SIDESKIRT;
typedef struct st_pDAT_RS {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
	float unknown1;
	float unknown2;
} P_DAT_REARSPOILER;
typedef struct st_pDAT_GR {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
	float unknown1;
} P_DAT_GRILL;
typedef struct st_pDAT_LT {
	uint16_t price[3]; // Each class
	int16_t weightAdjust;
	float unknown1;
	float unknown2;
} P_DAT_LIGHTS;
typedef struct st_pDAT_TB {
	uint16_t price[3]; // Each class
	int16_t unknown1;
	int16_t unknown2;
} P_DAT_TIREBRAKES;
typedef struct st_pDATE { 
	P_DAT_ENGINE engine[3][8];
	P_DAT_MUFFLER muffler[3][9];
	P_DAT_TRANSMISSION transmission[3][5];
	P_DAT_DIFFERENTIAL differential[5];
	P_DAT_SUSPENSION suspension[7];
	P_DAT_BODY body[6];
	P_DAT_BUMPER frontBumper[6];
	P_DAT_BONNET bonnet[3];
	P_DAT_OVERFENDER overFender[2];
	P_DAT_MIRROR mirror[3];
	P_DAT_SIDESKIRT sideSkirt[6];
	P_DAT_BUMPER rearBumper[6];
	P_DAT_REARSPOILER rearSpoiler[6];
	P_DAT_GRILL grill[3];
	P_DAT_LIGHTS lights[3];
	P_DAT_TIREBRAKES tireBrakes[10];
} P_DAT_ENTRY;
typedef struct st_pDAT {
	uint8_t count;
	P_DAT_ENTRY* pdat;
} P_DAT;
#pragma endregion
#pragma region sDAT Structure
typedef struct st_sDATE {
	uint8_t turbo;
	uint8_t unknown1[3];
	uint8_t driveType;
	uint8_t unknown2;
	uint8_t carClass;
	uint8_t shopTable;
	uint8_t name[16];
	float gear1Ratio;
	float gear2Ratio;
	float gear3Ratio;
	float gear4Ratio;
	float gear5Ratio;
	float gear6Ratio;
	float gearFinalDrive;
	uint8_t engineType;
	uint8_t cylinders;
	uint16_t displacement;
	float bhp;
	uint16_t bhpRPM1;
	uint16_t bhpRPM2;
	float torque;
	uint16_t torqueRPM1;
	uint16_t torqueRPM2;
	uint16_t limitRPM;
	uint16_t idleRPM;
	uint16_t unknown3[2];
	float unknown4[129];
	float length;
	float width;
	float height;
	float wheelbase;
	float frontwheelOffset;
	float rearwheelOffset;
	float unknown5;
	float weight;
	float unknown6[27];
	uint16_t unknown7[6];
	float unknown8[3];
	uint16_t unknown9[2];
	float unknown10;
	uint16_t unknown11[2];
	float unknown12;
	uint16_t pLookup[2];
	uint8_t pLookup2[8];
	float unknown13[18];
} S_DAT_ENTRY;
typedef struct st_sDAT {
	uint8_t count;
	S_DAT_ENTRY* sdat;
} S_DAT;
#pragma endregion
#pragma region Part Shop Data Structure
typedef struct st_partshopdataentry_redirect {
	uint16_t table;
	uint16_t unknown1[3];
} PARTSHOPDATA_REDIRECT;
typedef struct st_partshopdataentry {
	uint8_t unknown1[12]; // 0x00
	float wheelPrice; // 0x0c
	uint16_t unknown2[4]; // 0x10
	float unknown3; // 0x18
	float tireBrakeBase1; // 0x1c
	uint32_t unknown4; // 0x20
	uint16_t unknown5; // 0x24
	uint16_t unknown6; // 0x26
	uint32_t unknown7; // 0x28
	float unknown8; // 0x2c
	float tireBrakeBase2; // 0x30
	PARTSHOPDATA_REDIRECT redirect[9]; // 0x34
} PARTSHOPDATA_ENTRY;
typedef struct st_partshopdata {
	uint16_t count;
	PARTSHOPDATA_ENTRY* data;
} PARTSHOPDATA;
#pragma endregion
#pragma region Item Shop Data Structure
typedef struct st_itemdataentry {
	uint16_t itemID; // 0x00
	uint8_t itemType; // 0x02 
	uint8_t carClass; // 0x03
	uint16_t carManufacturer; // 0x04
	uint16_t carID; // 0x06
	uint8_t category; // 0x08
	uint8_t type; // 0x09
	uint8_t typeValue; // 0x0A
	uint8_t itemBoost; // 0x0B
	int16_t resaleValue; // 0x0C
	int16_t saleValue; // 0x0E
	uint16_t unknown3; // 0x10
} ITEMDATA_ENTRY;
typedef struct st_itemdata {
	uint16_t count;
	ITEMDATA_ENTRY* data;
} ITEMDATA;
#pragma endregion
#pragma region Rival structures
typedef struct st_rivaldifficulty {
	float f[11];    // 0x00 ... 0x28
	uint8_t b[12];  // 0x2C ... 0x37
} RIVALDIFFICULTY;
typedef struct st_rivalrequirements {
	int8_t level;				// Players level required
	int32_t wins;				// Players wins required
	int32_t kms;				// KMs on vehicle required
	int8_t day;					// Bit flag 7 bits for days of week 1 = Monday ... 64 = Sunday
	int32_t time;				// Bit flag 24 bits for hours in day
	int16_t previousRivals[8];	// Previous Rivals to have beaten
	int8_t driveTrain;			// Bit flag 1 = FWD, 2 = RWD, 4 = AWD
	int8_t engineCylinders;		// Bit flag 1 = 4, 2 = 6, 4 = 8, 8 = Rotary
	int8_t aspiration;			// Bit flag 1 = NA, 2 = Turbo
} RivalREQUIREMENTS;
typedef struct st_rivaldata {
	int32_t rivalID;
	bool customTeam;
	char name[18]; // Client uses string compare to find client so is important that this doesn't match the name of a client. Probably why it's 18 bytes and not 16 so a hidden character can be added to the end.
	int16_t carID;
	CARMODS carMods;
	CARSETTINGS carSettings;
	bool leader;
	TEAMDATA teamData;
	uint32_t cp;
	uint32_t level;
	int32_t routeTable;
	int32_t rewardTable;
	int32_t commonTable;
	float rewardChance;
	RIVALDIFFICULTY difficulty;
	RivalREQUIREMENTS requirements;
} RIVALDATA;
#pragma endregion
#pragma region Ticket Structures
typedef struct st_partticket {
	int16_t engine[8];
	int16_t muffler[9];
	int16_t transmission[5];
	int16_t differential[5];
	int16_t suspension[7];
	int16_t body[6];
	int16_t frontBumper[6];
	int16_t bonnet[3];
	int16_t overFender[2];
	int16_t mirror[3];
	int16_t sideSkirt[6];
	int16_t rearBumper[6];
	int16_t rearSpoiler[6];
	int16_t grill[3];
	int16_t lights[3];
	int16_t tireBrakes[10];
	int16_t bodyColour[1];
} PARTTICKETTABLE;
#pragma endregion
#endif