#pragma once

#include <set>
#include <cstdint>

namespace GraphLib {

class IDGenerator {
public:
    IDGenerator();
    IDGenerator(std::set<uint32_t> aldreadyTakenIDs);

    const std::set<uint32_t> &getTakenIDs();

    uint32_t generate();
    bool removeTaken(uint32_t id);

private:
    std::set<uint32_t> _takenIDs = {};
};

}