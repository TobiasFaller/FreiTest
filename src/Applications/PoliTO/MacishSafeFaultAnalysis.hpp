#pragma once

#include <vector>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Vcm/VcmMixin.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "Tpg/Sbst/SbstContext.hpp"
#include "Tpg/Sbst/SbstProgram.hpp"

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

template<typename FaultModel, typename FaultList>
class MacishSafeFaultAnalysis:
	public Scale4Edge::AtpgBase<FaultModel, FaultList>,
	public virtual Mixin::VcmMixin
{
public:
	MacishSafeFaultAnalysis();
	virtual ~MacishSafeFaultAnalysis();

	void Run();
	void Init();
	bool SetSetting(std::string key, std::string value);

private:
	enum class Target { Macish, Multiplier };
	enum class Bits { Bits8, Bits16, Bits32 };
	enum class Complete { Incomplete, Complete };
	enum class Trace { Off, On };

	friend uint64_t operator+(const Bits& bits) {
		switch (bits) {
			case Bits::Bits8: return 8ull;
			case Bits::Bits16: return 16ull;
			case Bits::Bits32: return 32ull;
			default: return 0ull;
		}
	}

	std::string watcherImportFilename;
	std::string printerImportFilename;
	Io::ValueWatcherImporter watcherImporter;
	Io::ValuePrinterImporter printerImporter;
	Target target;
	Bits bits;
	Complete complete;
	Trace trace;
	size_t incompleteMinDigits;
	size_t samples;

};

};
};
};
