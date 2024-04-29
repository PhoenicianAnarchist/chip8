#include "exceptions.hpp"

openglwrapper::openglwrapper_error::openglwrapper_error(const std::string &msg)
: msg(msg) {}
openglwrapper::openglwrapper_error::openglwrapper_error(const char *msg)
: msg(msg) {}
openglwrapper::openglwrapper_error::~openglwrapper_error() noexcept {}

const char *openglwrapper::openglwrapper_error::what() const noexcept {
   return msg.c_str();
}

openglwrapper::texture_error::texture_error(const std::string& msg)
: openglwrapper_error(msg) {}
openglwrapper::texture_error::texture_error(const char* msg)
: openglwrapper_error(msg) {}

openglwrapper::shader_error::shader_error(const std::string& msg)
: openglwrapper_error(msg) {}
openglwrapper::shader_error::shader_error(const char* msg)
: openglwrapper_error(msg) {}
