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

#include "door/console.h"

#include <memory>
#include <string>
#include <vector>

#include <uuid/console.h>
#include <uuid/log.h>

#include "door/app.h"
#include "app/config.h"
#include "app/console.h"

using ::uuid::flash_string_vector;
using ::uuid::console::Commands;
using ::uuid::console::Shell;
using LogLevel = ::uuid::log::Level;
using LogFacility = ::uuid::log::Facility;

using ::app::AppShell;
using ::app::CommandFlags;
using ::app::Config;
using ::app::ShellContext;

namespace door {

static constexpr inline AppShell &to_app_shell(Shell &shell) {
	return static_cast<AppShell&>(shell);
}

static constexpr inline App &to_app(Shell &shell) {
	return static_cast<App&>(to_app_shell(shell).app_);
}

static constexpr inline DoorShell &to_shell(Shell &shell) {
	return static_cast<DoorShell&>(shell);
}

#define NO_ARGUMENTS std::vector<std::string>{}

static void open_door(Shell &shell, const std::vector<std::string> &arguments) {
	to_app(shell).open();
}

static inline void setup_commands(std::shared_ptr<Commands> &commands) {
	auto buzzer_release_time = [] (Shell &shell,
			const std::vector<std::string> &arguments __attribute__((unused))) {
		Config config;
		unsigned long value = config.buzzer_release_time_ms();

		shell.printfln(F("Buzzer release time: %lums"), value);
	};

	auto door_open_time = [] (Shell &shell,
			const std::vector<std::string> &arguments __attribute__((unused))) {
		Config config;
		unsigned long value = config.door_open_time_ms();

		shell.printfln(F("Door open time: %lums"), value);
	};

	commands->add_command({F("open")}, open_door);

	commands->add_command(ShellContext::MAIN, CommandFlags::USER, CommandFlags::ADMIN,
		flash_string_vector{F("buzzer"), F("release"), F("time")},
		buzzer_release_time);

	commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN,
				flash_string_vector{F("buzzer"), F("release"), F("time")},
				flash_string_vector{F("[milliseconds]")},
				[=] (Shell &shell, const std::vector<std::string> &arguments) {
		Config config;

		if (!arguments.empty()) {
			unsigned long value = 0;
			int ret = std::sscanf(arguments[0].c_str(), "%lu", &value);

			if (ret < 1) {
				shell.println(F("Invalid value"));
				return;
			}

			config.buzzer_release_time_ms(value);
			config.commit();
		}

		buzzer_release_time(shell, NO_ARGUMENTS);
	});

	commands->add_command(ShellContext::MAIN, CommandFlags::USER, CommandFlags::ADMIN,
		flash_string_vector{F("door"), F("open"), F("time")},
		door_open_time);

	commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN,
				flash_string_vector{F("door"), F("open"), F("time")},
				flash_string_vector{F("[milliseconds]")},
				[=] (Shell &shell, const std::vector<std::string> &arguments) {
		Config config;

		if (!arguments.empty()) {
			unsigned long value = 0;
			int ret = std::sscanf(arguments[0].c_str(), "%lu", &value);

			if (ret < 1) {
				shell.println(F("Invalid value"));
				return;
			}

			config.door_open_time_ms(value);
			config.commit();
		}

		door_open_time(shell, NO_ARGUMENTS);
	});

	commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN,
				flash_string_vector{F("set"), F("mqtt"), F("hostname")},
				flash_string_vector{F("[hostname]")},
				[=] (Shell &shell, const std::vector<std::string> &arguments) {
		Config config;

		if (!arguments.empty()) {
			config.mqtt_hostname(arguments[0].c_str());
			config.commit();
		}

		shell.printfln(F("MQTT hostname: %ls"), config.mqtt_hostname().c_str());
		to_app(shell).mqtt_reconnect();
	});

	commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN,
				flash_string_vector{F("set"), F("mqtt"), F("username")},
				flash_string_vector{F("[username]")},
				[=] (Shell &shell, const std::vector<std::string> &arguments) {
		Config config;

		if (!arguments.empty()) {
			config.mqtt_username(arguments[0].c_str());
			config.commit();
		}

		shell.printfln(F("MQTT username: %ls"), config.mqtt_username().c_str());
		to_app(shell).mqtt_reconnect();
	});

	commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN,
				flash_string_vector{F("set"), F("mqtt"), F("password")},
				[=] (Shell &shell, const std::vector<std::string> &arguments) {
		Config config;

		shell.enter_password(F("Enter new password: "), [] (Shell &shell, bool completed, const std::string &password1) {
			if (completed) {
				shell.enter_password(F("Retype new password: "), [password1] (Shell &shell, bool completed, const std::string &password2) {
					if (completed) {
						if (password1 == password2) {
							Config config;
							config.mqtt_password(password2);
							config.commit();
							shell.println(F("MQTT password updated"));
							to_app(shell).mqtt_reconnect();
						} else {
							shell.println(F("Passwords do not match"));
						}
					}
				});
			}
		});
	});
}

DoorShell::DoorShell(app::App &app, Stream &stream, unsigned int context, unsigned int flags)
	: AppShell(app, stream, context, flags) {

}

} // namespace door

namespace app {

void setup_commands(std::shared_ptr<Commands> &commands) {
	door::setup_commands(commands);
}

} // namespace app
