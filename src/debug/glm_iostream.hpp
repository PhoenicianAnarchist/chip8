#ifndef __GLM_IOSTREAM__
#define __GLM_IOSTREAM__

#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

namespace debug {
  std::string f(double d, int w=8);
  int get_w(double d);

  std::ostream &operator<<(std::ostream &os, const glm::vec4 &v);
  std::ostream &operator<<(std::ostream &os, const glm::mat4 &m);
}

#endif // __GLM_IOSTREAM__
