#include <iomanip>
#include <sstream>

#include "error.hpp"

kate::system_error::system_error(const std::string &msg) : msg(msg) {}
kate::system_error::system_error(const char *msg) : msg(msg) {}
kate::system_error::~system_error() noexcept {}
const char *kate::system_error::what() const noexcept {
   return msg.c_str();
}

kate::invalid_address::invalid_address(const std::string &msg)
: system_error(msg) {}
kate::invalid_address::invalid_address(const char *msg)
: system_error(msg) {}

kate::invalid_instruction::invalid_instruction(const std::string &msg)
: system_error(msg) {}
kate::invalid_instruction::invalid_instruction(const char *msg)
: system_error(msg) {}

kate::stack_overflow::stack_overflow(const std::string &msg)
: system_error(msg) {}
kate::stack_overflow::stack_overflow(const char *msg)
: system_error(msg) {}

std::string kate::hex_string(std::size_t i, std::size_t w, bool b) {
  std::stringstream ss;
  if (b) {
    ss << "0x";
  }
  ss << std::hex << std::setw(w) << std::setfill('0') << i;

  return ss.str();
}

std::string kate::decode_OPCODE(OPCODE inst) {
  switch (inst) {
    case OPCODE::CLEAR         : return "CLEAR";
    case OPCODE::RET           : return "RET";
    case OPCODE::JMP           : return "JMP";
    case OPCODE::CALL          : return "CALL";
    case OPCODE::SKIP_EQ_IMM   : return "SKIP_EQ_IMM";
    case OPCODE::SKIP_NE_IMM   : return "SKIP_NE_IMM";
    case OPCODE::SKIP_EQ_REG   : return "SKIP_EQ_REG";
    case OPCODE::SKIP_NE_REG   : return "SKIP_NE_REG";
    case OPCODE::MOV           : return "MOV";
    case OPCODE::ADD           : return "ADD";
    case OPCODE::ALU           : return "ALU";
    case OPCODE::LDI           : return "LDI";
    case OPCODE::JMP_OFF       : return "JMP_OFF";
    case OPCODE::RANDOM        : return "RANDOM";
    case OPCODE::DRAW          : return "DRAW";
    case OPCODE::KEY_EQ        : return "KEY_EQ";
    case OPCODE::KEY_NE        : return "KEY_NE";
    case OPCODE::MISC          : return "MISC";
    default                    : return "UNKNOWN OPCODE";
  }
}

std::string kate::crashdump(const std::string &msg, const CPU &cpu) {
  std::stringstream ss;
  ss << "ABORTING EXECUTION: " << msg << '\n';
  ss << "------------------------------------------------------------------\n";
  ss << " current cycle " << hex_string(cpu.cycle_counter, 8) << '\n';
  ss << " current instruction " << hex_string(cpu.cur_inst.raw, 4, false);
  ss << " ( " << decode_OPCODE(cpu.cur_inst.op) << " )\n";
  ss << "------------------------------------------------------------------\n";
  ss << " PC : " << hex_string(cpu.prev_program_counter, 4) << " . ";
  ss << " SP : " << hex_string(cpu.stack_pointer, 4) << " . ";
  ss << " IR : " << hex_string(cpu.index_register, 4) << '\n';
  ss << "------------------------------------------------------------------\n";
  ss << "      STACK  | REGISTERS\n";
  for (std::size_t i = 0; i <= 0xf; ++i) {
    ss << hex_string(i, 1) << " : " << hex_string(cpu.stack[i], 4) << " | ";
    ss << hex_string(cpu.registers[i], 2) << '\n';
  }
  ss << "------------------------------------------------------------------\n";

  return ss.str();
}
