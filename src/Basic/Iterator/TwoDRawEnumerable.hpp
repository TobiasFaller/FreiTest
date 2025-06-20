#pragma once

#include <utility>

#include "Basic/Iterator/IndexIteratorBase.hpp"
#include "Basic/Iterator/RawIterable.hpp"

/**
 * @brief Provides an iterator that is an enumeration of the first dimension of the underlying C-array.
 *
 * The second dimension of the C-array is defined by the sizes parameter that is
 * further handed down to the iterator returned in the pair<.., ..>.
 *
 * @tparam T The underlying type of the C-array.
 * @tparam S The underlying type of the size-array.
 */
template<typename T, typename S>
class TwoDRawEnumerationIterator: public IndexIteratorBase<std::pair<T *const *const, S *const>>
{
public:
	TwoDRawEnumerationIterator(std::pair<T *const *const, S *const> container, size_t index):
		IndexIteratorBase<std::pair<T *const *const, S *const>>(container, index)
	{
	}

	TwoDRawEnumerationIterator(const TwoDRawEnumerationIterator<T, S>& rhs) = default;
	TwoDRawEnumerationIterator(TwoDRawEnumerationIterator<T, S>&& rhs) = default;

	TwoDRawEnumerationIterator<T, S>& operator=(const TwoDRawEnumerationIterator<T, S>& other) = default;
	TwoDRawEnumerationIterator<T, S>& operator=(TwoDRawEnumerationIterator<T, S>&& other) = default;

	inline std::pair<S, RawIterable<T, S>> operator*(void) {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}

    inline std::pair<S, RawIterable<T, S>> operator->(void) {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}

    inline std::pair<S, RawIterable<T, S>> operator[](size_t rhs) {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}

    inline const std::pair<S, RawIterable<T, S>> operator*(void) const {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}

    inline const std::pair<S, RawIterable<T, S>> operator->(void) const {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}

    inline const std::pair<S, RawIterable<T, S>> operator[](size_t rhs) const {
		return {
			this->_index,
			RawIterable<T, S>(this->_container.first[this->_index], this->_container.second[this->_index])
		};
	}
};

/**
 * @brief Provides an interface to an enumerable raw C-array with two dimensions.
 *
 * The first dimension of the C-array is defined by the size parameter.
 * The second dimension of the C-array is defined by the sizes parameter that is
 * further handed down to the enumeration.
 *
 * @tparam T The underlying type of the C-array.
 * @tparam S The underlying type of the size-array.
 */
template<typename T, typename S>
class TwoDRawEnumerable
{
public:
	TwoDRawEnumerable(T *const *const container, S *const sizes, size_t size):
		_container(container),
		_sizes(sizes),
		_size(size)
	{
	}

	TwoDRawEnumerable(const TwoDRawEnumerable<T, S>& rhs) = default;
	TwoDRawEnumerable(TwoDRawEnumerable<T, S>&& rhs) = default;

	TwoDRawEnumerable<T, S>& operator=(const TwoDRawEnumerable<T, S>& other) = default;
	TwoDRawEnumerable<T, S>& operator=(TwoDRawEnumerable<T, S>&& other) = default;

	TwoDRawEnumerationIterator<T, S> begin(void) { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, 0u); }
	const TwoDRawEnumerationIterator<T, S> begin(void) const { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, 0u); };
	const TwoDRawEnumerationIterator<T, S> cbegin(void) const { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, 0u); };

	TwoDRawEnumerationIterator<T, S> end(void) { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, _size); }
	const TwoDRawEnumerationIterator<T, S> end(void) const { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, _size); };
	const TwoDRawEnumerationIterator<T, S> cend(void) const { return TwoDRawEnumerationIterator<T, S>({ _container, _sizes }, _size); };

private:
	T *const *const _container;
	S *const _sizes;
	size_t _size;
};
