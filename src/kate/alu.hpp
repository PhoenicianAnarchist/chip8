#ifndef __ALU_HPP__
#define __ALU_HPP__

#include <cstdint>

namespace kate {
  enum class ALU_OP {
    MOV   = 0x00,
    OR    = 0x01,
    AND   = 0x02,
    XOR   = 0x03,
    ADD   = 0x04,
    SUB   = 0x05,
    RSUB  = 0x07,
    SHR   = 0x06,
    SHL   = 0x0e
  };

  class ALU {
  public:
    ALU() = default;

    void execute();

    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t f;
    ALU_OP op;

  private:
    void alu_mov();
    void alu_or();
    void alu_and();
    void alu_xor();
    void alu_add();
    void alu_sub();
    void alu_rsub();
    void alu_shl();
    void alu_shr();
  };
}

#endif // __ALU_HPP__
