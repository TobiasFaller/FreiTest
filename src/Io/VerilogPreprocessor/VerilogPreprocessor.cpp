#include "Io/VerilogPreprocessor/VerilogPreprocessor.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <stddef.h>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iterator>
#include <iostream>
#include <string_view>
#include <sstream>
#include <utility>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"

using namespace std;
using namespace boost;

namespace FreiTest {
namespace Io {
namespace Verilog {

VerilogPreprocessor::VerilogPreprocessor():
	_numberOfParsedFiles(0),
	_multilineComment(false),
	_skipEverything(false),
	_firstTokenInLine(true)
{
}

VerilogPreprocessor::~VerilogPreprocessor()
{
}

bool VerilogPreprocessor::Transform(const vector<string> files, ostream& output)
{
	// Copy defines from preconfigured set
	_definedSymbols.insert(_defaultDefinedSymbols.cbegin(), _defaultDefinedSymbols.cend());

	for (const string& file : files)
	{
		if (!ProcessFile(file, output))
		{
			return false;
		}
	}

	LOG(INFO) << "Successfully applied preprocessor to " << _numberOfParsedFiles << " source files";

	return true;
}

void VerilogPreprocessor::Reset(void)
{
	_numberOfParsedFiles = 0;
	_multilineComment = false;
	_definedSymbols.clear();
	_skipEverything = false;
	_firstTokenInLine = true;

	while (!_defineIdentifierResults.empty())
	{
		_defineIdentifierResults.pop();
	}
	while (!_currentBlock.empty())
	{
		_currentBlock.pop();
	}
}

bool VerilogPreprocessor::ProcessFile(const string& filename, ostream& output)
{
	FileHandle fileHandle(filename, true);
	istream& inFile = fileHandle.GetStream();

	if (!inFile.good())
	{
		LOG(INFO) << "\tFailed to load file " << fileHandle.GetFilename();
		return false;
	}

	LOG(DEBUG) << "\tPreprocessing file " << fileHandle.GetFilename();
	string line = "";
	uint64_t lineNumber = 1;
	while (getline(inFile, line))
	{
		if (!HandleLine(line, lineNumber, filename, output))
		{
			return false;
		}

		lineNumber++;
		EndLine(output);
	}

	_numberOfParsedFiles++;
	LOG(DEBUG) << "\tPreprocessed file " << fileHandle.GetFilename();
	return true;
}

bool VerilogPreprocessor::HandleLine(const string_view line, uint64_t lineNumber, const string& fileName, ostream& output)
{
	if (_skipEverything)
	{
		return true;
	}

	string lineWithoutComments = RemoveComments(line);
	if (lineWithoutComments.empty())
	{
		return true;
	}

	istringstream tokenStream(lineWithoutComments);
	string word;
	while (tokenStream >> word)
	{
		// Start of a NEW ifdef / ifndef block
		if (word == "`ifdef" || word == "`ifndef")
		{
			string _defineIdentifier;
			tokenStream >> _defineIdentifier;

			bool identifierDefined = IsDefinedSymbol(_defineIdentifier);
			if (word == "`ifndef")
			{
				// Check for the inverse when evaluating the expressions in the block
				identifierDefined = !identifierDefined;
			}

			_currentBlock.push("ifdef");
			_defineIdentifierResults.push(identifierDefined);
			break;
		}

		// Only activated when currently in a "special" block
		if (!_currentBlock.empty() && _currentBlock.top() == "ifdef")
		{
			assert(!_defineIdentifierResults.empty());
			bool identifierDefined = _defineIdentifierResults.top();


			if (word == "`else")
			{
				// Swap the #define
				_defineIdentifierResults.pop();
				_defineIdentifierResults.push(!identifierDefined);
				break;
			}

			if (word == "`endif")
			{
				_currentBlock.pop();
				_defineIdentifierResults.pop();
				break;
			}

			// Ignore the code inside #ifdef CONDITION where CONDITION is undefined
			if (!identifierDefined)
			{
				break;
			}
		}

		// Forced exit of parser
		if (word == "`exit")
		{
			_skipEverything = true;
			break;
		}

		// Reset the preprocessor state
		if (word == "`resetall") {
			while (!_currentBlock.empty()) {
				_currentBlock.pop();
			}
			while (!_defineIdentifierResults.empty()) {
				_defineIdentifierResults.pop();
			}
			_definedSymbols.clear();
			break;
		}

		// A bunch of ignored commands / lines
		if (word == "`delay_mode_distributed" || word == "`delay_mode_path"
				|| word == "`delay_mode_unit" || word == "`delay_mode_zero"
				|| word == "`default_trireg_strength" || word == "`timescale"
				|| word == "`default_decay_time" || word == "`unconnected_drive"
				|| word == "`nounconneced_drive" || word == "`line"
				|| word == "`default_nettype")
		{
			break;
		}

		if (word == "`define")
		{
			string macroName;
			string macroValue;

			tokenStream >> macroName;
			tokenStream >> macroValue;

			macroName = StringHelper::Trim(macroName);
			macroValue = StringHelper::Trim(macroValue);

			if (macroName.empty())
			{
				LOG(INFO) << "Macro definition without a macro name";
				return false;
			}

			if (!macroValue.empty())
			{
				DefineSymbol(macroName, macroValue);
			} else {
				DefineSymbol(macroName, "TRUE");
			}

			break;
		}

		if (word == "`undef") {
			string macroName;
			tokenStream >> macroName;

			if (macroName.empty())
			{
				LOG(INFO) << "Macro undef without a macro name";
				return false;
			}

			UndefineSymbol(macroName);
			break;
		}

		if (word == "`include")
		{
			string includeFile;
			tokenStream >> includeFile;

			if (includeFile.empty())
			{
				LOG(INFO) << "Include without filename";
				return false;
			}

			// Strip quotes from file name
			LOG(INFO) << "\tIncluding file: " << includeFile;
			if (StringHelper::StartsWith("\"", includeFile) && StringHelper::EndsWith("\"", includeFile))
			{
				includeFile = StringHelper::Trim(includeFile.substr(1, includeFile.length() - 2));
			}

			// Handle relative path to the current file
			if (StringHelper::StartsWith(".", includeFile))
			{
				std::filesystem::path p;
				p += fileName;
				p /= "..";
				p /= includeFile;

				auto newPath = std::filesystem::weakly_canonical(p);
				includeFile = newPath.native();
			}
			LOG(INFO) << "\tFull include file name: " << includeFile;
			EndLine(output);
			ProcessFile(includeFile, output);
			break;
		}

		// These blocks are kept for the downstream transformer
		if (word == "`celldefine" || word == "`endcelldefine"
				|| word == "`finalmodules" || word == "`endfinalmodules")
		{
			AppendToken(output, word);
			EndLine(output);
			break;
		}

		// Use reverse iterator to replace correctly when a prefix exists
		for (auto it = _definedSymbols.rbegin(); it != _definedSymbols.rend(); ++it)
		{
			string macro = "`" + it->first;
			string value = it->second;

			boost::replace_all(word, macro, value);
		}

		AppendToken(output, word);
	}

	return true;
}

string VerilogPreprocessor::RemoveComments(const string_view line)
{
	size_t startIndex = 0;
	string lineWithoutComments;

	while (true)
	{
		if (_multilineComment)
		{
			size_t commentEnd = line.find("*/", startIndex);
			if (commentEnd == string::npos) {
				break;
			}

			startIndex = commentEnd + 2;
			_multilineComment = false;
			continue;
		}

		size_t commentStart = line.find("//", startIndex);
		size_t multilineCommentStart = line.find("/*", startIndex);

		// No comment for the rest of the line
		if (commentStart == string::npos && multilineCommentStart == string::npos)
		{
			lineWithoutComments += line.substr(startIndex);
			lineWithoutComments += " ";
			break;
		}

		// At least one comment in the rest of the line
		if (commentStart != string::npos && multilineCommentStart != string::npos)
		{
			// A single-line comment is the first comment in the line
			if (commentStart <= multilineCommentStart)
			{
				lineWithoutComments += line.substr(startIndex, commentStart - startIndex);
				lineWithoutComments += " ";
				break;
			}

			// A multine comment is the first in the line
			lineWithoutComments += line.substr(startIndex, multilineCommentStart - startIndex);
			lineWithoutComments += " ";

			startIndex = multilineCommentStart + 2;
			_multilineComment = true;
			continue;
		}

		if (commentStart != string::npos)
		{
			lineWithoutComments += line.substr(startIndex, commentStart - startIndex);
			lineWithoutComments += " ";

			break;
		}

		lineWithoutComments += line.substr(startIndex, multilineCommentStart - startIndex);
		lineWithoutComments += " ";

		startIndex = multilineCommentStart + 2;
		_multilineComment = true;
	}

	return StringHelper::Trim(lineWithoutComments);
}

void VerilogPreprocessor::DefineSymbol(const string& key, const string& value)
{
	_defaultDefinedSymbols.emplace(key, value);
}

void VerilogPreprocessor::UndefineSymbol(const string& key)
{
	_defaultDefinedSymbols.erase(key);
}

bool VerilogPreprocessor::IsDefinedSymbol(const string& key) const
{
	return (_defaultDefinedSymbols.find(key) != _defaultDefinedSymbols.end());
}

string VerilogPreprocessor::GetDefinedSymbol(const string& key) const
{
	return _defaultDefinedSymbols.find(key)->second;
}

void VerilogPreprocessor::AppendToken(ostream& output, string token)
{
	if (!_firstTokenInLine)
	{
		output << ' ';
	}

	_firstTokenInLine = false;
	output << token;
}

void VerilogPreprocessor::EndLine(ostream& output)
{
	// Do not write empty lines
	if (_firstTokenInLine)
	{
		return;
	}

	_firstTokenInLine = true;
	output << endl;
}

};
};
};
