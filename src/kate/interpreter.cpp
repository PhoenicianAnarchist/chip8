#include <algorithm> // std::copy
#include <iomanip>
#include <sstream>
#include <iostream>

#include "interpreter.hpp"

/******************************************************************************
/ Exceptions                                                                  /
******************************************************************************/
kate::interpreter_error::interpreter_error(const std::string &msg) : msg(msg) {}
kate::interpreter_error::interpreter_error(const char *msg) : msg(msg) {}
kate::interpreter_error::~interpreter_error() noexcept {}
const char *kate::interpreter_error::what() const noexcept {
   return msg.c_str();
}

kate::invalid_address::invalid_address(const std::string &msg)
: interpreter_error(msg) {}
kate::invalid_address::invalid_address(const char *msg)
: interpreter_error(msg) {}

kate::invalid_instruction::invalid_instruction(const std::string &msg)
: interpreter_error(msg) {}
kate::invalid_instruction::invalid_instruction(const char *msg)
: interpreter_error(msg) {}

kate::stack_overflow::stack_overflow(const std::string &msg)
: interpreter_error(msg) {}
kate::stack_overflow::stack_overflow(const char *msg)
: interpreter_error(msg) {}

std::string kate::decode_INSTRUCTION(INSTRUCTION inst) {
  switch (inst) {
    case INSTRUCTION::CLEAR         : return "CLEAR";
    case INSTRUCTION::RET           : return "RET";
    case INSTRUCTION::JMP           : return "JMP";
    case INSTRUCTION::CALL          : return "CALL";
    case INSTRUCTION::SKIP_EQ_IMM   : return "SKIP_EQ_IMM";
    case INSTRUCTION::SKIP_NE_IMM   : return "SKIP_NE_IMM";
    case INSTRUCTION::SKIP_EQ_REG   : return "SKIP_EQ_REG";
    case INSTRUCTION::SKIP_NE_REG   : return "SKIP_NE_REG";
    case INSTRUCTION::MOV           : return "MOV";
    case INSTRUCTION::ADD           : return "ADD";
    case INSTRUCTION::ALU           : return "ALU";
    case INSTRUCTION::LDI           : return "LDI";
    case INSTRUCTION::JMP_OFF       : return "JMP_OFF";
    case INSTRUCTION::RANDOM        : return "RANDOM";
    case INSTRUCTION::DRAW          : return "DRAW";
    case INSTRUCTION::KEY_EQ        : return "KEY_EQ";
    case INSTRUCTION::KEY_NE        : return "KEY_NE";
    case INSTRUCTION::MISC          : return "MISC";
    default                         : return "UNKNOWN INSTRUCTION";
  }
}

std::string kate::hex_string(std::size_t i, std::size_t w, bool b) {
  std::stringstream ss;
  if (b) {
    ss << "0x";
  }
  ss << std::hex << std::setw(w) << std::setfill('0') << i;

  return ss.str();
}

/******************************************************************************
/ Interpreter                                                                 /
******************************************************************************/
kate::Interpreter::Interpreter() {
  reset();

  std::random_device rd;
  e.seed(rd());
  dist = std::uniform_int_distribution<int>(0x00, 0xff);
}

void kate::Interpreter::reset() {
  registers.fill(0);
  key_states.fill(0);
  stack.fill(0);
  program_counter = entry_point;
  stack_pointer = 0;
  index_register = 0;
  delay_timer = 0;
  sound_timer = 0;

  output_buffer.resize(SCR_W * SCR_H);
  std::fill(output_buffer.begin(), output_buffer.end(), 0);
  is_blocking = false;
  is_vblank = false;
  cur_inst = {0, INSTRUCTION::NOP, 0, 0, 0};
  cycle_counter = 0;
  last_key_event = {0, KEY_EVENT::NONE};

  std::copy(
    char_data.begin(),
    char_data.end(),
    memory.get_active_bank().data().begin() + char_pointer
  );
}

void kate::Interpreter::load_rom(const std::vector<std::uint8_t> &rom) {
  reset();
  memory.write(entry_point, rom);
}

const std::vector<std::uint8_t> &kate::Interpreter::get_output_buffer() const {
  return output_buffer;
}

std::uint8_t kate::Interpreter::get_sound_timer() const {
  return sound_timer;
}

std::string kate::Interpreter::crashdump(const std::string &msg) const {
  std::stringstream ss;
  ss << "ABORTING EXECUTION: " << msg << '\n';
  ss << "------------------------------------------------------------------\n";
  ss << " current cycle " << hex_string(cycle_counter, 8) << '\n';
  ss << " current instruction " << hex_string(cur_inst.raw, 4, false);
  ss << " ( " << decode_INSTRUCTION(cur_inst.inst) << " )\n";
  ss << "------------------------------------------------------------------\n";
  ss << " PC : " << hex_string(prev_program_counter, 4) << " . ";
  ss << " SP : " << hex_string(stack_pointer, 4) << " . ";
  ss << " IR : " << hex_string(index_register, 4) << '\n';
  ss << "------------------------------------------------------------------\n";
  ss << "      STACK  | REGISTERS\n";
  for (std::size_t i = 0; i <= 0xf; ++i) {
    ss << hex_string(i, 1) << " : " << hex_string(stack[i], 4) << " | ";
    ss << hex_string(registers[i], 2) << '\n';
  }
  ss << "------------------------------------------------------------------\n";

  return ss.str();
}

std::string kate::Interpreter::debug_line() const {
  std::stringstream ss;
  ss << "PC : " << kate::hex_string(prev_program_counter, 4) << " . ";
  ss << "inst : ";
  ss << kate::hex_string(cur_inst.raw, 4, false) << " ( ";
  ss << decode_INSTRUCTION(cur_inst.inst) << " )";

  return ss.str();
}

std::string kate::Interpreter::debug_filename() const {
  std::stringstream ss;
  ss << kate::hex_string(cycle_counter, 8, false) << "_";
  ss << kate::hex_string(prev_program_counter, 4, false) << "_";
  ss << kate::hex_string(cur_inst.raw, 4, false);

  return ss.str();
}


void kate::Interpreter::decrement_timers() {
  if (delay_timer > 0) {
    --delay_timer;
  }
  if (sound_timer > 0) {
    --sound_timer;
  }
}

void kate::Interpreter::keypress(std::uint8_t k) {
  key_states[k] = true;
  last_key_event = {k, KEY_EVENT::PRESS};
}

void kate::Interpreter::keyrelease(std::uint8_t k) {
  key_states[k] = false;
  last_key_event = {k, KEY_EVENT::RELEASE};
}


std::uint8_t kate::Interpreter::random_uint8() {
  return dist(e);
}

void kate::Interpreter::step() {
  fetch();
  decode();
  execute();

  ++cycle_counter;
}

void kate::Interpreter::vblank_trigger() {
  is_vblank = true;
}

void kate::Interpreter::fetch() {
  if (program_counter >= 0x4000) {
    throw invalid_address(crashdump("PC out of range"));
  }
  cur_inst = {0, INSTRUCTION::NOP, 0, 0, 0};

  // save address of current instruction for debug purposes
  prev_program_counter = program_counter;

  // fetch next instruction and increment PC
  cur_inst.raw <<= 8;
  cur_inst.raw |= memory[program_counter];
  ++program_counter;
  cur_inst.raw <<= 8;
  cur_inst.raw |= memory[program_counter];
  ++program_counter;
}

void kate::Interpreter::decode() {
  std::uint8_t o = (cur_inst.raw & 0xf000) >> 12;
  switch (o) {
    case 0x00: case 0x0e:
      cur_inst.inst = static_cast<INSTRUCTION>(cur_inst.raw & 0x00ff);
      cur_inst.x = (cur_inst.raw & 0x0f00) >> 8;
      break;
    case 0x01: case 0x02: case 0x0a:
      cur_inst.inst = static_cast<INSTRUCTION>(o);
      cur_inst.n = cur_inst.raw & 0x0fff;;
      break;
    case 0x0b:
      cur_inst.inst = static_cast<INSTRUCTION>(o);
      cur_inst.x = (cur_inst.raw & 0x0f00) >> 8;
      cur_inst.n = cur_inst.raw & 0x0fff;;
      break;
    case 0x03: case 0x04: case 0x06: case 0x07: case 0x0c: case 0x0f:
      cur_inst.inst = static_cast<INSTRUCTION>(o);
      cur_inst.x = (cur_inst.raw & 0x0f00) >> 8;
      cur_inst.n = cur_inst.raw & 0x00ff;
      break;
    case 0x05: case 0x08: case 0x09: case 0x0d:
      cur_inst.inst = static_cast<INSTRUCTION>(o);
      cur_inst.x = (cur_inst.raw & 0x0f00) >> 8;
      cur_inst.y = (cur_inst.raw & 0x00f0) >> 4;
      cur_inst.n = (cur_inst.raw & 0x000f);
      break;
  }
}

void kate::Interpreter::execute() {
  // execute instruction
    std::uint8_t r = cur_inst.x;
    std::uint8_t k = registers[cur_inst.x];
  switch (cur_inst.inst) {
    case INSTRUCTION::CLEAR:
      std::fill(output_buffer.begin(), output_buffer.end(), 0);
      break;
    case INSTRUCTION::RET:
      --stack_pointer;
      program_counter = stack[stack_pointer];
      break;
    case INSTRUCTION::JMP:
      prev_program_counter = program_counter - 2;
      program_counter = cur_inst.n;
      break;
    case INSTRUCTION::CALL:
      if (stack_pointer >= 16) {
        throw stack_overflow(crashdump("STACK OVERFLOW"));
      }
      stack[stack_pointer] = program_counter;
      ++stack_pointer;
      program_counter = cur_inst.n;
    case INSTRUCTION::SKIP_EQ_IMM:
      if (registers[cur_inst.x] == cur_inst.n) {
        program_counter += 2;
      }
      break;
    case INSTRUCTION::SKIP_NE_IMM:
      if (registers[cur_inst.x] != cur_inst.n) {
        program_counter += 2;
      }
      break;
    case INSTRUCTION::SKIP_EQ_REG:
      if (registers[cur_inst.x] == registers[cur_inst.y]) {
        program_counter += 2;
      }
      break;
    case INSTRUCTION::SKIP_NE_REG:
      if (registers[cur_inst.x] != registers[cur_inst.y]) {
        program_counter += 2;
      }
      break;
    case INSTRUCTION::MOV:
      registers[cur_inst.x] = cur_inst.n;
      break;
    case INSTRUCTION::ADD:
      registers[cur_inst.x] += cur_inst.n;
      break;
    case INSTRUCTION::ALU:
      alu.x = registers[cur_inst.x];
      alu.y = registers[cur_inst.y];
      alu.f = registers[0xf];
      alu.op = static_cast<ALU_OP>(cur_inst.n);
      alu.execute();
      registers[cur_inst.x] = alu.x;
      registers[0xf] = alu.f;

      break;
    case INSTRUCTION::LDI:
      index_register = cur_inst.n;
      break;
    case INSTRUCTION::JMP_OFF:
      prev_program_counter = program_counter - 2;
      if (quirks_jump_high_nubble_as_register) {
        program_counter = cur_inst.n + registers[cur_inst.x];
      } else {
        program_counter = cur_inst.n + registers[0];
      }
      break;
    case INSTRUCTION::RANDOM:
      registers[cur_inst.x] = random_uint8() & cur_inst.n;
      break;
    case INSTRUCTION::DRAW: _DXYN(); break;
    case INSTRUCTION::KEY_EQ:
      if (key_states[registers[cur_inst.x]] == true) {
        prev_program_counter = program_counter;
        program_counter += 2;
      }
      break;
    case INSTRUCTION::KEY_NE:
      if (key_states[registers[cur_inst.x]] == false) {
        prev_program_counter = program_counter;
        program_counter += 2;
      }
      break;
    case INSTRUCTION::MISC: _FXNN(); break;
    default:
      throw invalid_instruction(crashdump("NOT YET IMPLEMENTED"));
  }
}

void kate::Interpreter::_DXYN() {
  if (quirks_vblank_wait && !is_vblank) {
    // soft-block
    program_counter = prev_program_counter;
    return;
  }

  // offsets wrap, drawing does not
  std::uint8_t h_offset = registers[cur_inst.x];
  std::uint8_t v_offset = registers[cur_inst.y];

  if (quirks_sprite_clipping) {
    h_offset %= SCR_W;
    v_offset %= SCR_H;
  }

  // clear flags register
  registers[0xf] = 0;

  // each row in the sprite is 1 byte wide, stored sequentially
  for (std::size_t i = 0; i < cur_inst.n; ++i) {
    std::uint8_t data = memory[index_register + i];
    std::uint8_t ypos = v_offset + i;
    if (!quirks_sprite_clipping) {
      ypos %= SCR_H;
    }
    std::size_t pixel_offset = ypos * SCR_W;

    // loop over pixels
    for (int p = 7; p >= 0; --p) {
      std::uint8_t xpos = h_offset + (7 - p);
      if (!quirks_sprite_clipping) {
        xpos %= SCR_W;
      }
      std::size_t pixel_index = xpos + pixel_offset;

      bool pixel = (data >> p) & 1;

      // set flag if pixel will be turned off
      if (pixel & output_buffer[pixel_index]) {
        registers[0xf] = 1;
      }

      // screen is updated via xor
      output_buffer[pixel_index] ^= pixel;

      // skip to next line if edge of screen reached
      if (quirks_sprite_clipping && (xpos >= SCR_W)) {
        break;
      }
    }

    // stop drawing if off bottom of screen
    if (quirks_sprite_clipping && (ypos >= SCR_H)) {
      break;
    }
  }
  is_vblank = false;
}

void kate::Interpreter::_FXNN() {
  switch (static_cast<MISC_OP>(cur_inst.n)) {
    case MISC_OP::GET_DT:
      registers[cur_inst.x] = delay_timer;
      break;
    case MISC_OP::GET_KEY:
      // if last key_event is not a release, soft-block
      if (!is_blocking) {
        last_key_event = {0, KEY_EVENT::NONE};
        is_blocking = true;
      }

      if (last_key_event.second != KEY_EVENT::RELEASE) {
        program_counter -= 2;
      } else {
        registers[cur_inst.x] = last_key_event.first;
        is_blocking = false;
        last_key_event = {0, KEY_EVENT::NONE};
      }
      break;
    case MISC_OP::SET_DT:
      delay_timer = registers[cur_inst.x];
      break;
    case MISC_OP::SET_ST:
      sound_timer = registers[cur_inst.x];
      break;
    case MISC_OP::GET_CHAR:
      index_register = char_pointer + (registers[cur_inst.x & 0xf] * 5);
      break;
    case MISC_OP::ADD_IR:
      index_register += registers[cur_inst.x];
      break;
    case MISC_OP::BCD:
      memory[index_register]     =  registers[cur_inst.x]        / 100;
      memory[index_register + 1] = (registers[cur_inst.x] % 100) /  10;
      memory[index_register + 2] =  registers[cur_inst.x] %  10;
      break;
    case MISC_OP::STORE_REG:
      for (std::size_t i = 0; i <= cur_inst.x; ++i) {
        memory[index_register + i] = registers[i];
      }

      if (quirks_increment_index_register) {
        // Note: the index register points to the address _after_ the
        // last value written (write + increment for each register)
        index_register += cur_inst.x + 1;
      }
      break;
    case MISC_OP::LOAD_REG:
      for (std::size_t i = 0; i <= cur_inst.x; ++i) {
        registers[i] = memory[index_register + i];
      }

      if (quirks_increment_index_register) {
        // Note: the index register points to the address _after_ the
        // last value written (write + increment for each register)
        index_register += cur_inst.x + 1;
      }
      break;
    default:
      throw invalid_instruction(crashdump("NOT YET IMPLEMENTED"));
  }
}
