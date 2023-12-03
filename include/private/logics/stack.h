#pragma once

#include <deque>

#include "qtgraph.h"

namespace qtgraph {

template <class T>
class LStack
{
public:
    LStack(int length = 50);

    void push(const T &value);
    void push(T &&value);
    T pop();

    void setLength(int l);
    int getLength() const { return _length; }

    void clear() { _container.clear(); }
    
private:
    std::deque<T> _container;
    int _length;
};

}