#include "Io/ValueWatcherExporter/ValueWatcherExporter.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Io
{

bool ExportWatchers(std::ostream& output, const std::vector<Helper::ValueWatcher::WatcherValues>& watches)
{
    using ptree = boost::property_tree::ptree;

	ptree root;
	for (const auto& values: watches)
    {
        ptree timeframesWithValues;

        for (size_t i = 0; i < values.timeframes.size(); i++)
        {
            std::string tfValues;
            for (auto& value: values.timeframes[i].values)
            {
                tfValues += to_string(value);
            }

            ptree timeframeWithValue;
            timeframeWithValue.put("", tfValues);
            timeframesWithValues.push_back(std::make_pair("", timeframeWithValue));
        }
        root.put_child(values.name, timeframesWithValues);
    }

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
