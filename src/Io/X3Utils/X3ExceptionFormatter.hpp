#pragma once

#include <boost/spirit/home/x3.hpp>

#include <string>
#include <vector>
#include <iomanip>
#include <list>
#include <type_traits>

namespace x3 = boost::spirit::x3;

namespace FreiTest
{
namespace Io
{
namespace X3
{

template<typename Iterator>
static std::string FormatX3Message(Iterator begin, Iterator end, Iterator indicator, std::string message, size_t preceding = 2, size_t following = 1)
{
	Iterator curr = begin;

	size_t firstLineNumber = 1u;
	size_t indicatorLineNumber = 1u;
	size_t indicatorOffset = 0u;
	std::string indicatorLine;
	std::string indicatorArrow;
	std::list<std::string> precedingLines;
	std::list<std::string> followingLines;

	const auto translate = [](auto const& value) -> std::string {
		if (value == '\t') { return "    "; }
		return std::string(1u, value);
	};

	size_t lineNumber = 1u;
	std::string currentLine;
	bool isPrecedingLine = true;
	bool isFollowingLine = false;
	while (true)
	{
		if (curr == indicator)
		{
			isPrecedingLine = false;
			isFollowingLine = false;
			indicatorOffset = currentLine.size();
		}

		if (curr == end || *curr == '\n')
		{
			bool doContinue = true;
			if (isPrecedingLine)
			{
				precedingLines.push_back(currentLine);
				if (precedingLines.size() > preceding)
				{
					precedingLines.pop_front();
					firstLineNumber++;
				}
			}
			else if (!isFollowingLine)
			{
				indicatorLineNumber = lineNumber;
				indicatorLine = currentLine;
				indicatorArrow = std::string(indicatorOffset, '-');
				indicatorArrow += '^';
				if (indicatorOffset + 1u < currentLine.size())
					indicatorArrow += std::string(currentLine.size() - 1u - indicatorOffset, '-');
				isFollowingLine = true;
			}
			else if (isFollowingLine)
			{
				followingLines.push_back(currentLine);
				if (followingLines.size() >= following)
					doContinue = false;
			}
			currentLine.clear();
			lineNumber++;
			if (!doContinue || curr == end) break;
			curr++;
			continue;
		}

		currentLine += translate(*curr);
		curr++;
	}

	std::ostringstream oss;
	oss << "At line " << indicatorLineNumber << ": " << message << std::endl;
	for (auto& line : precedingLines)
	{
		oss << "    Line " << firstLineNumber++ << ": " << line << std::endl;
	}
	oss << "    Line " << firstLineNumber << ": " << indicatorLine << std::endl;
	oss << "     " << std::string(std::to_string(firstLineNumber++).size(), ' ') << "Here: " << indicatorArrow << std::endl;
	for (auto& line : followingLines)
	{
		oss << "    Line " << firstLineNumber++ << ": " << line << std::endl;
	}

	return oss.str();
}

template<typename Iterator>
static std::string FormatX3Exception(Iterator begin, Iterator end, boost::spirit::x3::expectation_failure<Iterator> const& exception, size_t preceding = 2, size_t following = 1)
{
	return FormatX3Message(begin, end, exception.where(), "Expecting " + exception.which(), preceding, following);
}

};
};
};
