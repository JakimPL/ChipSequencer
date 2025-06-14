#pragma once

#include <array>
#include <string>
#include "../song/core.hpp"

class Driver {
  public:
    virtual ~Driver() = default;

    virtual void terminate() = 0;
    virtual void play() = 0;
};
