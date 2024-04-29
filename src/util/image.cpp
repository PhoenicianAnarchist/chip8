#include <algorithm> // std::copy
#include <sstream>

#include "io.hpp"

#include "image.hpp"

int utils::save_pnm(
  const std::vector<std::uint8_t> &data, std::size_t width, std::size_t height,
  const std::filesystem::path &directory, const std::string &name,
  FORMAT file_format, bool create_dirs, const std::string &max_value
) {
  std::string ext;
  std::vector<std::uint8_t> image_data;

  std::vector<std::uint8_t> header;
  header.push_back('P');
  switch (file_format) {
    case PBM_PLAIN : header.push_back('1'); ext = ".pbm"; break;
    case PGM_PLAIN : header.push_back('2'); ext = ".pgm"; break;
    case PPM_PLAIN : header.push_back('3'); ext = ".ppm"; break;
    case PBM_RAW   : header.push_back('4'); ext = ".pbm"; break;
    case PGM_RAW   : header.push_back('5'); ext = ".pgm"; break;
    case PPM_RAW   : header.push_back('6'); ext = ".ppm"; break;
  }
  header.push_back('\n');

  std::string w_str = std::to_string(width);
  std::copy(w_str.begin(), w_str.end(), std::back_inserter(header));
  header.push_back(' ');
  std::string h_str = std::to_string(height);
  std::copy(h_str.begin(), h_str.end(), std::back_inserter(header));

  header.push_back('\n');
  std::copy(max_value.begin(), max_value.end(), std::back_inserter(header));
  header.push_back('\n');

  std::copy(header.begin(), header.end(), std::back_inserter(image_data));

  if (
    (file_format == PBM_RAW) ||
    (file_format == PGM_RAW) ||
    (file_format == PPM_RAW)
  ) {
    std::copy(data.begin(), data.end(), std::back_inserter(image_data));
  } else {
    for (auto b : data) {
      std::string b_str = std::to_string(b);
      std::copy(b_str.begin(), b_str.end(), std::back_inserter(image_data));
      image_data.push_back(' ');
    }
  }

  std::stringstream ss;
  ss << name << ext;
  std::filesystem::path filepath = directory / ss.str();
  return write_binary(filepath, image_data, create_dirs);
}
