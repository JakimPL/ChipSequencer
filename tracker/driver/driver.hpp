#ifndef DRIVER_DRIVER_HPP
#define DRIVER_DRIVER_HPP

#include <array>
#include <string>
#include "../song.hpp"

class Driver {
  public:
    Driver(const std::array<t_output, SONG_LENGTH> &target)
        : target(target) {}

    virtual ~Driver() = default;

    virtual bool initialize() = 0;
    virtual void terminate() = 0;
    virtual void play() = 0;

  protected:
    const std::array<t_output, SONG_LENGTH> &target;
};

#endif // DRIVER_DRIVER_HPP