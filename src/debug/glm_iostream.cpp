#include <algorithm> // std::max
#include <sstream>
#include <iomanip>

#include "glm_iostream.hpp"

std::string debug::f(double d, int w) {
  std::stringstream ss;
  if (d < 0) {
    ss << std::setw(w) << std::left << std::setprecision(2) << d;
  } else {
    ss << " " << std::setw(w-1) << std::left << std::setprecision(2) << d;
  }

  return ss.str();
}

int debug::get_w(double d) {
  std::stringstream ss;

  ss << std::setprecision(2) << d;

  // note: .length() returns number of bytes. in this case, it will do
  return ss.str().length();
}

std::ostream &debug::operator<<(std::ostream &os, const glm::vec4 &v) {
  int maxw = std::max( {get_w(v.x), get_w(v.y), get_w(v.z), get_w(v.w)} ) + 2;

  os << "┌" << f(v.x, maxw) << "┐\n";
  os << "│" << f(v.y, maxw) << "│\n";
  os << "│" << f(v.z, maxw) << "│\n";
  os << "└" << f(v.w, maxw) << "┘";

  return os;
}

std::ostream &debug::operator<<(std::ostream &os, const glm::mat4 &m) {

  int maxw0 = std::max( {get_w(m[0][0]), get_w(m[0][1]), get_w(m[0][2]), get_w(m[0][3])} ) + 2;
  int maxw1 = std::max( {get_w(m[1][0]), get_w(m[1][1]), get_w(m[1][2]), get_w(m[1][3])} ) + 2;
  int maxw2 = std::max( {get_w(m[2][0]), get_w(m[2][1]), get_w(m[2][2]), get_w(m[2][3])} ) + 2;
  int maxw3 = std::max( {get_w(m[3][0]), get_w(m[3][1]), get_w(m[3][2]), get_w(m[3][3])} ) + 2;

  os << "┌" << f(m[0][0], maxw0) << f(m[1][0], maxw1) << f(m[2][0], maxw2) << f(m[3][0], maxw3) << "┐\n";
  os << "│" << f(m[0][1], maxw0) << f(m[1][1], maxw1) << f(m[2][1], maxw2) << f(m[3][1], maxw3) << "│\n";
  os << "│" << f(m[0][2], maxw0) << f(m[1][2], maxw1) << f(m[2][2], maxw2) << f(m[3][2], maxw3) << "│\n";
  os << "└" << f(m[0][3], maxw0) << f(m[1][3], maxw1) << f(m[2][3], maxw2) << f(m[3][3], maxw3) << "┘";

  return os;
}
