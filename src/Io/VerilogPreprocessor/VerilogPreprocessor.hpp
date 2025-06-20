#pragma once

#include <sparsepp/spp.h>

#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace FreiTest {
namespace Io {
namespace Verilog {
class VerilogPreprocessor
{

public:

	VerilogPreprocessor();
	virtual ~VerilogPreprocessor();

	bool Transform(const std::vector<std::string> files, std::ostream& output);
	void Reset(void);

	void DefineSymbol(const std::string& key, const std::string& value);
	void UndefineSymbol(const std::string& key);
	bool IsDefinedSymbol(const std::string& key) const;
	std::string GetDefinedSymbol(const std::string& key) const;

private:

	bool ProcessFile(const std::string& filename, std::ostream& output);
	bool HandleLine(const std::string_view line, const uint64_t lineNumber, const std::string &fileName, std::ostream& output);
	std::string RemoveComments(const std::string_view line);
	std::string FixLineForTokenization(const std::string& line) const;

	void AppendToken(std::ostream& output, std::string token);
	void EndLine(std::ostream& output);

	int _numberOfParsedFiles;
	bool _multilineComment;

	std::map<std::string, std::string> _defaultDefinedSymbols;
	std::map<std::string, std::string> _definedSymbols;
	std::stack<bool> _defineIdentifierResults;
	std::stack<std::string> _currentBlock;
	bool _skipEverything;

	bool _firstTokenInLine;

};

};
};
};
