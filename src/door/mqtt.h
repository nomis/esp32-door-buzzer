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
#include <PubSubClient.h>
#include <WiFi.h>

#include <memory>

#include <uuid/log.h>

namespace door {

class App;

class MQTT {
public:
	MQTT(App &app);

	void start();
	void loop();

	void reconnect();
	void buzzer_pressed(bool on);

private:
	static constexpr uint64_t TIMEOUT_US = 10 * 1000 * 1000ULL;
	static const std::string OPEN_TOPIC;
	static const std::string BUZZER_TOPIC;
	static const std::string UPTIME_TOPIC;
	static uuid::log::Logger logger_;

	void receive(char *topic, uint8_t *payload, unsigned int length);
	void report();

	App &app_;
	WiFiClient wifi_;
	PubSubClient client_{wifi_};
	bool connect_{false};
	bool reconnect_{true};
	String id_;
	std::string hostname_;
	std::string username_;
	std::string password_;
	uint64_t last_connect_us_{0};
	uint64_t last_report_us_{0};
	bool buzzer_{false};
};

} // namespace door
