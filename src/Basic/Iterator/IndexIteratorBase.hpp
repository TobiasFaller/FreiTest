#pragma once

#include <iterator>

template<typename Container>
class IndexIteratorBase
{
public:
	using iterator_type = IndexIteratorBase<Container>;
	using iterator_category = std::random_access_iterator_tag;
	using size_type = size_t;
	using difference_type = size_t;

	IndexIteratorBase(Container container, size_t index): _container(container), _index(index) {}
	IndexIteratorBase(const iterator_type& rhs) = default;
	IndexIteratorBase(iterator_type&& rhs) = default;

	iterator_type& operator=(const iterator_type& other) = default;
	iterator_type& operator=(iterator_type&& other) = default;

	iterator_type& operator+=(difference_type rhs) { _index += rhs; return *this; }
	iterator_type& operator-=(difference_type rhs) { _index -= rhs; return *this; }
	iterator_type& operator++() { ++_index; return *this; }
	iterator_type& operator--() { --_index; return *this; }
	iterator_type operator++(int) { return iterator_type(_container, _index++); }
	iterator_type operator--(int) { return iterator_type(_container, _index--); }

	difference_type operator-(const iterator_type& rhs) const { return _index-rhs._index; }
	iterator_type operator+(difference_type rhs) const { return iterator_type(_container, _index+rhs); }
	iterator_type operator-(difference_type rhs) const { return iterator_type(_container, _index-rhs); }
	friend iterator_type operator+(difference_type lhs, const iterator_type& rhs) { return iterator_type(rhs._container, lhs+rhs._index); }
	friend iterator_type operator-(difference_type lhs, const iterator_type& rhs) { return iterator_type(rhs._container, lhs-rhs._index); }

	bool operator==(const iterator_type& rhs) const { return _index == rhs._index; }
	bool operator!=(const iterator_type& rhs) const { return _index != rhs._index; }
	bool operator>(const iterator_type& rhs) const { return _index > rhs._index; }
	bool operator<(const iterator_type& rhs) const { return _index < rhs._index; }
	bool operator>=(const iterator_type& rhs) const { return _index >= rhs._index; }
	bool operator<=(const iterator_type& rhs) const { return _index <= rhs._index; }

protected:
	Container _container;
	size_type _index;

};
