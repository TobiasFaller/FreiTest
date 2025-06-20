#pragma once

#include <string>

struct StringHelper
{
	static bool StartsWith(const std::string& needle, const std::string& haystack);
	static bool EndsWith(const std::string& needle, const std::string& haystack);
	static std::string ReplaceString( const std::string& search, const std::string& replace, const std::string& s);
	static std::string Trim(const std::string& value);
};
