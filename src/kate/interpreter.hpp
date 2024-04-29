#ifndef __KATE_CHIP8_INTERPRETER__
#define __KATE_CHIP8_INTERPRETER__

#include <array>
#include <exception>
#include <string>
#include <vector>

#include <cstdint>

#include "config.hpp"

namespace kate {
  class interpreter_error : public std::exception {
  public:
    explicit interpreter_error(const std::string &msg);
    explicit interpreter_error(const char *msg);
    virtual ~interpreter_error() noexcept;

    virtual const char *what() const noexcept;
  protected:
    std::string msg;
  };

  class invalid_address : public interpreter_error {
  public:
    explicit invalid_address(const std::string& msg);
    explicit invalid_address(const char* msg);
  };

  class invalid_instruction : public interpreter_error {
  public:
    explicit invalid_instruction(const std::string& msg);
    explicit invalid_instruction(const char* msg);
  };

  enum class INSTRUCTION {
    NOP
  };

  // will be able to hold data for any kind of instruction
  struct Instruction {
    std::uint16_t raw;

    INSTRUCTION inst;
    std::uint8_t x;
    std::uint8_t y;
    std::uint16_t n;
  };

  class Interpreter {
  public:
    Interpreter();

    void reset();
    void load_rom(const std::vector<std::uint8_t> &rom);
    const std::array<std::uint8_t, SCR_W * SCR_H> &get_output_buffer() const;
    void step();

    void fetch();
    void decode();
    void execute();

    void _00E0();
    void _1NNN();
    void _6XNN();
    void _7XNN();
    void _ANNN();
    void _DXYN();

  private:
    std::array<std::uint8_t, 0x4000> ram;
    std::array<std::uint8_t, 16> registers;
    std::array<std::uint16_t, 16> stack;
    std::uint16_t program_counter;
    std::uint16_t stack_pointer;
    std::uint16_t index_register;
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

    // each pixel is stored as a whole byte.
    // this is inneficcient but inconsequential.
    std::array<std::uint8_t, SCR_W * SCR_H> output_buffer;
    std::uint16_t current_instruction;
    std::uint64_t cycle_counter;
  };
}

#endif // __KATE_CHIP8_INTERPRETER__
