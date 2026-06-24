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

#define MCU_APP_CONFIG_DATA \
	MCU_APP_CONFIG_PRIMITIVE(unsigned long, "", door_open_time_ms, "", 5000) \
	MCU_APP_CONFIG_PRIMITIVE(unsigned long, "", buzzer_debounce_time_ms, "", 5) \
	MCU_APP_CONFIG_PRIMITIVE(unsigned long, "", buzzer_release_time_ms, "", 500) \
	MCU_APP_CONFIG_SIMPLE(std::string, "", mqtt_hostname, "", "") \
	MCU_APP_CONFIG_SIMPLE(std::string, "", mqtt_username, "", "") \
	MCU_APP_CONFIG_SIMPLE(std::string, "", mqtt_password, "", "")

public:
	unsigned long door_open_time_ms() const;
	void door_open_time_ms(unsigned long buzzer_debounce_time_ms);

	unsigned long buzzer_debounce_time_ms() const;
	void buzzer_debounce_time_ms(unsigned long buzzer_debounce_time_ms);

	unsigned long buzzer_release_time_ms() const;
	void buzzer_release_time_ms(unsigned long buzzer_release_time_ms);

	std::string mqtt_hostname() const;
	void mqtt_hostname(const std::string &mqtt_hostname);

	std::string mqtt_username() const;
	void mqtt_username(const std::string &mqtt_username);

	std::string mqtt_password() const;
	void mqtt_password(const std::string &mqtt_password);

private:
	static unsigned long door_open_time_ms_;
	static unsigned long buzzer_debounce_time_ms_;
	static unsigned long buzzer_release_time_ms_;
	static std::string mqtt_hostname_;
	static std::string mqtt_username_;
	static std::string mqtt_password_;
