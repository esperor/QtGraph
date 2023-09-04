#include "idgenerator.h"

namespace GraphLib {

IDGenerator::IDGenerator() {}
IDGenerator::IDGenerator(std::set<uint32_t> aldreadyTakenIDs)
{
    _takenIDs = aldreadyTakenIDs;
}

const std::set<uint32_t> &IDGenerator::getTakenIDs()
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

}