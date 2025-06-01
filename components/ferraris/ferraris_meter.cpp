/*
 * Copyright (c) 2024-2025 Jens-Uwe Rossbach
 *
 * This code is licensed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "ferraris_meter.h"
#include "esphome/core/log.h"

#include <cmath>


namespace esphome::ferraris
{
    static constexpr const uint32_t WATTS_PER_KW = 1000;
    static constexpr const uint32_t MS_PER_HOUR  = 60 * 60 * 1000;
    static constexpr const uint32_t KWH_TO_WMS   = WATTS_PER_KW * MS_PER_HOUR;

    static constexpr const char *const TAG = "ferraris";

    FerrarisMeter::FerrarisMeter(uint32_t rpkwh)
        : Component()
        , m_digital_input_pin(nullptr)
#ifdef USE_SENSOR
        , m_analog_input_sensor(nullptr)
        , m_power_consumption_sensor(nullptr)
        , m_energy_meter_sensor(nullptr)
        , m_analog_value_spectrum_sensor(nullptr)
#endif
#ifdef USE_BINARY_SENSOR
        , m_rotation_indicator_sensor(nullptr)
        , m_analog_calibration_state_sensor(nullptr)
        , m_analog_calibration_result_sensor(nullptr)
#endif
#ifdef USE_SWITCH
        , m_calibration_mode_switch(nullptr)
#endif
#ifdef USE_NUMBER
        , m_analog_input_threshold_number(nullptr)
        , m_off_tolerance_number(nullptr)
        , m_on_tolerance_number(nullptr)
        , m_energy_start_value_number(nullptr)
#endif
        , m_analog_input_threshold(0.0f)
        , m_off_tolerance(0.0f)
        , m_on_tolerance(0.0f)
        , m_rotations_per_kwh(rpkwh)
        , m_debounce_threshold(0)
        , m_last_state(false)
        , m_last_time(-1)
        , m_last_rising_time(-1)
        , m_rotation_counter(0)
        , m_off_level(0.0)
        , m_on_level(0.0)
        , m_num_captured_values(6000)
        , m_min_level_distance(6.0)
        , m_max_iterations(3)
        , m_iteration_counter(0)
        , m_level_value_counter(m_num_captured_values)
        , m_calibration_mode(false)
        , m_start_value_received(false)
    {
    }

    void FerrarisMeter::setup()
    {
        ESP_LOGCONFIG(TAG, "Setting up Ferraris Meter...");

#ifdef USE_SENSOR
        if (m_analog_input_sensor != nullptr)
        {
            m_analog_input_sensor->add_on_state_callback([this](float value)
            {
                bool state = false;

                if (m_last_state)
                {
                    state = (value > m_analog_input_threshold - m_off_tolerance);
                }
                else
                {
                    state = (value > m_analog_input_threshold + m_on_tolerance);
                }

                handle_state(state);

                if (m_level_value_counter < m_num_captured_values)
                {
                    if (m_level_value_counter == 0)
                    {
                        ++m_iteration_counter;

                        ESP_LOGD(
                            TAG, "Starting automatic analog calibration:  CAPT %u  DIST %.1f  ITER %u/%u",
                            m_num_captured_values, m_min_level_distance, m_iteration_counter, m_max_iterations);
                        set_analog_calibration_state(true);

                        // use current value as initial state
                        m_on_level = value;
                        m_off_level = value;

                        ESP_LOGD(TAG, "Calibrating initial levels:  VAL %.1f", value);
                    }
                    else
                    {
                        if (value > m_on_level)
                        {
                            m_on_level = value;
                            ESP_LOGD(TAG, "Calibrating ON level:  VAL %.1f", m_on_level);
                        }

                        if (value < m_off_level)
                        {
                            m_off_level = value;
                            ESP_LOGD(TAG, "Calibrating OFF level:  VAL %.1f", m_off_level);
                        }
                    }

                    ++m_level_value_counter;

                    if (m_level_value_counter == m_num_captured_values)
                    {
                        if ((m_on_level >= m_off_level) && ((m_on_level - m_off_level) >= m_min_level_distance))
                        {
                            float threshold = (m_off_level + m_on_level) / 2;

                            if (m_analog_input_threshold_number != nullptr)
                            {
                                m_analog_input_threshold_number->publish_state(threshold);
                            }
                            else
                            {
                                m_analog_input_threshold = threshold;
                            }

                            ESP_LOGD(TAG, "Automatic analog calibration finished:  OFF %.1f  ON %.1f  TRSH %.1f", m_off_level, m_on_level, threshold);
                            set_analog_calibration_state(false, m_on_level - m_off_level);
                        }
                        else if (m_iteration_counter < m_max_iterations)
                        {
                            ESP_LOGW(TAG, "Insufficient data for analog calibration, starting over");
                            m_level_value_counter = 0;
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Too many failed analog calibration iterations, giving up");
                            set_analog_calibration_state(false, m_on_level - m_off_level, true);
                        }
                    }
                }
            });
        }
#endif

#ifdef USE_BINARY_SENSOR
        if (m_rotation_indicator_sensor != nullptr)
        {
            m_rotation_indicator_sensor->publish_state(false);
        }

        if (m_analog_calibration_state_sensor != nullptr)
        {
            m_analog_calibration_state_sensor->publish_state(false);
        }
#endif

#ifdef USE_NUMBER
        if (m_analog_input_threshold_number != nullptr)
        {
            if (m_analog_input_threshold_number->has_state())
            {
                m_analog_input_threshold = m_analog_input_threshold_number->state;
            }

            m_analog_input_threshold_number->add_on_state_callback([this](float value)
            {
                m_analog_input_threshold = value;
            });
        }

        if (m_off_tolerance_number != nullptr)
        {
            if (m_off_tolerance_number->has_state())
            {
                m_off_tolerance = m_off_tolerance_number->state;
            }

            m_off_tolerance_number->add_on_state_callback([this](float value)
            {
                m_off_tolerance = value;
            });
        }

        if (m_on_tolerance_number != nullptr)
        {
            if (m_on_tolerance_number->has_state())
            {
                m_on_tolerance = m_on_tolerance_number->state;
            }

            m_on_tolerance_number->add_on_state_callback([this](float value)
            {
                m_on_tolerance = value;
            });
        }

        if (m_debounce_threshold_number != nullptr)
        {
            if (m_debounce_threshold_number->has_state())
            {
                m_debounce_threshold = m_debounce_threshold_number->state;
            }

            m_debounce_threshold_number->add_on_state_callback([this](float value)
            {
                m_debounce_threshold = value;
            });
        }

        if (m_energy_start_value_number != nullptr)
        {
            if (m_energy_start_value_number->has_state())
            {
                restore_energy_meter(m_energy_start_value_number->state);
            }
            else
            {
                m_energy_start_value_number->add_on_state_callback([this](float value)
                {
                    restore_energy_meter(value);
                });
            }
        }
        else
        {
            update_energy_counter();
        }
#endif

#ifdef USE_SWITCH
        if (m_calibration_mode_switch != nullptr)
        {
            optional<bool> initial_state = m_calibration_mode_switch->get_initial_state_with_restore_mode();

            if (initial_state.has_value())
            {
                if (initial_state.value())
                {
                    m_calibration_mode_switch->turn_on();
                }
                else
                {
                    m_calibration_mode_switch->turn_off();
                }
            }
        }
#endif
    }

    void FerrarisMeter::loop()
    {
        if (m_digital_input_pin != nullptr)
        {
            handle_state(m_digital_input_pin->digital_read());
        }
    }

    void FerrarisMeter::dump_config()
    {
        ESP_LOGCONFIG(TAG, "Ferraris Meter");
        LOG_PIN("  Digital input pin: ", m_digital_input_pin);
#ifdef USE_SENSOR
#ifdef USE_NUMBER
        if ((m_analog_input_sensor != nullptr) && (m_analog_input_threshold_number == nullptr))
        {
            ESP_LOGCONFIG(TAG, "  Static analog input threshold: %.2f", m_analog_input_threshold);
        }
        if ((m_analog_input_sensor != nullptr) && (m_off_tolerance_number == nullptr))
        {
            ESP_LOGCONFIG(TAG, "  Static OFF tolerance: %.2f", m_off_tolerance);
        }
        if ((m_analog_input_sensor != nullptr) && (m_on_tolerance_number == nullptr))
        {
            ESP_LOGCONFIG(TAG, "  Static ON tolerance: %.2f", m_on_tolerance);
        }
#else
        if (m_analog_input_sensor != nullptr)
        {
            ESP_LOGCONFIG(TAG, "  Static analog input threshold: %.2f", m_analog_input_threshold);
        }
        if (m_analog_input_sensor != nullptr)
        {
            ESP_LOGCONFIG(TAG, "  Static OFF tolerance: %.2f", m_off_tolerance);
        }
        if (m_analog_input_sensor != nullptr)
        {
            ESP_LOGCONFIG(TAG, "  Static ON tolerance: %.2f", m_on_tolerance);
        }
#endif
#endif
        ESP_LOGCONFIG(TAG, "  Rotations per kWh: %d", m_rotations_per_kwh);
#ifdef USE_NUMBER
        if (m_debounce_threshold_number == nullptr)
        {
            ESP_LOGCONFIG(TAG, "  Static debounce threshold: %u ms", m_debounce_threshold);
        }
#else
        ESP_LOGCONFIG(TAG, "  Static debounce threshold: %d ms", m_debounce_threshold);
#endif
#ifdef USE_SENSOR
        LOG_SENSOR("", "Power consumption sensor", m_power_consumption_sensor);
        LOG_SENSOR("", "Energy meter sensor", m_energy_meter_sensor);
        LOG_SENSOR("", "Analog value spectrum sensor", m_analog_value_spectrum_sensor);
#endif
#ifdef USE_BINARY_SENSOR
        LOG_BINARY_SENSOR("", "Rotation indicator sensor", m_rotation_indicator_sensor);
        LOG_BINARY_SENSOR("", "Analog calibration state sensor", m_analog_calibration_state_sensor);
        LOG_BINARY_SENSOR("", "Analog calibration result sensor", m_analog_calibration_result_sensor);
#endif
#ifdef USE_SWITCH
        LOG_SWITCH("", "Calibration mode switch", m_calibration_mode_switch);
#endif
    }

    void FerrarisMeter::handle_state(bool state)
    {
        if (state != m_last_state)
        {
            ESP_LOGD(TAG, "State change:  %d -> %d", m_last_state, state);

            if (m_calibration_mode)
            {
#ifdef USE_BINARY_SENSOR
                if (m_rotation_indicator_sensor != nullptr)
                {
                    m_rotation_indicator_sensor->publish_state(state);
                }
#endif
            }
            else
            {
                uint32_t now = millis();

                if (state)
                {
                    if (m_last_rising_time < 0)
                    {
                        m_last_rising_time = now;
                    }
                    else
                    {
                        uint32_t falling_to_rising_duration = get_duration(m_last_time, now);

                        if (falling_to_rising_duration < m_debounce_threshold)
                        {
                            ESP_LOGD(TAG, "Ignoring falling to rising duration below threshold:  %u ms", falling_to_rising_duration);
                        }
                        else
                        {
                            uint32_t rotation_time = get_duration(m_last_rising_time, now);

                            ESP_LOGD(TAG, "Rotation time:  %u ms", rotation_time);

                            m_rotation_counter++;
                            ESP_LOGD(TAG, "Updated rotation counter:  %u rotations", m_rotation_counter);

                            update_power_consumption(rotation_time);
                            update_energy_counter();

                            m_last_rising_time = now;
                        }
                    }
                }

                m_last_time = now;
            }

            m_last_state = state;
        }
    }

    void FerrarisMeter::set_calibration_mode(bool mode)
    {
        m_calibration_mode = mode;

        if (m_calibration_mode)
        {
            m_last_time = -1;
            m_last_rising_time = -1;

#ifdef USE_SENSOR
            if (m_power_consumption_sensor != nullptr)
            {
                m_power_consumption_sensor->publish_state(0.0);
            }
#endif
        }

#ifdef USE_BINARY_SENSOR
        if (m_rotation_indicator_sensor != nullptr)
        {
            m_rotation_indicator_sensor->
                publish_state(
                    m_calibration_mode ? m_last_state : false);
        }
#endif
    }

    void FerrarisMeter::restore_energy_meter(float value)
    {
        if (!m_start_value_received)
        {
            m_rotation_counter = static_cast<uint64_t>(value / 1000 * m_rotations_per_kwh);
            ESP_LOGD(TAG, "Restored rotation counter:  %u rotations", m_rotation_counter);

            m_start_value_received = true;
            update_energy_counter();
        }
    }

    void FerrarisMeter::set_energy_meter(float value)
    {
        m_rotation_counter = static_cast<uint64_t>(std::round(value * m_rotations_per_kwh));
        ESP_LOGD(TAG, "Set energy meter:  %.2f kWh (%u rotations)", value, m_rotation_counter);

        update_energy_counter();
    }

    void FerrarisMeter::set_rotation_counter(uint64_t value)
    {
        m_rotation_counter = value;
        ESP_LOGD(TAG, "Set rotation counter:  %u rotations", m_rotation_counter);

        update_energy_counter();
    }

    void FerrarisMeter::update_power_consumption(uint32_t rotation_time)
    {
#ifdef USE_SENSOR
        if (m_power_consumption_sensor != nullptr)
        {
            float pwr = static_cast<float>(KWH_TO_WMS) / (rotation_time * m_rotations_per_kwh);

            m_power_consumption_sensor->publish_state(pwr);
            ESP_LOGD(TAG, "Published power consumption sensor state: %.2f W", pwr);
        }
#endif
    }

    void FerrarisMeter::update_energy_counter()
    {
#ifdef USE_SENSOR
        if (m_energy_meter_sensor != nullptr)
        {
            float energy = static_cast<float>(m_rotation_counter) / m_rotations_per_kwh * WATTS_PER_KW;

            m_energy_meter_sensor->publish_state(energy);
            ESP_LOGD(TAG, "Published energy meter sensor state: %.2f Wh (%d rotations)", energy, m_rotation_counter);
        }
#endif
    }

    void FerrarisMeter::set_analog_calibration_state(bool running, float range, bool problem)
    {
#ifdef USE_BINARY_SENSOR
        if (m_analog_calibration_state_sensor != nullptr)
        {
            m_analog_calibration_state_sensor->publish_state(running);
        }

        if (!running && (m_analog_calibration_result_sensor != nullptr))
        {
            m_analog_calibration_result_sensor->publish_state(problem);
        }
#endif
#ifdef USE_SENSOR
        if (!running && (m_analog_value_spectrum_sensor != nullptr))
        {
            m_analog_value_spectrum_sensor->publish_state(range);
        }
#endif
    }
}  // namespace esphome::ferraris
