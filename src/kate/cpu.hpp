#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <array>
#include <random>

#include <cstdint>

namespace kate {
  enum class OPCODE;
  enum class MISC_OP;

  // will be able to hold data for any kind of instruction
  struct Instruction {
    std::uint16_t raw;

    OPCODE op;
    std::uint8_t x;
    std::uint8_t y;
    std::uint16_t n;
  };

  class CPU {
  public:
    CPU();

    void reset();
    std::uint8_t random_uint8();

    void execute();

    std::array<std::uint8_t, 16> registers;
    std::uint16_t program_counter;
    std::uint16_t index_register;
    Instruction cur_inst;

    std::array<std::uint16_t, 16> stack;
    std::uint64_t cycle_counter;
    std::uint16_t prev_program_counter;
    std::uint16_t stack_pointer;
  private:
    std::default_random_engine e;
    std::uniform_int_distribution<int> dist;
  };

  enum class OPCODE {
    NOP         = 0x00,
    CLEAR       = 0xe0,
    RET         = 0xee,
    JMP         = 0x01,
    CALL        = 0x02,
    SKIP_EQ_IMM = 0x03,
    SKIP_NE_IMM = 0x04,
    SKIP_EQ_REG = 0x05,
    SKIP_NE_REG = 0x09,
    MOV         = 0x06,
    ADD         = 0x07,
    ALU         = 0x08,
    LDI         = 0x0a,
    JMP_OFF     = 0x0b,
    RANDOM      = 0x0c,
    DRAW        = 0x0d,
    KEY_EQ      = 0x9e,
    KEY_NE      = 0xa1,
    MISC        = 0x0f
  };

  enum class MISC_OP {
    GET_DT    = 0x07,
    GET_KEY   = 0x0A,
    SET_DT    = 0x15,
    SET_ST    = 0x18,
    GET_CHAR  = 0x29,
    ADD_IR    = 0x1E,
    BCD       = 0x33,
    STORE_REG = 0x55,
    LOAD_REG  = 0x65
  };
}

#endif // __CPU_HPP__
