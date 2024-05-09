#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include <exception>
#include <string>

#include "cpu.hpp"

namespace kate {
  class system_error : public std::exception {
  public:
    explicit system_error(const std::string &msg);
    explicit system_error(const char *msg);
    virtual ~system_error() noexcept;

    virtual const char *what() const noexcept;
  protected:
    std::string msg;
  };

  class invalid_address : public system_error {
  public:
    explicit invalid_address(const std::string& msg);
    explicit invalid_address(const char* msg);
  };

  class invalid_instruction : public system_error {
  public:
    explicit invalid_instruction(const std::string& msg);
    explicit invalid_instruction(const char* msg);
  };

  class stack_overflow : public system_error {
  public:
    explicit stack_overflow(const std::string& msg);
    explicit stack_overflow(const char* msg);
  };

  std::string hex_string(std::size_t i, std::size_t w, bool b=true);
  std::string decode_OPCODE(OPCODE inst);
  std::string crashdump(const std::string &msg, const CPU &cpu);
}

#endif // __ERROR_HPP__
