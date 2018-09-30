#include "MailData.h"
#include "Message.h"
#include "core/MessageManager.h"
#include "core/HelperFunc.h"
#include <fstream>
#include "data/TableManager.h"
#include "google/protobuf/io/coded_stream.h"
#include "utils/Comm.h"
#include "CVTime.h"


#define SAVE_MAIL_FILE "maildata1_2.db"  

MailData::MailData()
{
	initMail();

	mRealPorp.clear();
}

MailData::~MailData()
{
	
}


void MailData::initMail()
{
	std::string dbPath = FileUtils::getInstance()->getWritablePath() + SAVE_MAIL_FILE;
	if(FileUtils::getInstance()->isFileExist(dbPath) == true)
	{
		loadMailData(dbPath); 
	}
	else
	{
		initMailData();
	}

	loadDataByPb();

	removeOldMail();
}

void MailData::loadMailData(std::string dbPath)
{
	std::fstream dbReadHand(dbPath.c_str(), std::fstream::binary | std::ios::in );		

	if ( dbReadHand.fail() == true)  
		return;  //打开失败;	

	dbReadHand.seekg (0, std::ios::end);
	int length = dbReadHand.tellg();
	dbReadHand.seekg (0, std::ios::beg);
	if (length <= 0) 
		return;   //打开正常，但是没有数据;

	// allocate memory:
	char* buffer = new char [length];  
	// read data as a block:
	dbReadHand.read (buffer,length);		

	int dataLength = 0;
	memcpy(&dataLength, buffer, 4);

	mDataMail.ParseFromArray(buffer + 5 , dataLength - 4 -  buffer[4]);

	delete[] buffer;
	dbReadHand.close();  
}


void MailData::initMailData()
{
	/*mDataMail.set_newmail(false);
	mDataMail.set_mailno(0);*/
}


void MailData::saveToFile()
{
	saveMailData();
	
}

void MailData::saveMailData()
{
	getDataMailUin();
	std::string buff; 
	bool isOk = mDataMail.SerializeToString(&buff); 
	if(isOk == false) return;

	char addNum = (16 -  (buff.length() + 5 ) % 16);  //填充字节的个数; 
	int length =  buff.length() + (int)addNum + 5;   //填充后的字节数;
	char* buffer = new char [length];
	memcpy(buffer, &length ,4);
	memcpy(buffer + 4,&addNum, 1);
	memcpy(buffer + 5, buff.c_str(),buff.length());

	std::string dbPath = FileUtils::getInstance()->getWritablePath() + SAVE_MAIL_FILE;
	std::fstream* dbSaveHand = new std::fstream(dbPath.c_str(), std::fstream::binary  | std::ios::out );
	if (dbSaveHand)
	{
		dbSaveHand->seekp (0, std::ios::beg);
		dbSaveHand->write(buffer, length);
		dbSaveHand->flush();
	}
	delete[] buffer; 
}

void MailData::loadDataByPb()
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	setNewMail(selfMail.newmail());

}

void MailData::removeOldMail()
{
	std::vector<int> pDelMailVec;
	CVTime pTime(time(0));
	fgame::DataMailUin& selfMail = getDataMailUin();
	for (int i = 0; i<selfMail.mailinfovec().mailinfo_size(); i++)
	{
		if (pTime.GetTime() > selfMail.mailinfovec().mailinfo(i).outdate())
		{
			pDelMailVec.push_back(selfMail.mailinfovec().mailinfo(i).id());
		}
	}

	if (pDelMailVec.size()>0)
	{
		delMail(pDelMailVec);
	}
}

void MailData::addNewMail(const fgame::CSGetMailResp& getMail)
{
	fgame::DataMailUin& selfMail = getDataMailUin();

	selfMail.set_mailno(getMail.mailno());
	for (int i = 0; i<getMail.mailinfo_size(); i++)
	{
		fgame::DataMailInfoVec* pInfoVec = selfMail.mutable_mailinfovec();
		if(pInfoVec->mailinfo_size()>150)
		{
			std::vector<int> pRmId;
			pRmId.push_back(pInfoVec->mailinfo(0).id());
			delMail(pRmId);
		}

		fgame::DataMailInfo* pInfo = pInfoVec->add_mailinfo();
		pInfo->set_id(getMail.mailinfo(i).id());
		pInfo->set_title(getMail.mailinfo(i).title());
		pInfo->set_content(getMail.mailinfo(i).content());
		pInfo->set_plugin(getMail.mailinfo(i).plugin());
		pInfo->set_sendtime(getMail.mailinfo(i).sendtime());
		pInfo->set_outdate(getMail.mailinfo(i).outdate());
	}

	saveToFile();
}

bool MailData::isNewMail()
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	return selfMail.newmail();
}

void MailData::setNewMail(bool pbool)
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	selfMail.set_newmail(pbool);
	//saveToFile();
}

void MailData::delMail(std::vector<int> idVec)
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	

	fgame::DataMailUin pDataMail  = selfMail;

	selfMail.clear_mailinfovec();
	fgame::DataMailInfoVec* pInfoVec = selfMail.mutable_mailinfovec();
	for (int i = 0; i<pDataMail.mailinfovec().mailinfo_size(); i++)
	{
		bool pIsCanAdd = true;
		auto itt = std::find(idVec.begin(), idVec.end(), pDataMail.mailinfovec().mailinfo(i).id());
		if (itt != idVec.end())
			pIsCanAdd = false;

		if (pIsCanAdd)
		{
			fgame::DataMailInfo* pInfo = pInfoVec->add_mailinfo();
			pInfo->set_id(pDataMail.mailinfovec().mailinfo(i).id());
			pInfo->set_title(pDataMail.mailinfovec().mailinfo(i).title());
			pInfo->set_content(pDataMail.mailinfovec().mailinfo(i).content());
			pInfo->set_plugin(pDataMail.mailinfovec().mailinfo(i).plugin());
			pInfo->set_sendtime(pDataMail.mailinfovec().mailinfo(i).sendtime());
			pInfo->set_outdate(pDataMail.mailinfovec().mailinfo(i).outdate());
		}
	}

	saveToFile();
}

const fgame::DataMailInfo& MailData::getMailInfo(int order)
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	return  selfMail.mailinfovec().mailinfo(order);
}

const fgame::DataMailInfo* MailData::getMailInfoById(int id)
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	fgame::DataMailInfo* pRet = NULL;

	for (int i = 0; i< selfMail.mailinfovec().mailinfo_size(); i++)
	{
		if (selfMail.mailinfovec().mailinfo(i).id() == id)
		{
			pRet = selfMail.mutable_mailinfovec()->mutable_mailinfo(i);
		}
	}
	return pRet;
}

std::map<int,int> MailData::getAward(std::string plugIn)
{
	std::map<int,int> pRet;
	if (plugIn.size()>0)
	{
		std::vector<std::string > tPosVec1 = splitStr(plugIn, "{");
		if (tPosVec1.size()<2)
		{
			pRet.insert(std::make_pair(AWARDTYPE::A_DIAMOND, 0));
			return pRet;
		}
		std::vector<std::string > tPosVec2 =splitStr(tPosVec1[1],"}");
		if (tPosVec2.size()== 0)
		{
			pRet.insert(std::make_pair(AWARDTYPE::A_DIAMOND, 0));
			return pRet;
		}
		std::vector<std::string > tPosVec3 =splitStr(tPosVec2[0],",");
		for (int i = 0; i<tPosVec3.size(); i++)
		{
			std::vector<std::string > tPosVec4 =splitStr(tPosVec3[i],":");
			int pType = 0;
			int pNumber = 1;

			if (tPosVec4.size() >= 1)
				pType = std::atoi(tPosVec4[0].c_str());
				
			if (tPosVec4.size() >= 2)
				pNumber = std::atoi(tPosVec4[1].c_str());

			if (pType > 0)
			{
				pRet.insert(std::make_pair(pType, pNumber));
			}		  
			else if (strcmp(tPosVec4[0].c_str(), "itemqb.png") == 0) //if ("itemqb.png" == tPosVec4[0].c_str())				it->first.compare("SevenGoalNode.csb") != 0)
			{
				
				mRealPorp.insert(std::make_pair("itemqb.png", pNumber));
			}
		}
	}

	return pRet;
}

fgame::DataMailUin& MailData::getDataMailUin()
{
	int selfUin = sgUserObj()->getUin();
	for (int i = 0; i < mDataMail.mailuinvec_size(); i++)
	{
		if (mDataMail.mailuinvec(i).uin() == selfUin)
		{
			return (fgame::DataMailUin&)mDataMail.mailuinvec(i);
		}
	}

	fgame::DataMailUin* selfMail = mDataMail.add_mailuinvec();
	selfMail->set_newmail(false);
	selfMail->set_mailno(0);
	selfMail->set_uin(selfUin);
	fgame::DataMailInfoVec* mailInfoVec = selfMail->mutable_mailinfovec();
	mailInfoVec->Clear();

	return *selfMail;
}

int MailData::getMailSize(){
	fgame::DataMailUin& selfMail = getDataMailUin();
	return selfMail.mailinfovec().mailinfo_size();
}

int MailData::getMailNo(){
	fgame::DataMailUin& selfMail = getDataMailUin();
	return selfMail.mailno();
}

void MailData::setMailNo(int no)
{
	fgame::DataMailUin& selfMail = getDataMailUin();
	selfMail.set_mailno(no);
}

int MailData::getGoodsType(int id)
{
	int type = UserObj::PROPERTYTYPE::PT_PVEPOWER;
	auto lProp = sgTableManager()->getPropInfoById(id);
	if (lProp)
	{
		type = UserObj::PROPERTYTYPE::PT_ITEM;
	}
	auto lStuff = sgTableManager()->getGlyphStuffData(id);
	if (lStuff)
	{
		type = UserObj::PROPERTYTYPE::STUFF;
	}
	if (id == A_DIAMOND)
	{
		type = UserObj::PROPERTYTYPE::PT_DIAMOND;
	}
	else if (id == A_PVEPOWER)
	{
		type = UserObj::PROPERTYTYPE::PT_PVEPOWER;
	}
	else if (id == A_PVPPOWER)
	{
		type = UserObj::PROPERTYTYPE::PT_PVPPOWER;
	}
	else if (id == A_ROOMTICKET)
	{
		type = UserObj::PROPERTYTYPE::ROOM_TICKET;
	}
	else if (id == A_Honor)
	{
		type = UserObj::PROPERTYTYPE::Honor;
	}
	return type;
}
