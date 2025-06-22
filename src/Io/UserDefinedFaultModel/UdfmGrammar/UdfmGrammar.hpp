#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>

#include "Basic/Logging.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"
#include "Io/UserDefinedFaultModel/UdfmGrammar/SimpleUdfmGrammar.hpp"
#include "Io/UserDefinedFaultModel/UdfmGrammar/TessentUdfmGrammar.hpp"


namespace FreiTest
{
namespace Io
{
namespace Udfm
{
namespace UdfmGrammar
{
	// Implementation
	// See https://www.boost.org/doc/libs/1_77_0/libs/spirit/doc/x3/html/index.html for more details

	namespace x3 = boost::spirit::x3;

	const auto simple_format_to_udfm = [](auto& value) {
		auto udfm = std::make_unique<UdfmModel>();
		for (auto cell : x3::_attr(value))
		{
			auto udfmCell = udfm->AddCell(cell.type);
			for (auto fault : cell.faults)
			{
				auto udfmFault = udfmCell->AddFault(fault.name, "all");
				for (auto test : fault.tests)
				{
					ASSERT(test.conditions.size() != 0) << "Got UDFM entry for cell " << cell.type << " with fault name " << fault.name << " with no conditions";
					ASSERT(test.effects.size() != 0) << "Got UDFM entry for cell " << cell.type << " with fault name " << fault.name << " with no effects";

					UdfmPortMap testConditions;
					UdfmPortMap testEffects;
					for (auto condition : test.conditions)
					{
						testConditions[condition.name] = condition.logicConstraints;
					}
					for (auto effect : test.effects)
					{
						testEffects[effect.name] = effect.logicConstraints;
					}
					udfmFault->AddAlternative(test.type, testConditions, testEffects);
				}
			}
		}
		x3::_val(value) = std::move(udfm);
	};

	const auto tessent_format_to_udfm = [](auto& value) {
		auto udfm = std::make_unique<UdfmModel>();
		for (const auto& category : x3::_attr(value).categories)
		{
			for (auto cell : category.cells)
			{
				auto udfmCell = udfm->AddCell(cell.type);
				for (auto fault : cell.faults)
				{
					auto udfmFault = udfmCell->AddFault(fault.name, category.faultType);
					for (auto test : fault.tests)
					{
						ASSERT(test.conditions.size() != 0) << "Got UDFM entry for cell " << cell.type << " with fault name " << fault.name << " with no conditions";
						ASSERT(test.effects.size() != 0) << "Got UDFM entry for cell " << cell.type << " with fault name " << fault.name << " with no effects";

						UdfmPortMap testConditions;
						UdfmPortMap testEffects;
						for (auto condition : test.conditions)
						{
							testConditions[condition.name] = condition.logicConstraints;
						}
						for (auto effect : test.effects)
						{
							testEffects[effect.name] = effect.logicConstraints;
						}
						udfmFault->AddAlternative(test.type, testConditions, testEffects);
					}
				}
			}
		}
		x3::_val(value) = std::move(udfm);
	};

	const x3::rule<class skipper_class, x3::unused_type const> skipper = "Comment or Whitespace";
	const auto skipper_def = x3::space
		| ("/*" > *(x3::char_ - "*/") > "*/")
		| ("//" > *(x3::char_ - x3::eol) > x3::eol);
	BOOST_SPIRIT_DEFINE(skipper);

	const x3::rule<class udfm_class, std::unique_ptr<UdfmModel>> udfm = "Udfm";
	const auto udfm_def = (SimpleGrammar::simple_udfm [ simple_format_to_udfm ] | TessentGrammar::tessent_udfm [ tessent_format_to_udfm ] );
	BOOST_SPIRIT_DEFINE(udfm);

};
};
};
};
