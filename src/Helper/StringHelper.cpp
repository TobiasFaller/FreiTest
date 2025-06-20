#include "Helper/StringHelper.hpp"

#include <boost/algorithm/string.hpp>

#include <cstddef>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <sstream>

using namespace std;

string StringHelper::ReplaceString(const string& search, const string& replace, const string& s)
{
	string stringCopy = s;
	for(size_t pos = 0; ; pos += replace.length())
	{
		pos = stringCopy.find(search, pos);
		if(pos == string::npos)
		{
			break;
		}

		stringCopy.erase(pos, search.length());
		stringCopy.insert(pos, replace);
	}
	return stringCopy;
}

bool StringHelper::StartsWith(const string& needle, const string& haystack)
{
    return (needle.length() <= haystack.length())
        && equal(needle.begin(), needle.end(), haystack.begin());
}

bool StringHelper::EndsWith(const string& needle, const string& haystack)
{
	return (needle.length() <= haystack.length())
		&& equal(needle.begin(), needle.end(), haystack.begin() + (haystack.length() - needle.length()));
}

string StringHelper::Trim(const string& value)
{
	string stringCopy = value;
	boost::algorithm::trim(stringCopy);
	return stringCopy;
}
