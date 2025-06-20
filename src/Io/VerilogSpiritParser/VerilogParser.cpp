#include "Io/VerilogSpiritParser/VerilogParser.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>

#include "Basic/Logging.hpp"
#include "Io/VerilogSpiritParser/VerilogGrammar.hpp"
#include "Io/X3Utils/X3ExceptionFormatter.hpp"

using namespace std;

namespace FreiTest {
namespace Io {
namespace Verilog {

// ---------------------------------------------------------------------------
// Parser class
// ---------------------------------------------------------------------------

VerilogParser::VerilogParser(void)
{
}

VerilogParser::~VerilogParser(void)
{
}

bool VerilogParser::Parse(istream& input)
{
	// Do not skip whitespace
	input >> std::noskipws;

	namespace spirit = boost::spirit;
	namespace x3 = boost::spirit::x3;

	using Grammar::skipper;
	using Grammar::verilog_source;

	using Grammar::iterator_type;
	using Grammar::base_iterator_type;
	using Grammar::context_type;

	iterator_type begin = spirit::make_default_multi_pass(base_iterator_type(input));
	iterator_type end = spirit::make_default_multi_pass(base_iterator_type());
	iterator_type curr = begin;

	try
	{
		if (x3::phrase_parse(curr, end, verilog_source(), x3::standard::space, _modules))
		{
			return true;
		}

		LOG(ERROR) << "Parsing Verilog source failed here:" << std::endl
			<< Io::X3::FormatX3Message(begin, end, curr, "Unexpected content");
	}
	catch(x3::expectation_failure<iterator_type>& exception)
	{
		LOG(ERROR) << "Parsing Verilog source failed with error:" << std::endl
			<< Io::X3::FormatX3Exception(begin, end, exception);
	}

	return false;
}

void VerilogParser::Reset(void)
{
	this->_modules.clear();
}

ModuleCollection& VerilogParser::GetModules(void)
{
	return this->_modules;
}

};
};
};
