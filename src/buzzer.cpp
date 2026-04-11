/*
 * esp32-door-buzzer - ESP32 Door Buzzer
 * Copyright 2026  Simon Arlott
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "door/buzzer.h"

#include <Arduino.h>

#ifndef PSTR_ALIGN
# define PSTR_ALIGN 4
#endif

static const char __pstr__logger_name[] __attribute__((__aligned__(PSTR_ALIGN))) PROGMEM = "buzzer";

namespace door {

uuid::log::Logger Buzzer::logger_{FPSTR(__pstr__logger_name), uuid::log::Facility::AUTH};

Buzzer::Buzzer(int pin) : pin_(pin) {
}

void Buzzer::start() {
	pinMode(pin_, INPUT_PULLUP);
}

void Buzzer::loop() {
	bool active = digitalRead(pin_) == LOW;

	if (active_ != active) {
		uint64_t now_us = esp_timer_get_time();

		if (active) {
			logger_.info("Pressed after %llums", (now_us - last_us_) / 1000ULL);
		} else {
			logger_.info("Released after %llums", (now_us - last_us_) / 1000ULL);
		}

		active_ = active;
		last_us_ = now_us;
	}
}

} // namespace door
