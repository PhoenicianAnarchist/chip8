#ifndef __IO_HPP__
#define __IO_HPP__

#include <filesystem>
#include <string>
#include <vector>

#include <cstdint> // std::uint8_t

namespace utils {
  std::string read_file(const std::filesystem::path &path);
  std::vector<std::uint8_t> read_binary(const std::filesystem::path &path);

  int write_file(
    const std::filesystem::path &path, const std::string &data,
    bool create_dirs=false
  );
  int write_binary(
    const std::filesystem::path &path, const std::vector<std::uint8_t> &data,
    bool create_dirs=false
  );
}

#endif // __IO_HPP__
