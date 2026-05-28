// #include <Servo.h>

#include "pinouts.h"
#include "src/utils/BURT_utils.h"
#include "src/control.pb.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define DATA_SEND_INTERVAL 50  // ms (relays was 250 ms, drive was 50ms, so chose the smaller one)
#define MOTOR_UPDATE_INTERVAL 10  // ms

int16_t tfDist = 0;    // Distance to object in centimeters
int16_t tfFlux = 0;    // Strength or quality of return signal
int16_t tfTemp = 0;    // Internal temperature of Lidar sensor chip

const Version version = {major: 1, minor: 3};

const uint8_t errorPin = 33;

void handleCommand(const uint8_t* data, int length);

void handleMotorOutput(const CanMessage& message) {
  motors.handleMotorOutput(message);
}

BurtSerial serial(Device::Device_CONTROL_BOARD, handleCommand, ControlData_fields, ControlData_size);

// AK motors send data in the format (0x29 << 8) | MOTOR_ID
//
// Since we want to capture data from all the motors, the filter is set to include
// all commands starting with the ID 0x2900 (not including motor ID 0)
//
// AK motors use extended IDs, so the extended frame mailbox is listened to
BurtCan<Can1> motorCan(0x2901, 0x29FF, handleMotorOutput, true);

void sendData();
void updateMotors() { motors.sendMotorCommands(motorCan); }
void updateLedStrip() { led_strip.update(); }

BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);
BurtTimer blinkTimer(blinkInterval, updateLedStrip);

void setup() {
  pinMode(errorPin, OUTPUT);

	Serial.begin(9600);
	Serial.println("Initializing Drive subsystem");

	Serial2.begin(115200);
	delay(20);
	tfmp.begin(&Serial2);

	// safe reset lidar
	if (tfmp.sendCommand(SOFT_RESET, 0)) {
		Serial.println("LiDAR init success");
		delay(500);
	} else {
		Serial.println("Error: Unexpected response from LiDAR");
		tfmp.printReply();
	}

	if (tfmp.sendCommand(SET_FRAME_RATE, FRAME_20)) {
		Serial.print("LiDAR rate: ");
		Serial.print(FRAME_20);
		Serial.println("Hz.");
	} else {
		tfmp.printReply();
	}

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	Serial.println("Initializing software...");
	motorCan.setup();
	serial.setup();
	dataTimer.setup();
	motorTimer.setup();
	blinkTimer.setup();

	relays.setup();
	voltageSensor.setup();

	Serial.println("Initializing hardware...");
	motors.setup();
	buttons.setup();
	cameras.setup();
	led_strip.setup();
	voltageSensor.setup();
	temperatureSensor.setup();

	Serial.println("Drive subsystem initialized");
}

void loop() {
	serial.update();
	motorCan.update();
	blinkTimer.update();
	temperatureSensor.update();
	buttons.update();
	voltageSensor.update();
	relays.update();
	dataTimer.update();
	motorTimer.update();

	int16_t dist;
	if (tfmp.getData(dist)) {
		tfDist = dist;
	} else {
		tfDist = 0;
	}
}

void sendData() {
	DriveData driveData = DriveData_init_zero;
	RelaysData relayData = RelaysData_init_zero;

	ControlData controlData = ControlData_init_zero;

	// Drive Motor Data
	driveData.has_front_left_motor = motors.data.has_front_left_motor;
	driveData.has_front_right_motor = motors.data.has_front_right_motor;
	driveData.has_middle_left_motor = motors.data.has_middle_left_motor;
	driveData.has_middle_right_motor = motors.data.has_middle_right_motor;
	driveData.has_back_left_motor = motors.data.has_back_left_motor;
	driveData.has_back_right_motor = motors.data.has_back_right_motor;

	driveData.front_left_motor = motors.data.front_left_motor;

	driveData.front_right_motor = motors.data.front_right_motor;
	driveData.middle_left_motor = motors.data.middle_left_motor;
	driveData.middle_right_motor = motors.data.middle_right_motor;
	driveData.back_left_motor = motors.data.back_left_motor;
	driveData.back_right_motor = motors.data.back_right_motor;

	// Drive output data
	driveData.set_left = motors.data.set_left;
	driveData.left = motors.data.left;
	driveData.set_right = motors.data.set_right;
	driveData.right = motors.data.right;
	driveData.set_throttle = motors.data.set_throttle;
	driveData.throttle = motors.data.throttle;

	// Drive cameras data
	driveData.front_swivel = cameras.data.front_swivel;
	driveData.rear_swivel = cameras.data.rear_swivel;

	// Temperature sensors data
	driveData.battery_temperature = temperatureSensor.data.battery_temperature;

	// Voltage sensor data
	driveData.battery_voltage = voltageSensor.data.battery_voltage;

	// LED strip data
	driveData.color = led_strip.data.color;

	// Send LiDAR Dist
	driveData.lidar_dist = tfDist;

	// Version
	driveData.has_version = true;
	driveData.version = version;

	controlData.has_version = true;
	controlData.version = version;

	controlData.has_drive = true;
	controlData.drive = driveData;

	relayData.arm = relays.arm.relayData;
	relayData.science = relays.science.relayData;
	relayData.front_left_motor = relays.frontLeftMotor.relayData;
	relayData.front_right_motor = relays.frontRightMotor.relayData;
	relayData.back_left_motor = relays.backLeftMotor.relayData;
	relayData.back_right_motor = relays.backRightMotor.relayData;
	relayData.middle_left_motor = relays.middleLeftMotor.relayData;
	relayData.middle_right_motor = relays.middleRightMotor.relayData;

	relayData.battery_voltage = voltageSensor.data.battery_voltage;

	controlData.has_relays = true;
	controlData.relays = relayData;

	serial.send(&controlData);
}

void handleCommand(const uint8_t* data, int length) {
	
	auto command = BurtProto::decode<ControlCommand>(data, length, ControlCommand_fields);
	buttons.handleCommand(command.drive);
	motors.handleCommand(command.drive);
	cameras.handleCommand(command.drive);
	led_strip.handleCommand(command.drive);
	relays.handleCommand(command.relays);
	
}
