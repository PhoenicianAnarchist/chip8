#ifndef __TIMER_HPP__
#define __TIMER_HPP__
#include <chrono>

namespace utils {
  using hires_clock = std::chrono::high_resolution_clock;
  using time_point  = std::chrono::system_clock::time_point;
  using seconds     = std::chrono::duration<double>;

  class Clock {
  public:
    Clock();
    seconds get() const;
  private:
    time_point epoch;
  };
}

#endif // __TIMER_HPP__
