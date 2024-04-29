#ifndef __IO_HPP__
#define __IO_HPP__

#include <filesystem>
#include <vector>

#include <cstdint> // std::uint8_t

namespace utils {
  std::vector<std::uint8_t> load_file(const std::filesystem::path &path);
  int write_file(
    const std::filesystem::path &path, const std::vector<std::uint8_t> &data,
    bool create_dirs=false
  );
}

#endif // __IO_HPP__
