#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <filesystem>
#include <string>
#include <vector>

#include <cstdint> // std::uint8_t

namespace utils {
  enum FORMAT {
    PBM_PLAIN,
    PBM_RAW,
    PGM_PLAIN,
    PGM_RAW,
    PPM_PLAIN,
    PPM_RAW
  };

  int save_pnm(
    const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
    const std::filesystem::path &directory, const std::string &name,
    FORMAT file_format=PGM_RAW, bool create_dirs=false,
    const std::string &max_value="255"
  );
}

#endif // __IMAGE_HPP__
