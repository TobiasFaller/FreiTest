#include "Tpg/LogicGenerator/LogicGenerator.hpp"

#include <string>

#include "Circuit/CircuitEnvironment.hpp"
#include "SolverProxy/ISolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

using namespace SolverProxy;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
LogicGenerator<PinData>::LogicGenerator(std::shared_ptr<ISolverProxy> satSolver, std::shared_ptr<CircuitEnvironment> circuit):
	_context(satSolver, circuit)
{
}

template<typename PinData>
LogicGenerator<PinData>::~LogicGenerator(void) = default;

template<typename PinData>
void LogicGenerator<PinData>::Reset(void)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling Reset on circuit tagger " << tagger->GetName();
		tagger->Reset();
	}

	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling Reset on circuit encoder " << encoder->GetName();
		encoder->Reset();
	}

	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling Reset on constraint encoder " << encoder->GetName();
		encoder->Reset();
	}

	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging); ++timeframe)
	{
		if (_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::CircuitTagging))
		{
			_context.SetTimeframeCompletedForTarget(timeframe, GenerationTarget::CircuitTagging, false);
		}
	}
	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitEncoding); ++timeframe)
	{
		if (_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::CircuitEncoding))
		{
			_context.SetTimeframeCompletedForTarget(timeframe, GenerationTarget::CircuitEncoding, false);
		}
	}
	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding); ++timeframe)
	{
		if (_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::ConstraintEncoding))
		{
			_context.SetTimeframeCompletedForTarget(timeframe, GenerationTarget::ConstraintEncoding, false);
		}
	}
}

template<typename PinData>
bool LogicGenerator<PinData>::GenerateCircuitLogic(void)
{
	return PreTagCircuit()       && TagCircuit()       && PostTagCircuit()
		&& PreEncodeCircuit()    && EncodeCircuit()    && PostEncodeCircuit()
		&& PreConstrainCircuit() && ConstrainCircuit() && PostConstrainCircuit();
}

template<typename PinData>
bool LogicGenerator<PinData>::TagCircuit(void)
{
	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging); ++timeframe)
	{
		if (!_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::CircuitTagging)
			|| _context.IsTimeframeCompletedForTarget(timeframe, GenerationTarget::CircuitTagging))
		{
			continue;
		}

		if (!PreTagTimeframe(timeframe, TaggingDirection::Forward)
			|| !TagTimeframe(timeframe, TaggingDirection::Forward)
			|| !PostTagTimeframe(timeframe, TaggingDirection::Forward))
		{
			return false;
		}

		// Do not set completed flag yet as the backward iteration has to be completed first
	}

	for (size_t timeframe = _context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging); timeframe > 0u; --timeframe)
	{
		if (!_context.IsTimeframeIncludedForTarget(timeframe - 1u, GenerationTarget::CircuitTagging)
			|| _context.IsTimeframeCompletedForTarget(timeframe - 1u, GenerationTarget::CircuitTagging))
		{
			continue;
		}

		if (!PreTagTimeframe(timeframe - 1u, TaggingDirection::Backward)
			|| !TagTimeframe(timeframe - 1u, TaggingDirection::Backward)
			|| !PostTagTimeframe(timeframe - 1u, TaggingDirection::Backward))
		{
			return false;
		}

		// Now set the completed flag as the tagging is done
		_context.SetTimeframeCompletedForTarget(timeframe - 1u, GenerationTarget::CircuitTagging, true);
	}

	for (std::shared_ptr<ICircuitTagger<PinData>>& encoder : _circuitTaggers)
	{
		DVLOG(6) << "Calling TagCircuit on encoder " << encoder->GetName();
		if (!encoder->TagCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during TagCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreTagCircuit(void)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling PreTagCircuit on tagger " << tagger->GetName();
		if (!tagger->PreTagCircuit(_context))
		{
			DVLOG(3) << "Tagger " << tagger->GetName() << " has canceled the generation process during PreTagCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostTagCircuit(void)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling PostTagCircuit on tagger " << tagger->GetName();
		if (!tagger->PostTagCircuit(_context))
		{
			DVLOG(3) << "Tagger " << tagger->GetName() << " has canceled the generation process during PostTagCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::TagTimeframe(size_t timeframe, TaggingDirection direction)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling TagTimeframe on tagger " << tagger->GetName() << " with timeframe " << timeframe;
		if (!tagger->TagTimeframe(_context, timeframe, direction))
		{
			DVLOG(3) << "Tagger " << tagger->GetName() << " has canceled the generation process during TagTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreTagTimeframe(size_t timeframe, TaggingDirection direction)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling PreTagTimeframe on tagger " << tagger->GetName() << " with timeframe " << timeframe;
		if (!tagger->PreTagTimeframe(_context, timeframe, direction))
		{
			DVLOG(3) << "Tagger " << tagger->GetName() << " has canceled the generation process during PreTagTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostTagTimeframe(size_t timeframe, TaggingDirection direction)
{
	for (auto& tagger : _circuitTaggers)
	{
		DVLOG(6) << "Calling PostTagTimeframe on tagger " << tagger->GetName() << " with timeframe " << timeframe;
		if (!tagger->PostTagTimeframe(_context, timeframe, direction))
		{
			DVLOG(3) << "Tagger " << tagger->GetName() << " has canceled the generation process during PostTagTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::EncodeCircuit(void)
{
	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitEncoding); ++timeframe)
	{
		if (!_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::CircuitEncoding)
			|| _context.IsTimeframeCompletedForTarget(timeframe, GenerationTarget::CircuitEncoding))
		{
			continue;
		}

		if (!PreEncodeTimeframe(timeframe)
			|| !EncodeTimeframe(timeframe)
			|| !PostEncodeTimeframe(timeframe))
		{
			return false;
		}

		_context.SetTimeframeCompletedForTarget(timeframe, GenerationTarget::CircuitEncoding, true);
	}

	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling EncodeCircuit on encoder " << encoder->GetName();
		if (!encoder->EncodeCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during EncodeCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreEncodeCircuit(void)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling PreEncodeCircuit on encoder " << encoder->GetName();
		if (!encoder->PreEncodeCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PreEncodeCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostEncodeCircuit(void)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling PostEncodeCircuit on encoder " << encoder->GetName();
		if (!encoder->PostEncodeCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PostEncodeCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::EncodeTimeframe(size_t timeframe)
{
	const MappedCircuit& circuit = _context.GetMappedCircuit();
	for (size_t nodeId = 0; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
	{
		if (!PreEncodeNode(timeframe, nodeId)
			|| !EncodeNode(timeframe, nodeId)
			|| !PostEncodeNode(timeframe, nodeId))
		{
			return false;
		}
	}

	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling EncodeTimeframe on encoder " << encoder->GetName();
		if (!encoder->EncodeTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during EncodeTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreEncodeTimeframe(size_t timeframe)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling PreEncodeTimeframe on encoder " << encoder->GetName() << " with timeframe " << timeframe;
		if (!encoder->PreEncodeTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PreEncodeTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostEncodeTimeframe(size_t timeframe)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(6) << "Calling PostEncodeTimeframe on encoder " << encoder->GetName() << " with timeframe " << timeframe;
		if (!encoder->PostEncodeTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PostEncodeTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::EncodeNode(size_t timeframe, size_t node)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(9) << "Calling EncodeNode on encoder " << encoder->GetName() << " with timeframe " << timeframe << " and node " << node;
		if (!encoder->EncodeNode(_context, timeframe, node))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during EncodeNode";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreEncodeNode(size_t timeframe, size_t node)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(9) << "Calling PreEncodeNode on encoder " << encoder->GetName() << " with timeframe " << timeframe << " and node " << node;
		if (!encoder->PreEncodeNode(_context, timeframe, node))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PreEncodeNode";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostEncodeNode(size_t timeframe, size_t node)
{
	for (auto& encoder : _circuitEncoders)
	{
		DVLOG(9) << "Calling PostEncodeNode on encoder " << encoder->GetName() << " with timeframe " << timeframe << " and node " << node;
		if (!encoder->PostEncodeNode(_context, timeframe, node))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PostEncodeNode";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::ConstrainCircuit(void)
{
	for (size_t timeframe = 0; timeframe < _context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding); ++timeframe)
	{
		if (!_context.IsTimeframeIncludedForTarget(timeframe, GenerationTarget::ConstraintEncoding))
		{
			continue;
		}

		if (!PreConstrainTimeframe(timeframe)
			|| !ConstrainTimeframe(timeframe)
			|| !PostConstrainTimeframe(timeframe))
		{
			return false;
		}

		_context.SetTimeframeCompletedForTarget(timeframe, GenerationTarget::ConstraintEncoding, true);
	}

	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling ConstrainCircuit on encoder " << encoder->GetName();
		if (!encoder->ConstrainCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during ConstrainCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreConstrainCircuit(void)
{
	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling PreConstrainCircuit on encoder " << encoder->GetName();
		if (!encoder->PreConstrainCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PreConstrainCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostConstrainCircuit(void)
{
	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling PostConstrainCircuit on encoder " << encoder->GetName();
		if (!encoder->PostConstrainCircuit(_context))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PostConstrainCircuit";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::ConstrainTimeframe(size_t timeframe)
{
	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling ConstrainTimeframe on encoder " << encoder->GetName() << " with timeframe " << timeframe;
		if (!encoder->ConstrainTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during ConstrainTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PreConstrainTimeframe(size_t timeframe)
{
	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling PreConstrainTimeframe on encoder " << encoder->GetName() << " with timeframe " << timeframe;
		if (!encoder->PreConstrainTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PreConstrainTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
bool LogicGenerator<PinData>::PostConstrainTimeframe(size_t timeframe)
{
	for (auto& encoder : _constraintEncoders)
	{
		DVLOG(6) << "Calling PostConstrainTimeframe on encoder " << encoder->GetName() << " with timeframe " << timeframe;
		if (!encoder->PostConstrainTimeframe(_context, timeframe))
		{
			DVLOG(3) << "Encoder " << encoder->GetName() << " has canceled the generation process during PostConstrainTimeframe";
			return false;
		}
	}

	return true;
}

template<typename PinData>
GeneratorContext<PinData>& LogicGenerator<PinData>::GetContext(void)
{
	return _context;
}

template<typename PinData>
const GeneratorContext<PinData>& LogicGenerator<PinData>::GetContext(void) const
{
	return _context;
}

template<typename PinData>
void LogicGenerator<PinData>::AddCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> nodeTagger)
{
	_circuitTaggers.push_back(nodeTagger);
}

template<typename PinData>
void LogicGenerator<PinData>::RemoveCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> nodeTagger)
{
	if (auto it = std::find(_circuitTaggers.begin(), _circuitTaggers.end(), nodeTagger); it != _circuitTaggers.end())
	{
		_circuitTaggers.erase(it);
	}
}

template<typename PinData>
bool LogicGenerator<PinData>::HasCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> nodeTagger) const
{
	return (std::find(_circuitTaggers.begin(), _circuitTaggers.end(), nodeTagger) != _circuitTaggers.end());
}

template<typename PinData>
std::vector<std::shared_ptr<ICircuitTagger<PinData>>> const& LogicGenerator<PinData>::GetCircuitTaggers(void) const
{
	return _circuitTaggers;
}

template<typename PinData>
std::vector<std::shared_ptr<ICircuitTagger<PinData>>>& LogicGenerator<PinData>::GetCircuitTaggers(void)
{
	return _circuitTaggers;
}

template<typename PinData>
void LogicGenerator<PinData>::AddCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder)
{
	_circuitEncoders.push_back(circuitEncoder);
}

template<typename PinData>
void LogicGenerator<PinData>::RemoveCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder)
{
	if (auto it = std::find(_circuitEncoders.begin(), _circuitEncoders.end(), circuitEncoder); it != _circuitEncoders.end())
	{
		_circuitEncoders.erase(it);
	}
}

template<typename PinData>
bool LogicGenerator<PinData>::HasCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder) const
{
	return (std::find(_circuitEncoders.begin(), _circuitEncoders.end(), circuitEncoder) != _circuitEncoders.end());
}

template<typename PinData>
std::vector<std::shared_ptr<ICircuitEncoder<PinData>>> const& LogicGenerator<PinData>::GetCircuitEncoders(void) const
{
	return _circuitEncoders;
}

template<typename PinData>
std::vector<std::shared_ptr<ICircuitEncoder<PinData>>>& LogicGenerator<PinData>::GetCircuitEncoders(void)
{
	return _circuitEncoders;
}

template<typename PinData>
void LogicGenerator<PinData>::AddConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder)
{
	_constraintEncoders.push_back(constraintEncoder);
}

template<typename PinData>
void LogicGenerator<PinData>::RemoveConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder)
{
	if (auto it = std::find(_constraintEncoders.begin(), _constraintEncoders.end(), constraintEncoder); it != _constraintEncoders.end())
	{
		_constraintEncoders.erase(it);
	}
}

template<typename PinData>
bool LogicGenerator<PinData>::HasConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder) const
{
	return (std::find(_constraintEncoders.begin(), _constraintEncoders.end(), constraintEncoder) != _constraintEncoders.end());
}

template<typename PinData>
std::vector<std::shared_ptr<IConstraintEncoder<PinData>>> const& LogicGenerator<PinData>::GetContraintEncoder(void) const
{
	return _constraintEncoders;
}

template<typename PinData>
std::vector<std::shared_ptr<IConstraintEncoder<PinData>>>& LogicGenerator<PinData>::GetContraintEncoder(void)
{
	return _constraintEncoders;
}

template<typename PinData>
void LogicGenerator<PinData>::AddContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor)
{
	_context.AddContainerInterceptor(interceptor);
}

template<typename PinData>
void LogicGenerator<PinData>::RemoveContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor)
{
	_context.RemoveContainerInterceptor(interceptor);
}

template<typename PinData>
bool LogicGenerator<PinData>::HasContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor) const
{
	return _context.HasContainerInterceptor(interceptor);
}

template<typename PinData>
std::vector<std::shared_ptr<IContainerInterceptor<PinData>>> const& LogicGenerator<PinData>::GetContainerInterceptors(void) const
{
	return _context.GetContainerInterceptors();
}

template<typename PinData>
std::vector<std::shared_ptr<IContainerInterceptor<PinData>>>& LogicGenerator<PinData>::GetContainerInterceptors(void)
{
	return _context.GetContainerInterceptors();
}

#define FOR_PINDATA(PINDATA) template class LogicGenerator<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
