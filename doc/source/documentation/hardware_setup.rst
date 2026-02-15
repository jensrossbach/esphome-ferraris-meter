.. _hardware-setup:

===============
Hardware-Aufbau
===============

Hardware-seitig wird lediglich ein ESP-Mikrocontroller (z.B. ESP8266 oder ESP32, inkl. Spannungsversorgung) und ein Infrarotsensor (z.B. TCRT5000) benötigt. Für die reine Funktionalität des Ferraris Meters reicht ein ESP8266 als Mikrocontroller völlig aus. Für den Infrarotsensor gibt es fertige TCRT5000-basierte Breakout-Module mit 3,3V-5V Eingangsspannung, die auch über einen regelbaren Widerstand (Potentiometer) verfügen, um den digitalen Ausgang zu kalibrieren. Diese TCRT5000-Module haben 4 Pins - VCC und GND für die Stromversorgung des Sensor-Chips sowie einen digitalen Ausgang D0 und einen analogen Ausgang A0.

Beim Platzieren des Sensors auf der Abdeckplatte des Ferraris-Stromzählers ist ein wenig Geschick und Präzisionsarbeit gefragt. Das Infrarot Sender/Empfänger-Paar des Sensors muss mittig millimetergenau über der Drehscheibe ausgerichtet werden und geradlinig auf die Drehscheibe zeigen.

Die Ferraris-Plattform unterstützt prinzipiell folgende Aufbauvarianten:

- :ref:`Verwendung eines einzelnen Infrarotsensors über den digitalen Ausgang <energy-meter-digital>`
- :ref:`Verwendung eines einzelnen Infrarotsensors über den analogen Ausgang <energy-meter-analog>`
- :ref:`Verwendung mehrerer Infrarotsensoren <energy-meter-multiple>`
