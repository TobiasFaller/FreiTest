#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <tuple>
#include <type_traits>

namespace FreiTest
{
namespace Basic
{
namespace Container
{

template<typename... T>
class ConcurrentList;

template<typename ContainerT>
class ConcurrentForwardIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using iterator_type = ConcurrentForwardIterator<ContainerT>;
	using size_type = typename ContainerT::size_type;
	using difference_type = typename ContainerT::difference_type;

	using value_type = typename ContainerT::value_type;
	using const_value_type = typename ContainerT::const_value_type;
	using reference = typename ContainerT::reference;
	using const_reference = typename ContainerT::const_reference;
	using pointer = typename ContainerT::pointer;
	using const_pointer = typename ContainerT::const_pointer;

	ConcurrentForwardIterator(std::add_pointer_t<ContainerT> container, size_t index):
		_container(container),
		_index(index)
	{
	}

	ConcurrentForwardIterator(const iterator_type& rhs) = default;
	ConcurrentForwardIterator(iterator_type&& rhs) = default;
	virtual ~ConcurrentForwardIterator(void) = default;

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

	value_type operator*(void) { return (*this->_container)[this->_index]; };
	value_type operator->(void) { return (*this->_container)[this->_index]; };
	value_type operator[](difference_type rhs) { return (*this->_container)[this->_index]; };

	const value_type operator*(void) const { return (*this->_container)[this->_index]; };
	const value_type operator->(void) const { return (*this->_container)[this->_index]; };
	const value_type operator[](difference_type rhs) const { return (*this->_container)[this->_index]; };

private:
	std::add_pointer_t<ContainerT> _container;
	size_type _index;

};

template<typename ContainerT>
class ConcurrentBackwardIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using iterator_type = ConcurrentBackwardIterator<ContainerT>;
	using size_type = typename ContainerT::size_type;
	using difference_type = typename ContainerT::difference_type;

	using value_type = typename ContainerT::value_type;
	using const_value_type = typename ContainerT::const_value_type;
	using reference = typename ContainerT::reference;
	using const_reference = typename ContainerT::const_reference;
	using pointer = typename ContainerT::pointer;
	using const_pointer = typename ContainerT::const_pointer;

	ConcurrentBackwardIterator(std::add_pointer_t<ContainerT> container, size_t index):
		_container(container),
		_index(index)
	{
	}

	ConcurrentBackwardIterator(const iterator_type& rhs) = default;
	ConcurrentBackwardIterator(iterator_type&& rhs) = default;
	virtual ~ConcurrentBackwardIterator(void) = default;

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

	reference operator*(void) { return (*this->_container)[this->_container->size() - this->_index - 1u]; };
	reference operator->(void) { return (*this->_container)[this->_container->size() - this->_index - 1u]; };
	reference operator[](difference_type rhs) { return (*this->_container)[this->_container->size() - this->_index - 1u]; };

	const_reference operator*(void) const { return (*this->_container)[this->_container->size() - this->_index - 1u]; };
	const_reference operator->(void) const { return (*this->_container)[this->_container->size() - this->_index - 1u]; };
	const_reference operator[](difference_type rhs) const { return (*this->_container)[this->_container->size() - this->_index - 1u]; };

private:
	std::add_pointer_t<ContainerT> _container;
	size_type _index;

};

template<typename... T>
class ConcurrentList
{
private:
	struct wrap_into_shared
	{
		template<typename V> std::shared_ptr<V> operator()(V param);
		template<typename... V> std::tuple<std::shared_ptr<V>...> operator()(V... param);
	};

	template<typename... U>
	using wrap_into_shared_t = std::invoke_result_t<wrap_into_shared, U...>;

public:
	using container_type = ConcurrentList<T...>;
	using size_type = size_t;
	using index_type = size_t;
	using difference_type = size_t;

	using value_type = wrap_into_shared_t<T...>;
	using const_value_type = std::add_const_t<value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_const_t<reference>;
	using pointer = std::add_pointer_t<value_type>;
	using const_pointer = std::add_const_t<pointer>;

	using iterator = ConcurrentForwardIterator<container_type>;
	using const_iterator = ConcurrentForwardIterator<std::add_const_t<container_type>>;
	using reverse_iterator = ConcurrentBackwardIterator<container_type>;
	using const_reverse_iterator = ConcurrentBackwardIterator<std::add_const_t<container_type>>;

	ConcurrentList(void) = default;
	ConcurrentList(const container_type& other):
		_mutex(), _elements()
	{
		std::scoped_lock lock { other._mutex };
		_elements = other._elements;
	}
	ConcurrentList(container_type&& other):
		_mutex(), _elements()
	{
		std::scoped_lock lock { other._mutex };
		_elements = std::move(other._elements);
	}
	virtual ~ConcurrentList(void) = default;

	container_type& operator=(const container_type& other)
	{
		std::scoped_lock lock { _mutex, other._mutex };
		_elements = other._elements;
		return *this;
	}
	container_type& operator=(container_type&& other)
	{
		std::scoped_lock lock { _mutex, other._mutex };
		_elements = std::move(other._elements);
		return *this;
	}

	size_t size(void) const { std::scoped_lock lock { _mutex }; return _elements.size(); }
	void clear(void) { std::scoped_lock lock { _mutex }; _elements.clear(); }
	void swap(container_type& other) { std::scoped_lock lock { _mutex, other._mutex }; std::swap(_elements, other._elements); }

	reference operator[](index_type index) { std::scoped_lock lock { _mutex }; return _elements[index]; }
	const_reference operator[](index_type index) const { std::scoped_lock lock { _mutex }; return _elements[index]; }

	reference front(void) { return (*this)[0u]; }
	reference back(void) { return (*this)[this->size()]; }
	const_reference front(void) const { return (*this)[0u]; }
	const_reference back(void) const { return (*this)[this->size()]; }

	iterator begin(void) { return iterator(this, 0u); }
	iterator end(void) { return iterator(this, this->size()); }
	const_iterator begin(void) const { return const_iterator(this, 0u); }
	const_iterator end(void) const { return const_iterator(this, this->size()); }

	reverse_iterator rbegin(void) { return reverse_iterator(this, 0u); }
	reverse_iterator rend(void) { return reverse_iterator(this, this->size()); }
	const_reverse_iterator rbegin(void) const { return const_reverse_iterator(this, 0u); }
	const_reverse_iterator rend(void) const { return const_reverse_iterator(this, this->size()); }

	index_type push_back(value_type container)
	{
		std::scoped_lock lock { _mutex };

		size_t index = _elements.size();
		_elements.emplace_back(container);
		return index;
	}
	index_type emplace_back(T... values)
	{
		std::scoped_lock lock { _mutex };

		size_t index = _elements.size();

		if constexpr (sizeof...(T) == 1)
		{
			_elements.emplace_back(
				std::make_shared<T>(std::forward<T>(values))...
			);
		}
		else
		{
			_elements.emplace_back(std::make_tuple<std::shared_ptr<T>...>(
				std::make_shared<T>(std::forward<T>(values))...
			));
		}
		return index;
	}
	index_type emplace_back(std::shared_ptr<T>... values)
	{
		std::scoped_lock lock { _mutex };

		size_t index = _elements.size();

		if constexpr (sizeof...(T) == 1)
		{
			_elements.emplace_back(std::forward<std::shared_ptr<T>>(values)...);
		}
		else
		{
			_elements.emplace_back(std::make_tuple<std::shared_ptr<T>...>(std::forward<std::shared_ptr<T>>(values)...));
		}
		return index;
	}

protected:
	mutable std::mutex _mutex;
	mutable std::vector<value_type> _elements;

};

};
};
};
