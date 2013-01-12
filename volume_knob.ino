// a little sketch for adjusting a computer's volume level using a clicky knob
// by Jenna Fox - MIT License
#include "DigiKeyboard.h"
#define KEY_MUTE 127
#define KEY_VOLUME_UP 128
#define KEY_VOLUME_DOWN 129

#define BUTTON_PIN 5
#define KNOB_QUAD_LEFT 1
#define KNOB_QUAD_RIGHT 2

void setup() {
  // set encoder and button pins to be inputs
  pinMode(BUTTON_PIN, INPUT);
  pinMode(KNOB_QUAD_LEFT, INPUT);
  pinMode(KNOB_QUAD_RIGHT, INPUT);
  
  // turn on pullup resistors
  digitalWrite(BUTTON_PIN, HIGH);
  digitalWrite(KNOB_QUAD_LEFT, HIGH);
  digitalWrite(KNOB_QUAD_RIGHT, HIGH);
  
  delay(1);
}

void loop() {
  DigiKeyboard.update();
  // check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(1); // for debouncing
    button_pressed();
    while (digitalRead(BUTTON_PIN) == LOW) DigiKeyboard.update();
    delay(1);
  }
  
  alter_volume(read_knob());
}

void alter_volume(int amount) {
  while (amount) {
    if (amount > 0) {
      DigiKeyboard.sendKeyStroke(KEY_VOLUME_UP);
      amount -= 1;
    } else if (amount < 0) {
      DigiKeyboard.sendKeyStroke(KEY_VOLUME_DOWN);
      amount += 1;
    }
  }
}

void button_pressed() {
  DigiKeyboard.sendKeyStroke(0); //this is generally not necessary but with some older systems it seems to prevent missing the first character after a delay
  DigiKeyboard.sendKeyStroke(KEY_MUTE);
}

// returns the bits from the two knob wires as a 2-bit number
byte knob_bits() {
  return digitalRead(KNOB_QUAD_LEFT) << 1 | digitalRead(KNOB_QUAD_RIGHT);
}

// reads a simple incremental quad encoder knob, returns -1, 0, or +1 indicating if the knob has rotated
// to the left or right by one click. Call this function as often as possible so it handles knobs being turned
// very quickly okay
char read_knob() {
  static byte previous_state; // these two variables keep their values between function calls
  static byte armed;
  byte state = knob_bits();
  char result = 0;
  
  // to ensure bouncy mechanical switch stuff only makes one positive or negative impulse for each mechanical click
  // we "arm" it when it gets half way through a click
  if (state == 0b00) {
    armed = true; // allow one click result
  }
  
  // then when it finishes a click, we consume the one "armed" token and deliver the result just once to the caller
  if (armed == true && state == 0b11) {
    armed = false; // consumed.
    if (previous_state == 0b01) {
      result = -1;
    } else if (previous_state == 0b10) {
      result = +1;
    }
  }
   
  previous_state = state;
  return result;
}



