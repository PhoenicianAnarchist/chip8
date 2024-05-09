#include <sstream>

#include "error.hpp"
#include "cpu.hpp"

kate::CPU::CPU() {
  reset();

  std::random_device rd;
  e.seed(rd());
  dist = std::uniform_int_distribution<int>(0x00, 0xff);
}

void kate::CPU::reset() {
  registers.fill(0);
  stack.fill(0);
  stack_pointer = 0;
  index_register = 0;

  cur_inst = {0, OPCODE::NOP, 0, 0, 0};
  cycle_counter = 0;
  prev_program_counter = 0;
}

std::uint8_t kate::CPU::random_uint8() {
  return dist(e);
}

void kate::CPU::execute() {
  std::uint8_t r = cur_inst.x;
  std::uint8_t k = registers[cur_inst.x];
  switch (cur_inst.op) {
    case OPCODE::RET:
      --stack_pointer;
      program_counter = stack[stack_pointer];
      break;
    case OPCODE::JMP:
      prev_program_counter = program_counter - 2;
      program_counter = cur_inst.n;
      break;
    case OPCODE::CALL:
      if (stack_pointer >= 16) {
        throw stack_overflow(crashdump("STACK OVERFLOW", *this));
      }
      stack[stack_pointer] = program_counter;
      ++stack_pointer;
      program_counter = cur_inst.n;
    case OPCODE::SKIP_EQ_IMM:
      if (registers[cur_inst.x] == cur_inst.n) {
        program_counter += 2;
      }
      break;
    case OPCODE::SKIP_NE_IMM:
      if (registers[cur_inst.x] != cur_inst.n) {
        program_counter += 2;
      }
      break;
    case OPCODE::SKIP_EQ_REG:
      if (registers[cur_inst.x] == registers[cur_inst.y]) {
        program_counter += 2;
      }
      break;
    case OPCODE::SKIP_NE_REG:
      if (registers[cur_inst.x] != registers[cur_inst.y]) {
        program_counter += 2;
      }
      break;
    case OPCODE::MOV:
      registers[cur_inst.x] = cur_inst.n;
      break;
    case OPCODE::ADD:
      registers[cur_inst.x] += cur_inst.n;
      break;
    case OPCODE::LDI:
      index_register = cur_inst.n;
      break;
    case OPCODE::JMP_OFF:
      prev_program_counter = program_counter - 2;

      #ifdef QUIRK_J
      program_counter = cur_inst.n + registers[cur_inst.x];
      #else
      program_counter = cur_inst.n + registers[0];
      #endif

      break;
    case OPCODE::RANDOM:
      registers[cur_inst.x] = random_uint8() & cur_inst.n;
      break;
    default:
      throw invalid_instruction(crashdump("NOT YET IMPLEMENTED", *this));
  }
}
