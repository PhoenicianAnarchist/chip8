#ifndef __KATE_CONFIG__
#define __KATE_CONFIG__

#include <array>
#include <cstdint>

namespace kate {
  // This value controls how fast the interpreter runs. Some programs may need
  // this to be altered to run at the proper speed.
  constexpr std::size_t instructions_per_second = 700;

  /****************************************************************************
  / Some implementations differ in behaviour for various reasons, these flags /
  / will enable/disable "quirks".                                             /
  ****************************************************************************/

  // 8XY1, 8XY2, and 8XY3 reset the flags register to zero
  constexpr bool quirks_enable_flags_reset = true;

  // FX55 and FX65 increment the index register
  constexpr bool quirks_increment_index_register = true;

  // Sprite drawing waits for the vblank interupt
  constexpr bool quirks_vblank_wait = true;

  // Sprite coordinates wrap, but sprites are clipped
  constexpr bool quirks_sprite_clipping = true;

  // 8XY6 and 8XYE ignore the Y register and shift X instead
  constexpr bool quirks_shifting_ignores_y = false;

  // Treat the highest nibble in BNNN as the register address
  constexpr bool quirks_jump_high_nubble_as_register = false;

  /****************************************************************************
  / Internal Configuration.                                                   /
  / These values should not be changed unless you know what you're doing.     /
  ****************************************************************************/

  // size of the display screen
  constexpr std::size_t SCR_W = 64;
  constexpr std::size_t SCR_H = 32;
  constexpr std::size_t FADERATE = 16;
  constexpr std::size_t display_refresh_rate = 60;

  // location of the first address to execute
  constexpr std::uint16_t entry_point   = 0x0200;

  // built-in character sprite data
  constexpr std::uint16_t char_pointer  = 0x0050;
  constexpr std::array<std::uint8_t, 16*5> char_data = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
}

#endif // __KATE_CONFIG__
