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

from esphome.components import switch
from esphome.const      import (
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_DIAGNOSTIC
)
from ..                 import (
    ferraris_ns,
    FerrarisMeter,
    CONF_FERRARIS_ID
)


CODEOWNERS = ["@jensrossbach"]

CalibrationModeSwitch = ferraris_ns.class_("CalibrationModeSwitch", switch.Switch)

CONF_CALIBRATION_MODE = "calibration_mode"

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(CONF_FERRARIS_ID): cv.use_id(FerrarisMeter),
    cv.Optional(CONF_CALIBRATION_MODE): switch.switch_schema(
        CalibrationModeSwitch,
        icon="mdi:wrench-cog",
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    )
})


async def to_code(config):
    cmp = await cg.get_variable(config[CONF_FERRARIS_ID])

    if CONF_CALIBRATION_MODE in config:
        swt = await switch.new_switch(config[CONF_CALIBRATION_MODE])
        await cg.register_parented(swt, config[CONF_FERRARIS_ID])
        cg.add(cmp.set_calibration_mode_switch(swt))
