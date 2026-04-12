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
	commands->add_command({F("open")}, open_door);
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
