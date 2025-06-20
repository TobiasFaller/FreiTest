#pragma once

#include <string>

enum CellCategory
{
	MAIN_UNKNOWN = -1,

	MAIN_CONSTANT,

	MAIN_IN,
	MAIN_OUT,
	MAIN_INOUT,

	MAIN_BUF,
	MAIN_INV,

	MAIN_BUFIF,
	MAIN_NOTIF,

	MAIN_AND,
	MAIN_NAND,
	MAIN_OR,
	MAIN_NOR,
	MAIN_XOR,
	MAIN_XNOR,

	MAIN_MUX,
};

enum CellType
{
	UNDEFTYPE = -1,

	PRESET_X,
	PRESET_1,
	PRESET_0,
	PRESET_U,

	S_IN,
	S_OUT,
	S_OUT_CLK,
	S_OUT_EN,

	P_IN,
	P_OUT,
	P_INOUT,

	BUF,
	INV,

    BUFIF1,
    BUFIF0,
	NOTIF1,
	NOTIF0,

	AND,
	NAND,
	OR,
	NOR,
	XOR,
	XNOR,

	MUX,
};

std::string to_string(const CellType& type);
std::string to_string(const CellCategory& type);
