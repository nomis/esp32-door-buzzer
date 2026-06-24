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

#pragma once

#include <Arduino.h>

#include <memory>

#include <uuid/log.h>

namespace door {

class MQTT;

class Buzzer {
public:
	Buzzer(int pin, MQTT &mqtt);

	void start();
	void loop();

private:
	static uuid::log::Logger switch_logger_;
	static uuid::log::Logger buzzer_logger_;

	const int pin_;
	MQTT &mqtt_;
	uint64_t last_switch_us_{0};
	uint64_t last_buzzer_us_{0};
	uint64_t debounce_time_us_{0};
	uint64_t release_time_us_{0};
	bool switch_active_{false};
	bool buzzer_active_{false};
};

} // namespace door
