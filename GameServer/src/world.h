#pragma once
#include "singleton.h"
#include <thread>
using namespace std;

class world : public singleton<world>
{
public:
	world(void);
	~world(void);

	void start();
private:
	thread* m_ptread;
	bool	m_bRun;
};

