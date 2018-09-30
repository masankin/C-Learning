#include "Log.h"
#include <Windows.h>
#include <cassert>

NAMESPACE_ZL_START

#define		HTTP_PRINT_TIME

HttpLog		g_httplog("HttpServer.log");

HttpLog::HttpLog(const char *name)
{
	file_ = fopen(name, "w+t");
	assert(file_);
}

HttpLog::~HttpLog()
{
	fclose(file_);
}

void HttpLog::Print(char *message, ...)
{
#ifdef HTTP_PRINT_TIME
	char              msg_text[540];
	int offset = 0;

	SYSTEMTIME timenow;
	GetLocalTime(&timenow);

	sprintf(msg_text + offset, "[%.4d-", timenow.wYear);
	offset += 6;
	sprintf(msg_text + offset, "%.2d-", timenow.wMonth);
	offset += 3;
	sprintf(msg_text + offset, "%.2d ", timenow.wDay);
	offset += 3;
	sprintf(msg_text + offset, "%.2d:", timenow.wHour);
	offset += 3;
	sprintf(msg_text + offset, "%.2d:", timenow.wMinute);
	offset += 3;
	sprintf(msg_text + offset, "%.2d:", timenow.wSecond);
	offset += 3;
	sprintf(msg_text + offset, "%.6d] ", timenow.wMilliseconds);
	offset += 8;

	char              msg[512];
	va_list           marker;
	va_start(marker, message);
	vsprintf_s(msg, message, marker);
	va_end(marker);

	sprintf(msg_text + offset, "%s", msg);
	fprintf(file_, msg_text);
#else
	char              msg_text[512];
	va_list           marker;
	va_start(marker, message);
	vsprintf_s(msg_text, message, marker);
	va_end(marker);

	fprintf(file_, msg_text);
#endif

	fflush(file_);
}


NAMESPACE_ZL_END