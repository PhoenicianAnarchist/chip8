#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__
#include <filesystem>

#include <cstdint>

namespace utils {
  struct OPTIONS {
    int err;
    bool called_for_help;
    std::filesystem::path rom_path;
  };

  OPTIONS parse_command_line(int argc, const char *argv[]);
}

#endif // __OPTIONS_HPP__
