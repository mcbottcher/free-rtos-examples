# 01 Down Indicator

FreeRTOS example running on the STM32F3 Discovery board. Uses the onboard LSM303DLHC accelerometer/magnetometer to determine which way is "down" and lights the corresponding LED on the compass ring.

**Target**: STM32F303xC (Cortex-M4F)

## Project Configuration

The initial project structure (clock configuration, peripheral setup, linker script, and startup files) was generated using [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html). The `discover_free_rtos.ioc` file is the STM32CubeMX project file — it stores all the pin/peripheral/clock configuration and can be opened in STM32CubeMX to regenerate or modify the low-level setup.

## Prerequisites

[STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html) must be installed. The installer adds the following to your `PATH`:

- `cmake` — build system generator
- `ninja` — build tool
- `arm-none-eabi-gcc` — ARM cross-compiler
- `arm-none-eabi-objcopy`, `arm-none-eabi-size` — post-build utilities

The examples were built with STM32CubeCLT 1.21.0.

## Building

### Configure

```bash
cmake --preset Debug
```

Or for a release build:

```bash
cmake --preset Release
```

This generates Ninja build files under `build/Debug/` (or `build/Release/`).

### Compile

```bash
cmake --build --preset Debug
```

The output ELF is written to `build/Debug/discover_free_rtos.elf`. Memory usage is printed at the end of the build:

```
Memory region         Used Size  Region Size  %age Used
             RAM:        8224 B        40 KB     20.08%
          CCMRAM:           0 B         8 KB      0.00%
           FLASH:       25756 B       256 KB      9.83%
```

### Clean

Delete the build directory to do a clean build:

```bash
rm -rf build/
```
