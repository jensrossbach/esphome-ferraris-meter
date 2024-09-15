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


import esphome.codegen                         as cg
import esphome.config_validation               as cv
import esphome.components.homeassistant.number as ha_number

from esphome             import automation, pins
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const       import (
    CONF_ID,
    CONF_PIN,
    CONF_VALUE
)


CONF_FERRARIS_ID         = "ferraris_id"
CONF_ROTATIONS_PER_KWH   = "rotations_per_kwh"
CONF_LOW_STATE_THRESHOLD = "low_state_threshold"
CONF_ENERGY_START_VALUE  = "energy_start_value"

ferraris_ns = cg.esphome_ns.namespace("ferraris")
FerrarisMeter = ferraris_ns.class_("FerrarisMeter", cg.Component)
SetEnergyMeterAction = ferraris_ns.class_("SetEnergyMeterAction", automation.Action)
SetRotationCounterAction = ferraris_ns.class_("SetRotationCounterAction", automation.Action)

CODEOWNERS = ["@jens_rossbach"]

CONFIG_SCHEMA = cv.Schema(
{
    cv.GenerateID(): cv.declare_id(FerrarisMeter),
    cv.Required(CONF_PIN): pins.internal_gpio_input_pin_schema,
    cv.Optional(CONF_ROTATIONS_PER_KWH, default = 75): cv.int_range(min = 1),
    cv.Optional(CONF_LOW_STATE_THRESHOLD, default = 400): cv.int_range(min = 0),
    cv.Optional(CONF_ENERGY_START_VALUE): cv.use_id(ha_number.HomeassistantNumber)
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    pin = await gpio_pin_expression(config[CONF_PIN])

    cmp = cg.new_Pvariable(
                config[CONF_ID],
                pin,
                config[CONF_ROTATIONS_PER_KWH],
                config[CONF_LOW_STATE_THRESHOLD])
    await cg.register_component(cmp, config)

    if CONF_ENERGY_START_VALUE in config:
        num = await cg.get_variable(config[CONF_ENERGY_START_VALUE])
        cg.add(cmp.set_energy_start_value_number(num))

@automation.register_action(
    "ferraris.set_energy_meter",
    SetEnergyMeterAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(FerrarisMeter),
        cv.Required(CONF_VALUE): cv.templatable(cv.float_range(min = 0))
    }))
async def set_energy_meter_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    tmpl = await cg.templatable(config[CONF_VALUE], args, float)
    cg.add(act.set_energy_meter_value(tmpl))

    return act

@automation.register_action(
    "ferraris.set_rotation_counter",
    SetRotationCounterAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(FerrarisMeter),
        cv.Required(CONF_VALUE): cv.templatable(cv.uint64_t)
    }))
async def set_rotation_counter_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(action_id, template_arg, parent)

    tmpl = await cg.templatable(config[CONF_VALUE], args, int)
    cg.add(act.set_rotation_counter_value(tmpl))

    return act
