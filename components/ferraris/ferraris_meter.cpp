/*
 * Copyright (c) 2024 Jens-Uwe Rossbach
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
    static constexpr const uint32_t MS_PER_HOUR = 60 * 60 * 1000;
    static constexpr const uint32_t KWH_TO_WMS = WATTS_PER_KW * MS_PER_HOUR;

    static constexpr const char *const TAG = "ferraris";

    FerrarisMeter::FerrarisMeter(InternalGPIOPin* pin, uint32_t rpkwh, uint32_t low_state_threshold)
        : Component()
        , m_pin(pin)
#ifdef USE_SENSOR
        , m_power_consumption_sensor(nullptr)
        , m_energy_meter_sensor(nullptr)
#endif
#ifdef USE_BINARY_SENSOR
        , m_rotation_indicator_sensor(nullptr)
#endif
#ifdef USE_SWITCH
        , m_calibration_mode_switch(nullptr)
#endif
#ifdef USE_NUMBER
        , m_energy_start_value_number(nullptr)
#endif
        , m_rotations_per_kwh(rpkwh)
        , m_low_state_threshold(low_state_threshold)
        , m_last_state(false)
        , m_last_time(-1)
        , m_last_rising_time(-1)
        , m_rotation_counter(0)
        , m_calibration_mode(false)
        , m_start_value_received(false)
    {
    }

    void FerrarisMeter::setup()
    {
#ifdef USE_BINARY_SENSOR
        if (m_rotation_indicator_sensor != nullptr)
        {
            m_rotation_indicator_sensor->publish_state(false);
        }
#endif
#ifdef USE_SENSOR
        if (m_power_consumption_sensor != nullptr)
        {
            m_power_consumption_sensor->publish_state(0);
        }

        if (m_energy_start_value_number != nullptr)
        {
            m_energy_start_value_number->add_on_state_callback([this](float value)
            {
                if (!m_start_value_received)
                {
                    m_rotation_counter = static_cast<uint64_t>(value / 1000 * m_rotations_per_kwh);
                    ESP_LOGD(TAG, "Restored rotation counter:  %u rotations", m_rotation_counter);

                    m_start_value_received = true;
                    update_energy_counter();
                }
            });
        }
        else
        {
            update_energy_counter();
        }
#endif
    }

    void FerrarisMeter::loop()
    {
        bool state = m_pin->digital_read();

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
                        uint32_t low_state_duration = get_duration(m_last_time, now);

                        if (low_state_duration < m_low_state_threshold)
                        {
                            ESP_LOGD(TAG, "Ignoring low state duration below threshold:  %u ms", low_state_duration);
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

    void FerrarisMeter::dump_config()
    {
        ESP_LOGCONFIG(TAG, "Ferraris Meter");
        LOG_PIN("  Pin: ", m_pin);
        ESP_LOGCONFIG(TAG, "  Rotations per kWh: %d", m_rotations_per_kwh);
        ESP_LOGCONFIG(TAG, "  Low state threshold: %d ms", m_low_state_threshold);
#ifdef USE_SENSOR
        LOG_SENSOR("", "Power consumption sensor", m_power_consumption_sensor);
        LOG_SENSOR("", "Energy meter sensor", m_energy_meter_sensor);
#endif
#ifdef USE_BINARY_SENSOR
        LOG_BINARY_SENSOR("", "Rotation indicator sensor", m_rotation_indicator_sensor);
#endif
#ifdef USE_SWITCH
        LOG_SWITCH("", "Calibration mode switch", m_calibration_mode_switch);
#endif
    }

    void FerrarisMeter::set_calibration_mode(bool mode)
    {
        m_calibration_mode = mode;

#ifdef USE_BINARY_SENSOR
        if (m_rotation_indicator_sensor != nullptr)
        {
            m_rotation_indicator_sensor->
                publish_state(
                    mode ? m_last_state : false);
        }
#endif
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
}  // namespace esphome::ferraris
