#pragma once

#include <array>
#include <string>
#include "../song/core.hpp"

class Driver {
  public:
    virtual ~Driver() = default;
    Driver(const Driver &) = delete;
    Driver &operator=(const Driver &) = delete;
    Driver(Driver &&) = delete;
    Driver &operator=(Driver &&) = delete;

    virtual void terminate() = 0;
    virtual void play() = 0;

  protected:
    Driver() = default;
};
