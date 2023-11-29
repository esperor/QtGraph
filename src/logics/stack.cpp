#include "logics/stack.h"

namespace qtgraph {

template <class T>
LStack<T>::LStack(int length)
    : _container{ std::deque<T>() }
    , _length{ length }
{
    _container.resize(_length);
}

template <class T>
void LStack<T>::push(const T &value)
{
    if (_container.size() == _length)
        _container.pop_front();

    _container.push_back(value);
}

template <class T>
void LStack<T>::push(T &&value)
{
    if (_container.size() == _length)
        _container.pop_front();

    _container.push_back(value);
}

template <class T>
T LStack<T>::pop()
{
    T t = _container.back();
    _container.pop_back();
    return t;
}

template <class T>
void LStack<T>::setLength(int l)
{
    if (_length > l)
        _container.resize(l);

    _length = l; 
}

}