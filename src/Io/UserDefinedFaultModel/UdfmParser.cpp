#include "Io/UserDefinedFaultModel/UdfmParser.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include <iostream>
#include <vector>

#include "Basic/Logging.hpp"
#include "Io/UserDefinedFaultModel/UdfmGrammar/UdfmGrammar.hpp"
#include "Io/X3Utils/X3ExceptionFormatter.hpp"

namespace FreiTest
{
namespace Io
{
namespace Udfm
{

static void CheckUdfm(const std::unique_ptr<UdfmModel>& udfm);

std::unique_ptr<UdfmModel> Parse(std::istream& input)
{
	ASSERT(input.good()) << "Got bad istream";
	// Do not skip whitespace
	input >> std::noskipws;

	namespace spirit = boost::spirit;
	namespace x3 = boost::spirit::x3;

	using base_iterator_type = std::istreambuf_iterator<char>;
	using iterator_type = spirit::multi_pass<base_iterator_type>;

	iterator_type begin = spirit::make_default_multi_pass(base_iterator_type(input));
	iterator_type end = spirit::make_default_multi_pass(base_iterator_type());
	iterator_type curr = begin;

	try
	{
		std::unique_ptr<UdfmModel> udfm;
		if (x3::phrase_parse(curr, end, UdfmGrammar::udfm, UdfmGrammar::skipper, udfm))
		{
			CheckUdfm(udfm);
			return udfm;
		}

		LOG(ERROR) << "Parsing UDFM source failed here:" << std::endl
			<< Io::X3::FormatX3Message(begin, end, curr, "Unexpected content");
	}
	catch(x3::expectation_failure<iterator_type>& exception)
	{
		LOG(ERROR) << "Parsing Udfm source failed with error:" << std::endl
			<< Io::X3::FormatX3Exception(begin, end, exception);
	}

	return { };
}

static void CheckUdfm(const std::unique_ptr<UdfmModel>& udfm)
{
	for (auto [cellName, cell] : udfm->GetCells())
	{
		for (auto [faultName, fault] : cell->GetFaults())
		{
			size_t alternativeIndex { 0u };
			for (auto const& alternative : fault->GetAlternatives())
			{
				size_t length { 0u };
				for (auto [name, condition] : alternative->GetTestConditions())
				{
					length = std::max(length, condition.size());
				}
				for (auto [name, effect] : alternative->GetTestEffects())
				{
					length = std::max(length, effect.size());
				}

				for (auto [name, condition] : alternative->GetTestConditions())
				{
					ASSERT(condition.size() == length) << "Cell " << cellName
						<< " Fault " << faultName << " Alternative " << alternativeIndex
						<< " Condition " << name << " expected length of " << length
						<< " got " << condition.size() << " instead";
				}
				for (auto [name, effect] : alternative->GetTestEffects())
				{
					ASSERT(effect.size() == length) << "Cell " << cellName
						<< " Fault " << faultName << " Alternative " << alternativeIndex
						<< " Effect " << name << " expected length of " << length
						<< " got " << effect.size() << " instead";
				}
				alternativeIndex++;
			}
		}
	}
}

};
};
};
