#pragma once

#include <iterator>
#include <vector>
#include <utility>

namespace SolverProxy
{

template<typename T>
class ModernClauseDB;

template<typename T>
class Clause
{
public:
	typedef T*									iterator;
	typedef const T*							const_iterator;

	typedef T&									reference;
	typedef const T&							const_reference;
	typedef T*									pointer;
	typedef const T*							const_pointer;

	Clause(ModernClauseDB<T>& database, size_t index): _database(database), _index(index) {}
	Clause(const Clause<T>& rhs) = default;
	Clause(Clause<T>&& rhs) = default;

	inline Clause<T>& operator=(const Clause<T>& other) = default;
	inline Clause<T>& operator=(Clause<T>&& other) = default;

	inline iterator begin(void) { return &(_database._clauses[_database._clauseStartIndex[_index]]); };
	inline iterator end(void) { return &(_database._clauses[_database._clauseStartIndex[_index + 1u]]); };

	inline const_iterator begin(void) const { return &(_database._clauses[_database._clauseStartIndex[_index]]); };
	inline const_iterator end(void) const { return &(_database._clauses[_database._clauseStartIndex[_index + 1u]]); };

    inline reference operator*(void) { return _database._clauses[_database._clauseStartIndex[_index]]; }
    inline pointer operator->(void) { return &(_database._clauses[_database._clauseStartIndex[_index]]); }
	inline reference operator[](size_t index) { return _database._clauses[_database._clauseStartIndex[_index] + index]; };

    inline const_reference operator*(void) const { return _database._clauses[_database._clauseStartIndex[_index]]; }
    inline const_pointer operator->(void) const { return &(_database._clauses[_database._clauseStartIndex[_index]]); }
	inline const_reference operator[](size_t index) const { return _database._clauses[_database._clauseStartIndex[_index] + index]; };

	inline size_t size() const { return _database._clauseStartIndex[_index + 1u] - _database._clauseStartIndex[_index]; };

private:
	ModernClauseDB<T>& _database;
	size_t _index;

};

template<typename T>
class ClauseIterator
{
public:
    typedef Clause<T>							value_type;
    typedef size_t								difference_type;
    typedef std::random_access_iterator_tag		iterator_category;
	typedef ClauseIterator<T>					iterator_type;

    ClauseIterator(ModernClauseDB<T>& database, size_t index): _database(database), _index(index) {}
    ClauseIterator(const iterator_type& rhs) = default;
	ClauseIterator(iterator_type&& rhs) = default;

	inline iterator_type& operator=(const iterator_type& other) = default;
	inline iterator_type& operator=(iterator_type&& other) = default;

    inline Clause<T> operator*() { return Clause<T>(_database, _index); }
    inline Clause<T> operator->() { return Clause<T>(_database, _index); }
    inline Clause<T> operator[](difference_type rhs) { return Clause<T>(_database, _index); }
    inline const Clause<T> operator*() const { return Clause<T>(_database, _index); }
    inline const Clause<T> operator->() const { return Clause<T>(_database, _index); }
    inline const Clause<T> operator[](difference_type rhs) const { return Clause<T>(_database, _index); }

    inline iterator_type& operator+=(difference_type rhs) { _index += rhs; return *this; }
    inline iterator_type& operator-=(difference_type rhs) { _index -= rhs; return *this; }
    inline iterator_type& operator++() { ++_index; return *this; }
    inline iterator_type& operator--() { --_index; return *this; }
    inline iterator_type operator++(int) { return iterator_type(_database, _index++); }
    inline iterator_type operator--(int) { return iterator_type(_database, _index--); }

    inline difference_type operator-(const iterator_type& rhs) const { return _index-rhs._index; }
    inline iterator_type operator+(difference_type rhs) const { return iterator_type(_database, _index+rhs); }
    inline iterator_type operator-(difference_type rhs) const { return iterator_type(_database, _index-rhs); }
    friend inline iterator_type operator+(difference_type lhs, const iterator_type& rhs) { return iterator_type(rhs._database, lhs+rhs._index); }
    friend inline iterator_type operator-(difference_type lhs, const iterator_type& rhs) { return iterator_type(rhs._database, lhs-rhs._index); }

    inline bool operator==(const iterator_type& rhs) const { return _index == rhs._index; }
    inline bool operator!=(const iterator_type& rhs) const { return _index != rhs._index; }
    inline bool operator>(const iterator_type& rhs) const { return _index > rhs._index; }
    inline bool operator<(const iterator_type& rhs) const { return _index < rhs._index; }
    inline bool operator>=(const iterator_type& rhs) const { return _index >= rhs._index; }
    inline bool operator<=(const iterator_type& rhs) const { return _index <= rhs._index; }

private:
    ModernClauseDB<T>& _database;
	size_t _index;

};

template <typename T>
class ModernClauseDB
{
public:
	typedef ClauseIterator<T>					iterator;
	typedef const ClauseIterator<T>				const_iterator;

	ModernClauseDB(void): _clauses(), _clauseStartIndex({ 0u }) {}
	ModernClauseDB(const ModernClauseDB<T>& rhs) = default;
	ModernClauseDB(ModernClauseDB<T>&& rhs) = default;

	ModernClauseDB& operator=(const ModernClauseDB<T>& other) = default;
	ModernClauseDB& operator=(ModernClauseDB<T>&& other) = default;

	size_t size(void) { return _clauseStartIndex.size() - 1u; }
	void clear(void) { _clauses.clear(); _clauseStartIndex = { 0u }; }
	void swap(ModernClauseDB<T>& x)
	{
		std::swap(_clauses, x._clauses);
		std::swap(_clauseStartIndex, x._clauseStartIndex);
	}
	void push_back(const std::vector<T>& clause)
	{
		std::copy(clause.cbegin(), clause.cend(), std::back_inserter(_clauses));
		_clauseStartIndex.push_back(_clauses.size());
	}
	void push_back(std::vector<T>&& clause)
	{
		std::copy(clause.cbegin(), clause.cend(), std::back_inserter(_clauses));
		_clauseStartIndex.push_back(_clauses.size());
	}
	void pop_back(void)
	{
		_clauseStartIndex.pop_back();
		_clauses.resize(_clauseStartIndex.back());
	}

	iterator front(void) { return iterator(*this, 0u); };
	iterator back(void) { return iterator(*this, _clauseStartIndex.size() - 1u); };

	iterator begin(void) { return iterator(*this, 0u); };
	iterator end(void) { return iterator(*this, _clauseStartIndex.size() - 1u); };

	const_iterator cbegin(void) const { return const_iterator(*this, 0u); };
	const_iterator cend(void) const { return const_iterator(*this, _clauseStartIndex.size() - 1u); };

	Clause<T> operator[](size_t index) { return Clause(*this, index); }
	const Clause<T> operator[](size_t index) const { return Clause(*this, index); }

private:
	friend ClauseIterator<T>;
	friend Clause<T>;

	std::vector<T> _clauses;
	std::vector<size_t> _clauseStartIndex;

};

};
