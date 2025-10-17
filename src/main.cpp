/**
 * @file main.cpp
 * @author Louis Bertrand <louis@bertrandtech.ca>
 * @brief Send pulses to the bGeigieZen
 * @version 0.2
 * @date 2025-10-17
 * @details Target: Arduino on Adrafruit ItsyBitsy M4
 *
 * @copyright Copyright (c) 2023-2025 - BSD Two-Clause Licence
 *
 * This is an attempt to generate statistically predictable pulses to
 * simulate radioactive decay. At regular intervals (currently 10ms), a new 
 * 32-bit pseudorandom integer is generated and compared to a threshold.
 * If the number is below the threshold, a pulse is generated. In effect,
 * this is the same as running 1/interval experiments every second. The 
 * experiments are approximately independent of each other, limited only
 * by the short term unpredictability of the Mersenne Twister.
 * A pseudorandom sequence is used to provide repeatability.
 */

#include <Arduino.h>
#include <RBD_Timer.h>
#include <button_debounce.h>
#include <SFMT.h>

// SIMD Fast Mersenne Twister (but not using SIMD)
#define SEED 0x43313337  // It spells 'C137'
sfmt_t sfmt;  // Pseudorandom sequence generator


// Target CPM values
// Push button to toggle between low and high.
// Defaults to high if no button present
constexpr uint32_t HIGH_RATE = 48000;  // Approx. 1100 CPM
constexpr uint32_t LOW_RATE = 1200;  // Approx. 30 CPM
constexpr uint32_t THRESHOLD_CONSTANT = UINT32_MAX >> 18;  // Like Murphy's Constant, this will be variable.
constexpr uint32_t PULSE_DURATION = 1000;  // Microseconds
RBD::Timer repetion_timer{10};  // 1/constant trials per second (adjust later)
constexpr uint32_t BLINK_TIME = 20; // Milliseconds to flash onboard LED
RBD::Timer blink_timer{BLINK_TIME};
RBD::Timer minute_timer{60000};  // Output a CSV record every 1 minute

// Hardware constants
constexpr int PULSE_PIN = 7; // pulse this pin low then high
constexpr int BUTTON_PIN = 9; // Toggle rates on high-low transition.
constexpr int LED_PIN = 13; // Arduino onboard LED

Debouncer button{BUTTON_PIN_0};  // Active low GPIO pin

void setup()
{
  Serial.begin(9600);  // Terminal to IDE
  Serial1.begin(38400); // Serial1 is on the Rx and Tx pins at the corner of the ItsyBitsy M4
  digitalWrite(LED_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, HIGH);
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Start the PRN generator, check for consistency
  // Using seed 1234, the first 5 numbers should be
  // 3440181298 1564997079 1510669302 2930277156 1452439940
  sfmt_init_gen_rand(&sfmt, SEED);
  // Get the first 5 PRNs, send to terminal to verfiy
  for( int i = 0; i < 5; i+=1) {
    uint32_t nextrandom = sfmt_genrand_uint32(&sfmt);
    Serial.print(nextrandom); Serial.print(" ");
  }
  Serial.println("\nmillis,cpm");  // CSV header

  minute_timer.restart();
  repetion_timer.restart();
}

void loop()
{
  static uint32_t threshold = HIGH_RATE * THRESHOLD_CONSTANT;  // Guess at a reasonable threshold
  static bool fast = true;
  static int cpm = 0;  // count pulses every 1 minute

  button.ButtonProcess(digitalRead(BUTTON_PIN));  // feed the debouncer
  if(button.ButtonPressed(BUTTON_PIN_0)) {  // action on leading edge of button press
    if(fast) {
      fast = false;
      threshold = LOW_RATE * THRESHOLD_CONSTANT; // lower threshold, fewer hits
    }
    else {
      fast = true;
      threshold = HIGH_RATE * THRESHOLD_CONSTANT;  // higher threshold, more hits
    }
  }

  if (repetion_timer.onRestart())
  {
    // Generate a random integer and blink if it meets the threshold
    uint32_t nextrandom = sfmt_genrand_uint32(&sfmt);
    if(nextrandom < threshold) {
      blink_timer.restart();
      digitalWrite(PULSE_PIN, LOW);
      delayMicroseconds(PULSE_DURATION);
      digitalWrite(PULSE_PIN, HIGH);
      cpm += 1;
    }
  }
  if (blink_timer.onExpired())
  {
    digitalWrite(LED_PIN, LOW);
  }

  // 1-minute statistics
  if(minute_timer.onRestart()) {
    Serial.print(millis()); Serial.print(",");
    Serial.print(cpm); Serial.println("");
    cpm = 0;  // restart counting pulses
  }
}

