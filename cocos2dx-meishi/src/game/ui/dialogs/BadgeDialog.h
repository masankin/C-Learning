#pragma once

#include <array>
#include "game/ui/system/ActionWindow.h"
#include "game/Message.h"
//#include "ranking/RankingControl.h"

class BadgeData;

/************************************************************************/
/*                                                                      */
/************************************************************************/
class BadgeDialog : public ActionWindow, public MessageHandler
{
public:
	CREATE_FUNC(BadgeDialog);

protected:
	static void SelectBadgeBtn(ImageView* label, bool selected);

	BadgeDialog();
	virtual ~BadgeDialog();

	virtual bool init();
	virtual void onShow() override;
	virtual void onClose() override;

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	//virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	virtual void onReceiveMassage(GameMessage * message);

	int getPageSize();

	void onTurnPage(int page, bool force = false);
	void onSelectBadge(cocos2d::ui::ImageView* sender, bool showSwitchAction = true);
	void onDress(int id);
	void onGet(int id);

	void refresh();
	void updateBadgeFrame(const BadgeData* pData, int index);
	void updateBadgeInfoFrame(unsigned int id);
	void dressBadge(unsigned int id);

	void doInfoFrameSwitchAction();
protected:
	static const int CountPerPage = 6;

	int mTotalCount;
	unsigned int mCurrentBadge;
	int mCurrentPage;

	std::array<cocos2d::ui::ImageView*, BadgeDialog::CountPerPage> mBadgeBtns;
	cocos2d::Node* mBadgeInfoNode;
	cocos2d::ui::Text* mPageNumText;

	cocos2d::ui::ImageView* mSelectedBadgeBtn;
};


