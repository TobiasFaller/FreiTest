#include "Basic/SequentialConfiguration.hpp"

namespace FreiTest
{
namespace Basic
{

SequentialConfig MakeUnclockedSimpleFlipFlopModel(void)
{
	return {
		/* sequentialModel = */ SequentialModel::Unclocked,
		/* setResetModel = */ SetResetModel::None
	};
}

SequentialConfig MakeUnclockedSetResetFlipFlopModel(void)
{
	return {
		/* sequentialModel = */ SequentialModel::Unclocked,
		/* setResetModel = */ SetResetModel::SetHasPriority
	};
}

SequentialConfig MakeUnclockedResetSetFlipFlopModel(void)
{
	return {
		/* sequentialModel = */ SequentialModel::Unclocked,
		/* setResetModel = */ SetResetModel::ResetHasPriority
	};
}

};
};
