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

/******************************************************************************
/ Interpreter                                                                 /
******************************************************************************/
kate::Interpreter::Interpreter() {
  reset();
}

void kate::Interpreter::reset() {
  ram.fill(0);
  registers.fill(0);
  stack.fill(0);
  program_counter = entry_point;
  stack_pointer = 0;
  index_register = 0;
  delay_timer = 0;
  sound_timer = 0;

  output_buffer.fill(0);
  current_instruction = 0;
  cycle_counter = 0;

  std::copy(
    char_data.begin(),
    char_data.end(),
    &ram[char_pointer]
  );
}

void kate::Interpreter::load_rom(const std::vector<std::uint8_t> &rom) {
  reset();

  std::copy(
    rom.begin(),
    rom.end(),
    &ram[entry_point]
  );
}

const std::array<std::uint8_t, kate::SCR_W * kate::SCR_H> &
kate::Interpreter::get_output_buffer() const {
  return output_buffer;
}

void kate::Interpreter::step() {
  fetch();
  decode();
  execute();

  ++cycle_counter;
}

void kate::Interpreter::fetch() {
  if (program_counter >= 0x4000) {
    std::stringstream ss;
    ss << "PC out of range : 0x" ;
    ss << std::hex << std::setw(4) << std::setfill('0') << program_counter;
    throw std::out_of_range(ss.str());
  }

  // fetch next instruction and increment PC
  current_instruction <<= 8;
  current_instruction |= ram[program_counter];
  ++program_counter;
  current_instruction <<= 8;
  current_instruction |= ram[program_counter];
  ++program_counter;
}

void kate::Interpreter::decode() {
  std::uint8_t first = (current_instruction & 0xf000) >> 12;

  switch (first) {
    case 0x00: _00E0(); break;
    case 0x01: _1NNN(); break;
    case 0x06: _6XNN(); break;
    case 0x07: _7XNN(); break;
    case 0x0a: _ANNN(); break;
    case 0x0d: _DXYN(); break;

    default:
      std::stringstream ss;
      ss << "Invalid Instruction : " ;
      ss << std::hex << std::setw(4) << std::setfill('0');
      ss << current_instruction;

      throw invalid_instruction(ss.str());
  }
}

void kate::Interpreter::execute() {
  // execute instruction
}

void kate::Interpreter::_00E0() {
  // clear the output buffer
  output_buffer.fill(0);
}

void kate::Interpreter::_1NNN() {
  // jump
  program_counter = current_instruction & 0x0fff;
}

void kate::Interpreter::_6XNN() {
  // move
  std::uint8_t x = (current_instruction & 0x0f00) >> 8;
  std::uint8_t n = (current_instruction & 0x00ff);

  registers[x] = n;
}

void kate::Interpreter::_7XNN() {
  // add
  std::uint8_t x = (current_instruction & 0x0f00) >> 8;
  std::uint8_t n = (current_instruction & 0x00ff);

  registers[x] += n;
}

void kate::Interpreter::_ANNN() {
  // set index register
  index_register = current_instruction & 0x0fff;
}

void kate::Interpreter::_DXYN() {
  std::uint8_t x = (current_instruction & 0x0f00) >> 8;
  std::uint8_t y = (current_instruction & 0x00f0) >> 4;
  std::uint8_t n = (current_instruction & 0x000f);

  // offsets wrap, drawing does not
  std::uint8_t h_offset = registers[x] % SCR_W;
  std::uint8_t v_offset = registers[y] % SCR_H;

  // clear flags register
  registers[0xf] = 0;

  // each row in the sprite is 1 byte wide, stored sequentially
  for (std::size_t i = 0; i < n; ++i) {
    std::uint8_t data = ram[index_register + i];
    std::uint8_t ypos = v_offset + i;
    std::size_t pixel_offset = ypos * SCR_W;

    // loop over pixels
    for (int p = 7; p >= 0; --p) {
      std::uint8_t xpos = h_offset + (7 - p);
      std::size_t pixel_index = xpos + pixel_offset;

      bool pixel = (data >> p) & 1;

      // set flag if pixel will be turned off
      if (pixel & output_buffer[pixel_index]) {
        registers[0xf] = 1;
      }

      // screen is updated via xor
      output_buffer[pixel_index] ^= pixel;

      // skip to next line if edge of screen reached
      if (xpos >= SCR_W) {
        break;
      }
    }

    // stop drawing if off bottom of screen
    if (ypos >= SCR_H) {
      break;
    }
  }
}
