#ifndef DRIVER_FILE_HPP
#define DRIVER_FILE_HPP

#include <string>
#include "driver.hpp"

class FileDriver : public Driver {
  public:
    FileDriver(const std::string &filename);

    bool initialize() override;
    void terminate() override;
    void play() override;

  private:
    void save_output_to_file();
    std::string filename;
};

#endif // DRIVER_FILE_HPP