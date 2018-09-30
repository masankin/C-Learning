
#include "topRankModel_Cpp.h"

topRankModel_Cpp::topRankModel_Cpp() : mTimeStar(0), mTimeWin(0), mTimeWinStreak(0)
{

}

topRankModel_Cpp::~topRankModel_Cpp()
{

}

void topRankModel_Cpp::setTimeStar(float var)
{ 
	if (var < 0)
	{
		var = 0;
	}

	mTimeStar = var;
};


void topRankModel_Cpp::setTimeWin(float var)
{ 
	if (var < 0)
	{
		var = 0;
	}

	mTimeWin = var;
};


void topRankModel_Cpp::setTimeWinStreak(float var)
{ 
	if (var < 0)
	{
		var = 0;
	}

	mTimeWinStreak = var; 
};
