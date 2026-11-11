#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Fault
{

class CellAwareFaultSpecification
{
public:
	struct Port
	{
		std::string name;
		Basic::LogicConstraint constraint;
	};

	struct Transition
	{
		std::vector<Port> inputs;
		std::vector<Port> outputs;
		size_t toState;
		bool accepting;
	};

	struct State
	{
		std::vector<Transition> transitions;
	};

	CellAwareFaultSpecification(std::string name, std::vector<State> states):
		name(name),
		states(states)
	{
	}
	virtual ~CellAwareFaultSpecification(void) = default;

	const std::string& GetName(void) const { return name; }
	const std::vector<State>& GetStates(void) const { return states; }

private:
	std::string name;
	std::vector<State> states;

};

class CellAwareCellSpecification
{
	struct name_tag {};
	using Faults = boost::multi_index_container<
		CellAwareFaultSpecification,
		boost::multi_index::indexed_by<
			boost::multi_index::random_access<>,
			boost::multi_index::hashed_unique<
				boost::multi_index::tag<name_tag>,
				boost::multi_index::const_mem_fun<CellAwareFaultSpecification, std::string const&, &CellAwareFaultSpecification::GetName>
			>
		>
	>;

public:
	CellAwareCellSpecification(std::string name, std::vector<CellAwareFaultSpecification> faults);
	virtual ~CellAwareCellSpecification(void) = default;

	const Faults& GetFaults(void) const { return faults; }
	const std::string& GetName(void) const { return name; }

	bool HasFault(const std::string& name) const;
	const CellAwareFaultSpecification& GetFault(const std::string& name) const;

	Faults::const_iterator begin(void) const { return faults.begin(); }
	Faults::const_iterator end(void) const { return faults.end(); }

private:
	std::string name;
	Faults faults;

};

class CellAwareModelSpecification
{
	struct name_tag {};
	using Cells = boost::multi_index_container<
		CellAwareCellSpecification,
		boost::multi_index::indexed_by<
			boost::multi_index::random_access<>,
			boost::multi_index::hashed_unique<
				boost::multi_index::tag<name_tag>,
				boost::multi_index::const_mem_fun<CellAwareCellSpecification, std::string const&, &CellAwareCellSpecification::GetName>
			>
		>
	>;

public:
	CellAwareModelSpecification(std::vector<CellAwareCellSpecification> cells);
	virtual ~CellAwareModelSpecification(void) = default;

	const Cells& GetCells(void) const { return cells; }

	bool HasCell(const std::string& name) const;
	bool HasFault(const std::string& cellName, const std::string& faultName) const;
	const CellAwareCellSpecification& GetCell(const std::string& name) const;
	const CellAwareFaultSpecification& GetFault(const std::string& cellName, const std::string& faultName) const;

	Cells::const_iterator begin(void) const { return cells.begin(); }
	Cells::const_iterator end(void) const { return cells.end(); }

private:
	Cells cells;

};

std::string to_string(const CellAwareFaultSpecification& specification);
std::string to_string(const CellAwareCellSpecification& specification);
std::string to_string(const CellAwareModelSpecification& specification);

};
};
