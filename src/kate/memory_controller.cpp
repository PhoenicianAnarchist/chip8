#include <exception>
#include <sstream>

#include "error.hpp"
#include "memory_controller.hpp"

///////////////////////////////////////////////////////////////////////////////
// ROM

kate::ROM::ROM(const std::array<std::uint8_t, 0x1000>& data)
: memory(data) {}

const std::uint8_t& kate::ROM::at(std::size_t index) const {
  return memory.at(index);
}

std::size_t kate::ROM::size() const {
  return memory.size();
}

const std::array<std::uint8_t, 0x1000>& kate::ROM::data() const {
  return memory;
}

///////////////////////////////////////////////////////////////////////////////
// RAM

kate::RAM::RAM(const std::array<std::uint8_t, 0x1000>& data)
: memory(data) {}

const std::uint8_t& kate::RAM::at(std::size_t index) const {
  return memory.at(index);
}

std::uint8_t& kate::RAM::operator[](std::size_t index) {
  return memory[index];
}

std::size_t kate::RAM::size() const {
  return memory.size();
}

std::array<std::uint8_t, 0x1000>& kate::RAM::data() {
  return memory;
}

void kate::RAM::clear(std::uint8_t value) {
  memory.fill(value);
}

///////////////////////////////////////////////////////////////////////////////
// MemoryController

kate::MemoryController::MemoryController() : ram(ram_banks[0]) {}

std::uint8_t kate::MemoryController::at(std::size_t index) const {
  return ram.at(index);
}

std::uint8_t& kate::MemoryController::operator[](std::size_t index) {
  if (index >= ram.size()) {
    std::stringstream ss;
    ss << "Memory access out of range : " << index;
    throw invalid_address(ss.str());
  }

  return ram[index];
}

void kate::MemoryController::write(
  std::size_t index, const std::vector<std::uint8_t>& data
) {
  if ((index + data.size()) > ram.size()) {
    std::stringstream ss;
    ss << "Insertion out of bounds @ " << index;
    throw std::out_of_range(ss.str());
  }

  std::copy(
    data.begin(),
    data.end(),
    ram.data().begin() + index
  );
}

void kate::MemoryController::attach(std::size_t index, const RAM& ram) {
  if (index > ram_banks.size()) {
    std::stringstream ss;
    ss << "RAM bank out of bounds @ " << index;
    throw std::out_of_range(ss.str());
  }
  ram_banks[index] = ram;
}

kate::RAM& kate::MemoryController::get_active_bank() {
  return ram;
}
