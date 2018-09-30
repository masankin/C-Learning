#ifndef _ASST_EVENT_H
#define _ASST_EVENT_H

namespace AsstEvent
{
	const std::string ActiveSkillReq = "AsstEvent_ActiveSkillRequest";
	struct ActiveSkillRequestData
	{
		int campType;
		unsigned int skillID;
		int randomCount;
	};

}
#endif
