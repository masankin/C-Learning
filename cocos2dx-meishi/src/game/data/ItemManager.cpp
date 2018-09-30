#include "ItemManager.h"
#include "cocostudio/CCArmatureDataManager.h"
#include "game/data/TableManager.h"
USING_NS_CC;

ItemManager::ItemManager() : m_iProgress(NULL)
{
	mArmautureConfFiles.clear();
}

ItemManager::~ItemManager()
{

}

bool ItemManager::init(int id)
{
	bool ret = false;
	do
	{

		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("soliderAtlas.plist");
		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/uiAtlas.plist");
		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("jindutiaoAtlas.plist");
		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/Icon/Icon.plist");
		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/Pause/Pause.plist");
		//
		//// 加载建筑五个等级的animation
		//for (int i = 0; i < 5;i++)
		//{
		//	std::string str1 = StringUtils::format("soliderAtlas_fla_baozi%d00%s.png", i, "%d");
		//	std::string str2 = StringUtils::format("soliderAtlas_fla_baozi%d0%s.png", i, "%d");
		//	std::string temp = sgTableManager()->getAnimationNameByCamp(ECAMP_TYPE_Player) + "%d";
		//	std::string camp = StringUtils::format(temp.c_str(), i);
		//	prelodAnimation(str1, str2, 10, camp);

		//	
		//	str1 = StringUtils::format("soliderAtlas_fla_zongzi%d00%s.png", i, "%d");
		//	str2 = StringUtils::format("soliderAtlas_fla_zongzi%d0%s.png", i, "%d");
		//	temp = sgTableManager()->getAnimationNameByCamp(ECAMP_TYPE_AI) + "%d";
		//	camp = StringUtils::format(temp.c_str(), i);
		//	prelodAnimation(str1, str2, 10, camp);

		//	str1 = StringUtils::format("soliderAtlas_fla_hanbao%d00%s.png", i, "%d");
		//	str2 = StringUtils::format("soliderAtlas_fla_hanbao%d0%s.png", i, "%d");
		//	temp = sgTableManager()->getAnimationNameByCamp(ECAMP_TYPE_Player3) + "%d";
		//	camp = StringUtils::format(temp.c_str(), i);
		//	prelodAnimation(str1, str2, 10, camp);

		//	str1 = StringUtils::format("soliderAtlas_fla_dangao%d00%s.png", i, "%d");
		//	str2 = StringUtils::format("soliderAtlas_fla_dangao%d0%s.png", i, "%d");
		//	temp = sgTableManager()->getAnimationNameByCamp(ECAMP_TYPE_Player4) + "%d";
		//	camp = StringUtils::format(temp.c_str(), i);
		//	prelodAnimation(str1, str2, 10, camp);

		//}

		//prelodAnimation("jindutiao000%d.png", "jindutiao00%d.png", 21, "jindutiao",true);

		ret = true;
	} while (0);

	return ret;
}

void ItemManager::prelodAnimation(std::string format1, std::string format2, const unsigned int cout, std::string aniName,bool isSingel /* = false */)
{
	Vector<SpriteFrame*> animFrames(cout);
	for (int r = 1; r <= cout; r++)
	{
		if (r % 2 == 0) continue;
		std::string str = r < 10 ? StringUtils::format(format1.c_str(), r) : StringUtils::format(format2.c_str(), r);
		SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(animFrames, 0.07f);
	AnimationCache::getInstance()->addAnimation(animation, aniName);
}

void ItemManager::initArmatureData()
{
	mArmautureConfFiles.push_back("fla_Fantuan.xml");
	mArmautureConfFiles.push_back("fla_zongzi.xml");
	mArmautureConfFiles.push_back("fla_baozi.xml");
	mArmautureConfFiles.push_back("fla_zhuanhuanTX.xml");
	mArmautureConfFiles.push_back("fla_behurteffect.xml");
	mArmautureConfFiles.push_back("fla_dingweiguang.xml");
	mArmautureConfFiles.push_back("fla_shengji.xml");
	mArmautureConfFiles.push_back("fla_yingdao.xml");
	mArmautureConfFiles.push_back("fla_shenjizhong.xml");
	mArmautureConfFiles.push_back("fla_eqp041bz.xml");
	mArmautureConfFiles.push_back("fla_ghost.xml");
	mArmautureConfFiles.push_back("fla_range_didui.xml");
	mArmautureConfFiles.push_back("fla_QiZhiShengJi.xml");
	mArmautureConfFiles.push_back("fla_gaijian.xml");
	mArmautureConfFiles.push_back("fla_timeback.xml");
	mArmautureConfFiles.push_back("fla_beijingtx.xml");
	mArmautureConfFiles.push_back("fla_huidie.xml");
	mArmautureConfFiles.push_back("fla_jidan.xml");
	mArmautureConfFiles.push_back("fla_dacong.xml");
	mArmautureConfFiles.push_back("fla_nailao.xml");
	mArmautureConfFiles.push_back("fla_suantou.xml");
	mArmautureConfFiles.push_back("fla_xiami1.xml");
	mArmautureConfFiles.push_back("fla_lajiao.xml");
	mArmautureConfFiles.push_back("fla_Hanbao.xml");
	mArmautureConfFiles.push_back("fla_Dangao.xml");
	mArmautureConfFiles.push_back("fla_buff.xml");
	mArmautureConfFiles.push_back("fla_youxidingwei.xml");
}

void ItemManager::loadResDataAsync()
{
	initArmatureData();

	std::list<std::string>::iterator it = mArmautureConfFiles.begin();
	for (; it != mArmautureConfFiles.end(); ++it)
	{

		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfoAsync(*it, this, schedule_selector(ItemManager::loadArmatureCallback));

	}
}

void ItemManager::loadArmatureCallback(float dt)
{
	static int loaded = 0;

	loaded++;
	int total = mArmautureConfFiles.size();

	*m_iProgress = int(loaded * 100 / total);
}
