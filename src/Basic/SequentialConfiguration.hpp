#pragma once

namespace FreiTest
{
namespace Basic
{

enum class SequentialModel
{
	None,
	Keep,
	Unclocked,
	FlipFlop,
	Latch
};

enum class SetResetModel
{
	None,
	OnlySet,
	OnlyReset,
	SetHasPriority,
	ResetHasPriority
};

struct SequentialConfig
{
	SequentialModel sequentialModel;
	SetResetModel setResetModel;
};

SequentialConfig MakeUnclockedSimpleFlipFlopModel(void);
SequentialConfig MakeUnclockedSetResetFlipFlopModel(void);
SequentialConfig MakeUnclockedResetSetFlipFlopModel(void);

};
};
