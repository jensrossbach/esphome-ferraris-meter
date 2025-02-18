# ESPHome Ferraris Meter
> [!NOTE]
> This is the German version, for the English version, scroll down or click [here](#esphome-ferraris-meter-english).

Ferraris Meter ist eine ESPHome-Komponente zur Erstellung einer ESP-Firmware, die mithilfe eines ESP-Mikrocontrollers und eines Infrarotsensors die Geschwindigkeit und die Umdrehungen der Drehscheibe eines analogen Ferraris-Stromzählers ermitteln und daraus den momentanen Stromverbrauch und den Zählerstand berechnen kann. Diese Werte können dann zur weiteren Verarbeitung an eine Hausautomatisierungs-Software wie beispielsweise Home Assistant geschickt werden.

- [Haftungsausschluss](#haftungsausschluss)
- [Lizenz](LICENSE)
- [Hardware-Aufbau](#hardware-aufbau)
- [Software-Konfiguration](#software-konfiguration)
  - [Ferraris-Komponente](#ferraris-komponente)
  - [API/MQTT-Komponente](#apimqtt-komponente)
  - [WiFi-Komponente](#wifi-komponente)
  - [Sensoren](#sensoren)
    - [Primäre Sensoren](#primäre-sensoren)
    - [Diagnostische Sensoren](#diagnostische-sensoren)
  - [Aktoren](#aktoren)
  - [Aktionen](#aktionen)
- [Anwendungsbeispiele](#anwendungsbeispiele)
  - [Auslesen des Stromzählers über den digitalen Ausgang des Infrarotsensors](#auslesen-des-stromzählers-über-den-digitalen-ausgang-des-infrarotsensors)
  - [Auslesen des Stromzählers über den analogen Ausgang des Infrarotsensors](#auslesen-des-stromzählers-über-den-analogen-ausgang-des-infrarotsensors)
  - [Auslesen mehrerer Stromzähler](#auslesen-mehrerer-stromzähler)
  - [Kalibrierung](#kalibrierung)
    - [Kalibrierung des digitalen Ausgangssignals](#kalibrierung-des-digitalen-ausgangssignals)
    - [Kalibrierung des analogen Ausgangssignals](#kalibrierung-des-analogen-ausgangssignals)
  - [Entprellung](#entprellung)
    - [Entprellungsschwellwert](#entprellungsschwellwert)
    - [Hysterese-Kennlinie](#hysterese-kennlinie)
    - [Glättung des analogen Signals](#glättung-des-analogen-signals)
  - [Manuelles Überschreiben des Zählerstands](#manuelles-überschreiben-des-zählerstands)
    - [Händisches Setzen des Zählerstands über das User-Interface](#händisches-setzen-des-zählerstands-über-das-user-interface)
    - [Automatisiertes Setzen des Zählerstands](#automatisiertes-setzen-des-zählerstands)
  - [Wiederherstellung des Zählerstands nach einem Neustart](#wiederherstellung-des-zählerstands-nach-einem-neustart)
- [Hilfe/Unterstützung](SUPPORT.md)
- [Mitwirkung](CONTRIBUTING.md)
- [Änderungsprotokoll](https://github.com/jensrossbach/esphome-ferraris-meter/releases)
- [Bekannte Probleme](https://github.com/jensrossbach/esphome-ferraris-meter/issues?q=is%3Aissue+is%3Aopen+label%3A%22known+issue%22)

## Haftungsausschluss
**DIE SOFTWARE (EINSCHLIEßLICH DER DOKUMENTATION MIT HARDWARE BEISPIEL-AUFBAUTEN) WIRD OHNE MÄNGELGEWÄHR UND OHNE JEGLICHE AUSDRÜCKLICHE ODER STILLSCHWEIGENDE GEWÄHRLEISTUNG, EINSCHLIEẞLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT, DER EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND DER NICHTVERLETZUNG VON RECHTEN DRITTER, ZUR VERFÜGUNG GESTELLT. DIE AUTOREN ODER URHEBERRECHTSINHABER SIND IN KEINEM FALL HAFTBAR FÜR ANSPRÜCHE, SCHÄDEN ODER ANDERE VERPFLICHTUNGEN, OB IN EINER VERTRAGS- ODER HAFTUNGSKLAGE, EINER UNERLAUBTEN HANDLUNG ODER ANDERWEITIG, DIE SICH AUS ODER IN VERBINDUNG MIT DER SOFTWARE ODER DER NUTZUNG ODER ANDEREN GESCHÄFTEN MIT DER SOFTWARE ERGEBEN.**

## Hardware-Aufbau
Hardware-seitig wird lediglich ein ESP-Mikrocontroller (z.B. ESP8266 oder ESP32, inkl. Spannungsversorgung) und ein Infrarotsensor (z.B. TCRT5000) benötigt. Für die reine Funktionalität des Ferraris Meters reicht ein ESP8266 als Mikrocontroller völlig aus. Für den Infrarotsensor gibt es fertige TCRT5000-basierte Breakout-Module mit 3,3V-5V Eingangsspannung, die auch über einen regelbaren Widerstand (Potentiometer) verfügen, um den digitalen Ausgang zu kalibrieren. Diese TCRT5000-Module haben 4 Pins - VCC und GND für die Stromversorgung des Sensor-Chips sowie einen digitalen Ausgang D0 und einen analogen Ausgang A0.

Beim Platzieren des Sensors auf der Abdeckplatte des Ferraris-Stromzählers ist ein wenig Geschick und Präzisionsarbeit gefragt. Das Infrarot Sender/Empfänger-Paar des Sensors muss mittig millimetergenau über der Drehscheibe ausgerichtet werden und geradlinig auf die Drehscheibe zeigen.

Die Ferraris Meter Komponente unterstützt prinzipiell folgende Aufbauvarianten:
- [Verwendung eines einzelnen Infrarotsensors über den digitalen Ausgang](#auslesen-des-stromzählers-über-den-digitalen-ausgang-des-infrarotsensors)
- [Verwendung eines einzelnen Infrarotsensors über den analogen Ausgang](#auslesen-des-stromzählers-über-den-analogen-ausgang-des-infrarotsensors)
- [Verwendung mehrerer Infrarotsensoren](#auslesen-mehrerer-stromzähler)

## Software-Konfiguration
Um eine ESPHome-Firmware zu erstellen, muss eine YAML-basierte Konfigurationsdatei erstellt werden. Du kannst eine der in diesem Repository bereitgestellten [Beispielkonfigurationsdateien](example_config) als Ausgangspunkt verwenden und sie an deine Bedürfnisse anpassen.

Prinzipiell gibt es zwei Möglichkeiten, die ESPHome-Firmware zu bauen:

1. [Über Home Assistant mit dem ESPHome Device Compiler Add-on](https://www.esphome.io/guides/getting_started_hassio)
2. [Über die Kommandozeile mit dem ESPHome Python-Paket](https://www.esphome.io/guides/getting_started_command_line)

Für welche Methode du dich entscheiden solltest, hängt davon ab, wie vertraut du mit ESPHome bist und ob du lieber mit einer grafischen Benutzeroberfläche oder mit der Kommandozeile arbeitest. Außerdem könnte die Leistungsfähigkeit des Hosts, auf dem du die Firmware baust, eine Rolle spielen, um den Vorgang zu beschleunigen.

> [!NOTE]
> Es ist **nicht** nötig, dieses Repository zu kopieren ("forken") und die Anpassungen an der Beispielkonfiguration im kopierten Repository vorzunehmen. Stattdessen reicht es aus, die Beispielkonfiguration lokal zu speichern und anzupassen oder die angepasste Konfiguration auf deinem Home Assistant Host abzulegen (sollte die Erstellung der ESPHome-Firmware mithilfe des ESPHome Device Compiler Add-ons erwünscht sein).

Die folgenden Abschnitte beschreiben die wichtigsten Komponenten, die in der Firmware-Konfigurationsdatei enthalten sind.

### Ferraris-Komponente
Die Komponente Ferraris ist unabdingbar und muss hinzugefügt werden, um ihre Sensoren zu verwenden.

Da es sich um eine benutzerdefinierte Komponente handelt, die nicht Teil von ESPHome ist, muss sie explizit importiert werden. Am einfachsten ist es, die Komponente direkt aus diesem Repository zu laden.

##### Beispiel

```yaml
external_components:
  - source: github://jensrossbach/esphome-ferraris-meter
    components: [ferraris]
```

> [!TIP]
> Im obigen Beispiel wird der neueste Stand der Komponente aus dem `main` Branch des Repositories geladen. Ich empfehle aber, mittels Versionsnummer auf einen freigegebenen Stand zu verweisen, um mehr Kontrolle darüber zu haben, welcher Software-Stand verwendet wird und um besser auf "breaking changes" reagieren zu können. Siehe Beispielkonfiguration, wie das gemacht werden kann.

Die folgenden allgemeinen Einstellungen können konfiguriert werden:

| Option | Typ | Benötigt | Standard | Beschreibung |
| ------ | --- | -------- | -------- | ------------ |
| `id` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | nein <sup>1</sup> | - | Instanz der Ferraris-Komponente |
| `rotations_per_kwh` | Zahl | nein | 75 | Anzahl der Umdrehungen der Drehscheibe pro kWh (der Wert ist i.d.R. auf dem Ferraris-Stromzähler vermerkt) |
| `debounce_threshold` | Zahl&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | nein | 400 | Minimale Zeit in Millisekunden zwischen fallender und darauffolgender steigender Flanke, damit die Umdrehung berücksichtigt wird, siehe Abschnitt [Entprellungsschwellwert](#entprellungsschwellwert) für Details |
| `energy_start_value` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | nein | - | [Zahlen-Komponente](https://www.esphome.io/components/number), deren Wert beim Booten als Startwert für den Verbrauchszähler verwendet wird |

Die folgenden Einstellungen sind nur relevant, wenn der digitale Ausgang des Infrarotsensors verwendet wird:

| Option | Typ | Benötigt | Standard | Beschreibung |
| ------ | --- | -------- | -------- | ------------ |
| `digital_input` | [Pin](https://www.esphome.io/guides/configuration-types#pin) | ja <sup>2</sup> | - | GPIO-Pin, mit dem der digitale Ausgang des TCRT5000-Moduls verbunden ist |

Die folgenden Einstellungen sind nur relevant, wenn der analoge Ausgang des Infrarotsensors verwendet wird:

| Option | Typ | Benötigt | Standard | Beschreibung |
| ------ | --- | -------- | -------- | ------------ |
| `analog_input` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | ja <sup>2</sup> | - | [ADC-Sensor](https://www.esphome.io/components/sensor/adc.html), der den mit dem analogen Ausgang des TCRT5000-Moduls verbundenen Pin ausliest |
| `analog_threshold` | Zahl&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | nein | 50.0 | Schwellwert für die Erkennung einer Umdrehung über den analogen Eingang, siehe Abschnitt [Kalibrierung des analogen Ausgangssignals](#kalibrierung-des-analogen-ausgangssignals) für Details |
| `off_tolerance` | Zahl&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | nein | 0.0 | Negativer Versatz zum analogen Schwellwert für die fallende Flanke, siehe Abschnitt [Hysterese-Kennlinie](#hysterese-kennlinie) für Details |
| `on_tolerance` | Zahl&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | nein | 0.0 | Positiver Versatz zum analogen Schwellwert für die steigende Flanke, siehe Abschnitt [Hysterese-Kennlinie](#hysterese-kennlinie) für Details |
| `calibrate_on_boot` | Wörterbuch | nein | - | Wenn vorhanden, wird die automatische Kalibrierung des analogen Ausgangssignals vom Infrarotsensor nach dem Aufstarten ausgeführt, siehe Abschnitt [Kalibrierung des analogen Ausgangssignals](#kalibrierung-des-analogen-ausgangssignals) für Details |

Die folgenden Einstellungen können für `calibrate_on_boot` konfiguriert werden:

| Option | Typ | Benötigt | Standard | Beschreibung |
| ------ | --- | -------- | -------- | ------------ |
| `num_captured_values` | Zahl | nein | 6000 | Anzahl der zu erfassenden analogen Werte pro Kalibrierungsdurchlauf |
| `min_level_distance` | Zahl | nein | 6.0 | Mindestdifferenz zwischen niedrigstem und höchstem Analogwert, damit die Kalibrierung als erfolgreich angesehen und der analoge Schwellwert gesetzt wird |
| `max_iterations` | Zahl | nein | 3 | Maximale Anzahl fehlgeschlagener Kalibrierungsdurchläufe, bevor aufgegeben wird |

<sup>1</sup> Bestimmte [Anwendungsfälle](#anwendungsbeispiele) benötigen das Konfigurationselement `id`.

<sup>2</sup> Nur eines der beiden Konfigurationselemente - `digital_input` oder `analog_input` - wird benötigt, je nach [Hardware-Aufbauvariante](#hardware-aufbau).

<sup>3</sup> Die Konfigurationselemente `analog_threshold`, `off_tolerance`, `on_tolerance` und `debounce_threshold` erwarten entweder eine feste Zahl oder die ID einer [Zahlen-Komponente](https://www.esphome.io/components/number). Letzteres ermöglicht das Konfigurieren des Wertes über das User-Interface (z.B. durch die Verwendung einer [Template-Zahlen-Komponente](https://www.esphome.io/components/number/template.html)).

##### Beispiel
```yaml
ferraris:
  id: ferraris_meter
  digital_input: GPIO4
  rotations_per_kwh: 75
  debounce_threshold: 400
  energy_start_value: last_energy_value
```

### API/MQTT-Komponente
Eine [API-Komponente](https://www.esphome.io/components/api.html) ist erforderlich, wenn der ESP in Home Assistant integriert werden soll. Für den Fall, dass eine alternative Hausautomatisierungs-Software verwendet werden soll, muss stattdessen eine [MQTT-Komponente](https://www.esphome.io/components/mqtt.html) hinzugefügt werden. Allerdings funktionieren dann bestimmte Mechanismen wie das Überschreiben des Zählerstands oder das Wiederherstellen des letzten Zählerstands nach einem Neustart (siehe weiter unten für Details) u.U. nicht mehr.

##### Beispiel
Nachfolgend ein Beispiel für die Integration mit Home Assistant (und verschlüsselter API):

```yaml
api:
  encryption:
    key: !secret ha_api_key
```

Und hier ein Beispiel für die Verwendung mit einer alternativen Hausautomatisierungs-Software mittels MQTT:

```yaml
mqtt:
  broker: 10.0.0.2
  username: !secret mqtt_user
  password: !secret mqtt_password
```

### WiFi-Komponente
Eine [WiFi-Komponente](https://www.esphome.io/components/wifi.html) sollte vorhanden sein, da die Sensor-Werte ansonsten nicht ohne weiteres an ein anderes Gerät übertragen werden können.

##### Beispiel

```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

### Sensoren
Die Ferraris-Komponente verfügt über primäre Sensoren, um die berechneten Verbrauchswerte auszugeben sowie über diagnostische Sensoren für den Kalibrierungsmodus. Alle Sensoren sind optional und können weggelassen werden, wenn sie nicht benötigt werden.

#### Primäre Sensoren
Die folgenden primären Sensoren können konfiguriert werden:

| Sensor | Typ | Geräteklasse | Zustandsklasse | Einheit | Beschreibung |
| ------ | --- | ------------ | -------------- | ------- | ------------ |
| `power_consumption` | numerisch | `power` | `measurement` | W | Aktueller Stromverbrauch |
| `energy_meter` | numerisch | `energy` | `total_increasing` | Wh | Gesamtstromverbrauch (Stromzähler/Zählerstand) |

Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der [ESPHome Sensorkomponenten](https://www.esphome.io/components/sensor).

##### Beispiel
```yaml
sensor:
  - platform: ferraris
    power_consumption:
      name: Momentanverbrauch
    energy_meter:
      name: Verbrauchszähler
```

#### Diagnostische Sensoren
Die folgenden diagnostischen Sensoren können konfiguriert werden:

| Sensor | Typ | Beschreibung |
| ------ | --- | ------------ |
| `rotation_indicator` | binär | Zeigt an, ob die Markierung auf der Drehscheibe gerade vor dem Infrarotsensor ist (funktioniert nur im Kalibrierungsmodus) |

Detaillierte Informationen zu den Konfigurationsmöglichkeiten der einzelnen Elemente findest du in der Dokumentation der [ESPHome Binärsensorkomponenten](https://www.esphome.io/components/binary_sensor).

##### Beispiel
```yaml
binary_sensor:
  - platform: ferraris
    rotation_indicator:
      name: Umdrehungsindikator
```

### Aktoren
Zu diagnostischen Zwecken verfügt die Ferraris-Komponente über einen [Schalter](https://www.esphome.io/components/switch). Dieser hat den Namen `calibration_mode` und kann dazu verwendet werden, die Komponente in den Kalibierungsmodus zu versetzen (siehe Abschnitt [Kalibrierung](#kalibrierung) für weitere Informationen).

##### Beispiel
```yaml
switch:
  - platform: ferraris
    calibration_mode:
      name: Kalibrierungsmodus
```

### Aktionen
Die Ferraris-Komponente stellt verschiedene Aktionen zur Verfügung, um Werte zu setzen oder das Verhalten zu steuern.

#### Zählerstand setzen
| Aktion | Beschreibung |
| ------ | ------------ |
| `ferraris.set_energy_meter` | Setzt den Zählerstand auf den angegeben Wert |

##### Parameter
| Parameter | Typ | Bereich | Beschreibung |
| --------- | --- | ------- | ------------ |
| `value` | `float` | >=&nbsp;0 | Zielwert für den Zählerstand in Kilowattstunden (kWh) |

Anstelle eines festen Zahlenwerts kann auch ein Lambda-Ausdruck verwendet werden, der den zu übergebenden Wert zurückgibt.

> [!NOTE]
> Obwohl der Sensor für den aktuellen Zählerstand die Einheit **Wh (Wattstunden)** hat, verwendet die Aktion zum Überschreiben des Zählerstands die Einheit **kWh (Kilowattstunden)**, da die analogen Ferraris-Stromzähler den Zählerstand üblicherweise auch in dieser Einheit anzeigen.

#### Umdrehungszähler setzen
| Aktion | Beschreibung |
| ------ | ------------ |
| `ferraris.set_rotation_counter` | Setzt den Umdrehungszähler auf den angegeben Wert |

> [!NOTE]
> Die Aktion zum Setzen des Zählerstands setzt indirekt auch den Umdrehungszähler, da die Ferraris-Komponente intern mit Umdrehungen und nicht mit Wattstunden oder Kilowattstunden arbeitet.

##### Parameter
| Parameter | Typ | Bereich | Beschreibung |
| --------- | --- | ------- | ------------ |
| `value` | `uint64` | >=&nbsp;0 | Zielwert für den Umdrehungszähler in Anzahl Umdrehungen |

Anstelle eines festen Zahlenwerts kann auch ein Lambda-Ausdruck verwendet werden, der den zu übergebenden Wert zurückgibt.

#### Automatische analoge Kalibrierung
| Aktion | Beschreibung |
| ------ | ------------ |
| `ferraris.start_analog_calibration` | Startet die automatische Kalibrierung des analogen Ausgangssignals vom Infrarotsensor |

##### Parameter
Alle Parameter sind optional und können weggelassen werden.

| Parameter | Typ | Bereich | Standard | Beschreibung |
| --------- | --- | ------- | -------- | ------------ |
| `num_captured_values` | `uint32` | 100&nbsp;...&nbsp;100000 | 6000 | Anzahl der zu erfassenden analogen Werte pro Kalibrierungsdurchlauf |
| `min_level_distance` | `float` | >=&nbsp;0 | 6.0 | Mindestdifferenz zwischen niedrigstem und höchstem Analogwert, damit die Kalibrierung als erfolgreich angesehen und der analoge Schwellwert gesetzt wird |
| `max_iterations` | `uint8` | 1&nbsp;...&nbsp;10 | 3 | Maximale Anzahl fehlgeschlagener Kalibrierungsdurchläufe, bevor aufgegeben wird |

## Anwendungsbeispiele
In diesem Abschnitt sind verschiedene Anwendungsbeispiele für die Ferraris-Komponente beschrieben.

### Auslesen des Stromzählers über den digitalen Ausgang des Infrarotsensors
In dieser Variante wird der digitale Ausgang des Infrarotsensors verwendet, um Umdrehungen der Drehscheibe zu erkennen. Der analoge Ausgang wird nicht benötigt, die anderen Pins müssen mit den entsprechenden Pins des Mikrocontrollers verbunden werden. Für VCC sollte der 3,3V-Ausgang des ESPs verwendet werden und der digitale Ausgang D0 muss mit einem freien GPIO-Pin (z.B. GPIO4, entspricht dem Pin D2 auf dem D1 Mini) verbunden werden.

Der folgende Steckplatinen-Schaltplan zeigt ein Beispiel für einen Versuchsaufbau mit einem ESP8266 D1 Mini Entwicklungsboard als Mikrocontroller.

![Steckplatinen-Schaltplan (digitaler Pin)](img/breadboard_schematic.png)

Die Kalibrierung des digitalen Ausgangssignals erfolgt mithilfe des auf der Platine des Infrarotsensors befindlichen Potentiometers, siehe Abschnitt [Kalibrierung des digitalen Ausgangssignals](#kalibrierung-des-digitalen-ausgangssignals) für Details.

> [!TIP]
> Sollte es nicht gelingen, eine passende und funktionierende Einstellung für das Potentiometer zu finden, kann alternativ der analoge Ausgang des Infrarotsensors verwendet werden, siehe nächsten Abschnitt.

Software-seitig muss für die Ferraris-Komponente in der YAML-Konfigurationsdatei der Pin konfiguriert werden, der mit dem digitalen Ausgang des TCRT5000-Moduls verbunden ist:
```yaml
ferraris:
  id: ferraris_meter
  digital_input: GPIO4
  # ...
```

**Beispiel-Konfiguration:** [ferraris_meter_digital.yaml](example_config/ferraris_meter_digital.yaml)

### Auslesen des Stromzählers über den analogen Ausgang des Infrarotsensors
In dieser Variante wird der analoge Ausgang des Infrarotsensors verwendet, um Umdrehungen der Drehscheibe zu erkennen. Der digitale Ausgang wird nicht benötigt, die anderen Pins müssen mit den entsprechenden Pins des Mikrocontrollers verbunden werden. Für VCC sollte der 3,3V-Ausgang des ESPs verwendet werden und der analoge Ausgang A0 muss mit einem freien ADC-Pin (z.B. GPIO17, entspricht dem Pin A0 auf dem D1 Mini) verbunden werden.

Der folgende Steckplatinen-Schaltplan zeigt ein Beispiel für einen Versuchsaufbau mit einem ESP8266 D1 Mini Entwicklungsboard als Mikrocontroller.

![Steckplatinen-Schaltplan (analoger Pin)](img/breadboard_schematic_analog.png)

Eine Kalibrierung mittels des Potientiometers auf dem TCRT5000-Modul entfällt. Stattdessen müssen software-seitig der Schwellwert für den analogen Eingang und optional die Versatzwerte für eine Hysterese-Kennlinie konfiguriert werden, siehe Abschnitt [Kalibrierung des analogen Ausgangssignals](#kalibrierung-des-analogen-ausgangssignals) für Details.

Software-seitig müssen nun beispielsweise folgende Konfigurations-Schritte durchgeführt werden:
1.  In der YAML-Konfigurationsdatei wird ein [ADC-Sensor](https://www.esphome.io/components/sensor/adc.html) konfiguriert, der den mit dem analogen Ausgang des TCRT5000-Moduls verbundenen ADC-Pin ausliest.
    ```yaml
    sensor:
      - platform: adc
        id: adc_input
        pin: GPIO17
        internal: true
        raw: true
        samples: 10
        update_interval: 50ms
    ```
2.  Unter der Konfiguration der Ferraris-Komponente verweist der Eintrag `analog_input` auf den unter 1. angelegten ADC-Sensor und der Eintrag `analog_threshold` gibt den analogen Schwellwert an (siehe Abschnitt [Kalibrierung des analogen Ausgangssignals](#kalibrierung-des-analogen-ausgangssignals) für weitere Informationen zum analogen Schwellwert). Zudem können optional die Versatzwerte `off_tolerance` und `on_tolerance` konfiguriert werden (siehe Abschnitt [Hysterese-Kennlinie](#hysterese-kennlinie) für weitere Details).
    ```yaml
    ferraris:
      id: ferraris_meter
      analog_input: adc_input
      analog_threshold: 45.0
      off_tolerance: 1.0
      on_tolerance: 1.0
      # ...
    ```

**Beispiel-Konfiguration:** [ferraris_meter_analog.yaml](example_config/ferraris_meter_analog.yaml)

### Auslesen mehrerer Stromzähler
Es ist auch möglich, mehr als einen Ferraris-Stromzähler mit einem einzigen ESP-Mikrocontroller auszulesen. Dazu benötigt man weitere Infrarotsensoren / TCRT5000-Module und zusätzliche freie GPIO-Pins am Mikrocontroller. Die TCRT5000-Module werden wie schon vorher beschrieben über VCC und GND an die Spannungsquelle des ESP-Mikrocontrollers angeschlossen und die D0-Ausgänge werden jeweils mit einem freien GPIO-Pin an dem ESP-Board verbunden.

> [!NOTE]
> Theoretisch kann auch die Variante mit dem analogen Ausgang des Infrarotsensors verwendet werden, allerdings sind die ADC-fähigen Pins auf den ESP-Mikrocontrollern stärker limitiert als die rein digitalen Pins. Insbesondere der ESP8266, der nur einen einzigen ADC hat, wäre daher ungeeignet, mehrere Infrarotsensoren über deren analoge Ausgänge zu unterstützen.

Der folgende Steckplatinen-Schaltplan zeigt ein Beispiel für einen Versuchsaufbau mit zwei TCRT5000-Modulen, die mit einem ESP8266 D1 Mini verbunden sind.

![Steckplatinen-Schaltplan (2 TCRT5000-Module)](img/breadboard_schematic_2_sensors.png)

Es ist aber zu bedenken, dass jeder weitere Infrarotsensor die Last auf dem Mikrocontroller erhöht und insbesondere bei sehr hohen Geschwindigkeiten der Drehscheiben die Hardware näher an ihre Grenzen bringt.

Software-seitig müssen nun beispielsweise folgende Konfigurations-Schritte durchgeführt werden:
1.  In der YAML-Konfigurationsdatei müssen mehrere Instanzen der Ferraris-Komponente konfiguriert werden (hier beispielhaft 2 Instanzen).
    ```yaml
    ferraris:
      - id: ferraris_meter_1
        digital_input: GPIO4
        # ...
      - id: ferraris_meter_2
        digital_input: GPIO5
        # ...
    ```
2.  Alle von der Ferraris-Komponente bereitgestellten Sensoren und Komponenten müssen, sofern benötigt, vervielfacht und den entsprechenden Instanzen der Ferraris-Komponente über den Eintrag `ferraris_id` zugewiesen werden.
    ```yaml
    sensor:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        power_consumption:
          name: Momentanverbrauch 1
        energy_meter:
          name: Verbrauchszähler 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        power_consumption:
          name: Momentanverbrauch 2
        energy_meter:
          name: Verbrauchszähler 2

    binary_sensor:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        rotation_indicator:
          name: Umdrehungsindikator 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        rotation_indicator:
          name: Umdrehungsindikator 2

    switch:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        calibration_mode:
          name: Kalibrierungsmodus 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        calibration_mode:
          name: Kalibrierungsmodus 2
    ```
3.  Alle weiteren in der YAML-Konfigurationsdatei definierten Komponenten, die mit den Ferraris-Sensoren und -Komponenten interagieren, müssen eventuell vervielfacht und/oder angepasst werden.

**Beispiel-Konfiguration:** [ferraris_meter_multi.yaml](example_config/ferraris_meter_multi.yaml)

### Kalibrierung
Während der Positionierung und Ausrichtung des Infrarotsensors sowie der Einstellung des Potentiometers oder des analogen Schwellwerts ist es wenig sinnvoll, die Umdrehungen der Drehscheibe des Ferraris-Stromzählers zu messen und die Verbräuche zu berechnen, da die Zustandsänderungen des Sensors nicht der tatsächlichen Erkennung der Markierung auf der Drehscheibe entsprechen. Deshalb gibt es die Möglichkeit, die Ferraris-Komponente in den Kalibrierungsmodus zu versetzen, indem man den Schalter für den Kalibrierungsmodus (siehe [Aktoren](#aktoren)) einschaltet. Solange der Kalibrierungsmodus aktiviert ist, wird keine Berechnung der Verbrauchsdaten durchgeführt und die entsprechenden Sensoren (siehe [Primäre Sensoren](#primäre-sensoren)) werden nicht verändert. Stattdessen ist der diagnostische Sensor für die Umdrehungsindikation (siehe [Diagnostische Sensoren](#diagnostische-sensoren)) aktiv und kann zusätzlich verwendet werden, um bei der korrekten Ausrichtung zu unterstützen. Der Sensor befindet sich in dem Zustand `on` wenn die Markierung auf der Drehscheibe erkannt wurde und `off` wenn keine Markierung erkannt wurde.

Um den Kalibierungsmodus vollständig nutzen zu können, müssen die Komponenten `calibration_mode` und `rotation_indicator` in der YAML-Datei konfiguriert sein:
```yaml
binary_sensor:
  - platform: ferraris
    rotation_indicator:
      name: Umdrehungsindikator

switch:
  - platform: ferraris
    calibration_mode:
      name: Kalibrierungsmodus
```

#### Kalibrierung des digitalen Ausgangssignals
Mithilfe eines Schraubenziehers kann über das Potientiometer das digitale Ausgangssignal des Infrarotsensors kalibriert werden. Dabei helfen die beiden grünen LEDs auf der Rückseite des Sensors. Die rechte LED leuchtet dauerhaft, wenn der Sensor mit Strom versorgt wird. Die linke LED leuchtet, solange kein "Hindernis" erkannt wurde und erlischt, wenn die Reflektion unterbrochen wurde. Letzteres ist der Zustand, wenn die Markierung auf der Drehscheibe des Ferraris-Stromzählers vor den Sensor wandert. Das Potentiometer sollte also so eingestellt werden, dass die linke LED gerade noch leuchtet, wenn die Markierung nicht im Bereich des Infrarot Sender/Empfänger-Paares ist und erlischt, sobald sich die Markierung davor schiebt. Dies ist nur ein sehr kleiner Bereich und es kann etwas schwierig werden, diese Einstellung zu finden.

#### Kalibrierung des analogen Ausgangssignals
Das analoge Ausgangssignal des Infrarotsensors wird software-seitig je nach Präferenz entweder manuell oder automatisch kalibriert. Der Schwellwert `analog_threshold` steuert, wann das analoge Signal als "erkannt" (markierter Bereich der Drehscheibe) und wann als "nicht erkannt" (nicht markierter Bereich der Drehscheibe) behandelt wird. Ist der Wert des ADC-Sensors `analog_input` größer als der Schwellwert, gilt die Markierung als erkannt, ist er kleiner oder gleich, gilt sie als nicht erkannt.

![Analoger Schwellwert](img/analog_threshold.png)

Für die manuelle Kalibrierung muss ein passender Wert für den analogen Schwellwert ermittelt und konfiguriert werden.

Wenn der Schwellwert ermittelt wurde und nicht mehr konfiguriert/verändert werden muss, kann ein fester Zahlenwert für `analog_threshold` angegeben werden:
```yaml
ferraris:
  # ...
  analog_threshold: 45.0
  # ...
```

Wenn der Schwellwert noch nicht bekannt ist oder später über das User-Interface veränderbar sein soll, kann anstelle eines festen Zahlenwerts eine [Template-Zahlen-Komponente](https://www.esphome.io/components/number/template.html) (oder alternativ eine [Home Assistant Zahlen-Komponente](https://www.esphome.io/components/number/homeassistant.html)) eingetragen werden:
```yaml
ferraris:
  # ...
  analog_threshold: adc_threshold
  # ...

number:
  - platform: template
    id: adc_threshold
    name: ADC Schwellwert
    icon: mdi:speedometer-slow
    entity_category: config
    mode: box
    optimistic: true
    initial_value: 50.0
    min_value: 0.0
    max_value: 1000.0
    step: 0.5
```

Es ist auch möglich, den analogen Schwellwert automatisch von der Ferraris-Software berechnen zu lassen. Dies geschieht durch das Aufrufen der Aktion `start_analog_calibration` (siehe [Aktionen](#aktionen)). Dabei analysiert die Software eine konfigurierbare Anzahl analoger Samples vom Infrarotsensor und ermittelt den kleinsten und den größten analogen Wert. Anschließend wird das arithmetische Mittel aus den beiden Grenzwerten berechnet und als analoger Schwellwert verwendet.

Um die automatische Kalibrierung vom User-Interface aus starten zu können, kann ein [Template-Button](https://www.esphome.io/components/button/template.html) konfiguriert werden, der beim Drücken die Aktion aufruft:
```yaml
button:
  - platform: template
    name: Auto-Kalibrierung starten
    icon: mdi:auto-fix
    entity_category: config
    on_press:
      - ferraris.start_analog_calibration:
          id: ferraris_meter
          num_captured_values: 6000
          min_level_distance: 6.0
          max_iterations: 3
```

Um die automatische Kalibrierung bei jedem Aufstarten des Mikrocontrollers zu starten, kann folgender Eintrag zur Konfiguration der Ferraris-Komponente hinzugefügt werden:
```yaml
ferraris:
  # ...
  calibrate_on_boot:
    num_captured_values: 6000
    min_level_distance: 6.0
    max_iterations: 3
  # ...
```

### Entprellung
Der Übergang von nicht markiertem zu markiertem Bereich und umgekehrt auf der Drehscheibe kann zu einem schnellen Hin-und Herspringen ("Prellen") des Erkennungszustands des Sensors führen, das vor allem bei langsamen Drehgeschwindigkeiten auftritt und nicht vollständig durch die Kalibrierung unterdrückt werden kann. Dieses Prellen führt zu verfälschten Messwerten und um diese zu vermeiden, gibt es folgende Einstellungensmöglichkeiten.

#### Entprellungsschwellwert
Der Entprellungsschwellwert `debounce_threshold` spezifiziert die minimale Zeit in Millisekunden zwischen fallender und darauffolgender steigender Flanke. Nur wenn die gemessene Zeit zwischen den zwei Flanken über dem konfigurierten Wert liegt, wird die Sensorauslösung berücksichtigt. Diese Art der Entprellung funktioniert bei der Verwendung sowohl des digitalen als auch des analogen Eingangssignals des Infrarotsensors.

![Entprellungsschwellwert](img/debounce_threshold.png)

Um eine Entprellung über den Schwellwert zu aktivieren, muss der entsprechende Wert in der Konfiguration der Ferraris-Komponente gesetzt werden:
```yaml
ferraris:
  # ...
  debounce_threshold: 400
  # ...
```

Wenn der Wert dynamisch über das User-Interface einstellbar sein soll, kann anstelle eines festen Zahlenwerts eine [Template-Zahlen-Komponente](https://www.esphome.io/components/number/template.html) (oder alternativ eine [Home Assistant Zahlen-Komponente](https://www.esphome.io/components/number/homeassistant.html)) eingetragen werden:
```yaml
ferraris:
  # ...
  debounce_threshold: debounce_threshold
  # ...

number:
  - platform: template
    id: debounce_threshold
    name: Entprellungsschwellwert
    icon: mdi:speedometer-slow
    entity_category: config
    mode: box
    optimistic: true
    initial_value: 400
    min_value: 0
    max_value: 2000
    step: 1
```

#### Hysterese-Kennlinie
Die beiden Versatzwerte `off_tolerance` und `on_tolerance` können konfiguriert werden, um eine Hysterese-Kennlinie für die Erkennung des markiertes Bereichs auf der Drehscheibe über das analoge Signal zu verwenden. Dadurch wird ein "Zittern" des analogen Signals kompensiert und damit ein mögliches Prellen des Erkennungszustands für den markierten Bereich auf der Drehscheibe minimiert. Diese Art der Entprellung funktioniert nur bei der Verwendung des analogen Eingangssignals des Infrarotsensors.

![Hysterese-Kennlinie](img/hysteresis.png)

Um eine Entprellung über die Hysterese-Kennlinie zu aktivieren, müssen die entsprechenden Werte in der Konfiguration der Ferraris-Komponente gesetzt werden:
```yaml
ferraris:
  # ...
  off_tolerance: 1.0
  on_tolerance: 1.0
  # ...
```

Wenn die Werte dynamisch über das User-Interface einstellbar sein sollen, können anstelle der festen Zahlenwerte [Template-Zahlen-Komponenten](https://www.esphome.io/components/number/template.html) (oder alternativ [Home Assistant Zahlen-Komponenten](https://www.esphome.io/components/number/homeassistant.html)) eingetragen werden:
```yaml
ferraris:
  # ...
  off_tolerance: off_tolerance
  on_tolerance: on_tolerance
  # ...

number:
  - platform: template
    id: off_tolerance
    name: Hysterese Aussschalttoleranz
    icon: mdi:speedometer-slow
    entity_category: config
    mode: slider
    optimistic: true
    initial_value: 1.0
    min_value: 0.0
    max_value: 5.0
    step: 0.5
  - platform: template
    id: on_tolerance
    name: Hysterese Einschalttoleranz
    icon: mdi:speedometer-slow
    entity_category: config
    mode: slider
    optimistic: true
    initial_value: 1.0
    min_value: 0.0
    max_value: 5.0
    step: 0.5
```

#### Glättung des analogen Signals
Durch eine geschickte Konfiguration des Aktualisierungsintervalls `update_interval` und der Anzahl Abtastungen pro Aktualisierung (`samples`) für den analogen Sensor `analog_input` kann die Kurve des analogen Signals so weit geglättet werden, dass kurzfristige Schwankungen eliminiert werden. Es ist aber zu bedenken, dass zu große Aktualisierungsintervalle dazu führen können, dass einzelne Umdrehungen bei sehr hohen Drehgeschwindigkeiten nicht mehr erkannt werden, da dann die Zeit zwischen steigender und darauffolgender fallender Flanke kleiner als das eingestellte Aktualisierungsintervall ist. Auch diese Art der Entprellung funktioniert nur bei der Verwendung des analogen Eingangssignals des Infrarotsensors.

### Manuelles Überschreiben des Zählerstands
Um den Zählerstand in der Ferraris-Komponente mit dem tatsächlichen Zählerstand des Ferraris-Stromzählers abzugleichen, kann der Wert des Verbrauchszähler-Sensors explizit überschrieben werden. Dazu werden die zwei Aktionen `ferraris.set_energy_meter` und `ferraris.set_rotation_counter` (siehe [Aktionen](#aktionen)) zur Verfügung gestellt.

> [!TIP]
> Normalerweise ist nur eine der beiden Aktionen nötig, je nachdem, ob man den Zählerstand in Kilowattstunden oder lieber in Anzahl Umdrehungen setzen möchte.

Abhängig davon, ob das Setzen des Zählerstands händisch über das User-Interface oder automatisiert über Automationen und Skripte erfolgen soll, können die Aktionen auf unterschiedliche Weise verwendet werden. Nachfolgend sind zwei mögliche Anwendungsbeispiele beschrieben, es gibt aber noch weitere, hier nicht beschriebene Möglichkeiten.

#### Händisches Setzen des Zählerstands über das User-Interface
Dafür führt man beispielsweise folgende Konfigurations-Schritte durch (in diesem Beispiel zum Setzen des Zählerstands als Kilowattstunden-Wert):
1.  In der YAML-Konfigurationsdatei wird eine [Template-Zahlen-Komponente](https://www.esphome.io/components/number/template.html) angelegt und für einen Zählerstand in der Einheit Kilowattstunden konfiguriert.
    ```yaml
    number:
      - platform: template
        id: target_energy_value
        name: Manueller Zählerstand
        icon: mdi:counter
        unit_of_measurement: kWh
        device_class: energy
        entity_category: config
        mode: box
        optimistic: true
        min_value: 0
        max_value: 1000000
        step: 0.01
    ```
2.  In der YAML-Konfigurationsdatei wird ein [Template-Button](https://www.esphome.io/components/button/template.html) angelegt und so konfiguriert, dass beim Drücken die Aktion zum Setzen des Zählerstands ausgeführt wird. Der zu übergebende Zielwert wird dabei aus der unter 1. angelegten Zahlen-Komponente gelesen.
    ```yaml
    button:
      - platform: template
        name: Verbrauchszähler überschreiben
        icon: mdi:download
        entity_category: config
        on_press:
          - ferraris.set_energy_meter:
              id: ferraris_meter
              value: !lambda |-
                float val = id(target_energy_value).state;
                return (val >= 0) ? val : 0;
    ```

#### Automatisiertes Setzen des Zählerstands
Dafür führt man beispielsweise folgende Konfigurations-Schritte durch:
1.  In der YAML-Konfigurationsdatei wird eine [benutzerdefinierte Aktion](https://www.esphome.io/components/api.html#api-device-actions) angelegt, die eine der Ferraris-Aktionen (im folgenden Beispiel die Aktion `ferraris.set_energy_meter`) aufruft.
    ```yaml
    api:
      # ...
      actions:
        - action: set_energy_meter
          variables:
            target_value: float
          then:
            - ferraris.set_energy_meter:
                id: ferraris_meter
                value: !lambda |-
                  return (target_value >= 0)
                            ? target_value
                            : 0;
    ```
2.  In Home Assistant wird eine [Automation](https://www.home-assistant.io/docs/automation) erstellt, die die benutzerdefinierte ESPHome-Aktion aufruft (im folgenden Beispiel wird der Zählerstand am Anfang eines jeden Monats zurückgesetzt).
    ```yaml
    - id: '1234567890'
      alias: Zurücksetzen des Verbrauchszählers
      trigger:
        - platform: time
          at: 00:00:00
      condition:
        - condition: template
          value_template: '{{ now().day == 1 }}'
      action:
        - action: esphome.ferraris_meter_set_energy_meter
          data:
            target_value: 0
      mode: single
    ```

### Wiederherstellung des Zählerstands nach einem Neustart
Um die Lebensdauer des Flash-Speichers auf dem ESP-Mikrocontroller nicht zu verringern, speichert die Ferraris-Komponente keine Daten persistent im Flash. Dadurch kann sie sich zunächst einmal den Zählerstand über einen Neustart des Mikrocontrollers hinweg nicht merken und der Zähler beginnt bei jedem Boot-Vorgang bei 0 kWh zu zählen. Somit müsste man nach jedem Neustart den Zählerstand manuell durch einen am Ferraris-Stromzähler abgelesenen Wert überschreiben. Da dies nicht sehr benutzerfreundlich ist, gibt es die Möglichkeit, den letzten Zählerstand in Home Assistant zu persistieren und beim Booten des Mikrocontrollers an diesen zu übertragen.

Damit dies funktioniert, müssen beispielsweise folgende Konfigurations-Schritte durchgeführt werden:
1.  In Home Assistant wird ein [Zahlenwert-Eingabehelfer](https://www.home-assistant.io/integrations/input_number) angelegt (in diesem Beispiel mit der Entitäts-ID `input_number.stromzaehler_letzter_wert`).
2.  In der YAML-Konfigurationsdatei wird eine [Home Assistant Zahlen-Komponente](https://www.esphome.io/components/number/homeassistant.html) angelegt, die den unter 1. angelegten Zahlenwert-Eingabehelfer importiert.
    ```yaml
    number:
      - platform: homeassistant
        id: last_energy_value
        entity_id: input_number.stromzaehler_letzter_wert
    ```
3.  Unter der Konfiguration der Ferraris-Komponente verweist der Eintrag `energy_start_value` auf die unter 2. angelegte Zahlen-Komponente.
    ```yaml
    ferraris:
      # ...
      energy_start_value: last_energy_value
    ```
4.  In Home Assistant wird eine [Automation](https://www.home-assistant.io/docs/automation/basics) erstellt, die bei Änderung des Verbrauchszähler-Sensors den aktuellen Sensorwert in den unter 1. angelegten Zahlenwert-Eingabehelfer kopiert.
    ```yaml
    - id: '1234567890'
      alias: Aktualisierung Verbrauchszähler-Cache
      trigger:
      - platform: state
        entity_id:
          - sensor.ferraris_meter_verbrauchszaehler
      condition: []
      action:
      - action: input_number.set_value
        target:
          entity_id: input_number.stromzaehler_letzter_wert
        data:
          value: '{{ states(trigger.entity_id) }}'
      mode: single
    ```
    Alternativ kann auch eine [Sensor-Automation](https://www.esphome.io/components/sensor/#sensor-automation) für den Sensor `energy_meter` in der YAML-Konfigurationsdatei angelegt werden, die die unter 2. angelegte Zahlen-Komponente direkt von ESPHome aus aktualisiert. Allerdings verlängert dies die Verarbeitungszeit pro Umdrehung im Mikrocontroller und kann u.U. dazu führen, dass bei sehr hohen Stromverbräuchen (und damit sehr hohen Drehgeschwindigkeiten) einzelne Umläufe der Drehscheibe nicht erfasst werden. Daher empfehle ich die Variante mit der Automation in Home Assistant.

-----

# ESPHome Ferraris Meter (English)
Ferraris Meter is an ESPHome component for creating an ESP firmware that uses an ESP microcontroller and an infrared sensor to capture the number of rotations and the speed of the turntable of an analog Ferraris electricity meter and to calculate the current electricity consumption and meter reading. These values can then be sent to a home automation software such as Home Assistant for further processing.

- [Disclaimer](#disclaimer)
- [License](LICENSE)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
  - [Ferraris Component](#ferraris-component)
  - [API/MQTT Component](#apimqtt-component)
  - [WiFi Component](#wifi-component)
  - [Sensors](#sensors)
    - [Primary Sensors](#primary-sensors)
    - [Diagnostic Sensors](#diagnostic-sensors)
  - [Actors](#actors)
  - [Actions](#actions)
- [Usage Examples](#usage-examples)
  - [Reading the Electricity Meter via the digital Output of the Infrared Sensor](#reading-the-electricity-meter-via-the-digital-output-of-the-infrared-sensor)
  - [Reading the Electricity Meter via the analog Output of the Infrared Sensor](#reading-the-electricity-meter-via-the-analog-output-of-the-infrared-sensor)
  - [Reading multiple Electricity Meters](#reading-multiple-electricity-meters)
  - [Calibration](#calibration)
    - [Calibration of the digital Output Signal](#calibration-of-the-digital-output-signal)
    - [Calibration of the analog Output Signal](#calibration-of-the-analog-output-signal)
  - [Debouncing](#debouncing)
    - [Debounce Threshold](#debounce-threshold)
    - [Hysteresis Curve](#hysteresis-curve)
    - [Smoothing of the analog Signal](#smoothing-of-the-analog-signal)
  - [Explicit Meter Reading Replacement](#explicit-meter-reading-replacement)
    - [Setting Energy Meter manually via the User Interface](#setting-energy-meter-manually-via-the-user-interface)
    - [Setting Energy Meter automatically](#setting-energy-meter-automatically)
  - [Meter Reading Recovery after Restart](#meter-reading-recovery-after-restart)
- [Help/Support](SUPPORT.md#-getting-support-for-esphome-ferraris-meter)
- [Contributing](CONTRIBUTING.md#contributing-to-esphome-ferraris-meter)
- [Change Log](https://github.com/jensrossbach/esphome-ferraris-meter/releases)
- [Known Issues](https://github.com/jensrossbach/esphome-ferraris-meter/issues?q=is%3Aissue+is%3Aopen+label%3A%22known+issue%22)

## Disclaimer
**THE SOFTWARE (INCLUDING THE DOCUMENTATION WITH THE EXAMPLE HARDWARE SETUP) IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**

## Hardware Setup
On the hardware side, only an ESP microcontroller (e.g. ESP8266 or ESP32, incl. power supply) and an infrared sensor (e.g. TCRT5000) are required. An ESP8266 microcontroller is completely sufficient for the pure functionality of the Ferraris Meter. For the infrared sensor, there are ready-made TCRT5000-based breakout modules with 3.3V-5V input voltage available, which also have an adjustable resistor (potentiometer) to calibrate the digital output of the sensor. These TCRT5000 modules have 4 pins - VCC and GND for the power supply of the sensor chip as well as a digital output D0 and an analog output A0.

Placing the sensor on the cover plate of the Ferraris electricity meter requires a little skill and precision work. The infrared transmitter/receiver pair of the sensor must be aligned centrally above the turntable with millimeter precision and point in a straight line to the turntable.

The Ferraris Meter component basically supports the following setup variants:
- [Use of a single infrared sensor via the digital output](#reading-the-electricity-meter-via-the-digital-output-of-the-infrared-sensor)
- [Use of a single infrared sensor via the analog output](#reading-the-electricity-meter-via-the-analog-output-of-the-infrared-sensor)
- [Use of multiple infrared sensors](#reading-multiple-electricity-meters)

## Software Setup
To build an ESPHome firmware, you have to create a YAML based configuration file. You can use one of the [example configuration files](example_config) provided in this repository as a starting point and adapt it to your needs.

In principle, there are two ways to build the ESPHome firmware:

1. [Via Home Assistant with the ESPHome Device Compiler add-on](https://www.esphome.io/guides/getting_started_hassio)
2. [Via the command line with the ESPHome Python package](https://www.esphome.io/guides/getting_started_command_line)

Which method you should choose depends on how familiar you are with ESPHome and whether you prefer to work with a graphical user interface or the command line. In addition, the performance of the host on which you are building the firmware could play a role in speeding up the process.

> [!NOTE]
> It is **not** necessary to fork this repository and do the adaptations to the example configuration directly inside the forked repository. Instead, it is sufficient to save and adapt the example configuration locally or store it on your Home Assistant host (if you wish to build the ESPHome firmware with the ESPHome Device Compiler add-on).

The following sections describe the most notable components contained in the firmware configuration file.

### Ferraris Component
The Ferraris component is essential and must be added in order to use its sensors.

As this is a custom component which is not part of ESPHome, it must be imported explicitly. The easiest way is to load the component directly from this repository.

##### Example

```yaml
external_components:
  - source: github://jensrossbach/esphome-ferraris-meter
    components: [ferraris]
```

> [!TIP]
> In the above example, the newest version of the component from the `main` branch of the repository is loaded. However, I recommend using a version number to refer to a released version in order to have more control over which software version is used and to be able to react better to "breaking changes". See the example configuration for how this can be done.

The following common configuration items can be configured:

| Option | Type | Required | Default | Description |
| ------ | ---- | -------- | ------- | ----------- |
| `id` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | no <sup>1</sup> | - | Ferraris component instance |
| `rotations_per_kwh` | Number | no | 75 | Number of rotations of the turntable per kWh (that value is usually noted on the Ferraris electricity meter) |
| `debounce_threshold` | Number&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | no | 400 | Minimum time in milliseconds between falling and subsequent rising edge to take the rotation into account, see section [Debounce Threshold](#debounce-threshold) for details |
| `energy_start_value` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | no | - | [Number component](https://www.esphome.io/components/number) whose value will be used as starting value for the energy counter at boot time |

The following configuration items are only relevant, if the digital output of the infrared sensor is used:

| Option | Type | Required | Default | Description |
| ------ | ---- | -------- | ------- | ----------- |
| `digital_input` | [Pin](https://www.esphome.io/guides/configuration-types#pin) | yes <sup>2</sup> | - | GPIO pin to which the digital output of the TCRT5000 module is connected |

The following configuration items are only relevant, if the analog output of the infrared sensor is used:

| Option | Type | Required | Default | Description |
| ------ | ---- | -------- | ------- | ----------- |
| `analog_input` | [ID](https://www.esphome.io/guides/configuration-types#config-id) | yes <sup>2</sup> | - | [ADC sensor](https://www.esphome.io/components/sensor/adc.html) which reads out the pin connected to the analog output of the TCRT5000 module |
| `analog_threshold` | Number&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | no | 50.0 | Threshold value for the detection of rotations via the analog input, see section [Calibration of the analog Output Signal](#calibration-of-the-analog-output-signal) for details |
| `off_tolerance` | Number&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | no | 0.0 | Negative offset to the analog threshold for the falling edge, see section [Hysteresis Curve](#hysteresis-curve) for details |
| `on_tolerance` | Number&nbsp;/ [ID](https://www.esphome.io/guides/configuration-types#config-id)&nbsp;<sup>3</sup> | no | 0.0 | Positive offset to the analog threshold for the rising edge, see section [Hysteresis Curve](#hysteresis-curve) for details |
| `calibrate_on_boot` | Map | no | - | If present, the automatic calibration of the analog output signal from the infrared sensor will be started after boot, see section [Calibration of the analog Output Signal](#calibration-of-the-analog-output-signal) for details |

The following configuration items can be configured for the `calibrate_on_boot` entry:

| Option | Type | Required | Default | Description |
| ------ | ---- | -------- | ------- | ----------- |
| `num_captured_values` | Number | no | 6000 | Number of analog values to capture per calibration iteration |
| `min_level_distance` | Number | no | 6.0 | Minimum difference between lowest and highest analog value to accept the calibration and set the analog threshold |
| `max_iterations` | Number | no | 3 | Maximum number of failed calibration iterations before giving up |

<sup>1</sup> Some [use cases](#usage-examples) require the configuration element `id`.

<sup>2</sup> Only one of `digital_input` or `analog_input` is required, depending on the [hardware setup variant](#hardware-setup).

<sup>3</sup> The configuration elements `analog_threshold`, `off_tolerance`, `on_tolerance` and `debounce_threshold` expect either a static number or the ID on a [number component](https://www.esphome.io/components/number). The latter allows the configuration of the value via the user interface (e.g., by using a [template number](https://www.esphome.io/components/number/template.html)).

##### Example
```yaml
ferraris:
  id: ferraris_meter
  digital_input: GPIO4
  rotations_per_kwh: 75
  debounce_threshold: 400
  energy_start_value: last_energy_value
```

### API/MQTT Component
An [API component](https://www.esphome.io/components/api.html) is required if the ESP shall be integrated into Home Assistant. For the case that an alternative home automation software shall be used, a [MQTT component](https://www.esphome.io/components/mqtt.html) has to be added instead. However, certain mechanisms such as manually overwriting the energy meter or restoring the last meter reading after a restart (see below for details) will then possibly no longer work.

##### Example
See below example for the integration into Home Assistant (with encrypted API):

```yaml
api:
  encryption:
    key: !secret ha_api_key
```

And below an example for usage with an alternative home automation software via MQTT:

```yaml
mqtt:
  broker: 10.0.0.2
  username: !secret mqtt_user
  password: !secret mqtt_password
```

### WiFi Component
A [WiFi component](https://www.esphome.io/components/wifi.html) should be present, as otherwise the sensor values cannot be easily transmitted to another computer.

##### Example

```yaml
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```

### Sensors
The Ferraris component provides primary sensors to expose the calculated consumption values as well as diagnostic sensors for the calibration mode. All sensors are optional and can be omitted if not needed.

#### Primary Sensors
The following primary sensors can be configured:

| Sensor | Type | Device Class | State Class | Unit | Description |
| ------ | ---- | ------------ | ----------- | ---- | ----------- |
| `power_consumption` | numeric | `power` | `measurement` | W | Current power consumption |
| `energy_meter` | numeric | `energy` | `total_increasing` | Wh | Total energy consumption (meter reading) |

For detailed configuration options of each item, please refer to ESPHome [sensor component configuration](https://www.esphome.io/components/sensor).

##### Example
```yaml
sensor:
  - platform: ferraris
    power_consumption:
      name: Power consumption
    energy_meter:
      name: Meter reading
```

#### Diagnostic Sensors
The following diagnostic sensors can be configured:

| Sensor | Type | Description |
| ------ | ---- | ----------- |
| `rotation_indicator` | binary | Indicates if the mark on the turntable is in front of the infrared sensor (only works in calibration mode) |

For detailed configuration options of each item, please refer to ESPHome [binary sensor component configuration](https://www.esphome.io/components/binary_sensor).

##### Example
```yaml
binary_sensor:
  - platform: ferraris
    rotation_indicator:
      name: Rotation indicator
```

### Actors
For diagnostic purposes, the Ferraris component provides a [switch](https://www.esphome.io/components/switch) with the name `calibration_mode`. It can be used to set the component to calibration mode (see section [calibration](#calibration) for further information).

##### Example
```yaml
switch:
  - platform: ferraris
    calibration_mode:
      name: Calibration mode
```

### Actions
The Ferraris component provides various actions for setting values or controlling the behavior.

#### Set Energy Meter
| Action | Description |
| ------ | ----------- |
| `ferraris.set_energy_meter` | Sets the energy meter reading to the provided value |

##### Parameters
| Parameter | Type | Range | Description |
| --------- | ---- | ----- | ----------- |
| `value` | `float` | >=&nbsp;0 | Target value for the energy meter reading in kilowatt hours (kWh) |

Instead of a fixed numeric value, it is also possible to specify a lambda expression that returns the value to be passed to the action.

> [!NOTE]
> Although the sensor for the current meter reading has the unit **Wh (watt hours)**, the action for overwriting the meter reading has the unit **kWh (kilowatt hours)**, as the analog Ferraris electricity meters usually also display the meter reading in this unit.

#### Set Rotation Counter
| Action | Description |
| ------ | ----------- |
| `ferraris.set_rotation_counter` | Sets the rotation counter to the provided value |

> [!NOTE]
> The action for setting the energy meter reading indirectly also sets the rotation counter as the Ferraris component internally works with rotations and not with watt hours or kilowatt hours.

##### Parameters
| Parameter | Type | Range | Description |
| --------- | ---- | ----- | ----------- |
| `value` | `uint64` | >=&nbsp;0 | Target value for the rotation counter in number of rotations |

Instead of a fixed numeric value, it is also possible to specify a lambda expression that returns the value to be passed to the action.

#### Automatic analog Calibration
| Action | Description |
| ------ | ----------- |
| `ferraris.start_analog_calibration` | Starts the automatic calibration of the analog output signal from the infrared sensor |

##### Parameters
All parameters are optional and can be omitted.

| Parameter | Type | Range | Default | Description |
| --------- | ---- | ----- | ------- | ----------- |
| `num_captured_values` | `uint32` | 100&nbsp;...&nbsp;100000 | 6000 | Number of analog values to capture per calibration iteration |
| `min_level_distance` | `float` | >=&nbsp;0 | 6.0 | Minimum difference between lowest and highest analog value to accept the calibration and set the analog threshold |
| `max_iterations` | `uint8` | 1&nbsp;...&nbsp;10 | 3 | Maximum number of failed calibration iterations before giving up |

## Usage Examples
This section describes various examples of usage for the Ferraris component.

### Reading the Electricity Meter via the digital Output of the Infrared Sensor
In this variant, the digital output of the infrared sensor is used to detect rotations of the turntable. The analog output is not required, the other pins must be connected to the corresponding pins of the microcontroller. The 3.3V output of the ESP should be used for VCC and the digital output D0 must be connected to a free GPIO pin (e.g. GPIO4, corresponding to pin D2 on the D1 Mini).

The following breadboard schematic shows an example test setup using an ESP8266 D1 Mini development board as microcontroller.

![Breadboard Schematic (digital Pin)](img/breadboard_schematic.png)

The digital output signal is calibrated using the potentiometer on the infrared sensor circuit board, see section [Calibration of the digital Output Signal](#calibration-of-the-digital-output-signal) for details.

> [!TIP]
> In case you are unable to find an appropriate and working adjustment of the potentiometer, you can alternatively use the analog output of the infrared sensor, see next section.

On the software side, the pin which is connected to the digital output of the TCRT5000 module has to be configured for the Ferraris component in the YAML configuration file:
```yaml
ferraris:
  id: ferraris_meter
  digital_input: GPIO4
  # ...
```

**Example configuration file:** [ferraris_meter_digital.yaml](example_config/ferraris_meter_digital.yaml)

### Reading the Electricity Meter via the analog Output of the Infrared Sensor
In this variant, the analog output of the infrared sensor is used to detect rotations of the turntable. The digital output is not required, the other pins must be connected to the corresponding pins of the microcontroller. The 3.3V output of the ESP should be used for VCC and the analog output A0 must be connected to a free ADC pin (e.g. GPIO17, corresponding to pin A0 on the D1 Mini).

The following breadboard schematic shows an example test setup using an ESP8266 D1 Mini development board as microcontroller.

![Breadboard Schematic (analog Pin)](img/breadboard_schematic_analog.png)

A calibration using the potentiometer on the TCRT5000 module is not needed. Instead, the threshold for the analog input and optionally the offset values for a hysteresis curve must be configured on the software side, see section [Calibration of the analog Output Signal](#calibration-of-the-analog-output-signal) for details.

On the software side, for instance, the following configuration steps must now be carried out:
1.  An [ADC sensor](https://www.esphome.io/components/sensor/adc.html) is configured in the YAML configuration file, which reads out the ADC pin connected to the analog output of the TCRT5000 module.
    ```yaml
    sensor:
      - platform: adc
        id: adc_input
        pin: GPIO17
        internal: true
        raw: true
        samples: 10
        update_interval: 50ms
    ```
2.  Under the configuration of the Ferraris component, the entry `analog_input` refers to the ADC sensor created under 1. and the entry `analog_threshold` specifies the analog threshold (see section [Calibration of the analog Output Signal](#calibration-of-the-analog-output-signal) for further information about the analog threshold). Additionally, the offset values  `off_tolerance` and `on_tolerance` can be configured (see section [Hysteresis Curve](#hysteresis-curve) für further details).
    ```yaml
    ferraris:
      id: ferraris_meter
      analog_input: adc_input
      analog_threshold: 45.0
      off_tolerance: 1.0
      on_tolerance: 1.0
      # ...
    ```

**Example configuration file:** [ferraris_meter_analog.yaml](example_config/ferraris_meter_analog.yaml)

### Reading multiple Electricity Meters
It is also possible to read more than one Ferraris electricity meter with a single ESP microcontroller. This requires multiple infrared sensors / TCRT5000 modules and additional free GPIO pins on the microcontroller. The TCRT5000 modules have to be connected to the voltage source of the ESP microcontroller via VCC and GND as described in the section [Hardware Setup](#hardware-setup) and the D0 outputs have to be connected to free GPIO pins on the ESP board.

> [!NOTE]
> Theoretically, the variant with the analog output of the infrared sensor can also be used, but the ADC-capable pins on the ESP microcontrollers are stronger limited than the pure digital pins. Especially the ESP8266, which has a single ADC only, would therefore not be suitable to support multiple infrared sensors via their analog outputs.

The following breadboard schematic shows an example of an example test setup with two TCRT5000 modules connected to an ESP8266 D1 Mini.

![Breadboard Schematic (two TCRT5000 modules)](img/breadboard_schematic_2_sensors.png)

However, bear in mind that each additional infrared sensor increases the load on the microcontroller and brings the hardware closer to its limits, especially with very high rotation speeds of the turntables.

On the software side, for instance, the following configuration steps must now be carried out:
1. Multiple instances of the Ferraris component must be configured in the YAML configuration file (here 2 instances as an example).
    ```yaml
    ferraris:
      - id: ferraris_meter_1
        digital_input: GPIO4
        # ...
      - id: ferraris_meter_2
        digital_input: GPIO5
        # ...
    ```
2.  All needed sensors and components provided by the Ferraris component must be duplicated and assigned to the corresponding Ferraris component instances via the `ferraris_id` configuration entry.
    ```yaml
    sensor:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        power_consumption:
          name: Power consumption 1
        energy_meter:
          name: Meter reading 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        power_consumption:
          name: Power consumption 2
        energy_meter:
          name: Meter reading 2

    binary_sensor:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        rotation_indicator:
          name: Rotation indicator 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        rotation_indicator:
          name: Rotation indicator 2

    switch:
      - platform: ferraris
        ferraris_id: ferraris_meter_1
        calibration_mode:
          name: Calibration mode 1
      - platform: ferraris
        ferraris_id: ferraris_meter_2
        calibration_mode:
          name: Calibration mode 2
    ```
3.  All other components defined in the YAML configuration file that interact with the Ferraris sensors and components may need to be multiplied and/or adapted.

**Example configuration file:** [ferraris_meter_multi.yaml](example_config/ferraris_meter_multi.yaml)

### Calibration
During the positioning and alignment of the infrared sensor as well as the adjustment of the potentiometer or the analog threshold, it makes little sense to measure the rotations of the Ferraris electricity meter's turntable and calculate the consumption values, as the changes in state of the sensor do not correspond to the actual detection of the mark on the turntable. It is therefore possible to set the Ferraris component to calibration mode by turning on the calibration mode switch (see [Actors](#actors)). As long as the calibration mode is activated, no calculation of the consumption data is performed and the corresponding sensors (see [Primary Sensors](#primary-sensors)) are not changed. Instead, the diagnostic sensor for the rotation indication (see [Diagnostic Sensors](#diagnostic-sensors)) is active and can additionally be used to assist with correct alignment. The sensor has the `on` state when the marker on the turntable is detected and the `off` state when it is not detected.

To be able to use the calibration mode, the components `calibration_mode` and `rotation_indicator` must be configured in the YAML file:
```yaml
binary_sensor:
  - platform: ferraris
    rotation_indicator:
      name: Rotation indicator

switch:
  - platform: ferraris
    calibration_mode:
      name: Calibration mode
```

### Calibration of the digital Output Signal
The digital output signal of the infrared sensor must be calibrated via the potentiometer using a screwdriver; the two green LEDs on the back of the sensor help with this. The right-hand LED lights up continuously when the sensor is supplied with power. The left-hand LED lights up as long as no "obstacle" has been detected and goes out when the reflection has been interrupted. The latter is the state when the mark on the Ferraris electricity meter's turntable moves in front of the sensor. The adjustment of the potentiometer should therefore be set so that the left-hand LED just lights up when the marker is not in the range of the infrared transmitter/receiver pair and goes out as soon as the marker moves in front of it. This is only a very small range and it can be a little difficult to find this setting.

### Calibration of the analog Output Signal
The analog output signal from the infrared sensor is calibrated either manually or automatically by the software, depending on your preference. The threshold value `analog_threshold` controls when the analog signal is treated as "detected" (marked area of the turntable) and when it is treated as "not detected" (unmarked area of the turntable). If the value from the ADC sensor `analog_input` is greater than the threshold value, the marking is considered detected; if it is smaller than or equal to the threshold value, it is considered not detected.

![Analoger Threshold](img/analog_threshold.png)

For manual calibration, a suitable value for the analog threshold value must be determined and configured.

If the threshold value has been determined and no longer needs to be configured/changed, a fixed numerical value can be entered for `analog_threshold`:
```yaml
ferraris:
  # ...
  analog_threshold: 45.0
  # ...
```

If the threshold value is not yet known or should be modifiable later via the user interface, a [template number component](https://www.esphome.io/components/number/template.html) (or alternatively a [Home Assistant number component](https://www.esphome.io/components/number/homeassistant.html)) can be entered instead of a fixed numerical value:
```yaml
ferraris:
  # ...
  analog_threshold: adc_threshold
  # ...

number:
  - platform: template
    id: adc_threshold
    name: ADC threshold
    icon: mdi:speedometer-slow
    entity_category: config
    mode: box
    optimistic: true
    initial_value: 50.0
    min_value: 0.0
    max_value: 1000.0
    step: 0.5
```

It is also possible to have the analog threshold value calculated automatically by the Ferraris software. This is done by calling the action `start_analog_calibration` (see [Actions](#actions)). The software analyzes a configurable number of analog samples from the infrared sensor and determines the lowest and highest analog value. The arithmetic mean of the two limit values is then calculated and used as the analog threshold value.

To start the automatic calibration from the user interface, a [template button](https://www.esphome.io/components/button/template.html) can be configured, which calls the action when pressed:
```yaml
button:
  - platform: template
    name: Start auto calibration
    icon: mdi:auto-fix
    entity_category: config
    on_press:
      - ferraris.start_analog_calibration:
          id: ferraris_meter
          num_captured_values: 6000
          min_level_distance: 6.0
          max_iterations: 3
```

To start the automatic calibration every time the microcontroller is started, the following entry can be added to the configuration of the Ferraris component:
```yaml
ferraris:
  # ...
  calibrate_on_boot:
    num_captured_values: 6000
    min_level_distance: 6.0
    max_iterations: 3
  # ...
```

### Debouncing
The transition from unmarked to marked area and vice versa on the turntable can lead to a rapid back and forth jump ("bouncing") in the detection state of the sensor, which occurs particularly at slow rotation speeds and cannot be completely suppressed by the calibration. This bouncing of the state leads to falsified measured values and to avoid this, the following settings can be applied.

#### Debounce Threshold
The debounce threshold value `debounce_threshold` specifies the minimum time in milliseconds between falling and subsequent rising edge. The trigger from the sensor is only taken into account if the measured time between the two edges is above the configured value. This type of debouncing can be applied to both the variant using the digital as well as the analog input signal of the infrared sensor.

![Debounce Threshold](img/debounce_threshold.png)

To activate debouncing via the threshold value, the corresponding item must be set in the configuration of the Ferraris component:
```yaml
ferraris:
  # ...
  debounce_threshold: 400
  # ...
```

If the value should be modifiable via the user interface, a [template number component](https://www.esphome.io/components/number/template.html) (or alternatively a [Home Assistant number component](https://www.esphome.io/components/number/homeassistant.html)) can be entered instead of a fixed numerical value:
```yaml
ferraris:
  # ...
  debounce_threshold: debounce_threshold
  # ...

number:
  - platform: template
    id: debounce_threshold
    name: Debounce threshold
    icon: mdi:speedometer-slow
    entity_category: config
    mode: box
    optimistic: true
    initial_value: 400
    min_value: 0
    max_value: 2000
    step: 1
```

#### Hysteresis Curve
The two offset values `off_tolerance` and `on_tolerance` can be configured to use a hysteresis curve for the detection of the marked area on the turntable via the analog signal. This compensates the jitter of the analog signal and thus minimizes any possible bouncing of the detection status for the marked area on the turntable. This type of debouncing only works when using the analog input signal of the infrared sensor.

![Hysteresis Curve](img/hysteresis.png)

To activate debouncing via the hysteresis curve, the corresponding items must be set in the configuration of the Ferraris component:
```yaml
ferraris:
  # ...
  off_tolerance: 1.0
  on_tolerance: 1.0
  # ...
```

If the values should be modifiable via the user interface, [template number components](https://www.esphome.io/components/number/template.html) (or alternatively [Home Assistant number components](https://www.esphome.io/components/number/homeassistant.html)) can be entered instead of fixed numerical values:
```yaml
ferraris:
  # ...
  off_tolerance: off_tolerance
  on_tolerance: on_tolerance
  # ...

number:
  - platform: template
    id: off_tolerance
    name: Hysteresis off tolerance
    icon: mdi:speedometer-slow
    entity_category: config
    mode: slider
    optimistic: true
    initial_value: 1.0
    min_value: 0.0
    max_value: 5.0
    step: 0.5
  - platform: template
    id: on_tolerance
    name: Hysteresis on tolerance
    icon: mdi:speedometer-slow
    entity_category: config
    mode: slider
    optimistic: true
    initial_value: 1.0
    min_value: 0.0
    max_value: 5.0
    step: 0.5
```

#### Smoothing of the analog Signal
By carefully configuring the update interval `update_interval` and the number of samples per update (`samples`) for the analog sensor `analog_input`, the curve of the analog signal can be smoothed to such an extent that short-term fluctuations are eliminated. However, bear in mind that excessive update intervals can lead to individual rotations no longer being detected at very high rotation speeds, as the time between the rising and subsequent falling edge is then shorter than the set update interval. Also this type of debouncing only works when using the analog input signal of the infrared sensor.

### Explicit Meter Reading Replacement
To synchronize the meter reading in the Ferraris component with the actual meter reading of the Ferraris electricity meter, the value of the energy meter sensor can be explicitly overwritten. The two actions `ferraris.set_energy_meter` and `ferraris.set_rotation_counter` (see [Actions](#actions)) are provided for this purpose.

> [!TIP]
> Usually, you need to use only one of the two actions, depending on whether you want to set the meter reading in kilowatt hours or in number of rotations.

The actions can be used in different ways, depending on whether the energy meter reading is to be set manually via the user interface or trigger-based via automations and scripts. Two possible usage examples are described below, but there are more possibilities existing which are not described here.

#### Setting Energy Meter manually via the User Interface
For instance, the following configuration steps are carried out (in this example to overwrite the energy meter with a kilowatt hours value):
1.  A [template number component](https://www.esphome.io/components/number/template.html) is created in the YAML configuration file and configured for a meter reading in the unit kilowatt hours.
    ```yaml
    number:
      - platform: template
        id: target_energy_value
        name: Manual meter reading
        icon: mdi:counter
        unit_of_measurement: kWh
        device_class: energy
        entity_category: config
        mode: box
        optimistic: true
        min_value: 0
        max_value: 1000000
        step: 0.01
    ```
2.  A [template button](https://www.esphome.io/components/button/template.html) is created in the YAML configuration file and configured so that the action for setting the energy meter or the rotation counter is executed when it is pressed. The target value to be set is retrieved from the number component created under 1.
    ```yaml
    button:
      - platform: template
        name: Overwrite meter reading
        icon: mdi:download
        entity_category: config
        on_press:
          - ferraris.set_energy_meter:
              id: ferraris_meter
              value: !lambda |-
                float val = id(target_energy_value).state;
                return (val >= 0) ? val : 0;
    ```

#### Setting Energy Meter automatically
For instance, the following configuration steps are carried out:
1.  A [user-defined action](https://www.esphome.io/components/api.html#api-device-actions) is created in the YAML configuration file, which calls one of the Ferraris actions (in the following example the action `ferraris.set_energy_meter` is used).
    ```yaml
    api:
      # ...
      actions:
        - action: set_energy_meter
          variables:
            target_value: float
          then:
            - ferraris.set_energy_meter:
                id: ferraris_meter
                value: !lambda |-
                  return (target_value >= 0)
                            ? target_value
                            : 0;
    ```
2.  An [automation](https://www.home-assistant.io/docs/automation) is created in Home Assistant that calls the user-defined ESPHome action (in the following example, the meter reading is reset at the beginning of each month).
    ```yaml
    - id: '1234567890'
      alias: Reset energy meter reading
      trigger:
        - platform: time
          at: 00:00:00
      condition:
        - condition: template
          value_template: '{{ now().day == 1 }}'
      action:
        - action: esphome.ferraris_meter_set_energy_meter
          data:
            target_value: 0
      mode: single
    ```

### Meter Reading Recovery after Restart
In order not to reduce the service life of the flash memory on the ESP microcontroller, the Ferraris component does not store any data persistently in the flash. As a result, it cannot remember the meter reading after a restart of the microcontroller and the meter starts counting at 0 kWh with every boot process. Therefore, the meter reading would have to be overwritten manually with a value read from the Ferraris electricity meter after each restart. As this is not very user-friendly, there is the option of persisting the last meter reading in Home Assistant and transferring it to the microcontroller when booting.

For this to work, the following configuration steps must be carried out:
1.  A [number input helper](https://www.home-assistant.io/integrations/input_number) is created in Home Assistant (in this example with the entity ID `input_number.electricity_meter_last_value`).
2.  A [Home Assistant number component](https://www.esphome.io/components/number/homeassistant.html) is created in the YAML configuration file, which imports the number input helper created under 1.
    ```yaml
    number:
      - platform: homeassistant
        id: last_energy_value
        entity_id: input_number.electricity_meter_last_value
    ```
3.  Under the configuration of the Ferraris component, the entry `energy_start_value` refers to the number component created under 2.
    ```yaml
    ferraris:
      # ...
      energy_start_value: last_energy_value
    ```
2.  An [automation](https://www.home-assistant.io/docs/automation/basics) is created in Home Assistant that copies the current sensor value to the number input helper created under 1. when the energy meter sensor is changed.
    ```yaml
    - id: '1234567890'
      alias: Update meter reading cache
      trigger:
      - platform: state
        entity_id:
          - sensor.ferraris_meter_energy
      condition: []
      action:
      - action: input_number.set_value
        target:
          entity_id: input_number.electricity_meter_last_value
        data:
          value: '{{ states(trigger.entity_id) }}'
      mode: single
    ```
    Alternatively, a [sensor automation](https://www.esphome.io/components/sensor/#sensor-automation) can be created for the sensor `energy_meter` in the YAML configuration file which updates the number component created under 2 directly from ESPHome. However, this leads to a longer processing time per rotation in the microcontroller and may result in individual rotations of the turntable not being detected in the event of very high power consumption (and hence, very high rotation speeds). Therefore, I recommend the variant with the automation in Home Assistant.
