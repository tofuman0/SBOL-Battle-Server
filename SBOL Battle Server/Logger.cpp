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

Logger::Logger()
{
	logpath = ".\\log";
	CheckLogPath(logpath);
}
Logger::Logger(char* path)
{
	logpath = path;
	CheckLogPath(logpath);
}
Logger::~Logger()
{
}
void Logger::Log(LOGTYPE type, const wchar_t* in, ...)
{
	try
	{
		bool isDebug = false;
#ifdef _DEBUG
		isDebug = true;
#endif
		va_list args;
		wchar_t text[MAX_MESG_LEN];
		wchar_t logbuf[MAX_MESG_LEN];
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
std::wstring Logger::toWide(std::string in)
{
	std::wstring temp(in.length(), L' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
std::string Logger::toNarrow(std::wstring in)
{
	std::string temp(in.length(), ' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
void Logger::setLogPath(char* in)
{
	logpath = in;
	CheckLogPath(logpath);
}
void Logger::setLogPath(std::string& in)
{
	logpath = in;
	CheckLogPath(logpath);
}
std::string Logger::getLogPath()
{
	return logpath;
}
bool Logger::isLogPathSet()
{
	return (logpath.length() > 0) ? true : false;
}
std::wstring Logger::packet_to_text(uint8_t* buf, uint32_t len)
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

void Logger::CheckLogPath(std::string path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) != 0)
	{
		std::cout << "Log path " << path.c_str() << " doesn't exist. Creating path" << std::endl;
		if (!CreateDirectoryA(path.c_str(), NULL))
		{
			std::cout << "Error creating log folder " << path.c_str() << std::endl;
		}
	}
}