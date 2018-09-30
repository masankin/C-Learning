#pragma once

#include "BaseProxy.h"

class DayTaskProxy : public BaseProxy
{
public:
	DayTaskProxy();
	~DayTaskProxy();

	virtual void	DoRsp(NetPack& stRsp);
	void			sendMissionListReq();                   // 请求每日任务列表
	void			sendMissionComplete(int missionId);  // 任务完成请求
};

