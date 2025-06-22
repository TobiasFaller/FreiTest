#pragma once

#include <string>
#include <vector>

class ReconfigurableScanNetwork
{
private:
	struct CircuitModule {
	public:
		CircuitModule()
		{
			moduleID = -1;
			level = -1;
			inputsCount = -1;
			outputsCount = -1;
			bidirsCount = -1; // <- Bi-directional
			scanChainsCount = -1;
			layoutLocationX = -1;
			layoutLocationY = -1;
			totalTests = -1;
			secLevel = -1;
		}

		int moduleID;
		int level;
		int inputsCount;
		int outputsCount;
		int bidirsCount; // <- Bi-directional
		int scanChainsCount;
		std::vector<int> scanChainsLength;
		int layoutLocationX;
		int layoutLocationY;
		int totalTests;
		int secLevel; // <-- Security level
		std::vector<bool> testUsesScan;
		std::vector<bool> testUsesTAM;
		std::vector<int> testPatternCount;
		std::vector<int> testPowerDissipation;
	};


	std::vector<CircuitModule> modules;
	int modulesCount;
	std::string socName;
	bool power; // true iff. data for power consumption is provided
	bool xy; // true iff. data for layout position is provided

	bool StringEquals(std::string str1, std::string str2);
public:
	ReconfigurableScanNetwork();
	virtual ~ReconfigurableScanNetwork();

	bool ParseFunctionalVerilog(const char* filename);
	void ExportAsDotFile(std::string filename);

	std::string GetSocName() { return socName;};

};
