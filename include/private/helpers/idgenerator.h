#pragma once

#include <set>
#include <cstdint>
#include <QMap>

namespace qtgraph {

class ID {
public:
    template <class T>
    static const std::set<uint32_t> &getTakenIDs();

    static const QMap<size_t, std::set<uint32_t>> &getTakenIDs() { return *_takenIDs; }

    template <class T>
    static uint32_t generate();

    // returns false if nothing was removed
    template <class T>
    static bool removeTaken(uint32_t id);

    // returns false if given id is already taken
    template <class T>
    static bool addTaken(uint32_t id);

    template <class T>
    static void clear();

    static void clear() { _takenIDs->clear(); }

private:
    template <class T>
    static std::set<uint32_t>& takenIDs();

    static QMap<size_t, std::set<uint32_t>> *_takenIDs;
};

template <class T>
const std::set<uint32_t> &ID::getTakenIDs()
{ return _takenIDs->operator[](typeid(T).hash_code()); }

template <class T>
std::set<uint32_t> &ID::takenIDs()
{ return _takenIDs->operator[](typeid(T).hash_code()); }

template <class T>
uint32_t ID::generate()
{   
    if (!_takenIDs->contains(typeid(T).hash_code()))
    {
        _takenIDs->insert(typeid(T).hash_code(), { 0 });
        return 0;
    }

    auto it = takenIDs<T>().begin();
    int prev = -1;
    while (it != takenIDs<T>().end())
    {
        if (*it - prev > 1)
            break;
        prev = *it;
        it++;
    }
    takenIDs<T>().insert(static_cast<uint32_t>(prev + 1));
    return prev + 1;
}

template <class T>
bool ID::removeTaken(uint32_t id)
{
    if (!_takenIDs->contains(typeid(T).hash_code())) return false;
    return takenIDs<T>().erase(id);
}

template <class T>
bool ID::addTaken(uint32_t id)
{
    if (takenIDs<T>().contains(id)) return false;
    takenIDs<T>().insert(id);
    return true;
}

template <class T>
void ID::clear()
{
    takenIDs<T>().clear();
}

}