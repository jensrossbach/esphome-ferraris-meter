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

#include "esphome/core/component.h"
#include "esphome/core/automation.h"

#include "ferraris_meter.h"


namespace esphome::ferraris
{
    template<typename... Ts> class SetEnergyMeterAction : public Action<Ts...>
    {
    public:
        SetEnergyMeterAction(FerrarisMeter *ferraris_meter)
            : m_ferraris_meter(ferraris_meter)
        {
        }

        void play(Ts... x) override
        {
            m_ferraris_meter->set_energy_meter(m_energy_meter_value.value(x...));
        }

        template<typename V> void set_energy_meter_value(V value)
        {
            m_energy_meter_value = value;
        }

    protected:
        FerrarisMeter *m_ferraris_meter;
        TemplatableValue<float, Ts...> m_energy_meter_value;
    };

    template<typename... Ts> class SetRotationCounterAction : public Action<Ts...>
    {
    public:
        SetRotationCounterAction(FerrarisMeter *ferraris_meter)
            : m_ferraris_meter(ferraris_meter)
        {
        }

        void play(Ts... x) override
        {
            m_ferraris_meter->set_rotation_counter(m_rotation_counter_value.value(x...));
        }

        template<typename V> void set_rotation_counter_value(V value)
        {
            m_rotation_counter_value = value;
        }

    protected:
        FerrarisMeter *m_ferraris_meter;
        TemplatableValue<uint64_t, Ts...> m_rotation_counter_value;
    };
}  // namespace esphome::ferraris
