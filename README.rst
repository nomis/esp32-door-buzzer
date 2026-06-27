ESP32 Door Buzzer
=================

Overview
--------

Software to connect an ESP32-S3 to a door buzzer and door lock.

Purpose
-------

Use an ESP32-S3 to release the door lock and monitor when the door buzzer is
pressed.

The 12V AC from the buzzer is converted to DC and regulated to 5V for use with
the optocoupler.

The normally open relay releases the door lock by connecting 0V AC to complete
the unlocking circuit.

Dependencies
------------

`PlatformIO <https://platformio.org/>`_

Build
-----

``platformio run``

Install
-------

``platformio run -t upload``

Usage
-----

Connect to the serial console and press Ctrl-L to begin. Configure WiFi network,
MQTT and timeouts. Type ``help`` for a list of commands.

* `Hardware schematic <https://github.com/nomis/esp32-door-buzzer-pcb>`_
