#pragma once

#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Basic/Pattern/TestPatternList.hpp"

namespace FreiTest
{
namespace Io
{

class TestPatternExchangeFormat
{
public:
	inline TestPatternExchangeFormat(const Circuit::CircuitEnvironment& circuit, Pattern::TestPatternList patterns, Pattern::InputCapture capture):
		_circuit(circuit), _patterns(patterns), _capture(capture)
	{
	}
	inline TestPatternExchangeFormat(const TestPatternExchangeFormat& other) = default;
	inline TestPatternExchangeFormat(TestPatternExchangeFormat&& other) = default;
	inline virtual ~TestPatternExchangeFormat(void) = default;

	inline TestPatternExchangeFormat& operator=(const TestPatternExchangeFormat& other) = delete;
	inline TestPatternExchangeFormat& operator=(TestPatternExchangeFormat&& other) = delete;

	inline const Circuit::CircuitEnvironment& GetCircuit(void) const { return _circuit; };
	inline const Pattern::TestPatternList& GetTestPatterns(void) const { return _patterns; };
	inline Pattern::InputCapture GetInputCapture(void) const { return _capture; };

	inline Pattern::TestPatternList& GetTestPatterns(void) { return _patterns; };
	inline void SetInputCapture(Pattern::InputCapture capture) { _capture = capture; };

	inline size_t GetNumberOfPatterns(void) const { return _patterns.size(); };
	inline const std::shared_ptr<Pattern::TestPattern> GetPattern(size_t index) const
	{
		assert(index < _patterns.size());
		return _patterns[index];
	}

	inline std::shared_ptr<Pattern::TestPattern> GetPattern(size_t index)
	{
		assert(index < _patterns.size());
		return _patterns[index];
	}

private:
	const Circuit::CircuitEnvironment& _circuit;
	Pattern::TestPatternList _patterns;
	Pattern::InputCapture _capture;

};

};
};
