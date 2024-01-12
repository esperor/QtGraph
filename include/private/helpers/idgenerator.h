#pragma once

#include <set>
#include <cstdint>
#include <QMap>

namespace qtgraph {

class IDGenerator {
public:
    IDGenerator() : _takenIDs{ new QMap<size_t, std::set<uint32_t>>() } {}
    IDGenerator(QMap<size_t, std::set<uint32_t>> alreadyTakenIDs)
    {
        _takenIDs = new QMap(alreadyTakenIDs);
    }

    ~IDGenerator() { delete _takenIDs; }

    template <class T>
    const std::set<uint32_t> &getTakenIDs() const;

    template <class T>
    uint32_t generate();

    // returns false if nothing was removed
    template <class T>
    bool removeTaken(uint32_t id);

    // returns false if given id is already taken
    template <class T>
    bool addTaken(uint32_t id);

private:
    template <class T>
    std::set<uint32_t>& takenIDs();

    QMap<size_t, std::set<uint32_t>> *_takenIDs;
};

template <class T>
const std::set<uint32_t> &IDGenerator::getTakenIDs() const
{ return _takenIDs->operator[](typeid(T).hash_code()); }

template <class T>
std::set<uint32_t> &IDGenerator::takenIDs()
{ return _takenIDs->operator[](typeid(T).hash_code()); }

template <class T>
uint32_t IDGenerator::generate()
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
bool IDGenerator::removeTaken(uint32_t id)
{
    if (!_takenIDs->contains(typeid(T).hash_code())) return false;
    return takenIDs<T>().erase(id);
}

template <class T>
bool IDGenerator::addTaken(uint32_t id)
{
    if (takenIDs<T>().contains(id)) return false;
    takenIDs<T>().insert(id);
    return true;
}

}