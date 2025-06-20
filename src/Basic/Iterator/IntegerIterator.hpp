#pragma once

#include <iterator>

template<typename Type>
class IntegerIterator
{
public:
    using iterator_concept  = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = ssize_t;
    using value_type        = Type;
    using pointer           = Type*;
    using reference         = Type&;

    IntegerIterator(Type rhs) : _value(rhs) {}
    IntegerIterator(const IntegerIterator<Type> &rhs) : _value(rhs._value) {}

    inline IntegerIterator<Type>& operator+=(difference_type rhs) {_value += rhs; return *this;}
    inline IntegerIterator<Type>& operator-=(difference_type rhs) {_value -= rhs; return *this;}
    inline const Type& operator*() const {return _value;}
    inline const Type* operator->() const {return &_value;}
    inline const Type& operator[](difference_type rhs) const { return _value; }

    inline IntegerIterator<Type>& operator++() {++_value; return *this;}
    inline IntegerIterator<Type>& operator--() {--_value; return *this;}
    inline IntegerIterator<Type> operator++(int) const {IntegerIterator<Type> tmp(*this); ++_value; return tmp;}
    inline IntegerIterator<Type> operator--(int) const {IntegerIterator<Type> tmp(*this); --_value; return tmp;}

    inline difference_type operator-(const IntegerIterator<Type>& rhs) const {return _value-rhs._value;}
    inline IntegerIterator<Type> operator+(difference_type rhs) const {return IntegerIterator<Type>(_value+rhs);}
    inline IntegerIterator<Type> operator-(difference_type rhs) const {return IntegerIterator<Type>(_value-rhs);}
    friend inline IntegerIterator<Type> operator+(difference_type lhs, const IntegerIterator<Type>& rhs) {return IntegerIterator<Type>(lhs+rhs._value);}
    friend inline IntegerIterator<Type> operator-(difference_type lhs, const IntegerIterator<Type>& rhs) {return IntegerIterator<Type>(lhs-rhs._value);}

    inline bool operator==(const IntegerIterator<Type>& rhs) const {return _value == rhs._value;}
    inline bool operator!=(const IntegerIterator<Type>& rhs) const {return _value != rhs._value;}
    inline bool operator>(const IntegerIterator<Type>& rhs) const {return _value > rhs._value;}
    inline bool operator<(const IntegerIterator<Type>& rhs) const {return _value < rhs._value;}
    inline bool operator>=(const IntegerIterator<Type>& rhs) const {return _value >= rhs._value;}
    inline bool operator<=(const IntegerIterator<Type>& rhs) const {return _value <= rhs._value;}

protected:
    Type _value;
};
