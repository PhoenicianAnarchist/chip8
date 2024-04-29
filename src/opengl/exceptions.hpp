#ifndef __GLW_EXCEPTIONS__
#define __GLW_EXCEPTIONS__

#include <exception>
#include <string>

namespace openglwrapper {
  class openglwrapper_error : public std::exception {
  public:
    explicit openglwrapper_error(const std::string &msg);
    explicit openglwrapper_error(const char *msg);
    virtual ~openglwrapper_error() noexcept;

    virtual const char *what() const noexcept;
  protected:
    std::string msg;
  };

  class texture_error : public openglwrapper_error {
  public:
    explicit texture_error(const std::string& msg);
    explicit texture_error(const char* msg);
  };

  class shader_error : public openglwrapper_error {
  public:
    explicit shader_error(const std::string& msg);
    explicit shader_error(const char* msg);
  };
}

#endif // __GLW_EXCEPTIONS__
