/******************************************************************************
* QUIRK_F : (quirks_enable_flags_reset)                                       *
*     8XY1, 8XY2, and 8XY3 reset the flags register to zero                   *
*                                                                             *
* QUIRK_Y : (quirks_shifting_ignores_y)                                       *
*     8XY6 and 8XYE ignore the Y register and shift X instead                 *
******************************************************************************/

#include "alu.hpp"

void kate::ALU::execute() {
  std::uint16_t tmp = 0;
  switch (op) {
    case ALU_OP::MOV:  alu_mov();  break;
    case ALU_OP::OR:   alu_or();   break;
    case ALU_OP::AND:  alu_and();  break;
    case ALU_OP::XOR:  alu_xor();  break;
    case ALU_OP::ADD:  alu_add();  break;
    case ALU_OP::SUB:  alu_sub();  break;
    case ALU_OP::RSUB: alu_rsub(); break;
    case ALU_OP::SHL:  alu_shl();  break;
    case ALU_OP::SHR:  alu_shr();  break;
  }
}

void kate::ALU::alu_mov() {
  x = y;
}

void kate::ALU::alu_or() {
  x |= y;
  #ifdef QUIRK_F
  f = 0;
  #endif
}

void kate::ALU::alu_and() {
  x &= y;
  #ifdef QUIRK_F
  f = 0;
  #endif
}

void kate::ALU::alu_xor() {
  x ^= y;
  #ifdef QUIRK_F
  f = 0;
  #endif
}

void kate::ALU::alu_add() {
  std::uint16_t tmp = x;
  x += y;
  f = x < tmp;
}

void kate::ALU::alu_sub() {
  std::uint16_t tmp = x;
  x -= y;
  f = (x <= tmp) && (y <= tmp);
}

void kate::ALU::alu_rsub() {
  std::uint16_t tmp = x;
  x = y - x;
  f = tmp <= y;
}

void kate::ALU::alu_shl() {
  #ifndef QUIRK_Y
  x = y;
  #endif
  std::uint16_t tmp = (x >> 7) & 0b1;
  x <<= 1;
  f = tmp;
}

void kate::ALU::alu_shr() {
  #ifndef QUIRK_Y
  x = y;
  #endif
  std::uint16_t tmp = x & 0b1;
  x >>= 1;
  f = tmp;
}
