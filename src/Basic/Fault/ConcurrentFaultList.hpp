#pragma once

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>
#include <utility>

#include "Basic/Container/ConcurrentList.hpp"

namespace FreiTest
{
namespace Fault
{

constexpr size_t REMOVED_FAULT = std::numeric_limits<size_t>::max();

enum class FilterPatternType {
	Normal, Inverted
};

template<typename FaultT, typename MetaDataT>
class ConcurrentFaultList: public Basic::Container::ConcurrentList<FaultT, MetaDataT>
{
public:
	using fault_type = FaultT;
	using metadata_type = MetaDataT;

	using container_type = ConcurrentFaultList<FaultT, MetaDataT>;
	using size_type = typename container_type::size_type;
	using index_type = typename container_type::index_type;
	using difference_type = typename container_type::difference_type;

	using value_type = typename container_type::value_type;
	using const_value_type = typename container_type::const_value_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;

	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	ConcurrentFaultList(void):
		Basic::Container::ConcurrentList<FaultT, MetaDataT>()
	{
	}

	ConcurrentFaultList(const container_type& other):
		Basic::Container::ConcurrentList<FaultT, MetaDataT>(other)
	{
	}

	ConcurrentFaultList(container_type&& other):
		Basic::Container::ConcurrentList<FaultT, MetaDataT>(other)
	{
	}

	ConcurrentFaultList(const std::vector<FaultT>& other):
		Basic::Container::ConcurrentList<FaultT, MetaDataT>()
	{
		this->_elements.reserve(other.size());
		std::transform(other.begin(), other.end(), std::back_inserter(this->_elements), [](const auto& fault) {
			return std::make_tuple(std::make_shared<FaultT>(fault), std::make_shared<MetaDataT>());
		});
	}

	virtual ~ConcurrentFaultList(void) = default;


	container_type& operator=(const container_type& other)
	{
		Basic::Container::ConcurrentList<FaultT, MetaDataT>::operator=(other);
		return *this;
	}

	container_type& operator=(container_type&& other)
	{
		Basic::Container::ConcurrentList<FaultT, MetaDataT>::operator=(other);
		return *this;
	}

	std::shared_ptr<FaultT> GetFault(index_type index) const
	{
		std::scoped_lock lock { this->_mutex };
		return std::get<0>(this->_elements[index]);
	}

	std::shared_ptr<MetaDataT> GetMetadata(index_type index) const
	{
		std::scoped_lock lock { this->_mutex };
		return std::get<1>(this->_elements[index]);
	}

};

};
};
