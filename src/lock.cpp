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

#include "door/lock.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

#include <algorithm>
#include <thread>

#include "app/config.h"

#ifndef PSTR_ALIGN
# define PSTR_ALIGN 4
#endif

using ::app::Config;

static const char __pstr__logger_name[] __attribute__((__aligned__(PSTR_ALIGN))) PROGMEM = "lock";

namespace door {

uuid::log::Logger Lock::logger_{FPSTR(__pstr__logger_name), uuid::log::Facility::AUTH};

Lock::Lock(int pin) : WakeupThread("lock", true), pin_(pin) {
}

void Lock::init() {
	digitalWrite(pin_, LOW);
	pinMode(pin_, OUTPUT);
}

void Lock::start() {
	std::thread t;
	make_thread(t, "lock", 4096, 19, &Lock::run_loop, this);
	t.detach();
}

void Lock::open() {
	Config config;

	stop_ = esp_timer_get_time() + config.door_open_time_ms() * 1000ULL;
	wake_up();
}

unsigned long Lock::run_tasks() {
	uint64_t now = esp_timer_get_time();

	esp_task_wdt_reset();

	if (now < stop_) {
		if (!active_) {
			logger_.notice("Opening door lock");
			digitalWrite(pin_, HIGH);
			active_ = true;
		}
	} else {
		if (active_) {
			digitalWrite(pin_, LOW);
			logger_.notice("Released door lock");
			active_ = false;
		}
	}

	if (active_) {
		return std::min((uint64_t)WATCHDOG_INTERVAL_MS, (stop_ - now) / 1000ULL);
	} else {
		return WATCHDOG_INTERVAL_MS;
	}
}

} // namespace door
