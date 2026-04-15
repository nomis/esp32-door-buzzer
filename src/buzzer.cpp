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

#include "app/config.h"

#ifndef PSTR_ALIGN
# define PSTR_ALIGN 4
#endif

using ::app::Config;

static const char __pstr__switch_logger_name[] __attribute__((__aligned__(PSTR_ALIGN))) PROGMEM = "switch";
static const char __pstr__buzzer_logger_name[] __attribute__((__aligned__(PSTR_ALIGN))) PROGMEM = "buzzer";

namespace door {

uuid::log::Logger Buzzer::switch_logger_{FPSTR(__pstr__switch_logger_name), uuid::log::Facility::AUTH};
uuid::log::Logger Buzzer::buzzer_logger_{FPSTR(__pstr__buzzer_logger_name), uuid::log::Facility::AUTH};

Buzzer::Buzzer(int pin) : pin_(pin) {
}

void Buzzer::start() {
	pinMode(pin_, INPUT_PULLUP);
}

void Buzzer::loop() {
	bool active = digitalRead(pin_) == LOW;

	if (switch_active_ != active) {
		uint64_t now_us = esp_timer_get_time();

		if (active) {
			switch_logger_.info("Pressed after %llums", (now_us - last_switch_us_) / 1000ULL);

			Config config;

			release_time_us_ = config.buzzer_release_time_ms() * 1000ULL;
		} else {
			switch_logger_.info("Released after %llums", (now_us - last_switch_us_) / 1000ULL);
		}

		switch_active_ = active;
		last_switch_us_ = now_us;
	}

	if (buzzer_active_ != switch_active_) {
		uint64_t now_us = esp_timer_get_time();

		if (switch_active_ || now_us - last_switch_us_ >= release_time_us_) {
			if (switch_active_) {
				buzzer_logger_.info("Pressed after %llums", (now_us - last_buzzer_us_) / 1000ULL);
			} else {
				buzzer_logger_.info("Released after %llums", (now_us - last_buzzer_us_) / 1000ULL);
			}

			buzzer_active_ = switch_active_;
			last_buzzer_us_ = now_us;
		}
	}
}

} // namespace door
