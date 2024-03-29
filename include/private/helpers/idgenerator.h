#pragma once

#include <set>
#include <cstdint>

namespace qtgraph {

class IDGenerator {
public:
    IDGenerator();
    IDGenerator(std::set<uint32_t> aldreadyTakenIDs);

    const std::set<uint32_t> &getTakenIDs() const;

    uint32_t generate();

    // returns false if nothing was removed
    bool removeTaken(uint32_t id);

    // returns false if given id is already taken
    bool addTaken(uint32_t id);

private:
    std::set<uint32_t> _takenIDs = {};
};

}