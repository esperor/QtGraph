#pragma once

#include <deque>

#include "qtgraph.h"

namespace qtgraph {

template <class T>
class Stack
{
public:
    Stack(int length = 50);

    void push(const T &value);
    void push(T &&value);
    T pop();

    void setLength(int l);
    int getLength() const { return _length; }
    const std::deque<T> &getContainer() const { return _container; }
    int size() const { return _container.size(); }

    void clear() { _container.clear(); }
    
private:
    std::deque<T> _container;
    int _length;
};

template <class T>
Stack<T>::Stack(int length)
    : _container{ std::deque<T>() }
    , _length{ length }
{}

template <class T>
void Stack<T>::push(const T& value)
{
    if (_container.size() == _length)
        _container.pop_front();

    _container.push_back(value);
}

template <class T>
void Stack<T>::push(T&& value)
{
    if (_container.size() == _length)
        _container.pop_front();

    _container.push_back(value);
}

template <class T>
T Stack<T>::pop()
{
    T t = _container.back();
    _container.pop_back();
    return t;
}

template <class T>
void Stack<T>::setLength(int l)
{
    if (_length > l)
        _container.resize(l);

    _length = l;
}

}