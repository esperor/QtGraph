#pragma once

#include <functional>
#include <string>
#include "qtgraph.h"

namespace qtgraph {

enum class EAction
{
    None,
    Deletion,
    Addition,
    Connection,
    Disconnection,
    Renaming,
    Moving,
    Conversion,
    Unknown,
};

struct IAction
{
public:
    IAction(EAction _code, std::string _desc, std::function<void()> _action)
        : code{ _code }, desc{ _desc }, action{ _action } {}

    std::function<void()> action = [](){};
    std::string desc = "";
    EAction code = EAction::Unknown;
};

}