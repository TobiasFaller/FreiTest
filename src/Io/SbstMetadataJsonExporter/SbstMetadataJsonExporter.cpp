#include "Io/SbstMetadataJsonExporter/SbstMetadataJsonExporter.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>

# include <iostream>
# include <fstream>

#include <utility>

#include "Basic/Overload.hpp"
#include "Tpg/Sbst/SbstProgram.hpp"
#include "Tpg/Sbst/SbstMemory.hpp"

#include "Basic/Logic.hpp"

using namespace FreiTest::Tpg::Sbst;

namespace FreiTest
{
namespace Io
{
namespace Sbst
{

using ptree = boost::property_tree::ptree;

bool ExportMetadata(std::ostream& output, const std::vector<FreiTest::Helper::ValueWatcher::WatcherValues>& processorwatches, const std::vector<FreiTest::Helper::ValueWatcher::WatcherValues>& vcmresult)
{
	ptree root;
	ptree processorBranch;
    ptree vcmBranch;

    // processorwatches and vcm results are vectors containing watcher values
    // which are made up of different parts of the processor and vcm and their
    // values for the predefined number of timeframes

	for (const auto& processorValues: processorwatches)
    {
        ptree timeframesWithValues;

        for (size_t i = 0; i < processorValues.timeframes.size(); i++)
        {
            std::string tfValues;
            for (auto& value: processorValues.timeframes[i].values)
            {
                tfValues += to_string(value);
            }

            ptree timeframeWithValue;
            timeframeWithValue.put("", tfValues);
            timeframesWithValues.push_back(std::make_pair("", timeframeWithValue));
        }
        processorBranch.put_child(processorValues.name, timeframesWithValues);
    }
    root.put_child("Processor", processorBranch);

    for (const auto& vcmValues: vcmresult)
    {
        ptree timeframesWithValues;

        for (size_t i = 0; i < vcmValues.timeframes.size(); i++)
        {
            std::string tfValues;
            for (auto& value: vcmValues.timeframes[i].values)
            {
                tfValues += to_string(value);
            }

            ptree timeframeWithValue;
            timeframeWithValue.put("", tfValues);
            timeframesWithValues.push_back(std::make_pair("", timeframeWithValue));
        }
        vcmBranch.put_child(vcmValues.name, timeframesWithValues);
    }
    root.put_child("VCM", vcmBranch);

	try
	{
		boost::property_tree::write_json(output, root, false);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
		return false;
	}

	return true;
}

};
};
};
