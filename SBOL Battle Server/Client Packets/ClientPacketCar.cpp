#include <Windows.h>
#include "..\server.h"

void ClientPacketCar(Client* client)
{
	uint16_t pType = client->inbuf.getType();
	// Prevent shop packets to be processed when not in shops
	if (pType != 0x200 && pType != 0x201 && pType != 0x210 && pType != 0x206 && client->inCourse) return;

	switch (pType)
	{
	case 0x200:
	{
		client->SendBayDetails();
		return;
	}
	break;
	case 0x201:
	{	// Car Settings
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number when requesting car data.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (!client->garagedata.activeCar)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client has no active car.");
			client->Disconnect();
			return;
		}
		client->SendCarData(bay);
		return;
	}
	break;
	case 0x202:
	{	// Client Switched cars
		uint32_t selectedCar = client->inbuf.get<uint32_t>(0x04);
		if (selectedCar > client->garagedata.car.size() || selectedCar < 1)
		{
			client->Disconnect();
			return;
		}
		if (client->garagedata.car[selectedCar - 1].carID == 0xFFFFFFFF)
		{
			client->Disconnect();
			return;
		}
		client->setActiveCar(selectedCar - 1);
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x282);
		client->outbuf.append<uint32_t>(selectedCar);
	}
	break;
	case 0x203:
	{	// Purchase Car
		// 0x04: Car ID
		// 0x08: Colour1 R
		// 0x0C: Colour1 G
		// 0x10: Colour1 B
		// 0x14: Colour1 R
		// 0x18: Colour1 G
		// 0x1C: Colour1 B
		// 0x20: Cost
		uint32_t carID = client->inbuf.get<uint32_t>(0x04);
		float r1 = client->inbuf.get<float>(0x08);
		float g1 = client->inbuf.get<float>(0x0C);
		float b1 = client->inbuf.get<float>(0x10);
		float r2 = client->inbuf.get<float>(0x14);
		float g2 = client->inbuf.get<float>(0x18);
		float b2 = client->inbuf.get<float>(0x1C);
		uint32_t cost = client->inbuf.get<uint32_t>(0x20);

		if (client->isInCarShop(carID) == -1)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car not in shop. Car ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				carID
			);
			client->Disconnect();
			return;
		}

		uint16_t finalPrice = client->getShopCarPrice(carID);
		if (finalPrice == -1 || cost != finalPrice)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has provided different price for car than expected. Car ID %u, Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				carID,
				cost,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		int32_t emptyBay = client->getEmptyBay();
		if (emptyBay == -1 || emptyBay > 3)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car with no room in garage. Car ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				carID
			);
			client->Disconnect();
			return;
		}

		if (client->enoughCP(finalPrice))
		{
			if (client->addCar(carID, emptyBay, r1, g1, b1, r2, g2, b2) == true) client->takeCP(finalPrice);
			else
			{
				client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car from the car but there was an error adding car to garage. Car ID %u, Car count: %u.",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str(),
					carID,
					client->getCarCount()
				);
				client->Disconnect();
				return;
			}
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car from the car shop without enough CP. Car ID %u, Client CP %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				carID,
				client->getCP(),
				finalPrice
			);
			client->Disconnect();
			return;
		}


		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x283);
		client->outbuf.append<int32_t>(emptyBay + 1); // Available Slot
		client->outbuf.append<int64_t>(client->getCP());
	}
	break;
	case 0x204:
	{	// Sell car
		// Bay @ 0x04
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;

		if (client->getCarCount() <= 1 || bay == client->garagedata.activeCarBay)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has tried to sell car but only has 1 car or is trying to sell active car.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number in car sell shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		uint32_t resalePrice = client->getCarSalePrice(bay);

		if (client->removeCar(bay) == false)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has failed to remove car from garage when selling. Bay %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				bay
			);
			client->Disconnect();
			return;
		}

		client->giveCP(resalePrice);

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x284);
		client->outbuf.append<int32_t>(bay + 1);
		client->outbuf.append<int64_t>(client->getCP());
	}
	break;
	case 0x205:
	{	// Occurs painting car. Car bay at 0x04, Then 6 floats for colour at 0x08, cost at 0x20
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x285);

		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint32_t cost = client->inbuf.get<uint32_t>(0x20);

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number in paint shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (cost != (PAINT_BASE_COST * (3 - client->getCarClass())))
		{	// Painting costs PAINT_COST
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has to purchase paint job and specified altered cost. Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				cost,
				(PAINT_BASE_COST * (3 - client->getCarClass()))
			);
			client->Disconnect();
			return;
		}
		else if (bay < (uint32_t)(client->garagedata.garageCount * 4) && client->garagedata.car[bay].carID != 0xFFFFFFFF && client->enoughCP(cost))
		{
			client->inbuf.setOffset(0x08);
			client->takeCP(cost);
			client->garagedata.car[bay].carMods.colour1_R = client->inbuf.get<float>();
			client->garagedata.car[bay].carMods.colour1_G = client->inbuf.get<float>();
			client->garagedata.car[bay].carMods.colour1_B = client->inbuf.get<float>();
			client->garagedata.car[bay].carMods.colour2_R = client->inbuf.get<float>();
			client->garagedata.car[bay].carMods.colour2_G = client->inbuf.get<float>();
			client->garagedata.car[bay].carMods.colour2_B = client->inbuf.get<float>();
			client->outbuf.append<uint32_t>(PAINT_BASE_COST * (3 - client->getCarClass())); // Cost
			client->outbuf.append<int64_t>(client->getCP()); // CP 
		}
		else
		{	// Client shouldn't be able to get here if they can't afford it.
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x206:
	{	// Battle end damage
		// Bay @ 0x04
		// Damage Value 1 @ 0x08
		// Damage Value 2 @ 0x0C - This is distance travelled divided by 100
		// Damage Value 3 @ 0x10
		// Damage Value 4 @ 0x14
		uint32_t bay = client->inbuf.get<uint32_t>(0x04);
		uint32_t damage1 = client->inbuf.get<uint32_t>(0x08);
		uint32_t damage2 = client->inbuf.get<uint32_t>(0x0C);
		uint32_t damage3 = client->inbuf.get<uint32_t>(0x10);
		uint32_t damage4 = client->inbuf.get<uint32_t>(0x14);
		client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has finished battle. Damage 1: %u, Damage 2: %u, Damage 3: %u, Damage 4: %u.",
			client->logger->toWide(client->handle).c_str(),
			client->driverslicense,
			client->logger->toWide((char*)&client->IP_Address).c_str(),
			client->courseID,
			damage1,
			damage2,
			damage3,
			damage4
		);

		uint32_t currentCondition = client->garagedata.activeCar->engineCondition;
		client->adjustEngineCondition(damage1, damage2, damage3, damage4);
		std::stringstream ss;
		int8_t state = 0;

		if (currentCondition < client->garagedata.activeCar->engineCondition)
			state = 1;
		else if (currentCondition > client->garagedata.activeCar->engineCondition)
			state = 2;

		float wear = (float)(currentCondition - client->garagedata.activeCar->engineCondition) / 100.0f;
		ss << std::setprecision(2) << wear << "% wear on engine occured.";
		client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);

		ss.str("");

		switch (state)
		{
		case 0:
			ss << "Engine condition unchanged!";
			break;
		case 1:
			ss << "Engine condition changed! Engine condition was increased during  last battle.";
			break;
		case 2:
			ss << "Engine condition changed! Engine condition was decreased during  last battle.";
			break;
		default:
			break;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x286);
		client->outbuf.append<uint8_t>(1); // Show Message
		client->outbuf.append<uint16_t>(client->garagedata.activeCar->engineCondition / 100);  // Engine Condition
		client->outbuf.append<uint8_t>(state); // State up right down
		client->outbuf.appendString(ss.str(), 96);
	}
	break;
	case 0x207:
	{	// Car Sale Value
		// Car Bay @ 0x04
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number in car sell shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (bay == client->getActiveCar())
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified active bay number in car sell shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		uint32_t salePrice = client->getCarSalePrice(bay);

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x287);
		client->outbuf.append<uint32_t>(bay + 1); // Slot
		client->outbuf.append<uint32_t>(salePrice); // Sale Value
	}
	break;
	case 0x208:
	{	// Car shop.
		client->enableShopPackets();
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x288);
		client->shopCars.clear();
		for (uint32_t i = 0; i < client->server->itemData.count; i++)
		{
			if (client->server->itemData.data[i].itemType == 0 && client->isValidCar(client->server->itemData.data[i].carID)) client->shopCars.push_back(client->server->itemData.data[i].carID);
		}
		client->outbuf.append<uint16_t>((uint16_t)client->shopCars.size());
		for (uint32_t i = 0; i < client->shopCars.size(); i++) client->outbuf.append<uint32_t>(client->shopCars[i]);
	}
	break;
	case 0x209:
	{	// Enter paintshop
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x289);
		client->outbuf.append<uint32_t>(client->getActiveCar() + 1); // Active Car
		client->outbuf.append<uint32_t>(PAINT_BASE_COST * (3 - client->getCarClass())); // Paint Cost
	}
	break;
	case 0x20A:
	{	// Get Engine Overhaul Price
		// TODO: adjust this if required
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number in engine overhaul.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}
		uint32_t overhaulPrice = client->calculateOverhaul(bay);

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x08);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x28A);
		client->outbuf.append<uint32_t>(client->getActiveCar() + 1); // Active Car
		client->outbuf.append<uint32_t>(overhaulPrice); // Overhaul Cost
	}
	break;
	case 0x20B:
	{	// Engine Overhaul
		// 0x04 - Car Bay
		// 0x08 - Cost
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint32_t cost = client->inbuf.get<uint32_t>(0x08);
		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number in engine overhaul.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}
		uint32_t overhaulPrice = client->calculateOverhaul(bay);
		if (cost != overhaulPrice)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has provided incorrect cost for engine overhaul. Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				cost,
				overhaulPrice
			);
			client->Disconnect();
			return;
		}
		if (client->enoughCP(overhaulPrice))
		{
			client->takeCP(overhaulPrice);
			client->garagedata.car[bay].engineCondition = 10099;
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x08);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x200);
			client->outbuf.setSubType(0x28B);
			client->outbuf.append<uint32_t>(bay + 1); // Active Car
			client->outbuf.append<int64_t>(client->getCP()); // CP
			client->outbuf.append<uint8_t>(1); // Success
			client->outbuf.append<uint16_t>((uint16_t)client->garagedata.car[bay].engineCondition / 100); // Condition
			client->outbuf.append<uint8_t>(0); // Status?
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has attempted to pay for engine overhaul without enough CP. Client CP %u, Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				overhaulPrice,
				client->getCP()
			);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x20C:
	{	// Tuned Car Purchase list
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x08);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x28C);
		client->outbuf.append<uint32_t>(0); // ????
		client->outbuf.append<uint32_t>(0); // ????
		client->outbuf.append<uint16_t>(0); // Car count
		//client->outbuf.append<uint32_t>(3); // ????
		//client->outbuf.append<uint32_t>(4); // ????
		//client->outbuf.append<uint32_t>(5); // ????
		//client->outbuf.append<uint32_t>(6); // ????
		//client->outbuf.addOffset(0x20);
		//client->outbuf.addSize(0x20);
		//client->outbuf.append<uint8_t>(0);
	}
	break;
	case 0x20F:
	{	// Tuned Car Purchase list
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x08);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x28F);
		client->outbuf.append<uint32_t>(0); // ????
		client->outbuf.append<uint32_t>(0); // ????
		client->outbuf.append<uint8_t>(1); // Success?
	}
	break;
	case 0x210:
	{	// Returning to highway. inform client of recent occurance
		client->enableCoursePackets();
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x290);
		client->outbuf.append<int64_t>(client->getCP(), false); // CP
		client->outbuf.append<uint8_t>(0); // Structure Count. Set 0 if no information
		break;
		//client->outbuf.append<uint32_t>(0); // ????
		//client->outbuf.append<uint16_t>(0); // Switch: O: Car Sold, 1: ?, 2: Car failed to sell, 3: ?
		//client->outbuf.append<uint32_t>(0); // CP Credited/Taken
	}
	break;
	case 0x212:
	{	// Putting Car up for sale
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x292);
		// TODO: Retrieve cost from data files.
		client->outbuf.append<uint32_t>(1000, false); // Standard Cost
		client->outbuf.append<uint32_t>(1); // ????
	}
	break;
	case 0x220:
	{	// Car setting change - Car slot at 0x04, Handle at 0x08, Config data at 0x18 (18 bytes)
		int32_t slot = client->inbuf.get<uint32_t>(0x04) - 1;
		if (client->inbuf.getString(0x08, 0x10).compare(client->handle) == 0 && slot < 12 && slot >= 0 && client->garagedata.car[slot].carID != 0xFFFFFFFF)
		{
			if (client->garagedata.car[slot].carMods.tuner[0] == 0) client->inbuf.getArray(&client->garagedata.car[slot].carMods.tuner[0], sizeof(CARMODS::tuner), 0x08);
			client->inbuf.getArray((uint8_t*)&client->garagedata.car[slot].carSettings, sizeof(CARSETTINGS), 0x18);
		}
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x2A0);
	}
	break;
	case 0x221:
	{	// Swapping car slot in garage
		// ?? at 0x04
		// ?? at 0x05
		// ?? at 0x06
		// From Garage at 0x07
		// From Bay at 0x08
		// ?? at 0x09
		// ?? at 0x0A
		// ?? at 0x0B
		// To Garage at 0x0C
		// To Bay at 0x0D
		uint32_t fromBay = client->inbuf.get<uint8_t>(0x08) + ((client->inbuf.get<uint8_t>(0x07) ? 1 : 0) * 4);
		uint32_t toBay = client->inbuf.get<uint8_t>(0x0D) + ((client->inbuf.get<uint8_t>(0x0C) ? 1 : 0) * 4);
		uint8_t result = 0;
		if (toBay == client->getActiveCar() ||
			toBay >= (uint32_t)(client->garagedata.garageCount * 4) ||
			fromBay >= (uint32_t)(client->garagedata.garageCount * 4) ||
			client->garagedata.car[fromBay].carID == 0xFFFFFFFF ||
			fromBay == toBay)
			result = 1;
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x08);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x200);
		client->outbuf.setSubType(0x2A1);
		client->outbuf.append<uint8_t>(result); // 0: Success 1: Failure
		if (!result)
		{
			uint8_t isSwapping = 0;
			if (client->garagedata.car[toBay].carID != 0xFFFFFFFF) isSwapping = 1;
			iter_swap(client->garagedata.car.begin() + (fromBay), client->garagedata.car.begin() + (toBay));
			client->server->managementserver.SwapCar(client->driverslicense, isSwapping, fromBay, toBay);
		}
	}
	break;
	default:
		return;
	}
	client->Send();
}
