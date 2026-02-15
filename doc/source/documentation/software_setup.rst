======================
Software-Konfiguration
======================

Um eine ESPHome-Firmware zu erstellen, muss eine YAML-basierte Konfigurationsdatei erstellt werden. Du kannst eine der in diesem Repository bereitgestellten `Beispielkonfigurationsdateien <https://github.com/jensrossbach/esphome-ferraris-meter/tree/main/example_config>`_ als Ausgangspunkt verwenden und sie an deine Bedürfnisse anpassen.

Prinzipiell gibt es zwei Möglichkeiten, die ESPHome-Firmware zu bauen:


#. `Über Home Assistant mit dem ESPHome Device Compiler Add-on <https://www.esphome.io/guides/getting_started_hassio>`_
#. `Über die Kommandozeile mit dem ESPHome Python-Paket <https://www.esphome.io/guides/getting_started_command_line>`_

Für welche Methode du dich entscheiden solltest, hängt davon ab, wie vertraut du mit ESPHome bist und ob du lieber mit einer grafischen Benutzeroberfläche oder mit der Kommandozeile arbeitest. Außerdem könnte die Leistungsfähigkeit des Hosts, auf dem du die Firmware baust, eine Rolle spielen, um den Vorgang zu beschleunigen.

.. note::

    Es ist **nicht** nötig, dieses Repository zu kopieren ("forken") und die Anpassungen an der Beispielkonfiguration im kopierten Repository vorzunehmen. Stattdessen reicht es aus, die Beispielkonfiguration lokal zu speichern und anzupassen oder die angepasste Konfiguration auf deinem Home Assistant Host abzulegen (sollte die Erstellung der ESPHome-Firmware mithilfe des ESPHome Device Compiler Add-ons erwünscht sein).

Die folgenden Abschnitte beschreiben die wichtigsten Komponenten, die in der Firmware-Konfigurationsdatei enthalten sind.

Ferraris-Komponente
===================

Die Ferraris-Komponente (``ferraris``) ist das Fundament der Ferraris-Plattform und muss hinzugefügt werden, um deren Sensoren, -Aktoren und -Aktionen zu verwenden.

Da es sich um eine benutzerdefinierte Komponente handelt, die nicht Teil von ESPHome ist, muss sie explizit importiert werden. Am einfachsten ist es, die Komponente direkt aus diesem Repository zu laden.

Beispiel
--------

.. code-block:: yaml

    external_components:
      - source: github://jensrossbach/esphome-ferraris-meter
        components: [ferraris]

.. hint::

    Im obigen Beispiel wird der neueste Stand der Komponente aus dem ``main`` Branch des Repositories geladen. Ich empfehle aber, mittels Versionsnummer auf einen freigegebenen Stand zu verweisen, um mehr Kontrolle darüber zu haben, welcher Software-Stand verwendet wird und um besser auf "breaking changes" reagieren zu können. Siehe Beispielkonfiguration, wie das gemacht werden kann.


Die folgenden allgemeinen Einstellungen können konfiguriert werden:

.. list-table::
    :header-rows: 1

    * - Option
      - Typ
      - Benötigt
      - Standard
      - Beschreibung
    * - ``id``
      - `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - nein [#fn1]_
      - \-
      - Instanz der Ferraris-Komponente
    * - ``rotations_per_kwh``
      - Zahl
      - nein
      - 75
      - Anzahl der Umdrehungen der Drehscheibe pro kWh (der Wert ist i.d.R. auf dem Ferraris-Stromzähler vermerkt)
    * - ``debounce_threshold``
      - Zahl |nbsp| / |nbsp| `ID <https://www.esphome.io/guides/configuration-types#config-id>`_ |nbsp| [#fn3]_
      - nein
      - 400
      - Minimale Zeit in Millisekunden zwischen fallender und darauffolgender steigender Flanke, damit die Umdrehung berücksichtigt wird; siehe Abschnitt :ref:`Entprellungsschwellwert <debounce-threshold>` für Details
    * - ``energy_start_value``
      - `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - nein
      - \-
      - `Zahlen-Komponente <https://www.esphome.io/components/number>`_, deren Wert beim Booten als Startwert für den Verbrauchszähler verwendet wird

Die folgenden Einstellungen sind nur relevant, wenn der digitale Ausgang des Infrarotsensors verwendet wird:

.. list-table::
    :header-rows: 1

    * - Option
      - Typ
      - Benötigt
      - Standard
      - Beschreibung
    * - ``digital_input``
      - `Pin <https://www.esphome.io/guides/configuration-types#pin>`_
      - ja [#fn2]_
      - \-
      - GPIO-Pin, mit dem der digitale Ausgang des TCRT5000-Moduls verbunden ist

Die folgenden Einstellungen sind nur relevant, wenn der analoge Ausgang des Infrarotsensors verwendet wird:

.. list-table::
    :header-rows: 1

    * - Option
      - Typ
      - Benötigt
      - Standard
      - Beschreibung
    * - ``analog_input``
      - `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - ja [#fn2]_
      - \-
      - `ADC-Sensor <https://www.esphome.io/components/sensor/adc.html>`_, der den mit dem analogen Ausgang des TCRT5000-Moduls verbundenen Pin ausliest
    * - ``analog_threshold``
      - Zahl |nbsp| / |nbsp| `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - nein
      - 50.0
      - Schwellwert für die Erkennung einer Umdrehung über den analogen Eingang, siehe Abschnitt :ref:`Kalibrierung des analogen Ausgangssignals <analog-calibration>` für Details
    * - ``off_tolerance``
      - Zahl |nbsp| / |nbsp| `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - nein
      - 0.0
      - Negativer Versatz zum analogen Schwellwert für die fallende Flanke, siehe Abschnitt :ref:`Hysterese-Kennlinie <hysteresis-curve>` für Details
    * - ``on_tolerance``
      - Zahl |nbsp| / |nbsp| `ID <https://www.esphome.io/guides/configuration-types#config-id>`_
      - nein
      - 0.0
      - Positiver Versatz zum analogen Schwellwert für die steigende Flanke, siehe Abschnitt :ref:`Hysterese-Kennlinie <hysteresis-curve>` für Details
    * - ``calibrate_on_boot``
      - Objekt
      - nein
      - \-
      - Wenn vorhanden, wird die automatische Kalibrierung des analogen Ausgangssignals vom Infrarotsensor nach dem Aufstarten ausgeführt, siehe Abschnitt :ref:`Kalibrierung des analogen Ausgangssignals <analog-calibration>` für Details


Die folgenden Einstellungen können für das Objekt ``calibrate_on_boot`` konfiguriert werden:

+------------------------+-------+----------+-----------+---------------------------------------------------------------------------------------+
| Option                 | Typ   | Benötigt | Standard  | Beschreibung                                                                          |
+========================+=======+==========+===========+=======================================================================================+
| ``num_captured_values``| Zahl  | nein     | 6000      | Anzahl der zu erfassenden analogen Werte pro Kalibrierungsdurchlauf                   |
+------------------------+-------+----------+-----------+---------------------------------------------------------------------------------------+
| ``min_level_distance`` | Zahl  | nein     | 6.0       | Mindestdifferenz zwischen niedrigstem und höchstem Analogwert, damit die Kalibrierung |
|                        |       |          |           | als erfolgreich angesehen und der analoge Schwellwert gesetzt wird                    |
+------------------------+-------+----------+-----------+---------------------------------------------------------------------------------------+
| ``max_iterations``     | Zahl  | nein     | 3         | Maximale Anzahl fehlgeschlagener Kalibrierungsdurchläufe, bevor aufgegeben wird       |
+------------------------+-------+----------+-----------+---------------------------------------------------------------------------------------+


.. [#fn1] Bestimmte :ref:`Anwendungsfälle <usage-examples>` benötigen das Konfigurationselement ``id``.

.. [#fn2] Nur eines der beiden Konfigurationselemente - ``digital_input`` oder ``analog_input`` - wird benötigt, je nach :ref:`Hardware-Aufbauvariante <hardware-setup>`.

.. [#fn3] Die Konfigurationselemente ``analog_threshold``\ , ``off_tolerance``\ , ``on_tolerance`` und ``debounce_threshold`` erwarten entweder eine feste Zahl oder die ID einer `Zahlen-Komponente <https://www.esphome.io/components/number>`_. Letzteres ermöglicht das Konfigurieren des Wertes über das User-Interface (z.B. durch die Verwendung einer `Template-Zahlen-Komponente <https://www.esphome.io/components/number/template.html>`_\ ).

Beispiel
--------

.. code-block:: yaml

    ferraris:
      id: ferraris_meter
      digital_input: GPIO4
      rotations_per_kwh: 75
      debounce_threshold: 400
      energy_start_value: last_energy_value

API/MQTT-Komponente
===================

Eine `API-Komponente <https://www.esphome.io/components/api.html>`_ ist erforderlich, wenn der ESP in Home Assistant integriert werden soll. Für den Fall, dass eine alternative Hausautomatisierungs-Software verwendet werden soll, muss stattdessen eine `MQTT-Komponente <https://www.esphome.io/components/mqtt.html>`_ hinzugefügt werden. Allerdings funktionieren dann bestimmte Mechanismen wie das Überschreiben des Zählerstands oder das Wiederherstellen des letzten Zählerstands nach einem Neustart (siehe weiter unten für Details) u.U. nicht mehr.

Beispiel
--------

Nachfolgend ein Beispiel für die Integration mit Home Assistant (und verschlüsselter API):

.. code-block:: yaml

    api:
      encryption:
        key: !secret ha_api_key

Und hier ein Beispiel für die Verwendung mit einer alternativen Hausautomatisierungs-Software mittels MQTT:

.. code-block:: yaml

    mqtt:
      broker: 10.0.0.2
      username: !secret mqtt_user
      password: !secret mqtt_password

WiFi-Komponente
===============

Eine `WiFi-Komponente <https://www.esphome.io/components/wifi.html>`_ sollte vorhanden sein, da die Sensor-Werte ansonsten nicht ohne weiteres an ein anderes Gerät übertragen werden können.

Beispiel
--------

.. code-block:: yaml

    wifi:
      ssid: !secret wifi_ssid
      password: !secret wifi_password

Sensoren
========

Die Ferraris-Plattform verfügt über primäre Sensoren, um die berechneten Verbrauchswerte auszugeben sowie über diagnostische Sensoren für den Kalibrierungsmodus. Alle Sensoren sind optional und können weggelassen werden, wenn sie nicht benötigt werden.

.. _primary-sensors:

Primäre Sensoren
----------------

Die folgenden primären Sensoren können konfiguriert werden:

+-----------------------+-----------+--------------+----------------------+---------+---------------------------+
| Sensor                | Typ       | Geräteklasse | Zustandsklasse       | Einheit | Beschreibung              |
+=======================+===========+==============+======================+=========+===========================+
| ``power_consumption`` | numerisch | ``power``    | ``measurement``      | W       | Aktueller Stromverbrauch  |
+-----------------------+-----------+--------------+----------------------+---------+---------------------------+
| ``energy_meter``      | numerisch | ``energy``   | ``total_increasing`` | Wh      | Gesamtstromverbrauch      |
|                       |           |              |                      |         | (Stromzähler/Zählerstand) |
+-----------------------+-----------+--------------+----------------------+---------+---------------------------+


Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der `ESPHome Sensorkomponenten <https://www.esphome.io/components/sensor>`_.

Beispiel
~~~~~~~~

.. code-block:: yaml

    sensor:
      - platform: ferraris
        power_consumption:
          name: Power consumption
        energy_meter:
          name: Meter reading

.. _diagnostic-sensors:

Diagnostische Sensoren
----------------------

Die folgenden diagnostischen Sensoren können konfiguriert werden:

.. list-table::
   :header-rows: 1

   * - Sensor
     - Typ
     - Beschreibung
   * - ``rotation_indicator``
     - binär
     - Zeigt an, ob die Markierung auf der Drehscheibe gerade vor dem Infrarotsensor ist (funktioniert nur im Kalibrierungsmodus)
   * - ``analog_calibration_state``
     - binär
     - Status der automatischen analogen Kalibrierung (ob aktiv oder nicht)
   * - ``analog_calibration_result``
     - binär
     - Ergebnis der letzten automatischen analogen Kalibrierung (ob erfolgreich oder nicht)
   * - ``analog_value_spectrum``
     - numerisch
     - Bandbreite der analogen Werte (Differenz zwischen kleinstem und größtem analogen Wert)


Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der `ESPHome Binärsensorkomponenten <https://www.esphome.io/components/binary_sensor>`_ und der `ESPHome Sensorkomponenten <https://www.esphome.io/components/sensor>`_.

Beispiel
~~~~~~~~

.. code-block:: yaml

    sensor:
      - platform: ferraris
        analog_value_spectrum:
          name: Analoge value spectrum

    binary_sensor:
      - platform: ferraris
        rotation_indicator:
          name: Rotation indicator
        analog_calibration_state:
          name: Analog calibration state
        analog_calibration_result:
          name: Analog calibration result

.. _actors:

Aktoren
=======

Zu diagnostischen Zwecken verfügt die Ferraris-Plattform über einen `Schalter <https://www.esphome.io/components/switch>`_. Dieser hat den Namen ``calibration_mode`` und kann dazu verwendet werden, die Komponente in den Kalibierungsmodus zu versetzen (siehe Abschnitt :ref:`Kalibrierung <calibration>` für weitere Informationen).

Beispiel
--------

.. code-block:: yaml

    switch:
      - platform: ferraris
        calibration_mode:
          name: Calibration mode

.. _actions:

Aktionen
========

Die Ferraris-Plattform stellt verschiedene Aktionen zur Verfügung, um Werte zu setzen oder das Verhalten zu steuern.

Zählerstand setzen
------------------

+-------------------------------+----------------------------------------------+
| Aktion                        | Beschreibung                                 |
+===============================+==============================================+
| ``ferraris.set_energy_meter`` | Setzt den Zählerstand auf den angegeben Wert |
+-------------------------------+----------------------------------------------+


Parameter
~~~~~~~~~

+-----------+-----------+---------+-------------------------------------------------------+
| Parameter | Typ       | Bereich | Beschreibung                                          |
+===========+===========+=========+=======================================================+
| ``value`` | ``float`` | >= 0    | Zielwert für den Zählerstand in Kilowattstunden (kWh) |
+-----------+-----------+---------+-------------------------------------------------------+


Anstelle eines festen Zahlenwerts kann auch ein Lambda-Ausdruck verwendet werden, der den zu übergebenden Wert zurückgibt.

.. note::

    Obwohl der Sensor für den aktuellen Zählerstand die Einheit **Wh (Wattstunden)** hat, verwendet die Aktion zum Überschreiben des Zählerstands die Einheit **kWh (Kilowattstunden)**, da die analogen Ferraris-Stromzähler den Zählerstand üblicherweise auch in dieser Einheit anzeigen.


Umdrehungszähler setzen
-----------------------

+-----------------------------------+---------------------------------------------------+
| Aktion                            | Beschreibung                                      |
+===================================+===================================================+
| ``ferraris.set_rotation_counter`` | Setzt den Umdrehungszähler auf den angegeben Wert |
+-----------------------------------+---------------------------------------------------+


.. note::

    Die Aktion zum Setzen des Zählerstands setzt indirekt auch den Umdrehungszähler, da die Ferraris-Komponente intern mit Umdrehungen und nicht mit Wattstunden oder Kilowattstunden arbeitet.


Parameter
~~~~~~~~~

+------------+-----------+---------+---------------------------------------------------------+
| Parameter  | Typ       | Bereich | Beschreibung                                            |
+============+===========+=========+=========================================================+
| ``value``  | ``uint64``| \>= 0   | Zielwert für den Umdrehungszähler in Anzahl Umdrehungen |
+------------+-----------+---------+---------------------------------------------------------+


Anstelle eines festen Zahlenwerts kann auch ein Lambda-Ausdruck verwendet werden, der den zu übergebenden Wert zurückgibt.

Automatische analoge Kalibrierung
---------------------------------

+---------------------------------------+---------------------------------------------------------------------------------------+
| Aktion                                | Beschreibung                                                                          |
+=======================================+=======================================================================================+
| ``ferraris.start_analog_calibration`` | Startet die automatische Kalibrierung des analogen Ausgangssignals vom Infrarotsensor |
+---------------------------------------+---------------------------------------------------------------------------------------+


Parameter
~~~~~~~~~

Alle Parameter sind optional und können weggelassen werden.

+-------------------------+------------+------------------------------+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------+
| Parameter               | Typ        | Bereich                      | Standard | Beschreibung                                                                                                                                             |
+=========================+============+==============================+==========+==========================================================================================================================================================+
| ``num_captured_values`` | ``uint32`` | 100 |nbsp| ... |nbsp| 100000 | 6000     | Anzahl der zu erfassenden analogen Werte pro Kalibrierungsdurchlauf                                                                                      |
+-------------------------+------------+------------------------------+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``min_level_distance``  | ``float``  | >= 0                         | 6.0      | Mindestdifferenz zwischen niedrigstem und höchstem Analogwert, damit die Kalibrierung als erfolgreich angesehen und der analoge Schwellwert gesetzt wird |
+-------------------------+------------+------------------------------+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------+
| ``max_iterations``      | ``uint16`` | 1 |nbsp| ... |nbsp| 10000    | 3        | Maximale Anzahl fehlgeschlagener Kalibrierungsdurchläufe, bevor aufgegeben wird                                                                          |
+-------------------------+------------+------------------------------+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------+
