
#include <Arduino.h>

#include "led_strip.h"

// red pin = 2
// green pin = 3
// blue pin = 36

void LedStrip::setup() {
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
}

void LedStrip::setColor(ProtoColor color) {
  // Turn off all pins first
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);

  // Turn on the requested color
  switch (color) {
    case ProtoColor::ProtoColor_PROTO_COLOR_UNDEFINED:
    case ProtoColor::ProtoColor_UNLIT:
      break;
    case ProtoColor::ProtoColor_RED:
      digitalWrite(red, HIGH);
      break;
    case ProtoColor::ProtoColor_GREEN:
      digitalWrite(green, HIGH);
      break;
    case ProtoColor::ProtoColor_BLUE:
      digitalWrite(blue, HIGH);
      break;
  }
}

void LedStrip::red() {
  setColor(ProtoColor::ProtoColor_RED);
  data.color = ProtoColor::ProtoColor_RED;
  oldColor = ProtoColor::ProtoColor_RED;
}

void LedStrip::green() {
  setColor(ProtoColor::ProtoColor_GREEN);
  data.color = ProtoColor::ProtoColor_GREEN;
  oldColor = ProtoColor::ProtoColor_GREEN;
}

void LedStrip::blue() {
  setColor(ProtoColor::ProtoColor_BLUE);
  data.color = ProtoColor::ProtoColor_BLUE;
  oldColor = ProtoColor::ProtoColor_BLUE;
}

void LedStrip::off() {
  setColor(ProtoColor::ProtoColor_UNLIT);
  data.color = ProtoColor::ProtoColor_UNLIT;
}

void LedStrip::update() {
  if (shouldBlink && !isOn) {
    off();
  } else {
    setColor(oldColor);
    data.color = oldColor;
  }
  isOn = !isOn;
}

void LedStrip::handleCommand(DriveCommand command) {
  switch (command.color) {
    case ProtoColor::ProtoColor_PROTO_COLOR_UNDEFINED: break;
    case ProtoColor::ProtoColor_RED: red(); break;
    case ProtoColor::ProtoColor_GREEN: green(); break;
    case ProtoColor::ProtoColor_BLUE: blue(); break;
    case ProtoColor::ProtoColor_UNLIT: off(); break;
  }
  switch (command.blink) {
    case BoolState::BoolState_BOOL_UNDEFINED: break;
    case BoolState::BoolState_ON: shouldBlink = true; break;
    case BoolState::BoolState_OFF: shouldBlink = false; break;
  }
}

