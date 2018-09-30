#ifndef _ASST_COMPONENT_H
#define _ASST_COMPONENT_H

#include "cocos2d.h"

class Assistant;

class AsstComponent : public cocos2d::Ref
{
public:
	virtual ~AsstComponent(void);

	virtual bool init();
	virtual void run();
	virtual void update(float delta);
	virtual void clean();

	virtual void setOwner(Assistant *pOwner);
	virtual Assistant* getOwner() const;

	virtual bool isEnabled() const;
	virtual void setEnabled(bool b);

	const std::string& getName() const;
	void setName(const std::string& name);

protected:
	AsstComponent(void);

protected:
	Assistant *mOwner;
	std::string mName;
	bool mEnabled;
};

#endif  // _ASST_COMPONENT_H
