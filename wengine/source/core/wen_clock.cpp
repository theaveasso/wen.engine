#include "wen/core/wen_clock.hpp"

#include "wen/platform/wen_platform.hpp"

void clock_update(wen_clock_t* clock_) {
  if (clock_->start_time != 0) {
    clock_->elapsed = platform_get_absolute_time() - clock_->start_time;
  }
}

void clock_start(wen_clock_t* clock_) {
  clock_->start_time = platform_get_absolute_time();
  clock_->elapsed = 0;
}

void clock_stop(wen_clock_t* clock_) {
  clock_->start_time = 0;
}
