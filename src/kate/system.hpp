#ifndef __KATE_CHIP8_INTERPRETER__
#define __KATE_CHIP8_INTERPRETER__

#include <array>
#include <string>
#include <vector>

#include <cstdint>

#include "config.hpp"
#include "alu.hpp"
#include "cpu.hpp"
#include "display_adapter.hpp"
#include "memory_controller.hpp"

namespace kate {
  enum class KEY_EVENT {
    NONE,
    PRESS,
    RELEASE
  };

  class System {
  public:
    System();

    void reset();
    void load_rom(const std::vector<std::uint8_t> &rom);
    const std::vector<std::uint8_t> &get_output_buffer() const;
    std::uint8_t get_sound_timer() const;

    void decrement_timers();
    void keypress(std::uint8_t k);
    void keyrelease(std::uint8_t k);

    void step();
    void vblank_trigger();

    std::string debug_line();
    std::string debug_filename();
  private:
    std::uint16_t fetch();
    Instruction decode(std::uint16_t raw);
    void execute();

    void _DXYN();
    void _FXNN();

    CPU cpu;
    ALU alu;
    MemoryController memory;
    Display display_adapter;

    std::array<std::uint8_t, 16> key_states;
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

    bool is_blocking;
    std::pair<int, KEY_EVENT> last_key_event;
  };
}

#endif // __KATE_CHIP8_INTERPRETER__
