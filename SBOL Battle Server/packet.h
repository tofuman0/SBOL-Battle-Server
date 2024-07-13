#pragma once
#define _WINSOCKAPI_
#include <string>
#include <Windows.h>
#include "globals.h"
#include "debug.h"

class PACKET
{
public:
	PACKET();
	~PACKET();
	uint8_t buffer[CLIENT_BUFFER_SIZE];
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
	uint32_t getVer();
	void setVer(uint32_t in);
	template <typename T>
	void append(T in, bool SWAP = true);
	template <typename T>
	void set(T in, uint32_t offset, bool SWAP = true);
	template <typename T>
	T get(uint32_t offset, bool SWAP = true);
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
	template<typename T>
	T swap(T in);
	virtual uint16_t swapShort(uint16_t in);
	virtual int16_t swapShort(int16_t in);
	virtual uint32_t swapLong(uint32_t in);
	virtual int32_t swapLong(int32_t in);
	virtual uint64_t swapLongLong(uint64_t in);
	virtual int64_t swapLongLong(int64_t in);
	virtual float swapFloat(float in);
	uint16_t pOffset;
	uint16_t pSize;
};

template<typename T>
void PACKET::append(T in, bool SWAP)
{
	if ((pOffset + sizeof(in)) < CLIENT_BUFFER_SIZE)
	{
		*(T*)&buffer[pOffset] = SWAP ? swap(in) : in;
		pOffset += sizeof(in);
		setSize(getSize() + sizeof(in));
	}
}

template<typename T>
void PACKET::set(T in, uint32_t offset, bool SWAP)
{
	if ((pOffset + sizeof(in)) < CLIENT_BUFFER_SIZE)
	{
		*(T*)&buffer[pOffset] = SWAP ? swap(in) : in;
	}
}

template<typename T>
T PACKET::get(uint32_t offset, bool SWAP)
{
	if (offset + sizeof(T) > CLIENT_BUFFER_SIZE)
		return T();
	else
		return (SWAP ? swap(*(T*)&buffer[offset]) : *(T*)&buffer[offset]);
}

template<typename T>
T PACKET::get()
{
	if (pOffset + sizeof(T) > CLIENT_BUFFER_SIZE)
		return T();
	else
	{
		T value = swap(*(T*)&buffer[pOffset]);
		pOffset += sizeof(T);
		return value;
	}
}

template<typename T>
T PACKET::swap(T in)
{
	if (strcmp(typeid(T).name(), "unsigned short") == 0 ||
		strcmp(typeid(T).name(), "signed short") == 0 ||
		strcmp(typeid(T).name(), "short") == 0)
	{
		return (T)SWAP_SHORT((uint16_t)in);
	}
	else if (strcmp(typeid(T).name(), "unsigned int") == 0 ||
			 strcmp(typeid(T).name(), "signed int") == 0 ||
			 strcmp(typeid(T).name(), "int") == 0)
	{
		return (T)SWAP_LONG((uint32_t)in);
	}
	else if (strcmp(typeid(T).name(), "float") == 0)
	{
		uint32_t tmp = *(uint32_t*)&in;
		tmp = SWAP_LONG(tmp);
		return *(T*)&tmp;
	}
	else if (strcmp(typeid(T).name(), "unsigned __int64") == 0 ||
			 strcmp(typeid(T).name(), "signed __int64") == 0 ||
			 strcmp(typeid(T).name(), "__int64") == 0)
	{
		return (T)SWAP_LONGLONG((uint64_t)in);
	}
	else
		return in;
}