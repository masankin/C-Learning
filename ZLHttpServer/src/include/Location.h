#ifndef _LOCATION_H
#define _LOCATION_H

#include <string>
#include <vector>
#include <unordered_map>
#include <forward_list>

typedef std::vector<std::string>::const_iterator LocationIterator;

//����URL��Я���������ļ����ƣ�/index.html������ʱ��û������ʹ�ã�HttpRequest��Ա������ʵ�ʵ��ã� lizheng
class Location
{
public:
    Location();
    Location(const std::string& requestURI);
    virtual ~Location();

    void SetRequestURI(const std::string& uri)
    {
        requestURI = uri;
		Parse();
    }
    const std::string& GetParameter(const std::string& key);
    const std::string& GetPath();

    LocationIterator GetIterator();
    LocationIterator GetEnd();

    // inlining
    const LocationIterator cbegin()
    {
        return dirs.cbegin();
    };
    const LocationIterator cend()
    {
        return dirs.cend();
    }
    LocationIterator begin()
    {
        return dirs.begin();
    };
    LocationIterator end()
    {
        return dirs.cend();
    }


protected:
    bool ParseQueryString(const std::string& query);
    bool Parse();

    void SetParameter(const std::string& key, const std::string& value);
    void SetPath(const std::string& path);

protected:
    std::string requestURI;
    std::string path;
    std::string queryString;
    std::vector<std::string> dirs;
    std::unordered_map<std::string, std::string> parameters;
};

#endif //_LOCATION_H