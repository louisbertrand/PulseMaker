
#include <Arduino.h>
#include <vector>

/**
 * @brief Stimulus sequence definition
 * 
 * @arg target_cpm: number of pulses per minute to generate.
 * @arg pulse_width: in milliseconds.
 * @arg duration: test duration in seconds.
 */
class Stimulus {

  Stimulus( uint16_t target_cpm, float pulse_width=1.0, float duration=60) {
  }
  /**
   * @brief Execute this particular test item.
   * @returns Number of pulse acknowledgements received (dots)
   */
  uint16_t execute(void);
};

/**
 * @brief A collection of tests to be executed in sequence.
 * 
 */
struct Test {
  char name[16] = "";
  uint16_t pin_number;
  std::vector<Stimulus> tests = {};
};
