#include "Circuit/CellLibrary.hpp"

std::string to_string(const CellType& type)
{
	switch (type)
	{
	case CellType::PRESET_X: return "PRESET_X";
	case CellType::PRESET_1: return "PRESET_1";
	case CellType::PRESET_0: return "PRESET_0";
	case CellType::PRESET_U: return "PRESET_U";

	case CellType::S_IN: return "S_IN";
	case CellType::S_OUT: return "S_OUT";
	case CellType::S_OUT_CLK: return "S_OUT_CLK";
	case CellType::S_OUT_EN: return "S_OUT_EN";

	case CellType::P_IN: return "P_IN";
	case CellType::P_OUT: return "P_OUT";

	case CellType::BUF: return "BUF";
	case CellType::INV: return "INV";

    case CellType::BUFIF1: return "BUFIF1";
    case CellType::BUFIF0: return "BUFIF0";
	case CellType::NOTIF1: return "NOTIF1";
	case CellType::NOTIF0: return "NOTIF0";

	case CellType::AND: return "AND";
	case CellType::NAND: return "NAND";
	case CellType::OR: return "OR";
	case CellType::NOR: return "NOR";
	case CellType::XOR: return "XOR";
	case CellType::XNOR: return "XNOR";

	case CellType::MUX: return "MUX";

	default: return "UNDEFTYPE";
	}
}

std::string to_string(const CellCategory& type)
{
	switch (type)
	{
	case CellCategory::MAIN_CONSTANT: return "MAIN_CONSTANT";

	case CellCategory::MAIN_IN: return "MAIN_IN";
	case CellCategory::MAIN_OUT: return "MAIN_OUT";

	case CellCategory::MAIN_BUF: return "MAIN_BUF";
	case CellCategory::MAIN_INV: return "MAIN_INV";

	case CellCategory::MAIN_BUFIF: return "MAIN_BUFIF";
	case CellCategory::MAIN_NOTIF: return "MAIN_NOTIF";

	case CellCategory::MAIN_AND: return "MAIN_AND";
	case CellCategory::MAIN_NAND: return "MAIN_NAND";
	case CellCategory::MAIN_OR: return "MAIN_OR";
	case CellCategory::MAIN_NOR: return "MAIN_NOR";
	case CellCategory::MAIN_XOR: return "MAIN_XOR";
	case CellCategory::MAIN_XNOR: return "MAIN_XNOR";

	case CellCategory::MAIN_MUX: return "MAIN_MUX";

	default: return "MAIN_UNKNOWN";
	}
}
