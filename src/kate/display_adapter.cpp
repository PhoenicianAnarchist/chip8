#include <algorithm>

#include "display_adapter.hpp"

kate::Display::Display()
: _is_waiting_for_data(true), _is_waiting_for_vblank(true) {
  output_buffer.resize(SCR_W * SCR_H);
  std::fill(output_buffer.begin(), output_buffer.end(), 0);
}

bool kate::Display::is_waiting_for_data() {
  return _is_waiting_for_data;
}

bool kate::Display::is_waiting_for_vblank() {
  #ifdef QUIRK_V
  return _is_waiting_for_vblank;
  #endif

  return false;
}

void kate::Display::data_interrupt() {
  _is_waiting_for_data = false;
}

void kate::Display::vblank_interrupt() {
  _is_waiting_for_vblank = false;
}

std::uint8_t kate::Display::draw() {
  #ifdef QUIRK_C
  h_offset %= SCR_W;
  v_offset %= SCR_H;
  #endif

  // clear flags register
  f = 0;

  // each row in the sprite is 1 byte wide, stored sequentially
  for (std::size_t i = 0; i < line_count; ++i) {
    std::uint8_t data = sprite_data[i];
    std::uint8_t ypos = v_offset + i;

    #ifndef QUIRK_C
    ypos %= SCR_H;
    #endif

    std::size_t pixel_offset = ypos * SCR_W;

    // loop over pixels
    for (int p = 7; p >= 0; --p) {
      std::uint8_t xpos = h_offset + (7 - p);
      #ifndef QUIRK_C
      xpos %= SCR_W;
      #endif

      std::size_t pixel_index = xpos + pixel_offset;

      bool pixel = (data >> p) & 1;

      // set flag if pixel will be turned off
      if (pixel & output_buffer[pixel_index]) {
        f = 1;
      }

      // screen is updated via xor
      output_buffer[pixel_index] ^= pixel;

      // skip to next line if edge of screen reached
      #ifdef QUIRK_C
      if (xpos >= SCR_W) {
        break;
      }
      #endif
    }

    // stop drawing if off bottom of screen
    #ifdef QUIRK_C
    if (ypos >= SCR_H) {
      break;
    }
    #endif
  }

  _is_waiting_for_data = true;
  _is_waiting_for_vblank = true;
  return f;
}

void kate::Display::clear() {
  std::fill(output_buffer.begin(), output_buffer.end(), 0);
}

const std::vector<std::uint8_t>& kate::Display::get_output_buffer() const {
  return output_buffer;
}
