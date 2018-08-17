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

// pin mappings
#define SERVO_PWM 10
#define TGC_CTRL A14 // DAC
#define TGC_TRIG 11
#define TGC_RESET 12
#define PULSE_INA 6
#define PULSE_INB 7
#define LED_ACQUISITION 13
#define BUTTON_TRIG 4

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
	digitalWrite(PULSE_INB, LOW);
	analogWrite(TGC_CTRL, 200);
}

void loop() {
	// TGC reset logic
	if (digitalRead(TGC_TRIG) == HIGH) {
		digitalWrite(TGC_RESET, HIGH);
		delayMicroseconds(5);
		digitalWrite(TGC_RESET, LOW);

		digitalWriteFast(PULSE_INA, HIGH);
		digitalWriteFast(PULSE_INB, HIGH);
		//delayMicroseconds(1);
		digitalWriteFast(PULSE_INA, LOW);
		digitalWriteFast(PULSE_INB, LOW);
		delayMicroseconds(5);
	}
}
