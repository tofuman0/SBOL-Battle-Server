#pragma once
#define _WINSOCKAPI_
#include <string>
#include <Windows.h>
#include "globals.h"
#include "debug.h"

class SERVERPACKET
{
public:
	SERVERPACKET();
	~SERVERPACKET();
	const enum PACKETTYPE {
		PACKETTYPE_AUTH,
		PACKETTYPE_ClientAUTH,
		PACKETTYPE_ClientREQ,
		PACKETTYPE_TBC,
		PACKETTYPE_ClientOPS,
		PACKETTYPE_SERVEROPS
	};
	uint8_t buffer[Client_BUFFER_SIZE];
	void setOffset(uint16_t in) { pOffset = in; }
	uint32_t getOffset() { return pOffset; }
	void addOffset(uint16_t in) { pOffset += in; }
	uint16_t getSize() { return pSize; }
	uint16_t getSizeFromBuffer() { return *(uint16_t*)&buffer[0]; }
	void setSize(uint16_t in) { pSize = in; }
	void addSize(uint16_t in) { pSize += in; }
	uint16_t getType();
	void setType(uint16_t in);
	uint16_t getSubType();
	void setSubType(uint16_t in);
	template <typename T>
	void append(T in);
	template <typename T>
	void set(T in, uint32_t offset);
	template <typename T>
	T get(uint32_t offset);
	template <typename T>
	T get();
	void appendArray(uint8_t * in, uint32_t size);
	void setArray(uint8_t * in, uint32_t size, uint32_t offset);
	void getArray(uint8_t * in, uint32_t size, uint32_t offset);
	void getArray(uint8_t * in, uint32_t size);
	int32_t appendString(std::string& cmd);
	int32_t appendString(std::string& cmd, uint32_t addToSize);
	int32_t setString(std::string& cmd, uint32_t offset);
	std::string getString(uint32_t offset);
	std::string getString(uint32_t offset, uint32_t size);
	std::string getStringA(uint32_t size);
	void clearBuffer();
	virtual void init();
private:
	uint16_t pOffset;
	uint16_t pSize;
};

template<typename T>
void SERVERPACKET::append(T in)
{
	if ((pOffset + sizeof(in)) < Client_BUFFER_SIZE)
	{
		*(T*)&buffer[pOffset] = in;
		pOffset += sizeof(in);
		setSize(getSize() + sizeof(in));
	}
}

template<typename T>
void SERVERPACKET::set(T in, uint32_t offset)
{
	if ((pOffset + sizeof(in)) < Client_BUFFER_SIZE)
	{
		*(T*)&buffer[pOffset] = in;
	}
}

template<typename T>
T SERVERPACKET::get(uint32_t offset)
{
	if (offset + sizeof(T) > Client_BUFFER_SIZE)
		return T();
	else
		return *(T*)&buffer[offset];
}

template<typename T>
T SERVERPACKET::get()
{
	if (pOffset + sizeof(T) > Client_BUFFER_SIZE)
		return T();
	else
	{
		T value = *(T*)&buffer[pOffset];
		pOffset += sizeof(T);
		return value;
	}
}
