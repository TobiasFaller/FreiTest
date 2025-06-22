#pragma once

namespace FreiTest
{
namespace Pattern
{

enum class InputCapture
{
	PrimaryInputsOnly,
	SecondaryInputsOnly,
	PrimaryAndSecondaryInputs,
	PrimaryAndInitialSecondaryInputs
};

enum class OutputCapture
{
	PrimaryOutputsOnly,
	SecondaryOutputsOnly,
	PrimaryAndSecondaryOutputs
};

struct CaptureOutputs
{
	bool capturePrimaryOutputs;
	bool captureSecondaryOutputs;
};

CaptureOutputs GetCaptureOutputs(OutputCapture capture);

};
};
