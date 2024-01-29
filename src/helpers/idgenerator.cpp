#include "helpers/idgenerator.h"

namespace qtgraph {

QMap<size_t, std::set<uint32_t>> *ID::_takenIDs = new QMap<size_t, std::set<uint32_t>>();

}