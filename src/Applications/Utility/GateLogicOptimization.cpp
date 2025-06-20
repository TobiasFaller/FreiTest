#include "Applications/Utility/GateLogicOptimization.hpp"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <map>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Helper/FileHandle.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"

using namespace std;
using namespace SolverProxy;
using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Application
{
namespace Utility
{

struct Encoding
{
	std::string name;
	std::string logic_values;
	size_t bits;
	std::vector<std::string> encoding_literals;
	std::map<char, std::string> encoding;
	std::string text_header;
	std::map<char, std::string> text;
};

struct Input
{
	std::string name;
	const Encoding& encoding;
};

struct Output
{
	std::string name;
	const Encoding& encoding;
	std::string logic_table;
};

struct Implementation
{
	std::string name;
	std::vector<Input> inputs;
	std::vector<Output> outputs;

	auto input_bit_count(void) const -> size_t;
	auto input_bit_sizes(void) const -> std::vector<size_t>;
	auto input_bit_offsets(void) const -> std::vector<size_t>;
};

struct Function {
	string name;
	vector<string> inputs;
	vector<string> outputs;
	vector<Implementation> implementations;
};

struct Term
{
	size_t value; //! < The value for the term. A bit of 1 indicates that the bit position is 1.
	size_t mask; //! < The mask for the term. A bit of 1 indicates that the bit position is a fixed value (not DON'T CARE).

	auto operator<(const Term& second) const -> bool {
		return mask < second.mask || (mask == second.mask && value < second.value);
	}
	auto cost(void) const -> size_t {
		size_t mask = this->mask;
		size_t cost = 0;
		while (mask) {
			cost += mask & 1u;
			mask >>= 1u;
		}
		return cost;
	}
};

struct TermSpecification
{
	std::vector<Term> pos_terms;
	std::vector<Term> neg_terms;
	std::vector<Term> nc_terms;
	size_t total_cost;
};

struct OptimizedTerms
{
	std::vector<Term> pos_terms;
	std::vector<Term> neg_terms;
	std::vector<Term> nc_terms;
	size_t total_cost;
};

const Encoding ENCODING_01 = {
	.name = "01",
    .logic_values = "01",
    .bits = 1u,
    .encoding_literals = { "l0" },
    .encoding = {
        { '0', "0" },
        { '1', "1" },
        { '*', "*" },
	},
	.text_header = "l0   ",
    .text = {
        { '0', "0 (0)" },
        { '1', "1 (1)" },
        { '*', "* (*)" },
    },
};
const Encoding ENCODING_01X = {
	.name = "01X",
    .logic_values = "X01*",
    .bits = 2u,
    .encoding_literals = { "l0", "l1" },
    .encoding = {
        { 'X', "00" },
        { '0', "10" },
        { '1', "01" },
        { '*', "**" },
    },
	.text_header = "l1 l0   ",
    .text = {
        { 'X', "0  0 (X)" },
        { '0', "0  1 (0)" },
        { '1', "1  0 (1)" },
        { '*', "*  * (*)" },
    },
};
const Encoding ENCODING_U01X = {
	.name = "U01X",
    .logic_values = "U01X",
    .bits = 2u,
    .encoding_literals = { "l0", "l1" },
    .encoding = {
        { 'U', "00" },
        { '0', "10" },
        { '1', "01" },
        { 'X', "11" },
        { '*', "**" },
    },
	.text_header = "l1 l0   ",
    .text = {
        { 'U', "0  0 (U)" },
        { '0', "0  1 (0)" },
        { '1', "1  0 (1)" },
        { 'X', "1  1 (X)" },
        { '*', "*  * (*)" },
    },
};
const Encoding ENCODING_01F = {
	.name = "01F",
    .logic_values = "X01*",
    .bits = 2u,
    .encoding_literals = { "l0", "l1" },
    .encoding = {
        { 'X', "00" },
        { '0', "F0" },
        { '1', "0F" },
        { '*', "**" },
    },
	.text_header = "l1 l0   ",
    .text = {
        { 'X', "0  0 (X)" },
        { '0', "0  F (0)" },
        { '1', "F  0 (1)" },
        { '*', "*  * (*)" },
    },
};
const Encoding ENCODING_U01F = {
	.name = "U01F",
    .logic_values = "U01X",
    .bits = 2u,
    .encoding_literals = { "l0", "l1" },
    .encoding = {
        { 'U', "00" },
        { '0', "1F" },
        { '1', "F1" },
        { 'X', "11" },
        { '*', "**" },
    },
	.text_header = "l1 l0   ",
    .text = {
        { 'U', "0  0 (U)" },
        { '0', "F  1 (0)" },
        { '1', "1  F (1)" },
        { 'X', "1  1 (X)" },
        { '*', "*  * (*)" },
    },
};


const vector<Function> FUNCTIONS = {
	{
		.name = "Buffer",
		.inputs = { "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "BUF",
				.inputs = {
					{ "in", ENCODING_01 }
				},
				.outputs = {
					{ "out", ENCODING_01, "01" }
				},
			},
			{
				.name = "BUF",
				.inputs = {
					{ "in", ENCODING_01X }
				},
				.outputs = {
					{ "out", ENCODING_01X, "X01*" }
				},
			},
			{
				.name = "BUF",
				.inputs = {
					{ "in", ENCODING_U01X }
				},
				.outputs = {
					{ "out", ENCODING_U01X, "U01X" }
				},
			},
			{
				.name = "BUF",
				.inputs = {
					{ "in", ENCODING_01F }
				},
				.outputs = {
					{ "out", ENCODING_01F, "X01*" }
				},
			},
			{
				.name = "BUF",
				.inputs = {
					{ "in", ENCODING_U01F }
				},
				.outputs = {
					{ "out", ENCODING_U01F, "U01X" }
				},
			},
		}
	},

	{
		.name = "Inverter",
		.inputs = { "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "INV",
				.inputs = {
					{ "in", ENCODING_01 }
				},
				.outputs = {
					{ "out", ENCODING_01, "10" }
				},
			},
			{
				.name = "INV",
				.inputs = {
					{ "in", ENCODING_01X }
				},
				.outputs = {
					{ "out", ENCODING_01X, "X10*" }
				},
			},
			{
				.name = "INV",
				.inputs = {
					{ "in", ENCODING_U01X }
				},
				.outputs = {
					{ "out", ENCODING_U01X, "U10X" }
				},
			},
			{
				.name = "INV",
				.inputs = {
					{ "in", ENCODING_01F }
				},
				.outputs = {
					{ "out", ENCODING_01F, "X10*" }
				},
			},
			{
				.name = "INV",
				.inputs = {
					{ "in", ENCODING_U01F }
				},
				.outputs = {
					{ "out", ENCODING_U01F, "U10X" }
				},
			},
		}
	},

	{
		.name = "Xor",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "XOR",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0110" }
				},
			},
			{
				.name = "XOR",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*X01*X10*****" }
				},
			},
			{
				.name = "XOR",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUUUU01XU10XUXXX" }
				},
			},
			{
				.name = "XOR",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*X01*X10*****" }
				},
			},
			{
				.name = "XOR",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUUUU01XU10XUXXX" }
				},
			},
		}
	},

	{
		.name = "And",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "AND",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0001" }
				},
			},
			{
				.name = "AND",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "X0X*000*X01*****" }
				},
			},
			{
				.name = "AND",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "U0UU0000U01XU0XX" }
				},
			},
			{
				.name = "AND",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "X0X*000*X01*****" }
				},
			},
			{
				.name = "AND",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "U0UU0000U01XU0XX" }
				},
			},
		}
	},

	{
		.name = "Or",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "OR",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0111" }
				},
			},
			{
				.name = "OR",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XX1*X01*111*****" }
				},
			},
			{
				.name = "OR",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UU1UU01X1111UX1X" }
				},
			},
			{
				.name = "OR",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XX1*X01*111*****" }
				},
			},
			{
				.name = "OR",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UU1UU01X1111UX1X" }
				},
			},
		}
	},

	{
		.name = "Xnor",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "XNOR",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "1001" }
				},
			},
			{
				.name = "XNOR",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*X10*X01*****" }
				},
			},
			{
				.name = "XNOR",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUUUU10XU01XUXXX" }
				},
			},
			{
				.name = "XNOR",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*X10*X01*****" }
				},
			},
			{
				.name = "XNOR",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUUUU10XU01XUXXX" }
				},
			},
		}
	},

	{
		.name = "Nand",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "NAND",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "1110" }
				},
			},
			{
				.name = "NAND",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "X1X*111*X10*****" }
				},
			},
			{
				.name = "NAND",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "U1UU1111U10XU1XX" }
				},
			},
			{
				.name = "NAND",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "X1X*111*X10*****" }
				},
			},
			{
				.name = "NAND",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "U1UU1111U10XU1XX" }
				},
			},
		}
	},

	{
		.name = "Nor",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "NOR",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "1000" }
				},
			},
			{
				.name = "NOR",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XX0*X10*000*****" }
				},
			},
			{
				.name = "NOR",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UU0UU10X0000UX0X" }
				},
			},
			{
				.name = "NOR",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XX0*X10*000*****" }
				},
			},
			{
				.name = "NOR",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UU0UU10X0000UX0X" }
				},
			},
		}
	},

	{
		.name = "TristateBuffer",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "BUFIF",
				.inputs = {
					{ "in", ENCODING_01 },
					{ "en", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0001" }
				},
			},
			{
				.name = "BUFIF",
				.inputs = {
					{ "in", ENCODING_01X },
					{ "en", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*XX0*XX1*****" }
				},
			},
			{
				.name = "BUFIF",
				.inputs = {
					{ "in", ENCODING_U01X },
					{ "en", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUXUUU0UUU1UUUUU" }
				},
			},
			{
				.name = "BUFIF",
				.inputs = {
					{ "in", ENCODING_01F },
					{ "en", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*XX0*XX1*****" }
				},
			},
			{
				.name = "BUFIF",
				.inputs = {
					{ "in", ENCODING_U01F },
					{ "en", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUXUUU0UUU1UUUUU" }
				},
			},
		}
	},

	{
		.name = "TristateInverter",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "NOTIF",
				.inputs = {
					{ "in", ENCODING_01 },
					{ "en", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "1110" }
				},
			},
			{
				.name = "NOTIF",
				.inputs = {
					{ "in", ENCODING_01X },
					{ "en", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*XX1*XX0*****" }
				},
			},
			{
				.name = "NOTIF",
				.inputs = {
					{ "in", ENCODING_U01X },
					{ "en", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUXUUU1UUU0UUUUU" }
				},
			},
			{
				.name = "NOTIF",
				.inputs = {
					{ "in", ENCODING_01F },
					{ "en", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*XX1*XX0*****" }
				},
			},
			{
				.name = "NOTIF",
				.inputs = {
					{ "in", ENCODING_U01F },
					{ "en", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUXUUU1UUU0UUUUU" }
				},
			},
		}
	},

	{
		.name = "Multiplexer",
		.inputs = { "LogicContainer", "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "MUX",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
					{ "sel", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "01010011" }
				},
			},
			{
				.name = "MUX",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
					{ "sel", ENCODING_01X },
				},
				.outputs = {
					{
						"out",
						ENCODING_01X,
						"XXX*XXX*XXX*****"
						"X01*X01*X01*****"
						"XXX*000*111*****"
						"****************"
					}
				},
			},
			{
				.name = "MUX",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
					{ "sel", ENCODING_U01X },
				},
				.outputs = {
					{
						"out",
						ENCODING_U01X,
						"UUUUUUUUUUUUUUUU"
						"U01XU01XU01XU01X"
						"UUUU00001111XXXX"
						"XXXXXXXXXXXXXXXX"
					}
				},
			},
			{
				.name = "MUX",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
					{ "sel", ENCODING_01F },
				},
				.outputs = {
					{
						"out",
						ENCODING_01F,
						"XXX*XXX*XXX*****"
						"X01*X01*X01*****"
						"XXX*000*111*****"
						"****************"
					}
				},
			},
			{
				.name = "MUX",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
					{ "sel", ENCODING_U01F },
				},
				.outputs = {
					{
						"out",
						ENCODING_U01F,
						"UUUUUUUUUUUUUUUU"
						"U01XU01XU01XU01X"
						"UUUU00001111XXXX"
						"XXXXXXXXXXXXXXXX"
					}
				},
			},
		}
	},

	{
		.name = "SetControl",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "SET",
				.inputs = {
					{ "in", ENCODING_01 },
					{ "set", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0111" }
				},
			},
			{
				.name = "SET",
				.inputs = {
					{ "in", ENCODING_01X },
					{ "set", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*X01*111*****" }
				},
			},
			{
				.name = "SET",
				.inputs = {
					{ "in", ENCODING_U01X },
					{ "set", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUUUU01X1111XXXX" }
				},
			},
			{
				.name = "SET",
				.inputs = {
					{ "in", ENCODING_01F },
					{ "set", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*X01*111*****" }
				},
			},
			{
				.name = "SET",
				.inputs = {
					{ "in", ENCODING_U01F },
					{ "set", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUUUU01X1111XXXX" }
				},
			},
		}
	},

	{
		.name = "ResetControl",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer" },
		.implementations = {
			{
				.name = "RESET",
				.inputs = {
					{ "in", ENCODING_01 },
					{ "reset", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0100" }
				},
			},
			{
				.name = "RESET",
				.inputs = {
					{ "in", ENCODING_01X },
					{ "reset", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01X, "XXX*X01*000*****" }
				},
			},
			{
				.name = "RESET",
				.inputs = {
					{ "in", ENCODING_U01X },
					{ "reset", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_U01X, "UUUUU01X0000XXXX" }
				},
			},
			{
				.name = "RESET",
				.inputs = {
					{ "in", ENCODING_01F },
					{ "reset", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01F, "XXX*X01*000*****" }
				},
			},
			{
				.name = "RESET",
				.inputs = {
					{ "in", ENCODING_U01F },
					{ "reset", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_U01F, "UUUUU01X0000XXXX" }
				},
			},
		}
	},

	{
		.name = "LogicEquivalenceDetector",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer01" },
		.implementations = {
			{
				.name = "EQ",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "1001" }
				},
			},
			{
				.name = "EQ",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01, "100*010*001*****" }
				},
			},
			{
				.name = "EQ",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_01, "1000010000100001" }
				},
			},
			{
				.name = "EQ",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01, "100*010*001*****" }
				},
			},
			{
				.name = "EQ",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_01, "1000010000100001" }
				},
			},
		}
	},

	{
		.name = "Logic01DifferenceDetector",
		.inputs = { "LogicContainer", "LogicContainer" },
		.outputs = { "LogicContainer01" },
		.implementations = {
			{
				.name = "01DIFF",
				.inputs = {
					{ "in1", ENCODING_01 },
					{ "in2", ENCODING_01 },
				},
				.outputs = {
					{ "out", ENCODING_01, "0110" }
				},
			},
			{
				.name = "01DIFF",
				.inputs = {
					{ "in1", ENCODING_01X },
					{ "in2", ENCODING_01X },
				},
				.outputs = {
					{ "out", ENCODING_01, "000*001*010*****" }
				},
			},
			{
				.name = "01DIFF",
				.inputs = {
					{ "in1", ENCODING_U01X },
					{ "in2", ENCODING_U01X },
				},
				.outputs = {
					{ "out", ENCODING_01, "0000001001000000" }
				},
			},
			{
				.name = "01DIFF",
				.inputs = {
					{ "in1", ENCODING_01F },
					{ "in2", ENCODING_01F },
				},
				.outputs = {
					{ "out", ENCODING_01, "000*001*010*****" }
				},
			},
			{
				.name = "01DIFF",
				.inputs = {
					{ "in1", ENCODING_U01F },
					{ "in2", ENCODING_U01F },
				},
				.outputs = {
					{ "out", ENCODING_01, "0000001001000000" }
				},
			},
		}
	},
};

auto to_encoding_string(const vector<Input>& inputs) -> string;
auto to_encoding_string(const vector<Output>& outputs) -> string;
auto to_string(const vector<Term>& terms, size_t bits) -> string;
auto to_string(const Term& term, size_t bits) -> string;
auto merge_streams(stringstream& left, stringstream& right, const string& separator, ostream& out) -> void;
auto prefix_stream(stringstream& stream, const string& prefix, ostream& out) -> void;
auto rtrim_stream(stringstream& stream, ostream& out) -> void;

auto simplify(const vector<Term>& pos_terms, const vector<Term>& neg_terms, const vector<Term>& not_cares, size_t bits) -> OptimizedTerms;
auto reduce_terms(vector<Term> terms, size_t bits) -> vector<Term>;
auto is_dominated_by(const Term& term, const Term& other) -> bool;
auto get_terms_for_literal(const vector<Input>& inputs, const Output& output, string literal) -> TermSpecification;

auto print_logic_table(const Implementation& gate, ostream& out) -> void;
auto print_kv_diagram_for_terms(const vector<Term>& pos_terms, const vector<Term>& neg_terms, const Implementation& gate, const Output& output, string output_literal, ostream& out) -> void;
auto print_implications_for_terms(const vector<Term>& terms, const Implementation& gate, const Output& output, string output_literal, bool output_inverted, ostream& out) -> void;
auto print_sat_calls_for_terms(const vector<Term>& terms, const Implementation& gate, const Output& output, string output_literal, bool output_inverted, ostream& out) -> void;
auto print_implementation(const Implementation& gate, ostream& out) -> void;

GateLogicOptimization::GateLogicOptimization(void) = default;
GateLogicOptimization::~GateLogicOptimization(void) = default;

void GateLogicOptimization::Init(void)
{
}

void GateLogicOptimization::Run(void)
{
	FileHandle handle("[DataExportDirectory]/LogicEncoder.gen.ipp", false);
	auto& out = handle.GetOutStream();
	out << "// Automatic generated file - do not edit!" << endl;
	out << "// Use the gate logic optimization workflow to re-generate it." << endl;
	out << endl;
	out << "#include \"Tpg/Encoder/LogicEncoder.hpp\"" << endl;
	out << endl;
	out << "#ifdef BASIC_LOGIC_ENCODER_GEN" << endl;
	out << endl;
	out << "namespace FreiTest" << endl;
	out << "{" << endl;
	out << "namespace Tpg" << endl;
	out << "{" << endl;
	out << endl;

	for (auto& function : FUNCTIONS) {
		auto& implementations = function.implementations;
		auto& inputTypes = function.inputs;
		auto& outputTypes = function.outputs;
		auto& inputNames = function.implementations[0u].inputs;
		auto& outputNames = function.implementations[0u].outputs;

		out << "template<typename LogicContainer>" << endl;
		out << "void LogicEncoder::Encode" << function.name << "(";

		bool hasPrevious = false;
		size_t index = 0u;
		for (auto& input : inputNames) {
			if (hasPrevious) out << ", ";
			out << "const " << inputTypes[index++] << "& " << input.name;
			hasPrevious = true;
		}
		index = 0u;
		for (auto& output : outputNames) {
			if (hasPrevious) out << ", ";
			out << "const " << outputTypes[index++] << "& " << output.name;
			hasPrevious = true;
		}
		out << ")" << endl;
		out << "{" << endl;

		for (auto& input : inputNames)
			out << "\tASSERT_SET(" << input.name << ");" << endl;
		for (auto& output : outputNames)
			out << "\tASSERT_SET(" << output.name << ");" << endl;
		out << endl;

		bool first = true;
		for (auto& implementation : implementations) {
			LOG(INFO) << "Implementation: " << implementation.name;
			LOG(INFO) << "Input Encoding: " << to_encoding_string(implementation.inputs);
			LOG(INFO) << "Outputs Encoding: " << to_encoding_string(implementation.outputs);

			stringstream impl_stream;
			stringstream impl_prefixed;
			print_implementation(implementation, impl_stream);
			prefix_stream(impl_stream, "\t\t", impl_prefixed);

			out << (first ? "\tif" : "\telse if") << " constexpr (is_" << implementation.inputs[0].encoding.name << "_encoding_v<LogicContainer>)" << endl;
			out << "\t{" << endl;
			rtrim_stream(impl_prefixed, out);
			out << "\t}" << endl;

			first = false;
		}

		out << "\telse" << endl;
    	out << "\t{" << endl;
    	out << "\t\tNOT_IMPLEMENTED;" << endl;
    	out << "\t}" << endl;
		out << "}" << endl;
		out << endl;
	}
	out << "}" << endl;
	out << "}" << endl;
	out << endl;
	out << "#endif // BASIC_LOGIC_ENCODER_GEN" << endl;
}

/**
 * @brief The central method to do the DON'T CARE assignments and the simplification of the logic terms.
 *
 * @param pos_terms The positive terms that are assigned with a predefined LOGIC_ONE value.
 * @param neg_terms The negative terms that are assigned with a predefined LOGIC_ZERO value.
 * @param not_cares The terms that are DON'T CARE and can be assigned either a LOGIC_ONE or LOGIC_ZERO value.
 * @param bits The sum of the input bits of the gate.
 * @return OptimizedTerms The optimized terms that are then used for clause generation.
 */
auto simplify(const vector<Term>& pos_terms, const vector<Term>& neg_terms, const vector<Term>& not_cares, size_t bits) -> OptimizedTerms {
	auto proxy = SolverProxy::Sat::MaxSatSolverProxy::CreateMaxSatSolver(SolverProxy::Sat::SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE);
	proxy->SetSolverTimeout(300); // 5 Minutes

	const auto values = (1u << bits);
	const auto bit_mask = (1u << bits) - 1u;

	// The logic values in the Karnaugh-Veitch / symmetry diagram (aka one cell).
	vector<BaseLiteral> zero_assignments(values);
	vector<BaseLiteral> one_assignments(values);

	// Fill in the predefined values from the logic table (pos_terms and neg_terms)
	// and leave the DON'T CARE values undecided. The Max-SAT solver will decide
	// an optimal assignment for these values.
	for (size_t value = 0u; value < values; ++value) {
		zero_assignments[value] = proxy->NewLiteral();
		one_assignments[value] = proxy->NewLiteral();

		// Add constraints imposed by the fixed logic values in pos_terms and neg_terms values.
		// This is done by encoding unit clauses that force the values to be a specific value.
		// The mask is all 1es to force the term to cover only one value (cell).
		const Term value_term = { .value = value, .mask = bit_mask };
		for (auto& term : pos_terms) {
			if (is_dominated_by(value_term, term)) {
				VLOG(3) << "Assignment " << to_string(value_term, bits) << ": positive";
				proxy->CommitClause(one_assignments[value]);
				proxy->CommitClause(-zero_assignments[value]);
				goto next_assignment;
			}
		}
		for (auto& term : neg_terms) {
			if (is_dominated_by(value_term, term)) {
				VLOG(3) << "Assignment " << to_string(value_term, bits) << ": negative";
				proxy->CommitClause(-one_assignments[value]);
				proxy->CommitClause(zero_assignments[value]);
				goto next_assignment;
			}
		}
		for (auto& term : not_cares) {
			if (is_dominated_by(value_term, term)) {
				VLOG(3) << "Assignment " << to_string(value_term, bits) << ": don't care";
				proxy->CommitClause(-zero_assignments[value], -one_assignments[value]);
				goto next_assignment;
			}
		}

		VLOG(3) << "Assignment " << to_string(value_term, bits) << ": free";
		proxy->CommitClause(-one_assignments[value]);
		proxy->CommitClause(-zero_assignments[value]);

	next_assignment:
		continue;
	}

	// Stores a list of groups that cover a specific value (cell) in the Karnaugh-Veitch / symmetry diagram.
	// If one of the groups is assigned a value of true (used) then
	// the underlying cell has to have the value true (in case of one assignment)
	// or false (in case of zero assignment).
	// The lists zero_assignments and one_assignments finally contain the following implications:
	// If the cell is zero then any of the zero groups covering the cell has to be assigned true:
	//     zero_assignment -> (zero_group_1 or zero_group_2 or ... or zero_group_n)
	// If the cell is one then any of the one groups covering the cell has to be assigned true:
	//     one_assignment -> (one_group_1 or one_group_2 or ... or one_group_n)
	// These clauses are finalized / committed further down below.
	vector<vector<BaseLiteral>> zero_assignment_impls(values);
	vector<vector<BaseLiteral>> one_assignment_impls(values);
	for (size_t value { 0u }; value < values; ++value) {
		zero_assignment_impls[value].push_back(-zero_assignments[value]);
		one_assignment_impls[value].push_back(-one_assignments[value]);
	}

	// Holds the term and assignment of the group.
	// If zero_assignment is true then the group is filled with zeroes.
	// If one_assignment is true then the group is filled with ones.
	// Zero_assignment and one_assignment can not be true at the same time.
	// If both assignments are false then this group is not used.
	struct Group {
		Term term;
		BaseLiteral zero_assignment;
		BaseLiteral one_assignment;
	};

	// Iterates all possible groups in the Karnaugh-Veitch / symmetry diagram.
	// This is done by iterating over all possible values and all possible masks.
	// This produces the following sequence which is then removed of duplicates:
	// 1.  ******* (all DON'T CARES)
	// 2.  0****** (only first bit is LOGIC_ZERO)
	// 3.  1****** (only first bit is LOGIC_ONE)
	// 4.  *0***** (only second bit is LOGIC_ZERO)
	// 5.  *1***** (only second bit is LOGIC_ONE)
	// 6.  00***** (first and second bit defined)
	// 7.  10***** (first and second bit defined)
	// 8.  01***** (first and second bit defined)
	// 9.  11***** (first and second bit defined)
	// 10. **0**** (only third bit is defined)
	//     .......
	map<Term, Group> groups;
	for (size_t mask { 0u }; mask < values; ++mask) {
		for (size_t value { 0u }; value < values; ++value) {
			// Because the outer "value" loop might produce duplicates we exclude them here.
			// Check if the masked value has already been encountered.
			const Term term = { .value = value & mask, .mask = mask };
			if (groups.find(term) != groups.end()) {
				continue; // Already handled this group.
			}

			const auto zero_assignment = proxy->NewLiteral();
			const auto one_assignment = proxy->NewLiteral();

			VLOG(3) << "Term " << to_string(term, bits)
				<< ": zero = " << zero_assignment.GetVariable()
				<< ", one = " << one_assignment.GetVariable();

			// Insert the new group and the corresponding literals to encode the zero / one assignment of the group.
			groups.emplace(term, Group { .term = term, .zero_assignment = zero_assignment, .one_assignment = one_assignment });

			// Connect the group with the literals in the region.
			// If the group has been used (true) then all covered literals
			// have to be assigned a value of true (in case of one assignment)
			// or false (in case of zero assignment).
			for (size_t j { 0u }; j < values; ++j) {
				if (is_dominated_by({ .value = j, .mask = bit_mask }, term)) {
					// Add this group to the list of covering groups for the value (cell).
					// This list is later finalized / committed as a single clause per value (cell).
					zero_assignment_impls[j].push_back(zero_assignment);
					one_assignment_impls[j].push_back(one_assignment);

					// Encode implication that forces assignment in case of a used group.
					// If zero group is true then the assignment is false: (zero_lit -> -assignment)
					// If one group is true then the assignment is true: (one_lit -> assignment)
					proxy->CommitClause(-zero_assignment, zero_assignments[j]);
					proxy->CommitClause(-one_assignment, one_assignments[j]);
				}
			}
		}
	}

	// Finalize / commit the clauses that enforce a value (cell) to be assigned
	// a true or false value when a group covers it.
	for (size_t value { 0u }; value < values; ++value) {
		proxy->CommitClause(zero_assignment_impls[value]);
		proxy->CommitClause(one_assignment_impls[value]);
	}

	// Add optimization target to have as a low number of groups used as possible.
	for (auto& [term, group] : groups) {
		// Each specified bit in the mask adds a literal to the clause.
		// We want to minimize the number of clauses but also the number of literals
		// per clause.
		if (term.cost() > 0) {
			proxy->NewClause();
			proxy->AddLiteral(-group.zero_assignment);
			proxy->CommitSoftClause(term.cost());

			proxy->NewClause();
			proxy->AddLiteral(-group.one_assignment);
			proxy->CommitSoftClause(term.cost());
		}
	}

	// Now solve the optimization problem.
	ASSERT(proxy->MaxSolve() == SolverProxy::Sat::SatResult::SAT);

	// Extract the used groups. If a group is used then the term is added to
	// the list of positive (one assignment) or negative (zero assignment) terms.
	vector<Term> simp_pos_terms;
	vector<Term> simp_neg_terms;
	vector<Term> simp_nc_terms;
	size_t total_cost = 0;
	for (auto& [term, group] : groups) {
		const auto one_value = proxy->GetLiteralValue(group.one_assignment);
		const auto zero_value = proxy->GetLiteralValue(group.zero_assignment);
		ASSERT(one_value == Value::Negative || zero_value == Value::Negative);

		if (one_value == Value::Positive) {
			simp_pos_terms.push_back(group.term);
			total_cost += group.term.cost();
		} else if (zero_value == Value::Positive) {
			simp_neg_terms.push_back(group.term);
			total_cost += group.term.cost();
		} else if (zero_value == Value::Negative && one_value == Value::Negative) {
			simp_nc_terms.push_back(group.term);
		} else {
			Logging::Panic("A cell is assigned zero and one at the same time");
		}
	}

	return { .pos_terms = simp_pos_terms, .neg_terms = simp_neg_terms, .nc_terms = simp_nc_terms, .total_cost = total_cost };
}

auto is_dominated_by(const Term& term, const Term& other) -> bool {
    return ((term.mask & other.mask) == other.mask)
		&& !(other.mask & (term.value ^ other.value));
}

auto to_encoding_string(const vector<Input>& inputs) -> string {
	string result;
	for (auto& input : inputs) {
		if (result != "")
			result += ", ";
		result += input.encoding.name;
	}
	return result;
};

auto to_encoding_string(const vector<Output>& outputs) -> string {
	string result;
	for (auto& output : outputs) {
		if (result != "")
			result += ", ";
		result += output.encoding.name;
	}
	return result;
};

auto to_string(const Term& term, size_t bits) -> string {
	string result;
	for (size_t bit { 0u }; bit < bits; ++bit) {
		if (!((term.mask >> bit) & 1u))
			result += '*';
		else if ((term.value >> bit) & 1u)
			result += '1';
		else
			result += '0';
	}
	return result;
}

auto to_string(const vector<Term>& terms, size_t bits) -> string {
	string result;
	for (auto& term : terms) {
		if (result != "")
			result += ", ";
		result += to_string(term, bits);
	}
	return result;
};

auto merge_streams(stringstream& left, stringstream& right, const string& separator, ostream& out) -> void
{
	size_t max_line_length = 0u;
	{
		istringstream left_in { left.str() };
		string line;
		while (getline(left_in, line))
		{
			max_line_length = max(max_line_length, line.size());
		}
	}

	istringstream left_in { left.str() };
	istringstream right_in { right.str() };

	string left_line;
	string right_line;
	while (getline(left_in, left_line) && getline(right_in, right_line))
	{
		out << setw(max_line_length) << left_line << separator << right_line << endl;
	}
}

auto prefix_stream(stringstream& stream, const string& prefix, ostream& out) -> void
{
	string line;
	while (getline(stream, line))
	{
		out << prefix << line << endl;
	}
}

auto rtrim_stream(stringstream& stream, ostream& out) -> void
{
	string line;
	while (getline(stream, line))
	{
		boost::trim_right(line);
		out << line << endl;
	}
}

auto term_sorter(const Term& term, const Term& other) -> bool {
	for (size_t shift = 0u; shift < numeric_limits<size_t>::digits; ++shift) {
		const auto mask_difference = ((term.mask ^ other.mask) >> shift) & 1u;
		const auto value_difference = ((term.value ^ other.value) >> shift) & 1u;

		if (mask_difference)
			return !((term.mask >> shift) & 1u);
		if (value_difference)
			return !((term.value >> shift) & 1u);
	}

	assert("There should never be two equal terms in any case");
	__builtin_unreachable();
};

auto get_terms_for_literal(const vector<Input>& inputs, const Output& output, string literal) -> TermSpecification {
	vector<Term> pos_terms;
	vector<Term> neg_terms;
	vector<Term> nc_terms;

    const size_t input_bits = accumulate(inputs.begin(), inputs.end(), 0u, [](size_t value, auto& input) {
		return value + input.encoding.bits;
	});

	const auto& encoding = output.encoding;
	const auto literal_index = find(
			encoding.encoding_literals.begin(),
			encoding.encoding_literals.end(),
			literal
		) - encoding.encoding_literals.begin();

	const size_t mask = (1u << input_bits) - 1u;
	size_t total_cost = 0;
	for (size_t value = 0u; value < (1u << input_bits); ++value) {
		auto logic_value = output.logic_table[value];
		auto literal_values = output.encoding.encoding.at(logic_value);
		auto literal_value = literal_values[literal_index];
		switch (literal_value)
		{
			case '1':
				pos_terms.push_back({ .value = value, .mask = mask });
				total_cost += input_bits;
				break;
			case '0':
				neg_terms.push_back({ .value = value, .mask = mask });
				total_cost += input_bits;
				break;
			case '*':
				nc_terms.push_back({ .value = value, .mask = mask });
				total_cost += input_bits;
				break;
			case 'F':
				break;
			default:
				Logging::Panic();
		}
	}

	return {
		.pos_terms = pos_terms,
		.neg_terms = neg_terms,
		.nc_terms = nc_terms,
		.total_cost = total_cost
	};
};


auto print_logic_table(const Implementation& gate, ostream& out) -> void {
    // The inputs are from low index to high index (left to right): highest bits to lowest bits
	const auto input_bit_count = gate.input_bit_count();
	const auto input_bit_offsets = gate.input_bit_offsets();

	const auto index_column_size = std::to_string((1u << input_bit_count) - 1u).size() + 1u;
	vector<size_t> input_column_size;
	vector<size_t> output_column_size;
	transform(gate.inputs.begin(), gate.inputs.end(), back_inserter(input_column_size),
		[](auto& input) { return input.encoding.text_header.size() + 1u; }
	);
	transform(gate.outputs.begin(), gate.outputs.end(), back_inserter(output_column_size),
		[](auto& output) { return output.encoding.text_header.size() + 1u; }
	);

	input_column_size.back() -= 1u;

    // Print input / output name header similar to below:
    //
    //      In2       In1      | Out
    //      l1 l0     l1 l0    | l1 l0
    // ------------------------+---------

	out << string(index_column_size + 1u, ' ');
	for (auto input { gate.inputs.size() }; input > 0u ; --input)
		out << " " << setw(input_column_size[input - 1u]) << left << gate.inputs[input - 1u].name;
	out << " |";
	for (auto output { gate.outputs.size() }; output > 0u; --output)
		out << " " << setw(output_column_size[output - 1u]) << left << gate.outputs[output - 1u].name;
	out << endl;

	out << string(index_column_size + 1u, ' ');
	for (auto input {  gate.inputs.size() }; input > 0u; --input)
		out << " " << setw(input_column_size[input - 1u]) << left << gate.inputs[input - 1u].encoding.text_header;
	out << " |";
	for (auto output { gate.outputs.size() }; output > 0u; --output)
		out << " " << setw(output_column_size[output - 1u]) << left << gate.outputs[output - 1u].encoding.text_header;
	out << endl;

	const auto left_size = index_column_size + 1u
		+ accumulate(input_column_size.begin(), input_column_size.end(), 0u)
		+ input_column_size.size() + 1u;
	const auto right_size = 1u
		+ accumulate(output_column_size.begin(), output_column_size.end(), 0u)
		+ output_column_size.size();

	out << string(left_size, '-') << "+" << string(right_size, '-') << endl;

    // Print the rest of the table similar to below:
    //
    // 0   0  0 (X)  0  0 (X) | 1 (1)
    // 1   0  0 (X)  0  1 (0) | 0 (0)
    // 2   0  0 (X)  1  0 (1) | 0 (0)
    // 3   0  0 (X)  *  * (*) | 0 (0)
    // .   .  . ...  .  . ... | . ...

    // Iterate all possible input literal assignments
	for (auto value { 0u }; value < (1u << input_bit_count); ++value) {
		if (value != 0u && (value % 4u) == 0u)
			out << string(left_size, ' ') << "|" << string(right_size, ' ') << endl;

		out << setw(index_column_size) << right << std::to_string(value) << " ";
		for (auto input { gate.inputs.size() }; input > 0u; --input) {
			const auto& encoding = gate.inputs[input - 1u].encoding;

            const auto mask = (1u << encoding.bits) - 1u;
            const auto shift = input_bit_offsets[input - 1u];
            const auto input_value = (value >> shift) & mask;

			const auto& encoded_value = encoding.text.at(encoding.logic_values[input_value]);
			out << " " << setw(input_column_size[input - 1u]) << left << encoded_value;
		}
		out << " |";
		for (auto output { gate.outputs.size() }; output > 0u; --output) {
			const auto& encoding = gate.outputs[output - 1u].encoding;
			const auto& logic_table = gate.outputs[output - 1u].logic_table;

			const auto output_value = logic_table[value];
			const auto& encoded_value = encoding.text.at(output_value);
			out << " " << setw(output_column_size[output - 1u]) << left << encoded_value;
		}
		out << endl;
	}
	out << endl;
}

auto print_implications_for_terms(const vector<Term>& terms, const Implementation& gate, const Output& output, string output_literal, bool output_inverted, ostream& out) -> void {
	const auto input_bit_offsets = gate.input_bit_offsets();

	for (auto& term : terms) {
		string literal_string;
		for (size_t input { 0u }; input < gate.inputs.size(); ++input) {
			const auto& encoding = gate.inputs[input].encoding;
			for (size_t input_literal { 0u }; input_literal < encoding.encoding_literals.size(); ++input_literal) {
				const auto shift = input_bit_offsets[input] + input_literal;

                const auto input_literal_value = (term.value >> shift) & 1u;
                const auto input_literal_mask = (term.mask >> shift) & 1u;
                const string input_literal_sign = input_literal_value ? " " : "-";
                const string input_literal_string = input_literal_sign + gate.inputs[input].name + "." + encoding.encoding_literals[input_literal] + " & ";
				literal_string += input_literal_mask ? input_literal_string : string(input_literal_string.size(), ' ');
			}
		}

        // Trim last ' & ' which is appended even though no literal follows
        literal_string = literal_string.substr(0u, literal_string.size()-3u);

		// Replace last & if it is not followed by literal
		if (auto ampIndex = literal_string.rfind('&'), dotIndex = literal_string.rfind('.');
				ampIndex != string::npos && dotIndex != string::npos && ampIndex > dotIndex) {
			literal_string[ampIndex] = ' ';
		}

		const string output_sign = output_inverted ? "-" : " ";
		out << "(" << literal_string << ") -> " << output_sign << output.name << "." << output_literal << endl;
	}
}

auto print_sat_calls_for_terms(const vector<Term>& terms, const Implementation& gate, const Output& output, string output_literal, bool output_inverted, ostream& out) -> void {
	const auto input_bit_offsets = gate.input_bit_offsets();

	for (auto& term : terms) {
		string literal_string;
		for (size_t input { 0u }; input < gate.inputs.size(); ++input) {
			const auto& encoding = gate.inputs[input].encoding;
			for (size_t input_literal { 0u }; input_literal < encoding.encoding_literals.size(); ++input_literal) {
				const auto shift = input_bit_offsets[input] + input_literal;

                const auto input_literal_value = (term.value >> shift) & 1u;
                const auto input_literal_mask = (term.mask >> shift) & 1u;
                const string input_literal_sign = input_literal_value ? "-" : " "; // Inverted value due to encoded implication
                const string input_literal_string = input_literal_sign + gate.inputs[input].name + "." + encoding.encoding_literals[input_literal] + ", ";
				literal_string += input_literal_mask ? input_literal_string : string(input_literal_string.size(), ' ');
			}
		}

		const string output_sign = output_inverted ? "-" : " ";
		out << "_solver->CommitClause(" << literal_string << output_sign << output.name << "." << output_literal << ");" << endl;
	}
}

auto print_kv_diagram_for_terms(const vector<Term>& pos_terms, const vector<Term>& neg_terms, const vector<Term>& not_cares, const Implementation& gate, const Output& output, string output_literal, ostream& out) -> void {
	const auto input_bit_count = gate.input_bit_count();
	const auto input_bits = gate.input_bit_sizes();
	const size_t mask = (1u << input_bit_count) - 1u;

    // Create a logic table for the provided terms
    string logic_table = "";
	for (size_t value { 0u }; value < (1u << input_bit_count); ++value) {
		for (auto& term : pos_terms) {
			if (is_dominated_by({ .value = value, .mask = mask }, term)) {
				logic_table += "1";
				goto next_value;
			}
		}

		for (auto& term : neg_terms) {
			if (is_dominated_by({ .value = value, .mask = mask }, term)) {
				logic_table += "0";
				goto next_value;
			}
		}

		for (auto& term : not_cares) {
			if (is_dominated_by({ .value = value, .mask = mask }, term)) {
				logic_table += "*";
				goto next_value;
			}
		}

		logic_table += "F";

		next_value:
			continue;
	}

	vector<string> input_port_names;
	vector<string> input_literal_names;
	for (auto& input : gate.inputs) {
		for (size_t index = 0u; index < input.encoding.bits; ++index) {
			input_port_names.push_back(input.name);
			input_literal_names.push_back(input.encoding.encoding_literals[index]);
		}
	}

	const auto left = [](string value, size_t width) -> string {
		if (ssize_t fill = width - value.size(); fill > 0)
			return value + string(fill, ' ');
		return value;
	};
	const auto center = [](string value, size_t width) -> string {
		if (ssize_t fill = width - value.size(); fill > 0)
			return string(fill / 2, ' ') + value + string(fill - (fill / 2), ' ');
		return value;
	};
	const auto right = [](string value, size_t width) -> string {
		if (ssize_t fill = width - value.size(); fill > 0)
			return string(fill, ' ') + value;
		return value;
	};
	const auto tab = [&logic_table, left=left](size_t index) -> string {
		return left(string(1, logic_table[index]), 2);
	};

	if (input_bits == vector<size_t> { 1u }) {
		// Example:
        //   out   |   in    |
        //   l0    |    | l0 |
        // --------+----+----+
        //         | 0  | 1  |
        //         +----+----+
		out << center(output.name, 7) << " | " << center(input_port_names[0], 7) << " |" << endl;
		out << center(output_literal, 7) << " |    | " << center(input_literal_names[0], 2) << " |" << endl;
		out << "--------+----+----+" << endl;
		out << "        | " << tab(0) << " | " << tab(1) << " |" << endl;
		out << "        +----+----+" << endl;
	} else if (input_bits == vector<size_t> { 1u, 1u } || input_bits == vector<size_t> { 2u }) {
		// Example:
        //   out   |   in2   |
        //   l0    |    | l0 |
        // --------+----+----+
        //         | 0  | 0  |
        // in1 ----+----+----+
        //      l0 | 0  | 1  |
        // --------+----+----+
		out << center(output.name, 7) << " | " << center(input_port_names[1], 7) << " |" << endl;
		out << center(output_literal, 7) << " |    | " << center(input_literal_names[1], 2) << " |" << endl;
		out << "--------+----+----+" << endl;
		out << "        | " << tab(0) << " | " << tab(2) << " |" << endl;
		out << right(input_port_names[0], 3) << " ----+----+----+" << endl;
		out << right(input_literal_names[0], 7) << " | " << tab(1) << " | " << tab(3) << " |" << endl;
		out << "--------+----+----+" << endl;
	} else if (input_bits == vector<size_t> { 1u, 1u, 1u }) {
		out << center(output.name, 7) << " | " << center(input_port_names[2], 7) << " |" << endl;
		out << center(output_literal, 7) << " |    | " << center(input_literal_names[2], 2) << " |" << endl;
		out << "--------+----+----+--------" << endl;
		out << "        | " << tab(0) << " | " << tab(4) << " |" << endl;
		out << "        +----+----+----" << endl;
		out << "        | " << tab(1) << " | " << tab(5) << " |" << endl;
		out << right(input_port_names[1], 3) << " ----+----+----+ " << left(input_literal_names[0], 3) << input_port_names[0] << endl;
		out << "        | " << tab(3) << " | " << tab(7) << " |" << endl;
		out << right(input_literal_names[1], 7) << " +----+----+----" << endl;
		out << "        | " << tab(2) << " | " << tab(6) << " |" << endl;
		out << "--------+----+----+--------" << endl;
	} else if (input_bits == vector<size_t> { 2u, 2u }) {
		out << center(output.name, 7) << " | " << center(input_port_names[3], 17) << " |" << endl;
		out << center(output_literal, 7) << " |         | " << center(input_literal_names[3], 7) << " |" << endl;
		out << "--------+----+----+----+----+" << endl;
		out << "        | " << tab(0) << " | " << tab(4) << " | " << tab(12) << " | " << tab(8) << " |" << endl;
		out << "        +----+----+----+----+----" << endl;
		out << "        | " << tab(1) << " | " << tab(5) << " | " << tab(13) << " | " << tab(9) << " |" << endl;
		out << right(input_port_names[1], 3) << "----+----+----+----+----+ " << input_literal_names[0] << endl;
		out << "        | " << tab(3) << " | " << tab(7) << " | " << tab(15) << " | " << tab(11) << " |" << endl;
		out << right(input_literal_names[1], 7) << " +----+----+----+----+----" << endl;
		out << "        | " << tab(2) << " | " << tab(6) << " | " << tab(14) << " | " << tab(10) << " |" << endl;
		out << "--------+----+----+----+----+" << endl;
		out << "             | " << center(input_literal_names[2], 7) << " |" << endl;
	} else if (input_bits == vector<size_t> { 2u, 2u, 2u }) {
		out << "                | " << center(input_port_names[5], 37) << " |" << endl;
		out << " " << center(output.name, 14) << " |                   | " << center(input_literal_names[5], 17) << " |" << endl;
		out << " " << center(output_literal, 14) << " |                   |                   |" << endl;
		out << "                | " << center(input_port_names[3], 17) << " | " << center(input_port_names[3], 17) << endl;
		out << "                |         | " << center(input_literal_names[3], 7) << " | " << center(input_literal_names[3], 7) << " |         |" << endl;
		out << "----------------+----+----+----+----+----+----+----+----+--------" << endl;
		out << "                | " << tab(0) << " | " << tab(4) << " | " << tab(12) << " | " << tab(8) << " | " << tab(40) << " | " << tab(44) << " | " << tab(36) << " | " << tab(32) << " |" << endl;
		out << "                +----+----+----+----+----+----+----+----+----" << endl;
		out << "                | " << tab(1) << " | " << tab(5) << " | " << tab(13) << " | " << tab(9) << " | " << tab(41) << " | " << tab(45) << " | " << tab(37) << " | " << tab(33) << " |" << endl;
		out << "        " << right(input_port_names[1], 3) << " ----+----+----+----+----+----+----+----+----+ " << left(input_literal_names[0], 3) << " " << input_port_names[0] << endl;
		out << "                | " << tab(3) << " | " << tab(7) << " | " << tab(15) << " | " << tab(11) << " | " << tab(43) << " | " << tab(47) << " | " << tab(39) << " | " << tab(35) << " |" << endl;
		out << "        " << right(input_literal_names[1], 7) << " +----+----+----+----+----+----+----+----+----" << endl;
		out << "                | " << tab(2) << " | " << tab(6) << " | " << tab(14) << " | " << tab(10) << " | " << tab(42) << " | " << tab(46) << " | " << tab(38) << " | " << tab(34) << " |" << endl;
		out << right(input_port_names[4], 3) << " ------------+----+----+----+----+----+----+----+----+--------" << endl;
		out << "                | " << tab(18) << " | " << tab(22) << " | " << tab(30) << " | " << tab(26) << " | " << tab(58) << " | " << tab(62) << " | " << tab(54) << " | " << tab(50) << " |" << endl;
		out << "        " << right(input_literal_names[1], 7) << " +----+----+----+----+----+----+----+----+----" << endl;
		out << "                | " << tab(19) << " | " << tab(23) << " | " << tab(31) << " | " << tab(27) << " | " << tab(59) << " | " << tab(63) << " | " << tab(55) << " | " << tab(51) << " |" << endl;
		out << right(input_literal_names[4], 3) << "     " << right(input_port_names[1], 3) << " ----+----+----+----+----+----+----+----+----+ " << left(input_literal_names[0], 3) << " " << input_port_names[0] << endl;
		out << "                | " << tab(17) << " | " << tab(21) << " | " << tab(29) << " | " << tab(25) << " | " << tab(57) << " | " << tab(61) << " | " << tab(53) << " | " << tab(49) << " |" << endl;
		out << "                +----+----+----+----+----+----+----+----+----" << endl;
		out << "                | " << tab(16) << " | " << tab(20) << " | " << tab(28) << " | " << tab(24) << " | " << tab(56) << " | " << tab(60) << " | " << tab(52) << " | " << tab(48) << " |" << endl;
		out << "----------------+----+----+----+----+----+----+----+----+--------" << endl;
		out << "                |    | " << center(input_literal_names[2], 7) << " |    |    | " << center(input_literal_names[2], 7) << " |    |" << endl;
		out << "                |      " << center(input_port_names[2], 7) << "      |      " << center(input_port_names[2], 7) << "      |" << endl;
	}
}

auto print_implementation(const Implementation& gate, ostream& out) -> void {
	stringstream comment;
	comment << "Implementation: " << gate.name << endl;
	comment << "Input Encodings: " << to_encoding_string(gate.inputs) << endl;
	comment << "Output Encodings: " << to_encoding_string(gate.outputs) << endl;
	comment << endl;

	const size_t input_bits = gate.input_bit_count();
	print_logic_table(gate, comment);

	vector<tuple<const Implementation&, const Output&, const string&, vector<Term>, vector<Term>>> results;
    for (auto& output : gate.outputs) {
		for (auto& literal : output.encoding.encoding_literals) {
			auto [pos_terms, neg_terms, nc_terms, costs] = get_terms_for_literal(gate.inputs, output, literal);
			auto [pos_terms_sim, neg_terms_sim, nc_terms_sim, costs_sim] = simplify(pos_terms, neg_terms, nc_terms, input_bits);
			auto costs_terms = pos_terms.size() + neg_terms.size() + nc_terms.size();
			auto costs_terms_sim = pos_terms_sim.size() + neg_terms_sim.size();

			// Add spacer
			comment << endl;

			// Make output stable.
			sort(pos_terms_sim.begin(), pos_terms_sim.end(), term_sorter);
			sort(neg_terms_sim.begin(), neg_terms_sim.end(), term_sorter);

			results.emplace_back(gate, output, literal, pos_terms_sim, neg_terms_sim);

			comment << "Output " << output.name << ", Literal " << literal << endl;
			comment << "-------------------------------" << endl;
			comment << endl;
			comment << "initial pos: [" << to_string(pos_terms, input_bits) << "]" << endl;
			comment << "initial neg: [" << to_string(neg_terms, input_bits) << "]" << endl;
			comment << "initial nc:  [" << to_string(nc_terms, input_bits) << "]" << endl;
			comment << "initial cost: " << costs_terms << " terms with a total of " << costs << " literals" << endl;
			comment << endl;

			if (nc_terms.size() > 0) {
				// Merge both KV-Diagrams to be side-by-side.
				stringstream orig_stream;
				stringstream sim_stream;
				print_kv_diagram_for_terms(pos_terms_sim, neg_terms_sim, nc_terms_sim, gate, output, literal, sim_stream);
				print_kv_diagram_for_terms(pos_terms, neg_terms, nc_terms, gate, output, literal, orig_stream);
				merge_streams(orig_stream, sim_stream, "    ", comment);
			} else {
				print_kv_diagram_for_terms(pos_terms, neg_terms, nc_terms, gate, output, literal, comment);
			}
			comment << endl;

			comment << "pos: [" << to_string(pos_terms_sim, input_bits) << "]" << endl;
			comment << "neg: [" << to_string(neg_terms_sim, input_bits) << "]" << endl;
			comment << "cost: " << costs_terms_sim << " terms with a total of " << costs_sim << " literals" << endl;
			comment << endl;

            print_implications_for_terms(pos_terms_sim, gate, output, literal, false, comment);
            print_implications_for_terms(neg_terms_sim, gate, output, literal, true, comment);
		}
	}

	out << "/*" << endl;
	prefix_stream(comment, "\t", out);
	out << "*/" << endl;
	for (auto [gate, output, literal, pos_terms_sim, neg_terms_sim] : results) {
		print_sat_calls_for_terms(pos_terms_sim, gate, output, literal, false, out);
		print_sat_calls_for_terms(neg_terms_sim, gate, output, literal, true, out);
	}
};

auto Implementation::input_bit_count(void) const -> size_t {
	return accumulate(inputs.begin(), inputs.end(), 0u, [](size_t value, auto& input) {
		return value + input.encoding.bits;
	});
}

auto Implementation::input_bit_sizes(void) const -> vector<size_t> {
	vector<size_t> result(inputs.size());
	for (size_t input { 0u }; input < inputs.size(); ++input)
		result[input] = inputs[input].encoding.bits;
	return result;
}

auto Implementation::input_bit_offsets(void) const -> vector<size_t> {
	vector<size_t> result(inputs.size());
	for (size_t input { 0u }, sum { 0u }; input < inputs.size(); ++input) {
		result[input] = sum;
		sum += inputs[input].encoding.bits;
	}
	return result;
}

};
};
};
