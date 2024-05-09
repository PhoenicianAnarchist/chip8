#include <algorithm> // std::copy
#include <iomanip>
#include <sstream>
#include <iostream>

#include "error.hpp"
#include "system.hpp"

/******************************************************************************
/ System                                                                 /
******************************************************************************/
kate::System::System() {
  reset();
}

void kate::System::reset() {
  cpu.reset();
  cpu.program_counter = entry_point;
  display_adapter.clear();

  key_states.fill(0);

  delay_timer = 0;
  sound_timer = 0;

  is_blocking = false;
  last_key_event = {0, KEY_EVENT::NONE};

  memory.get_active_bank().clear();
  memory.write(char_pointer, kate::character_data);
}

void kate::System::load_rom(const std::vector<std::uint8_t> &rom) {
  reset();
  memory.write(entry_point, rom);
}

const std::vector<std::uint8_t> &kate::System::get_output_buffer() const {
  return display_adapter.get_output_buffer();
}

std::uint8_t kate::System::get_sound_timer() const {
  return sound_timer;
}

void kate::System::decrement_timers() {
  if (delay_timer > 0) {
    --delay_timer;
  }
  if (sound_timer > 0) {
    --sound_timer;
  }
}

void kate::System::keypress(std::uint8_t k) {
  key_states[k] = true;
  last_key_event = {k, KEY_EVENT::PRESS};
}

void kate::System::keyrelease(std::uint8_t k) {
  key_states[k] = false;
  last_key_event = {k, KEY_EVENT::RELEASE};
}

void kate::System::step() {
  std::uint16_t raw = fetch();
  cpu.cur_inst = decode(raw);

  switch (cpu.cur_inst.op) {
    // graphics
    case OPCODE::CLEAR: display_adapter.clear(); break;
    case OPCODE::DRAW: _DXYN(); break;

    // misc
    case OPCODE::MISC: _FXNN(); break;

    // alu
    case OPCODE::ALU:
      alu.x = cpu.registers[cpu.cur_inst.x];
      alu.y = cpu.registers[cpu.cur_inst.y];
      alu.f = cpu.registers[0xf];
      alu.op = static_cast<ALU_OP>(cpu.cur_inst.n);
      alu.execute();
      cpu.registers[cpu.cur_inst.x] = alu.x;
      cpu.registers[0xf] = alu.f;

      break;

    // input
    case OPCODE::KEY_EQ:
      if (key_states[cpu.registers[cpu.cur_inst.x]] == true) {
        cpu.prev_program_counter = cpu.program_counter;
        cpu.program_counter += 2;
      }
      break;
    case OPCODE::KEY_NE:
      if (key_states[cpu.registers[cpu.cur_inst.x]] == false) {
        cpu.prev_program_counter = cpu.program_counter;
        cpu.program_counter += 2;
      }
      break;

    // cpu
    default: cpu.execute(); break;
  }

  ++cpu.cycle_counter;
}

void kate::System::vblank_trigger() {
  display_adapter.vblank_interrupt();
}

std::string kate::System::debug_line() {
  std::stringstream ss;
  ss << "PC : " << kate::hex_string(cpu.prev_program_counter, 4) << " . ";
  ss << "inst : ";
  ss << kate::hex_string(cpu.cur_inst.raw, 4, false) << " ( ";
  ss << decode_OPCODE(cpu.cur_inst.op) << " )";

  return ss.str();
}

std::string kate::System::debug_filename() {
  std::stringstream ss;
  ss << kate::hex_string(cpu.cycle_counter, 8, false) << "_";
  ss << kate::hex_string(cpu.prev_program_counter, 4, false) << "_";
  ss << kate::hex_string(cpu.cur_inst.raw, 4, false);

  return ss.str();
}


std::uint16_t kate::System::fetch() {
  std::uint16_t raw;

  // save address of current instruction for debug purposes
  cpu.prev_program_counter = cpu.program_counter;

  // fetch next instruction and increment PC
  raw <<= 8;
  raw |= memory[cpu.program_counter];
  ++cpu.program_counter;
  raw <<= 8;
  raw |= memory[cpu.program_counter];
  ++cpu.program_counter;

  return raw;
}

kate::Instruction kate::System::decode(std::uint16_t raw) {
  Instruction inst = {raw, OPCODE::NOP, 0, 0, 0};

  std::uint8_t o = (inst.raw & 0xf000) >> 12;
  switch (o) {
    case 0x00: case 0x0e:
      inst.op = static_cast<OPCODE>(inst.raw & 0x00ff);
      inst.x = (inst.raw & 0x0f00) >> 8;
      break;
    case 0x01: case 0x02: case 0x0a:
      inst.op = static_cast<OPCODE>(o);
      inst.n = inst.raw & 0x0fff;;
      break;
    case 0x0b:
      inst.op = static_cast<OPCODE>(o);
      inst.x = (inst.raw & 0x0f00) >> 8;
      inst.n = inst.raw & 0x0fff;;
      break;
    case 0x03: case 0x04: case 0x06: case 0x07: case 0x0c: case 0x0f:
      inst.op = static_cast<OPCODE>(o);
      inst.x = (inst.raw & 0x0f00) >> 8;
      inst.n = inst.raw & 0x00ff;
      break;
    case 0x05: case 0x08: case 0x09: case 0x0d:
      inst.op = static_cast<OPCODE>(o);
      inst.x = (inst.raw & 0x0f00) >> 8;
      inst.y = (inst.raw & 0x00f0) >> 4;
      inst.n = (inst.raw & 0x000f);
      break;
  }

  return inst;
}

void kate::System::_DXYN() {
  if (display_adapter.is_waiting_for_data()) {
    display_adapter.h_offset = cpu.registers[cpu.cur_inst.x];
    display_adapter.v_offset = cpu.registers[cpu.cur_inst.y];
    display_adapter.line_count = cpu.cur_inst.n;

    for (std::size_t i = 0; i < cpu.cur_inst.n; ++i) {
      display_adapter.sprite_data[i] = memory[cpu.index_register + i];
    }

    display_adapter.data_interrupt();
  }

  if (display_adapter.is_waiting_for_vblank()) {
    cpu.program_counter = cpu.prev_program_counter;
    return;
  }

  cpu.registers[0xf] = display_adapter.draw();
}

void kate::System::_FXNN() {
  switch (static_cast<MISC_OP>(cpu.cur_inst.n)) {
    case MISC_OP::GET_DT:
      cpu.registers[cpu.cur_inst.x] = delay_timer;
      break;
    case MISC_OP::GET_KEY:
      // if last key_event is not a release, soft-block
      if (!is_blocking) {
        last_key_event = {0, KEY_EVENT::NONE};
        is_blocking = true;
      }

      if (last_key_event.second != KEY_EVENT::RELEASE) {
        cpu.program_counter -= 2;
      } else {
        cpu.registers[cpu.cur_inst.x] = last_key_event.first;
        is_blocking = false;
        last_key_event = {0, KEY_EVENT::NONE};
      }
      break;
    case MISC_OP::SET_DT:
      delay_timer = cpu.registers[cpu.cur_inst.x];
      break;
    case MISC_OP::SET_ST:
      sound_timer = cpu.registers[cpu.cur_inst.x];
      break;
    case MISC_OP::GET_CHAR:
      cpu.index_register = char_pointer
                         + (cpu.registers[cpu.cur_inst.x & 0xf] * 5);
      break;
    case MISC_OP::ADD_IR:
      cpu.index_register += cpu.registers[cpu.cur_inst.x];
      break;
    case MISC_OP::BCD:
      memory[cpu.index_register]     =  cpu.registers[cpu.cur_inst.x]        / 100;
      memory[cpu.index_register + 1] = (cpu.registers[cpu.cur_inst.x] % 100) /  10;
      memory[cpu.index_register + 2] =  cpu.registers[cpu.cur_inst.x] %  10;
      break;
    case MISC_OP::STORE_REG:
      for (std::size_t i = 0; i <= cpu.cur_inst.x; ++i) {
        memory[cpu.index_register + i] = cpu.registers[i];
      }

      #ifdef QUIRK_I
      // Note: the index register points to the address _after_ the
      // last value written (write + increment for each register)
      cpu.index_register += cpu.cur_inst.x + 1;
      #endif

      break;
    case MISC_OP::LOAD_REG:
      for (std::size_t i = 0; i <= cpu.cur_inst.x; ++i) {
        cpu.registers[i] = memory[cpu.index_register + i];
      }

      #ifdef QUIRK_I
      // Note: the index register points to the address _after_ the
      // last value written (write + increment for each register)
      cpu.index_register += cpu.cur_inst.x + 1;
      #endif

      break;
  }
}
