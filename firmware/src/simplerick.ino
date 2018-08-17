/*
 * simplerick.ino
 *
 * This file is part of the SimpleRick firmware.
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

uint16_t dead_time = 5; // time between scan lines, in microseconds

Servo myservo;
uint8_t servo_angle = 90;

/*
 * @brief
 * Calibrate the TGC by using DAC output to vary TGC period.
 *
 * @param[in] desired scanline time, in microseconds
 */
void calibrate_TGC(const uint16_t scanline_time) {
	uint8_t DAC_output = 254;
	uint8_t previous_DAC_output = 255;
	uint16_t TGC_period = 0;

	Serial.println("Calibrating TGC...");
	Serial.printf("Setting scanline time to %d microseconds\n", scanline_time);

	while (1) {
		digitalWriteFast(TGC_RESET, HIGH); // reset TGC
		analogWrite(TGC_CTRL, DAC_output);

		// measure TGC_period
		elapsedMicros since_TGC_start = 0;
		digitalWriteFast(TGC_RESET, LOW); // enable TGC
		while (digitalRead(TGC_TRIG) == LOW) {} // wait for trigger
		TGC_period = since_TGC_start;
		Serial.printf("Measured TGC period as %d microseconds\n", TGC_period);

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
}

// call on interrupt
void reset_TGC() {
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

	// Initialize serial communication over USB
	Serial.begin(9600);
	Serial.println("Welcome to SimpleRick!");

	// Set initial state of outputs
	digitalWrite(LED_ACQUISITION, HIGH);
	digitalWrite(TGC_RESET, LOW);
	digitalWrite(PULSE_INA, LOW);
	digitalWrite(PULSE_INB, HIGH);
	calibrate_TGC(200);
}

void loop() {
	// TGC reset logic
	if (digitalRead(TGC_TRIG) == HIGH) {
		digitalWriteFast(TGC_RESET, HIGH);
		delayMicroseconds(dead_time);
		digitalWriteFast(TGC_RESET, LOW);

		digitalWriteFast(PULSE_INA, HIGH); // pulse of high voltage
		digitalWriteFast(PULSE_INA, LOW);
		digitalWriteFast(PULSE_INB, LOW); // damping
		delayMicroseconds(2);
		digitalWriteFast(PULSE_INB, HIGH);
	}
}
