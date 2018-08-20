/*
 * simplerick.ino
 *
 * This file is part of the SimpleRick firmware.
 *
 * Copyright (C) 2018 William Meng
 *
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <Servo.h>

// pin mappings
#define SERVO_PWM 10
#define TGC_CTRL A14 // DAC
#define TGC_TRIG 11
#define TGC_RESET 12
#define PULSE_INA 6
#define PULSE_INB 7
#define LED_ACQUISITION 13
#define BUTTON_TRIG 4

#define NOP3 "nop\n\t""nop\n\t""nop\n\t" // ~ 50 ns delay
#define NOP4 "nop\n\t""nop\n\t""nop\n\t""nop\n\t"
#define NOP6 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t" // ~ 100 ns delay

// TODO: put these "image parameters" in a global struct
uint32_t totalImageMillis = 1000; // total image time, in milliseconds
uint16_t TGC_time = 200;
uint16_t damping_time = 1; // duration of active damping via INB, in microseconds
uint16_t dead_time = 50; // time between scan lines, in microseconds

Servo myservo;
volatile uint8_t servo_angle = 90;
uint16_t startAngle = 30; // angle to start sweep (degrees)
uint16_t angleRange = 90; // range of angles to sweep (degrees)

volatile elapsedMillis millisSinceImageStart = 0;

/*
 * @brief
 *
 * @param[in] totalImageMillis: how many milliseconds the image should span
 * @param[in] angleRange: how many degrees the image should span
 */
int getServoAngle() {
	return millisSinceImageStart * angleRange / totalImageMillis;
}

/*
 * @brief
 * Calibrate the TGC by using DAC output to vary TGC period.
 *
 * @param[in] scanline_time: desired scanline time, in microseconds
 */
void calibrate_TGC(const uint16_t scanline_time) {
	uint8_t DAC_output = 254;
	uint8_t previous_DAC_output = 255;
	uint16_t TGC_period = 0;

	Serial.println("Calibrating TGC...");
	Serial.printf("Attempting to set scanline time to %d microseconds...\n", scanline_time);

	while (1) {
		digitalWriteFast(TGC_RESET, HIGH); // reset TGC
		analogWrite(TGC_CTRL, DAC_output);

		// measure TGC_period
		elapsedMicros since_TGC_start = 0;
		digitalWriteFast(TGC_RESET, LOW); // enable TGC
		while (digitalRead(TGC_TRIG) == LOW) {} // wait for trigger
		TGC_period = since_TGC_start;
		Serial.printf("TGC period=%d us\n", TGC_period);

		// adjust DAC output accordingly. check for oscillations around set point
		if (TGC_period > scanline_time && previous_DAC_output < DAC_output) { // TGC is too slow
			previous_DAC_output = DAC_output;
			DAC_output++;
		}
		else if (TGC_period < scanline_time && previous_DAC_output > DAC_output) { // TGC is too fast
			previous_DAC_output = DAC_output;
			DAC_output--;
		}
		else {
			break;
		}
		if (DAC_output == 0 || DAC_output == 255) {
			Serial.println("WARNING: specified scanline time is not possible.");
			break;
		}
	}
	Serial.printf("Finished TGC calibration with DAC output %d.\n", DAC_output);
	Serial.flush();
	
}


#define BUFSIZE 1024

// TODO: use ArduinoJSON to parse serial commands
/*
 * Parse a serial command, and perform the appropriate action.
 * The command API is a work-in-progress and subject to change.
 *
 * Example commands:
 * =================
 * set angle 20 			set motor angle to 20 degrees
 * set tgc time 200  		set TGC period to 200 microseconds
 * set dead time 50 		set dead time to 50 microseconds
 * set damping time 2 		set damping time to 2 microseconds
 * sweep angle 90 			sweep motor +90 degrees
 * scan lines 50 			acquire 50 scan lines without moving motor
 * scan angle 90 			acquire data while sweeping motor +90 degrees
 *
 * TODO: use the Arduino JSON library instead
 */
void parse_command() {
	static char recvbuf[BUFSIZE];

	if (Serial.available()) {
		char *p = recvbuf;
		uint16_t i;
		for (i = 0; i < sizeof(recvbuf); i++) {
			char incoming_byte = Serial.read();
			if (incoming_byte < 0) { // no more data to read
				recvbuf[i] = '\0'; // NULL termination
				break;
			}
			else {
				recvbuf[i] = incoming_byte;
			}
		}
		recvbuf[sizeof(recvbuf) - 1] = '\0'; // NULL terminate in case of truncation
	}

	if (strncmp(recvbuf, "set angle ", strlen("set angle "))) {
		sscanf(recvbuf, "set angle %d", servo_angle);
		myservo.write(servo_angle);
	}
	else if (strncmp(recvbuf, "END", strlen("END"))) {
	}
	else {
		Serial.printf("Failed to parse command: %s\n", recvbuf);
		Serial.send_now();
	}
}

void setup() {
	// Initialize inputs & outputs
	pinMode(SERVO_PWM, OUTPUT);
	pinMode(TGC_CTRL, OUTPUT);
	pinMode(TGC_TRIG, INPUT);
	pinMode(TGC_RESET, OUTPUT);
	pinMode(PULSE_INA, OUTPUT);
	pinMode(PULSE_INB, OUTPUT);
	pinMode(LED_ACQUISITION, OUTPUT);
	pinMode(BUTTON_TRIG, INPUT_PULLUP);

	// Initialize serial communication
	Serial.begin(115200);
	Serial.println("Welcome to SimpleRick!");

	// Calibrate TGC
	calibrate_TGC(TGC_time);

	// Set initial state of outputs
	digitalWrite(LED_ACQUISITION, LOW);
	digitalWrite(TGC_RESET, HIGH); // gain=0
	digitalWrite(PULSE_INA, LOW); // disconnect HV
	digitalWrite(PULSE_INB, HIGH); // disconnect damping
	myservo.write(startAngle);
}

void loop() {
	// TODO: check for serial data

	if (digitalReadFast(BUTTON_TRIG) == LOW) { // start image when user presses button
		myservo.write(startAngle);
		digitalWrite(LED_ACQUISITION, HIGH);
		Serial.println("Starting image sweep");
		millisSinceImageStart = 0;

		while (millisSinceImageStart < totalImageMillis) {
			// TGC reset logic
			if (digitalRead(TGC_TRIG) == HIGH) {
				digitalWriteFast(TGC_RESET, HIGH);
				delayMicroseconds(dead_time);
				digitalWriteFast(TGC_RESET, LOW);

				digitalWriteFast(PULSE_INA, HIGH); // pulse of high voltage
				digitalWriteFast(PULSE_INA, LOW);
				digitalWriteFast(PULSE_INB, LOW); // damping
				delayMicroseconds(damping_time);
				digitalWriteFast(PULSE_INB, HIGH);
			}
			servo.write(getServoAngle()); // update servo angle
		}

		// reset everything to idle state
		digitalWriteFast(TGC_RESET, HIGH);
		digitalWrite(LED_ACQUISITION, LOW);
		myservo.write(startAngle);
	}
}
