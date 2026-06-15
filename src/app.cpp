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

#include "door/app.h"

#include <Arduino.h>

namespace door {

App::App() : mqtt_(*this), buzzer_(BUZZER_PIN, mqtt_) {
}

void App::start() {
	lock_.init();
	app::App::start();
	buzzer_.start();
	lock_.start();
	mqtt_.start();
}

void App::loop() {
	app::App::loop();
	buzzer_.loop();
	mqtt_.loop();
}

void App::open() {
	lock_.open();
}

void App::mqtt_reconnect() {
	mqtt_.reconnect();
}

} // namespace door
