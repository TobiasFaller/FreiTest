#include "Basic/Pattern/Capture.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Pattern
{

CaptureOutputs GetCaptureOutputs(OutputCapture capture)
{
	switch (capture)
	{
		case OutputCapture::PrimaryOutputsOnly:
			return {
				.capturePrimaryOutputs = true,
				.captureSecondaryOutputs = false
			};

		case OutputCapture::SecondaryOutputsOnly:
			return {
				.capturePrimaryOutputs = false,
				.captureSecondaryOutputs = true
			};

		case OutputCapture::PrimaryAndSecondaryOutputs:
			return {
				.capturePrimaryOutputs = true,
				.captureSecondaryOutputs = true
			};

		default:
			Logging::Panic("Invalid input capture value");
	}
}

};
};
