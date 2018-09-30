#ifndef UpdateScene_h__
#define UpdateScene_h__

#include "updateView.h"

#include "cocos2d.h"
USING_NS_CC;

class UpdateScene: public cocos2d::Scene
{
public:

	bool init();
	CREATE_FUNC(UpdateScene);


	virtual void onEnter();

	virtual void onExit();

	virtual void update(float dt);

private:
	UpdateScene();
	~UpdateScene();

	static Scene* scene;
	UpdateView* m_pUpdateView;
};

#endif // UpdateResScene_h__
