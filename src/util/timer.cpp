#include "timer.hpp"

utils::Clock::Clock() : epoch(utils::hires_clock::now()) {}

utils::seconds utils::Clock::get() const {
  time_point now = hires_clock::now();
  return seconds(now - epoch);
}
