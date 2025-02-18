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

from esphome             import automation, pins
from esphome.components  import number, sensor
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const       import (
    CONF_ID,
    CONF_VALUE
)


CODEOWNERS = ["@jensrossbach"]
MULTI_CONF = True

# common
CONF_FERRARIS_ID         = "ferraris_id"
CONF_ROTATIONS_PER_KWH   = "rotations_per_kwh"
CONF_DEBOUNCE_THRESHOLD  = "debounce_threshold"
CONF_ENERGY_START_VALUE  = "energy_start_value"

# digital input
CONF_DIGITAL_INPUT       = "digital_input"

# analog input
CONF_ANALOG_INPUT        = "analog_input"
CONF_ANALOG_THRESHOLD    = "analog_threshold"
CONF_OFF_TOLERANCE       = "off_tolerance"
CONF_ON_TOLERANCE        = "on_tolerance"
CONF_CALIBRATE_ON_BOOT   = "calibrate_on_boot"
CONF_NUM_CAPTURED_VALUES = "num_captured_values"
CONF_MIN_LEVEL_DISTANCE  = "min_level_distance"
CONF_MAX_ITERATIONS      = "max_iterations"

ferraris_ns = cg.esphome_ns.namespace("ferraris")
FerrarisMeter = ferraris_ns.class_("FerrarisMeter", cg.Component)
SetEnergyMeterAction = ferraris_ns.class_("SetEnergyMeterAction", automation.Action)
SetRotationCounterAction = ferraris_ns.class_("SetRotationCounterAction", automation.Action)
StartAnalogCalibrationAction = ferraris_ns.class_("StartAnalogCalibrationAction", automation.Action)

def ensure_gpio_or_adc(value):
    if CONF_DIGITAL_INPUT not in value and CONF_ANALOG_INPUT not in value:
        raise cv.Invalid(f"One of '{CONF_DIGITAL_INPUT}' or '{CONF_ANALOG_INPUT}' must be specified.")
    if CONF_DIGITAL_INPUT in value and CONF_ANALOG_INPUT in value:
        raise cv.Invalid(f"Only one of '{CONF_DIGITAL_INPUT}' or '{CONF_ANALOG_INPUT}' can be specified, not both.")
    return value

ANALOG_CALIBRATION_SCHEMA = cv.Schema({
        cv.Optional(CONF_NUM_CAPTURED_VALUES, default = 6000): cv.int_range(min=100, max=100000),
        cv.Optional(CONF_MIN_LEVEL_DISTANCE, default = 6.0): cv.positive_float,
        cv.Optional(CONF_MAX_ITERATIONS, default = 3): cv.int_range(min=1, max=10)})

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(FerrarisMeter),
        cv.Optional(CONF_DIGITAL_INPUT): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_ANALOG_INPUT): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_ANALOG_THRESHOLD, default = 50): cv.Any(cv.Coerce(float), cv.use_id(number.Number)),
        cv.Optional(CONF_OFF_TOLERANCE, default = 0): cv.Any(cv.All(cv.positive_float, cv.Coerce(float)), cv.use_id(number.Number)),
        cv.Optional(CONF_ON_TOLERANCE, default = 0): cv.Any(cv.All(cv.positive_float, cv.Coerce(float)), cv.use_id(number.Number)),
        cv.Optional(CONF_ROTATIONS_PER_KWH, default = 75): cv.int_range(min = 1),
        cv.Optional(CONF_DEBOUNCE_THRESHOLD, default = 400): cv.Any(cv.int_range(min = 0), cv.use_id(number.Number)),
        cv.Optional(CONF_ENERGY_START_VALUE): cv.use_id(number.Number),
        cv.Optional(CONF_CALIBRATE_ON_BOOT): ANALOG_CALIBRATION_SCHEMA
    }).extend(cv.COMPONENT_SCHEMA),
    ensure_gpio_or_adc)


async def to_code(config):
    cmp = cg.new_Pvariable(
                config[CONF_ID],
                config[CONF_ROTATIONS_PER_KWH])
    await cg.register_component(cmp, config)

    if CONF_DIGITAL_INPUT in config:
        pin = await gpio_pin_expression(config[CONF_DIGITAL_INPUT])
        cg.add(cmp.set_digital_input_pin(pin))
    elif CONF_ANALOG_INPUT in config:
        sens = await cg.get_variable(config[CONF_ANALOG_INPUT])
        cg.add(cmp.set_analog_input_sensor(sens))

        if isinstance(config[CONF_ANALOG_THRESHOLD], float):
            cg.add(cmp.set_analog_input_threshold(config[CONF_ANALOG_THRESHOLD]))
        else:
            num = await cg.get_variable(config[CONF_ANALOG_THRESHOLD])
            cg.add(cmp.set_analog_input_threshold_number(num))

        if isinstance(config[CONF_OFF_TOLERANCE], float):
            cg.add(cmp.set_off_tolerance(config[CONF_OFF_TOLERANCE]))
        else:
            num = await cg.get_variable(config[CONF_OFF_TOLERANCE])
            cg.add(cmp.set_off_tolerance_number(num))

        if isinstance(config[CONF_ON_TOLERANCE], float):
            cg.add(cmp.set_on_tolerance(config[CONF_ON_TOLERANCE]))
        else:
            num = await cg.get_variable(config[CONF_ON_TOLERANCE])
            cg.add(cmp.set_on_tolerance_number(num))

        if CONF_CALIBRATE_ON_BOOT in config:
            calib_conf = config[CONF_CALIBRATE_ON_BOOT]
            cg.add(cmp.start_analog_calibration(
                            calib_conf[CONF_NUM_CAPTURED_VALUES],
                            calib_conf[CONF_MIN_LEVEL_DISTANCE],
                            calib_conf[CONF_MAX_ITERATIONS]))

    if isinstance(config[CONF_DEBOUNCE_THRESHOLD], int):
        cg.add(cmp.set_debounce_threshold(config[CONF_DEBOUNCE_THRESHOLD]))
    else:
        num = await cg.get_variable(config[CONF_DEBOUNCE_THRESHOLD])
        cg.add(cmp.set_debounce_threshold_number(num))

    if CONF_ENERGY_START_VALUE in config:
        num = await cg.get_variable(config[CONF_ENERGY_START_VALUE])
        cg.add(cmp.set_energy_start_value_number(num))

@automation.register_action(
    "ferraris.set_energy_meter",
    SetEnergyMeterAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(FerrarisMeter),
        cv.Required(CONF_VALUE): cv.templatable(cv.positive_float)
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

@automation.register_action(
    "ferraris.start_analog_calibration",
    StartAnalogCalibrationAction,
    cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(FerrarisMeter)
    }).extend(ANALOG_CALIBRATION_SCHEMA))
async def start_analog_calibration_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    act = cg.new_Pvariable(
                action_id,
                template_arg,
                parent,
                config[CONF_NUM_CAPTURED_VALUES],
                config[CONF_MIN_LEVEL_DISTANCE],
                config[CONF_MAX_ITERATIONS])

    return act
