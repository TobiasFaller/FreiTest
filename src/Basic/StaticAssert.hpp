#pragma once

namespace FreiTest
{

template<typename Type>
constexpr void NotImplemented(void)
{
	// Ugly hack to make static_assert dependent on the type used in LogicContainer template parameter
	static_assert(static_cast<int>(sizeof(Type)) < 0u, "This method has not been implemented for this type.");
}

};
