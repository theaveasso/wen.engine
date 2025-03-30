/**
 * @file wen_clock.hpp
 * @brief A clock system for tracking elapsed time.
 */
#ifndef WEN_CORE_WEN_CLOCK_HPP_
#define WEN_CORE_WEN_CLOCK_HPP_

/**
 * @struct wen_clock_t
 * @brief Represents a clock for measuring elapsed time.
 */
typedef struct wen_clock_t {
  double start_time; /**< The time when the clock started. */
  double elapsed;    /**< The total elapsed time since the clock started .*/
} wen_clock_t;

/**
 * @brief Updates the elapsed time of the clock.
 * @param clock_ Pointer to the clock instance to update.
 */
void clock_update(wen_clock_t* clock_);

/**
 * @brief Starts or resets the clock.
 * @param clock_ Pointer to the clock instance to start.
 */
void clock_start(wen_clock_t* clock_);

/**
 * @brief Stops the clock.
 * @param clock_ Pointer to the clock instance to stop.
 */
void clock_stop(wen_clock_t* clock_);

#endif // WEN_CORE_WEN_CLOCK_HPP_
