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


#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#include "esphome/core/hal.h"

#include <limits>


namespace esphome::ferraris
{
    class FerrarisMeter : public Component
    {
    public:
        FerrarisMeter(InternalGPIOPin *pin, uint32_t rpkwh, uint32_t low_state_threshold);
        virtual ~FerrarisMeter() = default;

        void setup() override;
        void loop() override;

        void dump_config() override;

        void set_calibration_mode(bool mode);

        void set_energy_meter(float value);
        void set_rotation_counter(uint64_t value);

#ifdef USE_SENSOR
        void set_power_consumption_sensor(sensor::Sensor *power_consumption_sensor)
        {
            m_power_consumption_sensor = power_consumption_sensor;
        }

        void set_energy_meter_sensor(sensor::Sensor *energy_meter_sensor)
        {
            m_energy_meter_sensor = energy_meter_sensor;
        }
#endif

#ifdef USE_BINARY_SENSOR
        void set_rotation_indicator_sensor(binary_sensor::BinarySensor *rotind_sensor)
        {
            m_rotation_indicator_sensor = rotind_sensor;
        }
#endif

#ifdef USE_SWITCH
        void set_calibration_mode_switch(switch_::Switch *calibration_mode_switch)
        {
            m_calibration_mode_switch = calibration_mode_switch;
        }
#endif

#ifdef USE_NUMBER
        void set_energy_start_value_number(number::Number* energy_start_value_number)
        {
            m_energy_start_value_number = energy_start_value_number;
        }
#endif

    private:
        void update_power_consumption(uint32_t rotation_time);
        void update_energy_counter();

        static inline uint32_t get_duration(uint32_t time1, uint32_t time2)
        {
            uint32_t ret = 0;

            if (time2 < time1)  // overflow after ~50 days
            {
                ret = std::numeric_limits<uint32_t>::max() - time1 + time2;
            }
            else
            {
                ret = time2 - time1;
            }

            return ret;
        }

    protected:
        InternalGPIOPin* m_pin;
#ifdef USE_SENSOR
        sensor::Sensor* m_power_consumption_sensor;
        sensor::Sensor* m_energy_meter_sensor;
#endif
#ifdef USE_BINARY_SENSOR
        binary_sensor::BinarySensor* m_rotation_indicator_sensor;
#endif
#ifdef USE_SWITCH
        switch_::Switch* m_calibration_mode_switch;
#endif
#ifdef USE_NUMBER
        number::Number* m_energy_start_value_number;
#endif

        uint32_t m_rotations_per_kwh;
        uint32_t m_low_state_threshold;

        bool m_last_state;
        int64_t m_last_time;
        int64_t m_last_rising_time;
        uint64_t m_rotation_counter;

        bool m_calibration_mode;
        bool m_start_value_received;
    };
}  // namespace esphome::ferraris
