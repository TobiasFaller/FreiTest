#pragma once

#include <string>
#include <vector>

#include "Basic/Pattern/Capture.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class PatternExclusionConstraintEncoder: public IConstraintEncoder<PinData>
{
public:
	PatternExclusionConstraintEncoder(std::shared_ptr<std::vector<Pattern::TestPattern>> excludedPatterns, Pattern::InputCapture inputCapture);
	virtual ~PatternExclusionConstraintEncoder(void);

	std::string GetName(void) const override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

private:
	std::shared_ptr<std::vector<Pattern::TestPattern>> _excludedPatterns;
	Pattern::InputCapture _inputCapture;

};

};
};
