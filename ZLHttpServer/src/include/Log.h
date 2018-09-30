#ifndef INCLUDE_HTTPLOG_FILE_H
#define INCLUDE_HTTPLOG_FILE_H
/***********************************************************************
2013-09-25 21:02:15
lizheng
***********************************************************************/
#include "Define.h"
#include <stdio.h>

static FILE *g_file;

#define INTIALIZE_LOG(filename) (g_file = fopen(filename, "w"))
#define DEINTIALIZE_LOG         (fclose(g_file))

#define LOGNAME_FLAG "HttpServer"

#define WRITE_LOG(level, s, ...) do {                     \
                   if(NULL == g_file) {                   \
                       break;}                            \
                   time_t time_now = time(NULL);          \
                   tm* tm_now = localtime(&time_now);     \
                   char time_str[50] = {0};               \
                   strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);         \
                   char log_string[300] = {0};            \
                   sprintf(log_string, "%s %s %s - %s \n", time_str, level, LOGNAME_FLAG, s); \
                   fprintf(g_file, log_string, __VA_ARGS__); \
                   fflush(g_file);                           \
                   } while (0)

#define LOG_TRACE(s, ...)    WRITE_LOG("TRACE", s, __VA_ARGS__)
#define LOG_INFO(s, ...)     WRITE_LOG("INFO ", s, __VA_ARGS__)
#define LOG_WARN(s, ...)     WRITE_LOG("WARN ", s, __VA_ARGS__)
#define LOG_ERROR(s, ...)    WRITE_LOG("ERROR", s, __VA_ARGS__)
#define LOG_FATAL(s, ...)    WRITE_LOG("FATAL", s, __VA_ARGS__)

NAMESPACE_ZL_START

class HttpLog
{
public:
	HttpLog(const char *name);
	~HttpLog();
public:
	void Print(char *message, ...);

private:
	FILE   *file_;
};

extern HttpLog		g_httplog;

NAMESPACE_ZL_END

#endif
