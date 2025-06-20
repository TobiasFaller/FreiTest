#pragma once

#include <boost/property_tree/ptree_fwd.hpp>

#include <iostream>
#include <string>
#include <memory>

namespace FreiTest
{
namespace Basic
{

class ApplicationStatistics {
public:
	ApplicationStatistics(void);

	// Define desctructor and move-constructor manually as they would
	// require the destructor of ptree to be fully defined.
	// As ptree has only been forward-declared this is not possible.
	ApplicationStatistics(ApplicationStatistics&& other);
	virtual ~ApplicationStatistics(void);

	template<typename T>
	void Add(const std::string& key, T value, std::string unit, std::string description);
	void Add(const std::string& key, boost::property_tree::ptree value);

	void Merge(const ApplicationStatistics& other);
	void Merge(const std::string& key, const ApplicationStatistics& other);

	void PrintHumanReadableToStream(std::ostream& out);
	void PrintJsonToStream(std::ostream& out);

private:
	std::unique_ptr<boost::property_tree::ptree> statistics;

};

};
};
