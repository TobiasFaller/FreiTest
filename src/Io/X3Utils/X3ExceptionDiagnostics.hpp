#pragma once

#include <boost/spirit/home/x3.hpp>

#include <string>
#include <vector>
#include <type_traits>

#include "Io/X3Utils/X3ExceptionFormatter.hpp"

namespace FreiTest
{
namespace Io
{
namespace X3
{

namespace x3 = boost::spirit::x3;

class with_error_handling
{
public:
	struct diagnostic_info_tag_t;
	struct preceding_lines_tag_t;
	struct following_lines_tag_t;

	template<typename Iterator, typename Context>
	x3::error_handler_result on_error(Iterator begin, Iterator end, x3::expectation_failure<Iterator> const& exception, Context const& context) const
	{
		size_t preceding = 2;
		size_t following = 1;
		if constexpr (!std::is_same_v<decltype(x3::get<preceding_lines_tag_t>(context)), x3::unused_type>)
		{
			preceding = x3::get<preceding_lines_tag_t>(context);
		}
		if constexpr (!std::is_same_v<decltype(x3::get<following_lines_tag_t>(context)), x3::unused_type>)
		{
			following = x3::get<following_lines_tag_t>(context);
		}

		std::string message = FormatX3Exception(begin, end, exception, preceding, following);
		x3::get<diagnostic_info_tag_t>(context).push_back(message);
		return x3::error_handler_result::fail;
	}
};

template <typename T = x3::unused_type>
auto const handle_exception = [](auto p) {
	struct _ : with_error_handling {};
	return x3::rule<_, T> {} = p;
};

};
};
};
