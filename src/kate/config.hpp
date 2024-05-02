#ifndef __KATE_CONFIG__
#define __KATE_CONFIG__

#include <array>
#include <cstdint>

#include <GLFW/glfw3.h>

namespace kate {
  // This value controls how fast the interpreter runs. Some programs may need
  // this to be altered to run at the proper speed.
  constexpr std::size_t instructions_per_second = 700;

  // slowly fade the display to reduce flickering
  constexpr bool do_display_fade = true;
  constexpr std::uint8_t display_fade_rate = 64;

  // keybindings
  constexpr std::array<int, 16> key_map = {
    GLFW_KEY_X, // keypad 0
    GLFW_KEY_1, // keypad 1
    GLFW_KEY_2, // keypad 2
    GLFW_KEY_3, // keypad 3
    GLFW_KEY_Q, // keypad 4
    GLFW_KEY_W, // keypad 5
    GLFW_KEY_E, // keypad 6
    GLFW_KEY_A, // keypad 7
    GLFW_KEY_S, // keypad 8
    GLFW_KEY_D, // keypad 9
    GLFW_KEY_Z, // keypad a
    GLFW_KEY_C, // keypad b
    GLFW_KEY_4, // keypad c
    GLFW_KEY_R, // keypad d
    GLFW_KEY_F, // keypad e
    GLFW_KEY_V  // keypad f
  };

  /****************************************************************************
  * Some implementations differ in behaviour for various reasons, these flags *
  * will enable/disable "quirks".                                             *
  *****************************************************************************
  * quirks_enable_flags_reset                                                 *
  *     8XY1, 8XY2, and 8XY3 reset the flags register to zero                 *
  *                                                                           *
  * quirks_increment_index_register                                           *
  *     FX55 and FX65 increment the index register                            *
  *                                                                           *
  * quirks_vblank_wait                                                        *
  *     Sprite drawing waits for the vblank interupt                          *
  *                                                                           *
  * quirks_sprite_clipping                                                    *
  *     Sprite coordinates wrap, but sprites are clipped                      *
  *                                                                           *
  * quirks_shifting_ignores_y                                                 *
  *     8XY6 and 8XYE ignore the Y register and shift X instead               *
  *                                                                           *
  * quirks_jump_high_nubble_as_register                                       *
  *     Treat the highest nibble in BNNN as the register address              *
  ****************************************************************************/

  // COSMAC VIP CHIP-8
  constexpr bool quirks_enable_flags_reset            = true;
  constexpr bool quirks_increment_index_register      = true;
  constexpr bool quirks_vblank_wait                   = true;
  constexpr bool quirks_sprite_clipping               = true;
  constexpr bool quirks_shifting_ignores_y            = false;
  constexpr bool quirks_jump_high_nubble_as_register  = false;

  /****************************************************************************
  / Internal Configuration.                                                   /
  / These values should not be changed unless you know what you're doing.     /
  ****************************************************************************/

  // display settings
  constexpr std::size_t SCR_W = 64;
  constexpr std::size_t SCR_H = 32;
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
