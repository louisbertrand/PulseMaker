/**
 * @file main.cpp
 * @author Louis Bertrand <louis@bertrandtech.ca>
 * @brief Send pulses to the bGeigieZen
 * @version 0.1
 * @date 2023-07-16
 * @details Target: Arduino on Adrafruit ItsyBitsy M4
 *
 * @copyright Copyright (c) 2023 - BSD Two-Clause Licence
 *
 */
#include <Arduino.h>
#include <RBD_Timer.h>
#include <button_debounce.h>

// Target CPM values
// Push button to toggle between low and high.
// Defaults to high if no button present
constexpr uint32_t HIGH_RATE = 1200;
constexpr uint32_t LOW_RATE = 30;
constexpr uint32_t PULSE_DURATION = 1000;  // Microseconds
RBD::Timer repetion_timer{1000 * 60 / LOW_RATE};  // initially slow rate
constexpr uint32_t BLINK_TIME = 100; // Milliseconds to flash onboard LED
RBD::Timer blink_timer{BLINK_TIME};

constexpr int PULSE_PIN = 7; // pulse this pin low then high
constexpr int BUTTON_PIN = 9; // Toggle rates on high-low transition.
constexpr int LED_PIN = 13; // Arduino onboard LED

Debouncer button{BUTTON_PIN_0};

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
}

void loop()
{
  static uint32_t rate = HIGH_RATE;
  static bool fast = true;

  button.ButtonProcess(digitalRead(BUTTON_PIN));  // feed the debouncer
  if(button.ButtonPressed(BUTTON_PIN_0)) {  // action on leading edge of button press
    if(fast) {
      rate = LOW_RATE;
      fast = false;
      blink_timer.setTimeout(5 * BLINK_TIME);
    }
    else {
      rate = HIGH_RATE;
      fast = true;
      blink_timer.setTimeout(BLINK_TIME);
    }
    repetion_timer.setTimeout(1000 * 60L / rate);
  }

  if (repetion_timer.onRestart())
  {
    blink_timer.restart();
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(PULSE_PIN, LOW);
    delayMicroseconds(PULSE_DURATION);
    digitalWrite(PULSE_PIN, HIGH);
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
