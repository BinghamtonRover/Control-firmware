#include "relays.h"

void Relay::setup() {
    pinMode(relayPin, OUTPUT);
}

void Relay::update() {
    if (softwareState) {
        turnOn();
    } else {
        turnOff();
    }
}

void Relay::turnOn() {
    digitalWrite(relayPin, HIGH);
    softwareState = true;
    relayData = BoolState_ON;
}

void Relay::turnOff() {
    digitalWrite(relayPin, LOW);
    softwareState = false;
    relayData = BoolState_OFF;
}

void Relay::handleCommand(BoolState command) { 
    switch (command) {
        case BoolState_ON:
            softwareState = true;
            break;
        case BoolState_OFF:
            softwareState = false;
            break;
        case BoolState_BOOL_UNDEFINED:
            break;
    }
}

void Relays::setup() {
    backLeftMotor.setup();
    Serial.println("Back left motor relay initialized");

    backRightMotor.setup();
    Serial.println("Back right motor relay initialized");

    frontLeftMotor.setup();
    Serial.println("Front left motor relay initialized");

    frontRightMotor.setup();
    Serial.println("Front right motor relay initialized");
    
    middleLeftMotor.setup();
    Serial.println("Middle left motor relay initialized");

    middleRightMotor.setup();
    Serial.println("Middle right motor relay initialized");

    arm.setup();
    Serial.println("Arm relay initialized");

    science.setup();
    Serial.println("Science relay initialized");


    backLeftMotor.turnOn();
    Serial.println("Back left motor relay engaged");

    delay(100);
    backRightMotor.turnOn();
    Serial.println("Back right motor relay engaged");

    delay(100);
    frontLeftMotor.turnOn();
    Serial.println("Front left motor relay engaged");

    delay(100);
    frontRightMotor.turnOn();
    Serial.println("Front right motor relay engaged");

    delay(100);
    middleLeftMotor.turnOn();
    Serial.println("Middle left motor relay engaged");

    delay(100);
    middleRightMotor.turnOn();
    Serial.println("Middle right motor relay engaged");

    delay(100);
    arm.turnOn();
    Serial.println("Arm relay engaged");

    delay(100);
    science.turnOn();
    Serial.println("Science relay engaged");
}

void Relays::update() {
    backLeftMotor.update();
    backRightMotor.update();
    frontLeftMotor.update();
    frontRightMotor.update();
    middleLeftMotor.update();
    middleRightMotor.update();
    arm.update();
    science.update();
}

void Relays::handleCommand(RelaysCommand command) {
    backLeftMotor.handleCommand(command.back_left_motor);
    
    backRightMotor.handleCommand(command.back_right_motor);
    frontLeftMotor.handleCommand(command.front_left_motor);
    frontRightMotor.handleCommand(command.front_right_motor);
    middleLeftMotor.handleCommand(command.middle_left_motor);
    middleRightMotor.handleCommand(command.middle_right_motor);
    arm.handleCommand(command.arm);
    science.handleCommand(command.science);
}

// void OverrideSwitch::setup() {
//     pinMode(switchPin, INPUT);
// }