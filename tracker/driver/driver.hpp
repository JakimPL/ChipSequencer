#ifndef DRIVER_DRIVER_HPP
#define DRIVER_DRIVER_HPP

#include <array>
#include <string>
#include "../song/data.hpp"

class Driver {
  public:
    virtual ~Driver() = default;

    virtual void terminate() = 0;
    virtual void play() = 0;
};

#endif // DRIVER_DRIVER_HPP