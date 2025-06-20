#include "Io/VerilogSpiritParser/VerilogGrammar.hpp"

#include <boost/spirit/home/x3.hpp>

#include <string>
#include <istream>

#include "Io/VerilogSpiritParser/VerilogGrammarImpl.hpp"

namespace FreiTest {
namespace Io {
namespace Verilog {
namespace Grammar {

    namespace spirit = boost::spirit;
	namespace x3 = boost::spirit::x3;

    namespace Comment {
        BOOST_SPIRIT_INSTANTIATE(skipper_type, test_iterator_type, x3::unused_type);

        BOOST_SPIRIT_INSTANTIATE(skipper_type, iterator_type, x3::unused_type);
    }
    namespace Generic {
        BOOST_SPIRIT_INSTANTIATE(quoted_string_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(identifier_type, test_iterator_type, test_context_type);
    }
    namespace Vector {
        BOOST_SPIRIT_INSTANTIATE(vector_size_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(vector_initializer_type, test_iterator_type, test_context_type);
    }
    namespace Port {
        BOOST_SPIRIT_INSTANTIATE(type_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(name_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(port_type, test_iterator_type, test_context_type);
    }
    namespace Parameter {
        BOOST_SPIRIT_INSTANTIATE(value_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(parameter_type, test_iterator_type, test_context_type);
    }
    namespace Attribute {
        BOOST_SPIRIT_INSTANTIATE(name_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(value_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(attribute_type, test_iterator_type, test_context_type);
    }
    namespace Wire {
        BOOST_SPIRIT_INSTANTIATE(name_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(wire_type, test_iterator_type, test_context_type);
    }
    namespace WireReference {
		BOOST_SPIRIT_INSTANTIATE(name_type, test_iterator_type, test_context_type);
		BOOST_SPIRIT_INSTANTIATE(wire_reference_type, test_iterator_type, test_context_type);
    }
    namespace Bus {
        BOOST_SPIRIT_INSTANTIATE(bus_type, test_iterator_type, test_context_type);
    }
    namespace Assignment {
        BOOST_SPIRIT_INSTANTIATE(assignment_type, test_iterator_type, test_context_type);
    }
    namespace Instantiation {
        BOOST_SPIRIT_INSTANTIATE(parameter_list_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(port_list_type, test_iterator_type, test_context_type);
        BOOST_SPIRIT_INSTANTIATE(instantiation_type, test_iterator_type, test_context_type);
    }
    namespace Module {
        BOOST_SPIRIT_INSTANTIATE(module_type, test_iterator_type, test_context_type);
    }
    namespace Source {
        BOOST_SPIRIT_INSTANTIATE(source_type, test_iterator_type, test_context_type);

        BOOST_SPIRIT_INSTANTIATE(source_type, iterator_type, context_type);
    }

    const Grammar::Comment::skipper_type& skipper(void)
    {
        return Grammar::Comment::skipper_impl;
    }

    const Grammar::Generic::quoted_string_type& quoted_string(void)
    {
        return Grammar::Generic::quoted_string_impl;
    }

    const Grammar::Generic::identifier_type& identifier(void)
    {
        return Grammar::Generic::identifier_impl;
    }

    const Grammar::Vector::vector_size_type& vector_size(void)
    {
        return Grammar::Vector::size_impl;
    }

    const Grammar::Vector::vector_initializer_type& vector_initializer(void)
    {
        return Grammar::Vector::initializer_impl;
    }

	const Grammar::WireReference::wire_reference_type& wire_reference(void)
	{
		return Grammar::WireReference::wire_reference_impl;
	}

	const Grammar::Bus::bus_type& bus(void)
	{
		return Grammar::Bus::bus_impl;
	}

	const Grammar::Attribute::attribute_type& attribute(void)
	{
		return Grammar::Attribute::attribute_impl;
	}

	const Grammar::Port::name_type& port_name(void)
	{
		return Grammar::Port::name_impl;
	}

	const Grammar::Port::type_type& port_type(void)
	{
		return Grammar::Port::type_impl;
	}

	const Grammar::Port::port_type& port_statement(void)
	{
		return Grammar::Port::port_impl;
	}

	const Grammar::Parameter::value_type& parameter_value(void)
	{
		return Grammar::Parameter::value_impl;
	}

	const Grammar::Parameter::parameter_type& parameter_statement(void)
	{
		return Grammar::Parameter::parameter_impl;
	}

	const Grammar::Wire::wire_type& wire_statement(void)
	{
		return Grammar::Wire::wire_impl;
	}

	const Grammar::Assignment::assignment_type& assignment_statement(void)
	{
		return Grammar::Assignment::assignment_impl;
	}

	const Grammar::Instantiation::instantiation_type& module_instantiation(void)
	{
		return Grammar::Instantiation::instantiation_impl;
	}

	const Grammar::Module::module_type& verilog_module(void)
	{
		return Grammar::Module::module_impl;
	}

	const Grammar::Source::source_type& verilog_source(void)
	{
		return Grammar::Source::module_list_impl;
	}

};
};
};
};
