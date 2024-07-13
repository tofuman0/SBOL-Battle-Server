#define _WINSOCKAPI_
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include "globals.h"
#include "Logger.h"

const wchar_t* LOGFILES[]{
	nullptr,
	L"server",
	L"packet",
	L"error",
	L"debug",
	L"comm",
	L"client",
	L"management"
};

LOGGER::LOGGER()
{
	logpath = ".\\log";
}
LOGGER::LOGGER(char* path)
{
	logpath = path;
}
LOGGER::~LOGGER()
{
}
void LOGGER::Log(LOGTYPE type, const wchar_t* in, ...)
{
	try
	{
		bool isDebug = false;
#ifdef _DEBUG
		isDebug = true;
#endif
		va_list args;
		wchar_t text[MAX_MESG_LEN];
		wchar_t logbuf[FILENAME_MAX];
		wchar_t buf[MAX_MESG_LEN];

		SYSTEMTIME rawtime;

		GetLocalTime(&rawtime);
		va_start(args, in);
		vswprintf(text, MAX_MESG_LEN - 10, in, args);
		va_end(args);

		wcscat_s(text, L"\n");
		if (wcsnlen(text, MAX_MESG_LEN - 10) == MAX_MESG_LEN - 10) wcscat_s(text, L"\n");
		swprintf(&buf[0], MAX_MESG_LEN - 2, L"[%02u-%02u-%u, %02u:%02u:%02u] %s", rawtime.wMonth, rawtime.wDay, rawtime.wYear, rawtime.wHour, rawtime.wMinute, rawtime.wSecond, text);

		if (type == LOGTYPE_DEBUG && isDebug)
			OutputDebugString(&buf[0]);

		wprintf(buf);

		if ((type == LOGTYPE_CLIENT || type == LOGTYPE_ERROR || type == LOGTYPE_PACKET || type == LOGTYPE_SERVER || (type == LOGTYPE_DEBUG && isDebug) || (isDebug && type != LOGTYPE_NONE)) && logpath.length() > 0)
		{
			swprintf(&logbuf[0], MAX_MESG_LEN - 10, L"%s\\%s%02u%02u%04u.log", toWide(logpath).c_str(), LOGFILES[type], rawtime.wMonth, rawtime.wDay, rawtime.wYear);
			
			std::wfstream logFile(logbuf, std::ios::app | std::ios::ate);
			if (logFile.is_open())
			{
				logFile << buf;
				logFile.close();
			}
			else
			{
				wprintf(L"Error opening logfile: %s", logbuf);
			}
		}
	}
	catch (std::exception ex)
	{
		wprintf(L"Error writing to logfile: %s", toWide(ex.what()).c_str());
	}
}
std::wstring LOGGER::toWide(std::string in)
{
	std::wstring temp(in.length(), L' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
std::string LOGGER::toNarrow(std::wstring in)
{
	std::string temp(in.length(), ' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
void LOGGER::setLogPath(char* in)
{
	logpath = in;
}
void LOGGER::setLogPath(std::string& in)
{
	logpath = in;
}
std::string LOGGER::getLogPath()
{
	return logpath;
}
bool LOGGER::isLogPathSet()
{
	return (logpath.length() > 0) ? true : false;
}
std::wstring LOGGER::packet_to_text(uint8_t* buf, uint32_t len)
{
	try
	{
		std::wstringstream wss;
		if (len > PACKET_BUFFER_SIZE)
		{
			return std::wstring();
		}
		wss << std::endl;
		for (uint32_t c = 0; c < len; c++)
		{
			wss << std::uppercase << std::setfill(L'0') << std::setw(2) << std::hex << buf[c] << " ";
			if (c != 0 && !((c + 1) % 16))
			{
				wss << "  ";
				for (int32_t c2 = 0; c2 < 16; c2++)
				{
					if (((c + 1) - 16) + c2 < len)
					{
						if (buf[((c + 1) - 16) + c2] >= 0x20 && buf[((c + 1) - 16) + c2] < 0x7F)
							wss << std::nouppercase << (char)buf[((c + 1) - 16) + c2];
						else
							wss << ".";
					}
				}
				wss << std::endl;
			}
			else if (c == (len - 1) && ((c + 1) % 16))
			{
				int32_t remaining = (len % 16);
				for (int32_t c2 = 0; c2 < 16 - remaining; c2++)
				{
					wss << "   ";
				}
				wss << "  ";
				for (int32_t c2 = 0; c2 < remaining; c2++)
				{
					if (((c + 1) - remaining) + c2 < len)
					{
						if (buf[((c + 1) - remaining) + c2] >= 0x20 && buf[((c + 1) - remaining) + c2] < 0x7F)
							wss << std::nouppercase << (char)buf[((c + 1) - remaining) + c2];
						else
							wss << ".";
					}
				}
				wss << std::endl;
			}
		}
		return wss.str();
	}
	catch (std::exception ex) { }
	return std::wstring();
}