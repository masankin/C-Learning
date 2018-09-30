#include "cocos2d.h"
#include "Singleton.h"

#include "cocos2d.h"
USING_NS_CC;

class ItemManager : public Ref
{
public:
	ItemManager();
	~ItemManager();

	bool init(int id);

	void setProgress(int* pro){ m_iProgress = pro; }

	void loadResDataAsync();


	void prelodAnimation(std::string format1, std::string format2, const unsigned int cout, std::string aniName,bool isSingel = false);

private:



	void initArmatureData();
	void loadArmatureCallback(float dt);

protected:

	int* m_iProgress;
	std::list<std::string> mArmautureConfFiles;
};

#define  sgItemManager() Singleton<ItemManager>::Get()
