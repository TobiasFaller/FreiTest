#pragma once

#include <utility>
#include <type_traits>

#include "Basic/Iterator/IndexIteratorBase.hpp"

template<typename T, typename S>
class RawEnumerationIterator: public IndexIteratorBase<T *const>
{
public:
	using value_type = std::conditional_t<std::is_pointer_v<T>, T, std::add_pointer_t<T>>;

	RawEnumerationIterator(T *const container, S index):
		IndexIteratorBase<T *const>(container, index)
	{
	}

	RawEnumerationIterator(const RawEnumerationIterator<T, S>& rhs) = default;
	RawEnumerationIterator(RawEnumerationIterator<T, S>&& rhs) = default;

	RawEnumerationIterator<T, S>& operator=(const RawEnumerationIterator<T, S>& other) = default;
	RawEnumerationIterator<T, S>& operator=(RawEnumerationIterator<T, S>&& other) = default;

	inline std::pair<S, value_type> operator*(void) {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index, this->_container[this->_index] };
		}
		else
		{
			return { this->_index, &(this->_container[this->_index]) };
		}
	}

    inline std::pair<S, value_type> operator->(void) {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index, this->_container[this->_index] };
		}
		else
		{
			return { this->_index, &(this->_container[this->_index]) };
		}
	}

    inline std::pair<S, value_type> operator[](size_t rhs) {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index + rhs, this->_container[this->_index + rhs] };
		}
		else
		{
			return { this->_index + rhs, &(this->_container[this->_index + rhs]) };
		}
	}

    inline const std::pair<S, value_type> operator*(void) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index, this->_container[this->_index] };
		}
		else
		{
			return { this->_index, &(this->_container[this->_index]) };
		}
	}

    inline const std::pair<S, value_type> operator->(void) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index, this->_container[this->_index] };
		}
		else
		{
			return { this->_index, &(this->_container[this->_index]) };
		}
	}

    inline const std::pair<S, value_type> operator[](size_t rhs) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return { this->_index + rhs, this->_container[this->_index + rhs] };
		}
		else
		{
			return { this->_index + rhs, &(this->_container[this->_index + rhs]) };
		}
	}
};

template<typename T, typename S>
class RawEnumerable
{
public:
	RawEnumerable(T *const container, S size): _container(container), _size(size) { }
	RawEnumerable(const RawEnumerable<T, S>& rhs) = default;
	RawEnumerable(RawEnumerable<T, S>&& rhs) = default;

	RawEnumerable<T, S>& operator=(const RawEnumerable<T, S>& other) = default;
	RawEnumerable<T, S>& operator=(RawEnumerable<T, S>&& other) = default;

	RawEnumerationIterator<T, S> begin(void) { return { _container, 0u }; }
	RawEnumerationIterator<const T, S> begin(void) const { return { _container, 0u }; };
	RawEnumerationIterator<const T, S> cbegin(void) const { return { _container, 0u }; };

	RawEnumerationIterator<T, S> end(void) { return { _container, _size }; }
	RawEnumerationIterator<const T, S> end(void) const { return { _container, _size }; };
	RawEnumerationIterator<const T, S> cend(void) const { return { _container, _size }; };

private:
	T *const _container;
	S _size;
};
