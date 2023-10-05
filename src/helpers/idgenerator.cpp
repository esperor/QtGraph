#include "idgenerator.h"

namespace qtgraph {

IDGenerator::IDGenerator() {}
IDGenerator::IDGenerator(std::set<uint32_t> aldreadyTakenIDs)
{
    _takenIDs = aldreadyTakenIDs;
}

const std::set<uint32_t> &IDGenerator::getTakenIDs() const
{ return _takenIDs; }

uint32_t IDGenerator::generate()
{   
    auto it = _takenIDs.begin();
    int prev = -1;
    while (it != _takenIDs.end())
    {
        if (*it - prev > 1)
            { _takenIDs.insert(prev + 1); return prev + 1; }
        prev = *it;
        it++;
    }
    _takenIDs.insert(prev + 1); return prev + 1;
}

bool IDGenerator::removeTaken(uint32_t id)
{
    return _takenIDs.erase(id);
}

bool IDGenerator::addTaken(uint32_t id)
{
    if (_takenIDs.contains(id)) return false;
    _takenIDs.insert(id);
    return true;
}
}