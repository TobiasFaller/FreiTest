#include "Io/ReconfigurableScanNetwork/ReconfigurableScanNetwork.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "Basic/Logging.hpp"

using namespace std;

ReconfigurableScanNetwork::ReconfigurableScanNetwork()
{
	modulesCount = -1;
//	socName = "";
	power = false;
	xy = false;
}

ReconfigurableScanNetwork::~ReconfigurableScanNetwork()
{
	// TODO Auto-generated destructor stub
}

bool ReconfigurableScanNetwork::ParseFunctionalVerilog(const char* filename)
{
	// create a file-reading object
	ifstream fin;
	fin.open(filename); // open a file
	if (!fin.good())
		return 1; // exit if file not found

	int currentModuleID = 0;

	// read an entire line into memory
	string currentLine;

	/*
	 * Now read the file
	 */
	// Parse first line
	if (fin.eof())
		return 0;

	getline(fin, currentLine);
	DASSERT(StringEquals(currentLine.substr(0, 8), "SocName ")) << "Wrong assumption while parsing";

	this->socName = currentLine.substr(8, (currentLine.length() - 8));

	// Parse second line
	if (fin.eof())
		return 0;
	getline(fin, currentLine);
	DASSERT(StringEquals(currentLine.substr(0, 13), "TotalModules ")) << "Wrong assumption while parsing";

	modulesCount = atoi(currentLine.substr(13, (currentLine.length() - 13)).c_str());

	// Parse third line
	if (fin.eof())
		return 0;
	getline(fin, currentLine);

	DASSERT(StringEquals(currentLine.substr(0, 14), "Options Power ")) << "Wrong assumption while parsing";
	DASSERT(StringEquals(currentLine.substr(15, 4), " XY ")) << "Wrong assumption while parsing";

	if (currentLine.substr(14, 1) == "1")
		power = true;
	else
	{
		DASSERT(currentLine.substr(14, 1) == "0") << "Wrong assumption while parsing";
		power = false;
	}

	if (currentLine.substr(19, 1) == "1")
		xy = true;
	else
	{
		DASSERT(currentLine.substr(19, 1) == "0") << "Wrong assumption while parsing";
		xy = false;
	}

//	DVAR(socName);
//	DVAR(modulesCount);
//	DVAR(power);
//	DVAR(xy);

	// Parse fourth line (empty)
	if (fin.eof())
		return 0;
	getline(fin, currentLine);
	DASSERT(currentLine.empty()) << "Wrong assumption while parsing";

	DASSERT(!fin.eof()) << "Wrong assumption while parsing"; // Assuming we have at least one module
	if (fin.eof())
		return 0; // Assuming we have at least one module

	// Before parsing the rest of the document, initialize some variables
	modules.resize(modulesCount);
	CircuitModule* currentModule;
	int testsCount;
	int currentTestID;

	getline(fin, currentLine);
	// read each line of the file beyond the fourth
	while (true)
	{
		DASSERT(currentModuleID < modulesCount) << "Counting error";
//		DVAR(currentModuleID);

		// Parse the first module line
		istringstream iss1(currentLine);
		vector<string> tokens1{istream_iterator<string>{iss1},
		                      istream_iterator<string>{}};
		currentModule = &(modules[currentModuleID]);

		DASSERT(tokens1.size() >= 13) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[0], "Module")) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[2], "Level")) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[4], "Inputs")) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[6], "Outputs")) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[8], "Bidirs")) << "Wrong assumption while parsing";

		int shiftSec = 0; // Shift if the optional security level parameter is set
		if (StringEquals(tokens1[10], "SecLevel"))
		{
			DASSERT(tokens1.size() >= 15) << "Wrong assumption while parsing";
			shiftSec = 2;
			currentModule->secLevel = atoi(tokens1[11].c_str());
			DVAR(currentModule->secLevel);
		}

		DASSERT(StringEquals(tokens1[10 + shiftSec], "ScanChains")) << "Wrong assumption while parsing";
		DASSERT(StringEquals(tokens1[12 + shiftSec], ":")) << "Wrong assumption while parsing";

		DASSERT(atoi(tokens1[1].c_str()) == currentModuleID) << "Counting error";

		currentModule->moduleID = atoi(tokens1[1].c_str());
		currentModule->level = atoi(tokens1[3].c_str());
		currentModule->inputsCount = atoi(tokens1[5].c_str());
		currentModule->outputsCount = atoi(tokens1[7].c_str());
		currentModule->bidirsCount = atoi(tokens1[9].c_str());
		currentModule->scanChainsCount = atoi(tokens1[11 + shiftSec].c_str());
		currentModule->scanChainsLength.resize(currentModule->scanChainsCount);
/*
		DVAR(currentModule->moduleID);
		DVAR(currentModule->level);
		DVAR(currentModule->inputsCount);
		DVAR(currentModule->outputsCount);
		DVAR(currentModule->bidirsCount);
		DVAR(currentModule->scanChainsCount);
*/
		for (int i = 0; i < currentModule->scanChainsCount; ++i)
		{
			DASSERT(tokens1.size() >= (size_t) 14+i) << "Wrong assumption while parsing";
			currentModule->scanChainsLength[i] = atoi(tokens1[13+shiftSec+i].c_str());

//			DVAR(i);
//			DVAR(currentModule->scanChainsLength[i]);
		}

		// Parse the (optional) second module line
		if (fin.eof())
			return 0;
		getline(fin, currentLine);
		istringstream iss2(currentLine);
		vector<string> tokens2{istream_iterator<string>{iss2},
		                      istream_iterator<string>{}};
		vector<string> currentTokens;

		if (StringEquals(tokens2[2], "X"))
		{
			DASSERT(StringEquals(tokens2[0], "Module")) << "Wrong assumption while parsing";
			DASSERT(atoi(tokens2[1].c_str()) == currentModuleID) << "Wrong assumption while parsing";

			// There's the optional line (location)
			DASSERT(tokens2.size() == 6) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens2[4], "Y")) << "Wrong assumption while parsing";

			currentModule->layoutLocationX = atoi(tokens2[3].c_str());
			currentModule->layoutLocationY = atoi(tokens2[5].c_str());

//			DVAR(currentModule->layoutLocationX);
//			DVAR(currentModule->layoutLocationY);

			// Parse the third module line (TotalTests)
			if (fin.eof())
				return 0;

			getline(fin, currentLine);
			istringstream iss3(currentLine);
			vector<string> tokens3{istream_iterator<string>{iss3},
			                      istream_iterator<string>{}};

			DASSERT(tokens3.size() == 4) << "Wrong assumption while parsing";

			currentTokens.push_back(tokens3[0]);
			currentTokens.push_back(tokens3[1]);
			currentTokens.push_back(tokens3[2]);
			currentTokens.push_back(tokens3[3]);
		} else
		{
			DASSERT(tokens2.size() == 4) << "Wrong assumption while parsing";

			currentTokens.push_back(tokens2[0]);
			currentTokens.push_back(tokens2[1]);
			currentTokens.push_back(tokens2[2]);
			currentTokens.push_back(tokens2[3]);
		}

		// Now the second/third module-line (TotalTests)
		DASSERT(StringEquals(currentTokens[0], "Module")) << "Wrong assumption while parsing";
		DASSERT(atoi(currentTokens[1].c_str()) == currentModuleID) << "Wrong assumption while parsing";

		DASSERT(StringEquals(currentTokens[2], "TotalTests")) << "Wrong assumption while parsing";
		currentModule->totalTests = atoi(currentTokens[3].c_str());
//		DVAR(currentModule->totalTests);

		// Before parsing the rest of the module, initialize some variables
		testsCount = currentModule->totalTests;
		currentModule->testPatternCount.resize(testsCount);
		currentModule->testPowerDissipation.resize(testsCount);
		currentModule->testUsesScan.resize(testsCount);
		currentModule->testUsesTAM.resize(testsCount);
		currentTestID = 0;

//		DVAR(testsCount);

		if (fin.eof())
		{
			if (testsCount == 0)
				return 1;
			else
				return 0;
		}
		getline(fin, currentLine);

		// Parse the rest of the module
		while(!currentLine.empty())
		{
			istringstream iss4(currentLine);
			vector<string> tokens4{istream_iterator<string>{iss4},
			                      istream_iterator<string>{}};

			DASSERT(currentTestID < testsCount) << "Counting error";
			DASSERT(tokens4.size() == 10 || tokens4.size() == 12) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens4[0], "Module")) << "Wrong assumption while parsing";
			DASSERT(atoi(tokens4[1].c_str()) == currentModuleID) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens4[2], "Test")) << "Wrong assumption while parsing";
			DASSERT(atoi(tokens4[3].c_str()) == (currentTestID + 1)) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens4[4], "ScanUse")) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens4[6], "TamUse")) << "Wrong assumption while parsing";
			DASSERT(StringEquals(tokens4[8], "Patterns")) << "Wrong assumption while parsing";
			DASSERT(atoi(tokens4[5].c_str()) == 1 || atoi(tokens4[5].c_str()) == 0) << "Wrong assumption while parsing";
			DASSERT(atoi(tokens4[7].c_str()) == 1 || atoi(tokens4[7].c_str()) == 0) << "Wrong assumption while parsing";

			currentModule->testUsesScan[currentTestID] = (atoi(tokens4[5].c_str()) == 1);
			currentModule->testUsesTAM[currentTestID] = (atoi(tokens4[7].c_str()) == 1);
			currentModule->testPatternCount[currentTestID] = atoi(tokens4[9].c_str());

//			DVAR(currentTestID);
//			DVAR(currentModule->testUsesScan[currentTestID]);
//			DVAR(currentModule->testUsesTAM[currentTestID]);
//			DVAR(currentModule->testPatternCount[currentTestID]);

			if (tokens4.size() == 12)
			{
				// The optional Power-Entry

				DASSERT(StringEquals(tokens4[10], "Power")) << "Wrong assumption while parsing";
				currentModule->testPowerDissipation[currentTestID] = atoi(tokens4[11].c_str());

//				DVAR(currentModule->testPowerDissipation[currentTestID]);
			}

			++currentTestID;
			if (fin.eof())
			{
				if (currentTestID == testsCount && currentModuleID+1 == modulesCount)
					return 1;
				else
					return 0;
			}

			getline(fin, currentLine);
		}
		DASSERT(currentTestID == testsCount) << "Counting error";

		++currentModuleID;

		// Skip the empty line
		DASSERT(currentLine.empty()) << "Wrong assumption while parsing";

		if (fin.eof())
		{
			if (currentModuleID == modulesCount)
				return 1;
			else
				return 0;
		}

		getline(fin, currentLine);
	}

	fin.close();
	return 0;
}

bool ReconfigurableScanNetwork::StringEquals(std::string str1, std::string str2)
{
	return (str1.compare(str2) == 0);
}




void ReconfigurableScanNetwork::ExportAsDotFile(std::string filename)
{
	LOG(DEBUG) << "Exporting RSN to " << filename;


	// Now create a graph
	ofstream dotFile;
	dotFile.open(filename.c_str());
	dotFile << "digraph " << socName << " {\n";
	dotFile << "rankdir=\"LR\"\n";

	int currentModuleOnLevel[modulesCount]; // modulesCount is an upper bound for the maximal level
	int currentLevel;
	// Step 1: Define the nodes
	for (int i = 0; i < modulesCount; ++i)
	{
		currentLevel = modules[i].level;

		DASSERT(currentLevel >= 0) << "Array out of bound";
		DASSERT(currentLevel < modulesCount) << "Array out of bound";
		currentModuleOnLevel[currentLevel] = modules[i].moduleID;

		DASSERT(i == modules[i].moduleID) << "Needed for correct output of security level";

		dotFile << "\tModule" << modules[i].moduleID <<
				"[label=\" Module " << modules[i].moduleID <<
				"|Security Level " << modules[i].secLevel <<
				"\", shape=record]\n";
	}
	dotFile << "\n";

	// Step 2: Draw the edges
	for (int i = 0; i < modulesCount; ++i)
	{
		currentLevel = modules[i].level;

		DASSERT(currentLevel >= 0) << "Array out of bound";
		DASSERT(currentLevel < modulesCount) << "Array out of bound";
		currentModuleOnLevel[currentLevel] = modules[i].moduleID;

		DASSERT(i == modules[i].moduleID) << "Needed for correct output of security level";

		if (currentLevel > 0)
		{
			dotFile << "\tModule" << currentModuleOnLevel[currentLevel-1] <<
					" -> Module" << currentModuleOnLevel[currentLevel] <<
					";\n";
		}
	}
	dotFile << "}\n";
	dotFile.close();
}
