#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Helper/ValuePrinter.hpp"

namespace FreiTest
{
namespace Io
{

class ValuePrinterImporter
{
public:
	ValuePrinterImporter(void);
	virtual ~ValuePrinterImporter(void);

	bool LoadPrinters(std::istream& stream, std::string watcherPrefix = "");
	void ClearPrinters(void);

	std::vector<std::shared_ptr<Helper::ValuePrinter::PrintConfig>> ApplyPrinters(Helper::ValuePrinter& printer, const std::vector<std::string>& tags, const std::string& target) const;

private:
	struct PrinterEntry
	{
		PrinterEntry(Helper::ValuePrinter::PrintConfig config, std::vector<std::string> tags, std::vector<std::string> targets);

		Helper::ValuePrinter::PrintConfig config;
		std::vector<std::string> tags;
		std::vector<std::string> targets;
	};

	std::vector<PrinterEntry> printers = {};

};

};
};
