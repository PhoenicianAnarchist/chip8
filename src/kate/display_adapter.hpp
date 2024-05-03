#ifndef __DISPLAY_ADAPTER__
#define __DISPLAY_ADAPTER__

#include <array>
#include <vector>

#include <cstdint>

namespace kate {
  constexpr std::size_t SCR_W = 64;
  constexpr std::size_t SCR_H = 32;
  constexpr std::size_t display_refresh_rate = 60;

  class Display {
  public:
    Display();

    bool is_waiting_for_data();
    bool is_waiting_for_vblank();

    std::uint8_t draw();
    void clear();

    void data_interrupt();
    void vblank_interrupt();
    const std::vector<std::uint8_t>& get_output_buffer() const;

    std::uint8_t f;
    std::uint8_t h_offset;
    std::uint8_t v_offset;
    std::uint8_t line_count;
    std::array<std::uint8_t, 16> sprite_data;

  private:
    bool _is_waiting_for_data;
    bool _is_waiting_for_vblank;
    std::vector<std::uint8_t> output_buffer;
  };
}

#endif // __DISPLAY_ADAPTER__
