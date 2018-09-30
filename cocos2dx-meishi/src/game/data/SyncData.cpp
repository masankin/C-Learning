#include "SyncData.h"
#include "utils/Comm.h"

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "sync/SyncControl.h"

#define SYNC_PROP_DATA_FILE "SyncPropData.json"
#define SYNC_CHECKPOINT_DATA_FILE "SyncCheckPointData.json"

std::string BaseSyncData::getSyncData()
{
	std::string pRet = "";
	return pRet;
}

SyncPropUseData::SyncPropUseData()
{

}
SyncPropUseData::~SyncPropUseData()
{

}

void SyncPropUseData::addDPropata(int propId, int count)
{
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_PROP_DATA_FILE;
	std::string jsStr= FileUtils::getInstance()->getStringFromFile(tPath.c_str());  
	rapidjson::Document doc;
	doc.Parse<0>(jsStr.c_str());
	if (doc.HasParseError()){
		cocos2d::log("doc haserror");

		doc.SetObject();
	}
	if (doc.HasMember("ItemData"))
	{
		rapidjson::Document::AllocatorType& allocator=doc.GetAllocator();
		rapidjson::Value& ItemDataValue = doc["ItemData"];

		rapidjson::Value uValue(rapidjson::kObjectType);
		uValue.AddMember("itemId", propId, allocator);
		uValue.AddMember("count", count, allocator);
		ItemDataValue.PushBack(uValue,allocator);

	}else
	{
		doc.SetObject();

		rapidjson::Document::AllocatorType& allocator=doc.GetAllocator();
		rapidjson::Value uList(rapidjson::kArrayType); 

		rapidjson::Value uValue(rapidjson::kObjectType);
		uValue.AddMember("itemId", propId, allocator);
		uValue.AddMember("count", count, allocator);
		uList.PushBack(uValue, allocator);

		doc.AddMember("ItemData",uList,allocator);
	}


	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	//CCLog("PlayerDataManager-----savePlayerDataToFile()----path is %s", tPath.c_str());
	FILE* file = fopen(tPath.c_str(), "wb"); 

	if (file)
	{
		//CCLog("PlayerDataManager-----savePlayerDataToFile()----content is %s", buffer.GetString());
		fwrite(buffer.GetString(),buffer.Size(), 1, file);
		fclose(file);
	}
}

std::string SyncPropUseData::getSyncData()
{
	std::string pRetStr = "";
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_PROP_DATA_FILE;
	std::string jsStr= FileUtils::getInstance()->getStringFromFile(tPath.c_str());  
	rapidjson::Document doc;
	doc.Parse<0>(jsStr.c_str());
	if (doc.HasParseError())
	{
		//CCLOG("UserManage::LoadUsers parse json error!");
		return pRetStr;
	}
	return jsStr;
}

void SyncPropUseData::clearData()
{
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_PROP_DATA_FILE;
	FILE* file = fopen(tPath.c_str(), "wb"); 
	if (file)
	{
		fwrite("",0, 1, file);
		fclose(file);
	}
}


SyncCheckPointData::SyncCheckPointData()
{

}

SyncCheckPointData::~SyncCheckPointData()
{

}

void SyncCheckPointData::addCheckPointData(int levelId, int star, int passTime)
{
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_CHECKPOINT_DATA_FILE;
	std::string jsStr= FileUtils::getInstance()->getStringFromFile(tPath.c_str());  
	rapidjson::Document doc;
	doc.Parse<0>(jsStr.c_str());
	if (doc.HasParseError()){
		cocos2d::log("doc haserror");

		doc.SetObject();
	}
	if (doc.HasMember("CheckPointData"))
	{
		rapidjson::Document::AllocatorType& allocator=doc.GetAllocator();
		rapidjson::Value& ItemDataValue = doc["CheckPointData"];

		rapidjson::Value uValue(rapidjson::kObjectType);
		uValue.AddMember("pointId", levelId, allocator);
		uValue.AddMember("time",passTime,allocator);
		uValue.AddMember("star",star, allocator);
		ItemDataValue.PushBack(uValue,allocator);

	}else
	{
		doc.SetObject();

		rapidjson::Document::AllocatorType& allocator=doc.GetAllocator();
		rapidjson::Value uList(rapidjson::kArrayType); 

		rapidjson::Value uValue(rapidjson::kObjectType);
		uValue.AddMember("pointId", levelId, allocator);
		uValue.AddMember("time",passTime,allocator);
		uValue.AddMember("star",star, allocator);
		uList.PushBack(uValue, allocator);

		doc.AddMember("CheckPointData",uList,allocator);
	}


	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	//CCLog("PlayerDataManager-----savePlayerDataToFile()----path is %s", tPath.c_str());
	FILE* file = fopen(tPath.c_str(), "wb"); 

	if (file)
	{
		//CCLog("PlayerDataManager-----savePlayerDataToFile()----content is %s", buffer.GetString());
		long long pSize = buffer.Size();
		if (pSize>1024*1024*64)
		{
			fclose(file);
			return;
		}
		fwrite(buffer.GetString(),pSize, 1, file);
		fclose(file);
	}
}

std::string SyncCheckPointData::getSyncData()
{
	std::string pRetStr = "";
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_CHECKPOINT_DATA_FILE;
	std::string jsStr= FileUtils::getInstance()->getStringFromFile(tPath.c_str());  
	rapidjson::Document doc;
	doc.Parse<0>(jsStr.c_str());
	if (doc.HasParseError())
	{
		//CCLOG("UserManage::LoadUsers parse json error!");
		return pRetStr;
	}
	return jsStr;
}

void SyncCheckPointData::clearData()
{
	std::string tPath = FileUtils::getInstance()->getWritablePath() + SYNC_CHECKPOINT_DATA_FILE;
	FILE* file = fopen(tPath.c_str(), "wb"); 
	if (file)
	{
		fwrite("",0, 1, file);
		fclose(file);
	}
}
