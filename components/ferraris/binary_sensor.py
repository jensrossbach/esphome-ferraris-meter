# Copyright (c) 2024-2025 Jens-Uwe Rossbach
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

from esphome.components import binary_sensor
from esphome.const      import (
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_RUNNING,
    ENTITY_CATEGORY_DIAGNOSTIC
)
from .                  import (
    FerrarisMeter,
    CONF_FERRARIS_ID
)


CODEOWNERS = ["@jensrossbach"]

CONF_ROTATION_INDICATOR        = "rotation_indicator"
CONF_ANALOG_CALIBRATION_STATE  = "analog_calibration_state"
CONF_ANALOG_CALIBRATION_RESULT = "analog_calibration_result"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_FERRARIS_ID): cv.use_id(FerrarisMeter),
    cv.Optional(CONF_ROTATION_INDICATOR): binary_sensor.binary_sensor_schema(
        icon="mdi:rotate-360",
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    ),
    cv.Optional(CONF_ANALOG_CALIBRATION_STATE): binary_sensor.binary_sensor_schema(
        icon="mdi:state-machine",
        device_class = DEVICE_CLASS_RUNNING,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    ),
    cv.Optional(CONF_ANALOG_CALIBRATION_RESULT): binary_sensor.binary_sensor_schema(
        device_class = DEVICE_CLASS_PROBLEM,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_FERRARIS_ID])

    if CONF_ROTATION_INDICATOR in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ROTATION_INDICATOR])
        cg.add(cmp.set_rotation_indicator_sensor(sens))

    if CONF_ANALOG_CALIBRATION_STATE in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ANALOG_CALIBRATION_STATE])
        cg.add(cmp.set_analog_calibration_state_sensor(sens))

    if CONF_ANALOG_CALIBRATION_RESULT in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ANALOG_CALIBRATION_RESULT])
        cg.add(cmp.set_analog_calibration_result_sensor(sens))
