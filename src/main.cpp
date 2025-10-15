/**
 * @file main.cpp
 * @author Louis Bertrand <louis@bertrandtech.ca>
 * @brief Send pulses to the bGeigieZen
 * @version 0.1
 * @date 2023-07-16
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

// SIMD Fast Mersenne Twister (go for maximal length, why not?)
#define SFMT_MEXP 19937
#define SEED 0x43313337  // It spells 'C137'
#include "SFMT.h"
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

constexpr int PULSE_PIN = 7; // pulse this pin low then high
constexpr int BUTTON_PIN = 9; // Toggle rates on high-low transition.
constexpr int LED_PIN = 13; // Arduino onboard LED

Debouncer button{BUTTON_PIN_0};  // Active low GPIO pin

void setup()
{
  Serial.begin(38400);  // Terminal to IDE
  Serial1.begin(38400); // Serial1 is on the Rx and Tx pins at the corner of the ItsyBitsy M4
  digitalWrite(LED_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, HIGH);
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  repetion_timer.restart();

  sfmt_init_gen_rand(&sfmt, SEED);
}

void loop()
{
  static uint32_t threshold = HIGH_RATE * THRESHOLD_CONSTANT;  // Guess at a reasonable threshold
  static bool fast = true;

  button.ButtonProcess(digitalRead(BUTTON_PIN));  // feed the debouncer
  if(button.ButtonPressed(BUTTON_PIN_0)) {  // action on leading edge of button press
    if(fast) {
      fast = false;
      threshold = LOW_RATE * THRESHOLD_CONSTANT;  // Guess at a reasonable threshold
    }
    else {
      fast = true;
      threshold = HIGH_RATE * THRESHOLD_CONSTANT;  // Guess at a reasonable threshold
    }
  }

  if (repetion_timer.onRestart())
  {
    // Generate a random integer and blink if it meets the threshold
    uint32_t nextrandom = sfmt_genrand_uint32(&sfmt);
    if(nextrandom < threshold) {
      blink_timer.restart();
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(PULSE_PIN, LOW);
      delayMicroseconds(PULSE_DURATION);
      digitalWrite(PULSE_PIN, HIGH);
    }
  }
  if (blink_timer.onExpired())
  {
    digitalWrite(LED_PIN, LOW);
  }
  if (Serial1.available() > 0)
  {
    Serial.print(Serial1.read());
  }
}
