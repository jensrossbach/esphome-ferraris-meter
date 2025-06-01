# Copyright (c) 2024 Jens-Uwe Rossbach
#
# This code is licensed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import esphome.codegen           as cg
import esphome.config_validation as cv

from esphome.components import sensor
from esphome.const      import (
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_ENERGY,
    ENTITY_CATEGORY_DIAGNOSTIC,
    UNIT_WATT,
    UNIT_WATT_HOURS
)
from .                  import (
    FerrarisMeter,
    CONF_FERRARIS_ID
)


CODEOWNERS = ["@jensrossbach"]

CONF_POWER_CONSUMPTION     = "power_consumption"
CONF_ENERGY_METER          = "energy_meter"
CONF_ANALOG_VALUE_SPECTRUM = "analog_value_spectrum"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_FERRARIS_ID): cv.use_id(FerrarisMeter),
    cv.Optional(CONF_POWER_CONSUMPTION): sensor.sensor_schema(
        icon="mdi:lightning-bolt",
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1
    ),
    cv.Optional(CONF_ENERGY_METER): sensor.sensor_schema(
        icon="mdi:transmission-tower",
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
        unit_of_measurement=UNIT_WATT_HOURS,
        accuracy_decimals=1
    ),
    cv.Optional(CONF_ANALOG_VALUE_SPECTRUM): sensor.sensor_schema(
        icon="mdi:arrow-expand-vertical",
        accuracy_decimals=0,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_FERRARIS_ID])

    if CONF_POWER_CONSUMPTION in config:
        sens = await sensor.new_sensor(config[CONF_POWER_CONSUMPTION])
        cg.add(cmp.set_power_consumption_sensor(sens))

    if CONF_ENERGY_METER in config:
        sens = await sensor.new_sensor(config[CONF_ENERGY_METER])
        cg.add(cmp.set_energy_meter_sensor(sens))

    if CONF_ANALOG_VALUE_SPECTRUM in config:
        sens = await sensor.new_sensor(config[CONF_ANALOG_VALUE_SPECTRUM])
        cg.add(cmp.set_analog_value_spectrum_sensor(sens))
