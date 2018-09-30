#pragma once
#include "singleton.h"
#include "IOCPServer.h"

class worldServer : public singleton<worldServer>
{
public:
	worldServer(void);
	~worldServer(void);

	bool start();
	void update();
private:
	IOCPServer *m_pIOCPServer;
};

