
#include "Location.h"

#include <regex>

using namespace std;

#define ROOT_DIR  "D:\\html\\"
#define DEFAULT_PAGE "index.html"

Location::Location() : path(ROOT_DIR)
{
    //Location::Location("");
}
Location::Location(const string& _requestURI) : path(ROOT_DIR), requestURI(_requestURI)
{
	if (!requestURI.empty())
		Parse();
}
Location::~Location()
{
}

void Location::SetParameter(const std::string& key, const std::string& value)
{
    parameters[ key ] = value;
}

const std::string& Location::GetParameter(const std::string& key)
{
    auto it = parameters.find( key );

    if( it == parameters.end() )
		return std::string("");

    return parameters[ key ];
}

void Location::SetPath(const std::string& _location)
{
	path = _location;
}

const std::string& Location::GetPath()
{
	return path;
}

LocationIterator Location::GetIterator()
{
    return dirs.begin();
}

LocationIterator Location::GetEnd()
{
    return dirs.end();
}

bool Location::ParseQueryString(const std::string& _query)
{
    const regex expr("([^&]+)=([^&]+)");
    smatch match;

    std::string query = _query;

    while( regex_search( query, match, expr ) )
    {
		std::string key = match[1].str();
		std::string value = match[2].str();
        SetParameter(key, value);
        query = match.suffix().str();
    }

    return true;
}
/*
void Request::parseParameterString(std::string parameter_string) {
	if (parameter_string.size()) {
		utils::StrVectorSharedPtr pairs = utils::split_string(parameter_string, '&');

		for (auto entry : *pairs) {
			utils::StrVectorSharedPtr pair = utils::split_string(entry, '=');
			if (pair->size() == 2) {
				auto &key = (*pair)[0];
				auto &encoded_value = (*pair)[1];

				auto unencoded_value = uri_unencode_string(encoded_value);

				if (unencoded_value == encoded_value) {
					std::cout << key << "= " << unencoded_value << std::endl;
				}
				else {
					std::cout << key << "= " << unencoded_value << "  (" << encoded_value << ")" << std::endl;
				}

				params.insert({ key, unencoded_value });

				if (!params_multi.count(key)) {
					std::vector<std::string> new_vector;
					params_multi[key] = new_vector; // TODO: CHECK MEMORY RELEASE.
				}
				params_multi[key].push_back(unencoded_value);
			}
		}
	}
}
*/

bool Location::Parse()
{
	const std::regex expr("([a-zA-Z0-9_%]+)+");
	std::smatch match;

    std::string uri = requestURI;

	auto pos = uri.find_first_of('?');
	if (pos != std::string::npos)
    {
		path += uri.substr(0, pos);     //得到path
		queryString = uri.substr(pos + 1);
		ParseQueryString(queryString);     //解析参数
    }
	else
	{
		path += uri.substr(1, uri.size() - 1);
	}

	while(std::regex_search(uri, match, expr))
    {
		std::string f = match[0].str();
        dirs.push_back( f );
        uri = match.suffix().str();
    }

    return true;
}