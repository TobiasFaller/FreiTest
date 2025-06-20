#pragma once

namespace FreiTest
{
namespace Basic
{

// For visitor creation
template <class... Fs>
struct overload: Fs... {
	overload(Fs const&... fs): Fs { fs }...
	{
	}

	using Fs::operator()...;
};

};
};
