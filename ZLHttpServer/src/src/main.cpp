#include <iostream>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "ConfigFile.h"
#include "TcpServer.h"
#include "HttpServer.h"
#include "Log.h"
#include "Define.h"

void test_log()
{
	ZL::g_httplog.Print("Hello World [%d]\n", 1000);

	INTIALIZE_LOG("HttpServer2.log");
	LOG_TRACE("Hello World [%d]", 1000);
	LOG_TRACE("Hello World [%d][%s]", 1000, "ddddert");
}

bool IsValidUrl(const std::string& url)
{
#ifdef OS_LINUX
	std::string sl_pattern = "^(POST|GET)\\s/.*\\sHTTP/1.1\\s*\n(.*:\\s.*\n).*$";
	regex http_valid_regex(sl_pattern);
	cmatch sl_matches;

	if (!regex_match(http_raw_request.c_str(), sl_matches, http_valid_regex))
		throw bad_request("Request is not valid HTTP.");
#endif
	return true;
}

// Termination signal handler (Ctrl-C)
void HandleTermSig(int snum)
{
	return;
}

// other signals handler
void HandleSigPipe(int snum)
{
	return;
}

bool StartServer(const char *configPath)
{
	ConfigFile config(configPath, "=", "#");
	std::string serverIP = config.read<std::string>("ServerIP", "127.0.0.1");
	short serverPort = config.read<short>("ServerPort", 80);
	std::cout << serverIP << ":" << serverPort << "\n";

	//ZL::TcpServer server(serverPort);
	ZL::HttpServer server(serverPort);
	//ZL::Monitor *monitor = new ZL::HttpMonitor(&server);
	//monitor->Start();

	server.Run("127.0.0.1", serverPort);

	return true;
}

int main()
{
	int i = 0;
	ZL_ASSERT(i > 1);
	ZL_ASSERTEX(i > 1, __FILE__, __LINE__, __FUNCTION__);

#ifdef OS_LINUX
	// Ignore SIGPIPE "Broken pipe" signals when socket connections are broken.
	signal(SIGPIPE, handleSigPipe);

	// Register termination signals
	signal(SIGABRT, &handleTermSig);
	signal(SIGINT, &handleTermSig);
	signal(SIGTERM, &handleTermSig);
#endif

	StartServer("conf/httpserver.conf");

	return 0;
}

//使用chorme浏览器请求127.0.0.1:888/index.html时，server收到的http消息头
//GET /index.html HTTP/1.1
//Host: 127.0.0.1:888
//Connection: keep-alive
//Cache-Control: max-age=0
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*; q = 0.8
//User - Agent: Mozilla / 5.0 (Windows NT 6.1; WOW64) AppleWebKit / 537.36 (KHTML, like
//Gecko) Chrome / 35.0.1916.153 Safari / 537.36
//Accept - Encoding : gzip, deflate, sdch
//Accept - Language : zh - CN, zh; q = 0.8
//RA - Ver: 2.2.22
//RA - Sid : 7B747245 - 20140622 - 042030 - f79ea7 - 5f07a8