/*


*/

#pragma once

#include <string>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include "cocos2d.h"
#include "Constant.h"

USING_NS_CC;

class TableReader
{
public:
	virtual bool init() { return true; };
	bool parse(const std::string& filename, int jumplines = 0);
	bool parseFromBuffer(const char*, int jumplines = 0);
protected:
	virtual void readline(const std::string& line){};
	virtual void readline(std::stringstream& _stream){};
	virtual ~TableReader();

};
