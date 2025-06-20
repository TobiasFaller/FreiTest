#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/variant.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <map>

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

	namespace multi_index = boost::multi_index;

	struct VectorSize;
	struct VectorInitializer;
	struct VectorPlaceholder;
	struct Attribute;
	struct Port;
	struct Parameter;
	struct Wire;
	struct WireReference;
	struct Bus;
	struct Assignment;
	struct ParameterMapping;
	struct PortMapping;
	struct Instantiation;
	struct Module;

	enum class PortType: uint64_t;

	struct sequence_index_tag {};
	struct name_index_tag {};

	using AttributeValue = boost::variant<std::string, int64_t, VectorInitializer>;
	using ParameterValue = boost::variant<int64_t, VectorInitializer>;
	using BusValue = boost::variant<VectorInitializer, WireReference, VectorPlaceholder>;

	using PortList = std::vector<Verilog::Port>;
	using ParameterList = std::vector<Verilog::Parameter>;
	using ParameterMappingList = std::vector<Verilog::ParameterMapping>;
	using PortMappingList = std::vector<Verilog::PortMapping>;

	std::string GetAttributeName(const Attribute& attribute);
	std::string GetPortName(const Port& port);
	std::string GetParameterName(const Parameter& paremeter);
	std::string GetWireName(const Wire& wire);
	std::string GetInstantiationName(const Instantiation& wire);
	std::string GetParameterMappingName(const ParameterMapping& mapping);
	std::string GetPortMappingName(const PortMapping& mapping);
	std::string GetModuleName(const Module& module);

	using AttributeCollection = boost::multi_index_container<
		Attribute,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Attribute&, std::string, &GetAttributeName>
			>
		>
	>;
	using PortCollection = boost::multi_index_container<
		Port,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Port&, std::string, &GetPortName>
			>
		>
	>;
	using ParameterCollection = boost::multi_index_container<
		Parameter,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Parameter&, std::string, &GetParameterName>
			>
		>
	>;
	using WireCollection = boost::multi_index_container<
		Wire,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Wire&, std::string, &GetWireName>
			>
		>
	>;
	using AssignmentCollection = std::vector<Assignment>;
	using InstantiationCollection = boost::multi_index_container<
		Instantiation,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Instantiation&, std::string, &GetInstantiationName>
			>
		>
	>;
	using ParameterMappingCollection = boost::multi_index_container<
		ParameterMapping,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const ParameterMapping&, std::string, &GetParameterMappingName>
			>
		>
	>;
	using PortMappingCollection = boost::multi_index_container<
		PortMapping,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const PortMapping&, std::string, &GetPortMappingName>
			>
		>
	>;
	using ModuleCollection = boost::multi_index_container<
		Module,
		multi_index::indexed_by<
			multi_index::random_access<>,
			multi_index::hashed_non_unique<
				multi_index::tag<name_index_tag>,
				multi_index::global_fun<const Module&, std::string, &GetModuleName>
			>
		>
	>;

	enum class PortType: uint64_t {
		PORT_IN,
		PORT_OUT,
		PORT_INOUT,
		PORT_UNKNOWN
	};

	struct VectorSize {
		VectorSize(void);
		VectorSize(const VectorSize& other) = default;
		VectorSize(VectorSize&& other) = default;
		VectorSize(uint64_t top, uint64_t bottom);

		uint64_t top;
		uint64_t bottom;
		bool unknown;

		bool operator==(const VectorSize& other) const;
		VectorSize& operator=(const VectorSize& other) = default;
		VectorSize& operator=(VectorSize&& other) = default;

		friend std::string to_string(const VectorSize& size);
		friend std::ostream& operator<<(std::ostream& out, const VectorSize& size);

		uint64_t size(void) const;
		uint64_t min(void) const;
		uint64_t max(void) const;
	};

	enum class VectorBit: uint8_t {
		BIT_0 = '0',
		BIT_1 = '1',
		BIT_X = 'X',
		BIT_U = 'U'
	};

	std::string to_string(const VectorBit& bit);
	std::ostream& operator<<(std::ostream& out, const VectorBit& bit);

	struct VectorInitializer {
		VectorInitializer(void);
		VectorInitializer(const VectorInitializer& other) = default;
		VectorInitializer(VectorInitializer&& other) = default;
		VectorInitializer(std::vector<VectorBit> value);
		VectorInitializer(uint64_t bits, uint64_t value);

		std::vector<VectorBit> value;

		size_t size(void) const;
		VectorBit operator[](size_t index) const;
		bool operator==(const VectorInitializer& other) const;
		VectorInitializer& operator=(const VectorInitializer& other) = default;
		VectorInitializer& operator=(VectorInitializer&& other) = default;

		friend std::string to_string(const VectorInitializer& size);
		friend std::ostream& operator<<(std::ostream& out, const VectorInitializer& size);
	};
	struct VectorPlaceholder {
		VectorPlaceholder(void);
		VectorPlaceholder(const VectorPlaceholder& other) = default;
		VectorPlaceholder(VectorPlaceholder&& other) = default;

		bool operator==(const VectorPlaceholder& other) const;
		VectorPlaceholder& operator=(const VectorPlaceholder& other) = default;
		VectorPlaceholder& operator=(VectorPlaceholder&& other) = default;

		friend std::string to_string(const VectorPlaceholder& placeholder);
		friend std::ostream& operator<<(std::ostream& out, const VectorPlaceholder& placeholder);
	};

	struct Attribute {
		Attribute(void);
        Attribute(const Attribute& other) = default;
        Attribute(Attribute&& other) = default;
        Attribute(std::string name, std::string value);
        Attribute(std::string name, uint64_t value);
        Attribute(std::string name, VectorInitializer value);

		std::string name;
		AttributeValue value;

		bool operator==(const Attribute& other) const;
		Attribute& operator=(const Attribute& other) = default;
		Attribute& operator=(Attribute&& other) = default;

		friend std::string to_string(const Attribute& attribute);
		friend std::ostream& operator<<(std::ostream& out, const Attribute& attribute);
	};

	struct Port {
		Port(void);
    	Port(const Port& other) = default;
    	Port(Port&& other) = default;
        Port(std::string name, PortType type, VectorSize size);

		std::string name;
		PortType type;
		VectorSize size;
		AttributeCollection attributes;

		bool operator==(const Port& other) const;
		Port& operator=(const Port& other) = default;
		Port& operator=(Port&& other) = default;

		friend std::string to_string(const Port& port);
		friend std::ostream& operator<<(std::ostream& out, const Port& port);
	};

	struct Parameter {
		Parameter(void);
    	Parameter(const Parameter& other) = default;
    	Parameter(Parameter&& other) = default;
		Parameter(std::string name, int64_t value);
		Parameter(std::string name, VectorInitializer value);

		std::string name;
		ParameterValue value;

		bool operator==(const Parameter& other) const;
		Parameter& operator=(const Parameter& other) = default;
		Parameter& operator=(Parameter&& other) = default;

		friend std::string to_string(const Parameter& port);
		friend std::ostream& operator<<(std::ostream& out, const Parameter& port);
	};

	struct Wire {
		Wire(void);
    	Wire(const Wire& other) = default;
    	Wire(Wire&& other) = default;
		Wire(std::string name, VectorSize size);

		std::string name;
		VectorSize size;
		AttributeCollection attributes;

		bool operator==(const Wire& other) const;
		Wire& operator=(const Wire& other) = default;
		Wire& operator=(Wire&& other) = default;

		friend std::string to_string(const Wire& port);
		friend std::ostream& operator<<(std::ostream& out, const Wire& port);
	};

	struct WireReference {
		WireReference(void);
    	WireReference(const WireReference& other) = default;
    	WireReference(WireReference&& other) = default;
		WireReference(std::string name, VectorSize size);
		WireReference(std::string name, uint64_t index);

		std::string name;
		VectorSize size;

		bool operator==(const WireReference& other) const;
		WireReference& operator=(const WireReference& other) = default;
		WireReference& operator=(WireReference&& other) = default;

		friend std::string to_string(const WireReference& port);
		friend std::ostream& operator<<(std::ostream& out, const WireReference& port);
	};

	struct Bus {
		Bus(void);
		Bus(const Bus& other) = default;
		Bus(Bus&& other) = default;
		Bus(std::vector<BusValue> sources);

		std::vector<BusValue> sources;

		bool operator==(const Bus& other) const;
		Bus& operator=(const Bus& other) = default;
		Bus& operator=(Bus&& other) = default;

		friend std::string to_string(const Bus& port);
		friend std::ostream& operator<<(std::ostream& out, const Bus& port);
	};

	struct Assignment {
		Assignment(void);
    	Assignment(const Assignment& other) = default;
    	Assignment(Assignment&& other) = default;
		Assignment(Bus source, Bus target);

		Bus source;
		Bus target;
		AttributeCollection attributes;

		bool operator==(const Assignment& other) const;
		Assignment& operator=(const Assignment& other) = default;
		Assignment& operator=(Assignment&& other) = default;

		friend std::string to_string(const Assignment& assignment);
		friend std::ostream& operator<<(std::ostream& out, const Assignment& assignment);
	};

	struct ParameterMapping {
		ParameterMapping(void);
		ParameterMapping(const ParameterMapping& other) = default;
		ParameterMapping(ParameterMapping&& other) = default;
		ParameterMapping(std::string name, ParameterValue value);

		std::string name;
		ParameterValue value;

		bool operator==(const ParameterMapping& other) const;
		ParameterMapping& operator=(const ParameterMapping& other) = default;
		ParameterMapping& operator=(const VectorInitializer& other);
		ParameterMapping& operator=(const int64_t& other);
		ParameterMapping& operator=(ParameterMapping&& other) = default;

		friend std::string to_string(const ParameterMapping& assignment);
		friend std::ostream& operator<<(std::ostream& out, const ParameterMapping& assignment);
	};

	struct PortMapping {
		PortMapping(void);
		PortMapping(const PortMapping& other) = default;
		PortMapping(PortMapping&& other) = default;
		PortMapping(std::string name, Bus bus);

		std::string name;
		Bus bus;

		bool operator==(const PortMapping& other) const;
		PortMapping& operator=(const PortMapping& other) = default;
		PortMapping& operator=(PortMapping&& other) = default;

		friend std::string to_string(const PortMapping& assignment);
		friend std::ostream& operator<<(std::ostream& out, const PortMapping& assignment);
	};

	struct Instantiation {
		Instantiation(void);
		Instantiation(const Instantiation& other) = default;
		Instantiation(Instantiation&& other) = default;
		Instantiation(std::string name, std::string type);
		Instantiation(std::string name, std::string type, ParameterMappingList parameter, PortMappingList ports);

		std::string name;
		std::string type;

		ParameterMappingCollection parameter;
		PortMappingCollection ports;
		AttributeCollection attributes;

		bool operator==(const Instantiation& other) const;
		Instantiation& operator=(const Instantiation& other) = default;
		Instantiation& operator=(Instantiation&& other) = default;

		friend std::string to_string(const Instantiation& assignment);
		friend std::ostream& operator<<(std::ostream& out, const Instantiation& assignment);
	};

	struct Module {
		Module(void);
        Module(const Module& other) = default;
        Module(Module&& other) = default;
        Module(std::string name);
        Module(std::string name, std::vector<Port> ports);

		std::string name;
		bool libraryModule;

		PortCollection ports;
		WireCollection wires;
		ParameterCollection parameter;
		AssignmentCollection assignments;
		InstantiationCollection instantiations;
		AttributeCollection attributes;

		bool operator==(const Module& other) const;
		Module& operator=(const Module& other) = default;
		Module& operator=(Module&& other) = default;

		friend std::string to_string(const Module& module);
		friend std::ostream& operator<<(std::ostream& out, const Module& module);
	};

	std::string to_string(const VectorSize& size);
	std::string to_string(const VectorInitializer& size);
	std::string to_string(const Attribute& attribute);
	std::string to_string(const Port& port);
	std::string to_string(const Parameter& port);
	std::string to_string(const Wire& port);
	std::string to_string(const WireReference& port);
	std::string to_string(const Bus& port);
	std::string to_string(const Assignment& assignment);
	std::string to_string(const ParameterMapping& assignment);
	std::string to_string(const PortMapping& assignment);
	std::string to_string(const Instantiation& assignment);
	std::string to_string(const Module& module);

	std::ostream& operator<<(std::ostream& out, const PortType& type);
    std::ostream& operator<<(std::ostream& out, const std::vector<ParameterMapping>& list);
	std::ostream& operator<<(std::ostream& out, const std::vector<PortMapping>& list);

	std::string to_string(const PortType& type);
	std::string to_string(const std::vector<ParameterMapping>& list);
	std::string to_string(const std::vector<PortMapping>& list);

};
};
};
