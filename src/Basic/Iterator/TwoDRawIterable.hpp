#pragma once

#include <utility>

#include "Basic/Iterator/IndexIteratorBase.hpp"
#include "Basic/Iterator/RawIterable.hpp"

/**
 * @brief Provides an iterator of the first dimension of the underlying C-array.
 *
 * The second dimension of the C-array is defined by the sizes parameter that is
 * further handed down to the iterator returned.
 *
 * @tparam T The underlying type of the C-array.
 * @tparam S The underlying type of the size-array.
 */
template<typename T, typename S>
class TwoDRawIterator: public IndexIteratorBase<std::pair<T *const *const, S *const>>
{
public:
	TwoDRawIterator(std::pair<T *const *const, S *const> container, size_t index):
		IndexIteratorBase<std::pair<T *const *const, S *const>>(container, index)
	{
	}

	TwoDRawIterator(const TwoDRawIterator<T, S>& rhs) = default;
	TwoDRawIterator(TwoDRawIterator<T, S>&& rhs) = default;

	TwoDRawIterator<T, S>& operator=(const TwoDRawIterator<T, S>& other) = default;
	TwoDRawIterator<T, S>& operator=(TwoDRawIterator<T, S>&& other) = default;

	inline RawIterable<T, S> operator*(void) {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}

    inline RawIterable<T, S> operator->(void) {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}

    inline RawIterable<T, S> operator[](size_t rhs) {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}

    inline const RawIterable<T, S> operator*(void) const {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}

    inline const RawIterable<T, S> operator->(void) const {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}

    inline const RawIterable<T, S> operator[](size_t rhs) const {
		return RawIterable<T, S>(
			this->_container.first[this->_index],
			this->_container.second[this->_index]
		);
	}
};

/**
 * @brief Provides an interface to an iterable raw C-array with two dimensions.
 *
 * The first dimension of the C-array is defined by the size parameter.
 * The second dimension of the C-array is defined by the sizes parameter that is
 * further handed down to the enumeration.
 *
 * @tparam T The underlying type of the C-array.
 * @tparam S The underlying type of the size-array.
 */
template<typename T, typename S>
class TwoDRawIterable
{
public:
	TwoDRawIterable(T *const *const container, S *const sizes, size_t size):
		_container(container),
		_sizes(sizes),
		_size(size)
	{
	}

	TwoDRawIterable(const TwoDRawIterable<T, S>& rhs) = default;
	TwoDRawIterable(TwoDRawIterable<T, S>&& rhs) = default;

	TwoDRawIterable<T, S>& operator=(const TwoDRawIterable<T, S>& other) = default;
	TwoDRawIterable<T, S>& operator=(TwoDRawIterable<T, S>&& other) = default;

	TwoDRawIterator<T, S> begin(void) { return TwoDRawIterator<T, S>({ _container, _sizes }, 0u); }
	const TwoDRawIterator<T, S> begin(void) const { return TwoDRawIterator<T, S>({ _container, _sizes }, 0u); };
	const TwoDRawIterator<T, S> cbegin(void) const { return TwoDRawIterator<T, S>({ _container, _sizes }, 0u); };

	TwoDRawIterator<T, S> end(void) { return TwoDRawIterator<T, S>({ _container, _sizes }, _size); }
	const TwoDRawIterator<T, S> end(void) const { return TwoDRawIterator<T, S>({ _container, _sizes }, _size); };
	const TwoDRawIterator<T, S> cend(void) const { return TwoDRawIterator<T, S>({ _container, _sizes }, _size); };

private:
	T *const *const _container;
	S *const _sizes;
	size_t _size;
};
