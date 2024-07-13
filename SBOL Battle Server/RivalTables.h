#pragma once
#include "structures.h"

RIVALDATA RandomRivals[]{
	{
		0, // Rival ID
		true, // Custom team doesn't exist in rival table
		//"Tofuman..........", // Rival Name.
		"\x83\x67\x83\x74\x83\x7d\x83\x93", // Rival Name.
		1, // Car ID
		{ // Car Mods
			1.0f, // C1 R
			1.0f, // C1 G
			1.0f, // C1 B
			0.0f, // C2 R
			0.0f, // C2 G
			0.0f, // C2 B
			8, // Engine
			9, // Muffler
			5, // Transmission
			5, // Differential
			10, // Tyre and Brakes
			0, // Unknown
			6, // Suspension
			6, // Body
			0, // Overfenders
			0, // Front bumper
			0, // Bonnet
			0, // Mirrors
			0, // Side skirts
			0, // Rear bumper
			3, // Rear spoiler
			0, // Grill
			0, // Lights
			{ 0, 0, 0, 0, 0 }, // Unknown
			250, // Wheels
			1, // Colour wheels
			{ 0, 0, 0 }, // Unknown
			0.2f, // Wheel C1 R
			0.2f, // Wheel C1 G
			0.2f, // Wheel C1 B
			0.2f, // Wheel C2 R
			0.2f, // Wheel C2 G
			0.2f, // Wheel C2 B
			3, // Sticker
			"\0" // Tuner Name
		}, // Car Mods
		{ // Car Settings
			0, // Handle
			0, // Acceleration
			0, // Brake
			0, // Brake Balance
			0, // Spring Rate Front
			0, // Spring Rate Rear
			0, // Damper Rate Front
			0, // Damper Rate Rear
			0, // Gear 1
			0, // Gear 2
			0, // Gear 3
			0, // Gear 4
			0, // Gear 5
			0, // Gear 6
			-2, // Ride Height Front
			-2, // Ride Height Rear
			2, // Final Drive
			-15  // Boost Pressure
		}, // Car Settings
		true, // Leader
		{ // Team Data
			0, // Team ID
			{ 0, 0, 0, 0 }, // Unknown1
			0, // Member ID
			"Fujiwara\0", // Name
			"\0", // Comment
			1, // Member Count
			1, // Invite Only
			0, // Unknown
			0, // Unknown
			0, // Survival Wins
			0, // Survival Loses
			"Tofuman\0", // Leader Name
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } // Unknown2
		}, // Team Data
		10000, // CP
		20, // Level
		1, // Route Table
		0, // Reward Table
		0, // Common Table
		0.0f, // Reward Chance
		{ // Difficulty
			{ 30.0f, 3.0f, 30.0f, 3.0f, 30.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f }, // Floats
			{ 1,2,3,4,5,6,7,8,9,10,11,12 } // Bytes
		},
		{ // Requirements
			0
		},
	}
};