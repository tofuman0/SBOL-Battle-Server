#include <Windows.h>
#include <stdio.h>
#include "debug.h"
#include "globals.h"

void DEBUGSTR(const char* in, ...)
{
	char text[MAX_MESG_LEN] = { 0 };
	va_list args;
	va_start(args, in);
	vsnprintf(text, sizeof(text) - 1, in, args);
	va_end(args);
	OutputDebugStringA(text);
}