# ESP32-C3 boot loop / crash on startup (Load access fault)

This document describes a boot-loop issue observed when using this Ferraris meter ESPHome component on an **ESP32-C3** (e.g. “Supermini”) and the fix that resolves it.

## Symptoms

- The device resets very early during boot, often right after the log line:
  - `Setting up Ferraris Meter...`
- Serial log shows a Guru Meditation crash:
  - `Guru Meditation Error: Core 0 panic'ed (Load access fault)`
- The decoded backtrace points into:
  - `esphome::EntityBase::has_state() const`
  - called from `esphome::ferraris::FerrarisMeter::setup()`.

This looks like a “boot loop” (repeated resets). Adding bulk capacitance may not help because this is a **software crash**, not a brownout.

## Root cause

In `FerrarisMeter`, the member pointer `m_debounce_threshold_number` (type `number::Number*`) was **not initialized** in the constructor.

- Uninitialized pointers may contain random non-null values.
- In `FerrarisMeter::setup()` the code checks `if (m_debounce_threshold_number != nullptr)` and then calls `m_debounce_threshold_number->has_state()`.
- If the uninitialized pointer happens to be non-null, the `has_state()` call dereferences an invalid address, causing a **Load access fault** and immediate reboot.

This explains why the device could “sometimes boot”: the pointer contents differ between runs/builds.

## Fix

The fix is to **explicitly initialize** `m_debounce_threshold_number` to `nullptr` in the constructor initializer list.

- File: [components/ferraris/ferraris_meter.cpp](components/ferraris/ferraris_meter.cpp)
- Change: add `, m_debounce_threshold_number(nullptr)` next to the other `number::Number*` initializations.

### Additional hardening (unrelated but crash-class)

A second issue was also fixed:

- Logging used the wrong format specifier for a `uint64_t` rotation counter (`%u` instead of `%llu`).
- On ESP32 targets, `printf`-style format mismatches are undefined behavior and can corrupt the stack.

- File: [components/ferraris/ferraris_meter.cpp](components/ferraris/ferraris_meter.cpp)
- Change: log `m_rotation_counter` via `%llu` and cast to `unsigned long long`.

## How to verify

1. Flash the firmware with the updated component.
2. Watch the serial log.
3. The crash should no longer occur immediately after `Setting up Ferraris Meter...`.

If you still see resets, check the reset reason:

- **Brownout** messages → power supply / wiring / sensor current draw.
- **Watchdog** messages → long blocking code or excessive work in callbacks.
- **Load access fault / IllegalInstruction** → still a software crash (collect a backtrace).

## Workarounds (if you cannot update the component)

There is no reliable configuration-only workaround for an uninitialized pointer bug.

If you are stuck on an older version:

- Remove the component from the build or temporarily disable it to confirm the crash source.
- Then update to a version that includes the initialization fix.
