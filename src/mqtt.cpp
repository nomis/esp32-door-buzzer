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

#include "door/mqtt.h"

#include <Arduino.h>

#include <functional>
#include <string>
#include <utility>

#include "app/config.h"
#include "door/app.h"

#ifndef PSTR_ALIGN
# define PSTR_ALIGN 4
#endif

using ::app::Config;

static const char __pstr__logger_name[] __attribute__((__aligned__(PSTR_ALIGN))) PROGMEM = "mqtt";

namespace door {

uuid::log::Logger MQTT::logger_{FPSTR(__pstr__logger_name), uuid::log::Facility::AUTH};

const std::string MQTT::OPEN_TOPIC = "door/main/open";
const std::string MQTT::BUZZER_TOPIC = "door/main/buzzer";
const std::string MQTT::UPTIME_TOPIC = "door/main/uptime";

MQTT::MQTT(App &app) : app_(app) {
}

void MQTT::start() {
	using namespace std::placeholders;
	Config config;

	client_.setBufferSize(512);
	client_.setCallback(std::bind(&MQTT::receive, this, _1, _2, _3));

	connect_ = !config.mqtt_hostname().empty();
}

void MQTT::loop() {
	if (app_.connected()) {
		if (reconnect_) {
			Config config;

			if (client_.connected()) {
				logger_.info("Disconnecting");
				client_.disconnect();
			}
			last_connect_us_ = 0;

			connect_ = !config.mqtt_hostname().empty();
			reconnect_ = false;
		}

		if (connect_ && !client_.connected()
				&& (!last_connect_us_ || esp_timer_get_time() - last_connect_us_ > TIMEOUT_US)) {
			Config config;

			id_ = String("door_") + String(ESP.getEfuseMac(), HEX);
			hostname_ = config.mqtt_hostname();
			username_ = config.mqtt_username();
			password_ = config.mqtt_password();

			logger_.info("Connecting");
			client_.setServer(hostname_.c_str(), 1883);

			if (username_.empty() || password_.empty()) {
				client_.connect(id_.c_str());
			} else {
				client_.connect(id_.c_str(),
					username_.c_str(),
					password_.c_str());
			}
			last_connect_us_ = esp_timer_get_time();
			last_report_us_ = 0;

			if (client_.connected()) {
				logger_.info("Connected");
				client_.subscribe(OPEN_TOPIC.c_str());
			} else {
				logger_.err("Connection failed");
			}
		}

		if (client_.connected()) {
			report();
		}
	} else if (!reconnect_) {
		reconnect_ = connect_;
		connect_ = false;
	}

	client_.loop();
}

void MQTT::reconnect() {
	reconnect_ = true;
}

void MQTT::buzzer_pressed(bool on) {
	buzzer_ = on;
	if (app_.connected() && client_.connected()) {
		client_.publish(BUZZER_TOPIC.c_str(), (buzzer_ ? "1" : "0"));
	}
}

void MQTT::report() {
	if (!last_report_us_ || esp_timer_get_time() - last_report_us_ > TIMEOUT_US) {
		last_report_us_ = esp_timer_get_time();
		client_.publish(UPTIME_TOPIC.c_str(),
			std::to_string(last_report_us_).c_str());
		client_.publish(BUZZER_TOPIC.c_str(), (buzzer_ ? "1" : "0"));
	}
}

void MQTT::receive(char *topic, uint8_t *payload, unsigned int length) {
	std::string payload_str{(const char*)payload, length};

	if (topic == OPEN_TOPIC) {
		app_.open();
	}
}

} // namespace door
