#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"


class ConnectingNode :public Window
{
public:
	ConnectingNode();
	~ConnectingNode();

	virtual bool init();

	virtual void setShow(bool b);

	void hide();

	virtual void update(float delta);

	CREATE_FUNC(ConnectingNode);

	CC_SYNTHESIZE(int, mCurrTaskId, CurrTaskId);

protected:
	ui::Text* mContentLabel;
	Sprite*   mJuhuaSprite;
	float mCounter;
	bool mShowJuhua;

private:
	void setlucency();
	void setUnlucency();
};
