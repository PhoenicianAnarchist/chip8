#ifndef __MEMORY_CONTROLLER_HPP__
#define __MEMORY_CONTROLLER_HPP__

#include <array>
#include <vector>
#include <cstdint>

namespace kate {
  constexpr std::size_t rom_bank_size = 0x1000;
  constexpr std::size_t rom_bank_count = 1;
  constexpr std::size_t ram_bank_size = 0x1000;
  constexpr std::size_t ram_bank_count = 1;

  class ROM {
  public:
    ROM() = default;
    ROM(const std::array<std::uint8_t, rom_bank_size>& data);

    const std::uint8_t& at(std::size_t index) const;

    std::size_t size() const;
    const std::array<std::uint8_t, rom_bank_size>& data() const;
  private:
    mutable std::array<std::uint8_t, rom_bank_size> memory;
  };

  class RAM {
  public:
    RAM() = default;
    RAM(const std::array<std::uint8_t, ram_bank_size>& data);

    const std::uint8_t& at(std::size_t index) const;
    std::uint8_t& operator[](std::size_t index);

    std::size_t size() const;
    std::array<std::uint8_t, ram_bank_size>& data();
  private:
    std::array<std::uint8_t, ram_bank_size> memory;
  };

  class MemoryController {
  public:
    MemoryController();

    std::uint8_t at(std::size_t index) const;
    std::uint8_t& operator[](std::size_t index);

    // for attaching banks
    void attach(std::size_t index, const RAM& ram);

    // writes a block of data
    void write(std::size_t index, const std::vector<std::uint8_t>& data);

    // temporary interface
    [[deprecated]] RAM& get_active_bank();
  private:
    RAM& ram;
    std::array<RAM, ram_bank_count> ram_banks;
  };
}

#endif // __MEMORY_CONTROLLER_HPP__
