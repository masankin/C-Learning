
#include "SevenGoalControl.h"
#include "game/ui/PageFunction.h"

SevenGoalControl::SevenGoalControl():
  mNowDay(0)
, mIsRetrived(false)
, mNowNum(0)
, mNextNum(0)
{
	mProxy = new SevenGoalProxy();
}

SevenGoalControl::~SevenGoalControl()
{

}

void SevenGoalControl::sendGetSevenGoalListRqst()
{
	mProxy->sendGetSevenGoalListRqst();
}

void SevenGoalControl::sendRetriveSevenGoalRqst(int nowDay)
{
	mProxy->sendRetriveSevenGoalRqst(nowDay);
}

void SevenGoalControl::doGetSevenGoalListRsp(const fgame::CSGetSevenGoalListResp& stMsg)
{	
	setNowDay(stMsg.nowday());				//当前第几天
	setIsRetrived(stMsg.isretrived());		//今天是否领取
	setNowNum(stMsg.num());					//今天完成了第几次
	setNextNum(stMsg.nextnum());			//明天完成了几次

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eSevenGoalUpdate", NULL);
}

void SevenGoalControl::setSevenGoalNodeState()
{
	//第七天，并且领过奖
	if (mNowDay == 7 && mIsRetrived == true)		
	{
		return;
	}

	//判断在不在7天范围内 若在则显示图标，否则不显示
	if ((mNowDay > 0 && mNowDay <= 7))
	{
		//sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "SevenGoalNode.csb", 1);
	}
}

void SevenGoalControl::doRetriveSevenGoalResp(const fgame::CSRetriveSevenGoalResp& stMsg)
{
	//有这条消息就表明领取成功了
}
