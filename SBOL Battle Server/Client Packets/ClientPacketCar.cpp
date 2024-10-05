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
		client->SendCarSettings(bay);
	}
	return;
	case 0x202:
	{	// Client Switched cars
		uint32_t bay = client->inbuf.get<uint32_t>(0x04);
		client->SendSwitchCar(bay);
	}
	return;
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
		COLOUR2 colour = {
			client->inbuf.get<float>(0x08),
			client->inbuf.get<float>(0x0C),
			client->inbuf.get<float>(0x10),
			client->inbuf.get<float>(0x14),
			client->inbuf.get<float>(0x18),
			client->inbuf.get<float>(0x1C)
		};
		uint32_t cost = client->inbuf.get<uint32_t>(0x20);
		client->SendPurchaseCar(carID, colour, cost);
	}
	return;
	case 0x204:
	{	// Sell car
		// Bay @ 0x04
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		client->SendSellCar(bay);
	}
	return;
	case 0x205:
	{	// Occurs painting car. Car bay at 0x04, Then 6 floats for colour at 0x08, cost at 0x20
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		client->inbuf.setOffset(0x08);
		COLOUR2 colour = {
			client->inbuf.get<float>(),
			client->inbuf.get<float>(),
			client->inbuf.get<float>(),
			client->inbuf.get<float>(),
			client->inbuf.get<float>(),
			client->inbuf.get<float>()
		};
		uint32_t cost = client->inbuf.get<uint32_t>(0x20);
		client->SendPaintCar(bay, cost, colour);
	}
	return;
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
		client->SendBattleDamage(bay, damage1, damage2, damage3, damage4);
	}
	return;
	case 0x207:
	{	// Car Sale Value
		// Car Bay @ 0x04
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		client->SendCarValue(bay);
	}
	return;
	case 0x208:
	{	// Car shop.
		client->enableShopPackets();
		client->SendCarShop();
	}
	return;
	case 0x209:
	{	// Enter paintshop
		client->SendPaintShopPrice();
	}
	return;
	case 0x20A:
	{	// Get Engine Overhaul Price
		// TODO: adjust this if required
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		client->SendOverhaulPrice(bay);
	}
	return;
	case 0x20B:
	{	// Engine Overhaul
		// 0x04 - Car Bay
		// 0x08 - Cost
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint32_t cost = client->inbuf.get<uint32_t>(0x08);
		client->SendOverhaul(bay, cost);
	}
	return;
	case 0x20C:
	{	// Tuned Car Purchase list
		client->SendTunedCarPurchaseList();
	}
	return;
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
