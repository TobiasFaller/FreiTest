#pragma once

#include "Basic/Iterator/IndexIteratorBase.hpp"

template<typename T, typename S>
class RawIterator: public IndexIteratorBase<T *const>
{
public:
	using value_type = std::conditional_t<std::is_pointer_v<T>, T, std::add_pointer_t<T>>;

	RawIterator(T *const container, S index):
		IndexIteratorBase<T *const>(container, index)
	{
	}

	RawIterator(const RawIterator<T, S>& rhs) = default;
	RawIterator(RawIterator<T, S>&& rhs) = default;

	RawIterator<T, S>& operator=(const RawIterator<T, S>& other) = default;
	RawIterator<T, S>& operator=(RawIterator<T, S>&& other) = default;

	inline value_type operator*(void) {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}

    inline value_type operator->(void) {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}

    inline value_type operator[](size_t rhs) {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}

    inline const value_type operator*(void) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}

    inline const value_type operator->(void) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}

    inline const value_type operator[](size_t rhs) const {
		if constexpr (std::is_pointer_v<T>)
		{
			return this->_container[this->_index];
		}
		else
		{
			return &(this->_container[this->_index]);
		}
	}
};

template<typename T, typename S>
class RawIterable
{
public:
	RawIterable(T *const container, S size): _container(container), _size(size) { }
	RawIterable(const RawIterable<T, S>& rhs) = default;
	RawIterable(RawIterable<T, S>&& rhs) = default;

	RawIterable<T, S>& operator=(const RawIterable<T, S>& other) = default;
	RawIterable<T, S>& operator=(RawIterable<T, S>&& other) = default;

	RawIterator<T, S> begin(void) { return { _container, 0u }; }
	RawIterator<const T, S> begin(void) const { return { _container, 0u }; };
	RawIterator<const T, S> cbegin(void) const { return { _container, 0u }; };

	RawIterator<T, S> end(void) { return { _container, _size }; }
	RawIterator<const T, S> end(void) const { return { _container, _size }; };
	RawIterator<const T, S> cend(void) const { return { _container, _size }; };

private:
	T *const _container;
	S _size;
};
