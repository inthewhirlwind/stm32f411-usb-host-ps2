# STM32F411 USB Host to PS/2 Converter

A comprehensive project template for creating a USB keyboard to PS/2 protocol converter using the STM32F411CEU6 microcontroller.

## Overview

This project provides a complete framework to configure the STM32F411 as a USB host that can interface with USB keyboards and translate their input to PS/2 protocol output. This is useful for connecting modern USB keyboards to legacy systems that only support PS/2 input.

## Features

- **USB OTG Host Mode**: Full USB host implementation for HID keyboard detection
- **PS/2 Protocol Output**: Hardware-accurate PS/2 timing and signaling
- **Real-time Translation**: USB HID scan codes to PS/2 scan codes conversion
- **CMake Build System**: Modern, cross-platform build configuration
- **Comprehensive Documentation**: Well-documented code with clear examples
- **Modular Architecture**: Clean separation of USB, PS/2, and translation logic

## Hardware Requirements

### STM32F411CEU6 Board
- ARM Cortex-M4F running at 84 MHz
- 512KB Flash memory
- 128KB SRAM
- USB OTG FS peripheral
- Multiple timers for precise PS/2 timing

### Pin Configuration
```
USB OTG FS:
- PA11: USB_DM (Data Minus)
- PA12: USB_DP (Data Plus) 
- PA10: USB_ID (Optional for OTG detection)

PS/2 Interface:
- PA0: PS2_CLK (PS/2 Clock line, open-drain with pull-up)
- PA1: PS2_DATA (PS/2 Data line, open-drain with pull-up)

Status LED:
- PC13: STATUS_LED (Status indicator)
```

### External Components
- 25 MHz crystal oscillator (HSE)
- USB Type-A receptacle for keyboard connection
- PS/2 connector (6-pin mini-DIN) for output
- Pull-up resistors (4.7kΩ) for PS/2 lines
- Status LED with current limiting resistor

## Software Architecture

### Directory Structure
```
├── CMakeLists.txt              # Main CMake configuration
├── README.md                   # This file
├── build/                      # Build output directory
├── cmake/                      # CMake modules and configurations
│   ├── toolchain/              # ARM GCC toolchain configuration
│   ├── STM32F411CEUx_FLASH.ld  # Linker script
│   └── startup_stm32f411xe.s   # Startup assembly code
├── docs/                       # Documentation
├── examples/                   # Example configurations
├── include/                    # Header files
│   ├── hal/                    # HAL headers
│   ├── ps2/                    # PS/2 protocol headers
│   ├── usb/                    # USB host headers
│   └── main.h                  # Main application header
└── src/                        # Source files
    ├── hal/                    # Hardware abstraction layer
    ├── ps2/                    # PS/2 implementation
    ├── usb/                    # USB host implementation
    ├── main.c                  # Main application
    └── system_init.c           # System initialization
```

### Key Modules

#### USB Host (`src/usb/`)
- **usb_host_init.c**: USB OTG FS host mode initialization
- **usb_host_hid.c**: HID class driver implementation
- **keyboard_handler.c**: USB keyboard data processing and buffering

#### PS/2 Protocol (`src/ps2/`)
- **ps2_init.c**: PS/2 interface initialization and low-level functions
- **ps2_protocol.c**: PS/2 protocol implementation and scan code handling
- **scancode_translator.c**: USB HID to PS/2 scan code translation

#### HAL Layer (`src/hal/`)
- **system_init.c**: System clock and peripheral initialization
- **stm32f4xx_hal_msp.c**: HAL MSP (MCU Support Package) functions
- **stm32f4xx_it.c**: Interrupt service routines

## Building the Project

### Prerequisites

1. **ARM GCC Toolchain**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gcc-arm-none-eabi
   
   # macOS with Homebrew
   brew install --cask gcc-arm-embedded
   
   # Windows
   # Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
   ```

2. **CMake** (version 3.16 or higher)
   ```bash
   # Ubuntu/Debian
   sudo apt-get install cmake
   
   # macOS with Homebrew
   brew install cmake
   
   # Windows
   # Download from: https://cmake.org/download/
   ```

3. **Make** or **Ninja** build system

### Build Steps

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd stm32f411-usb-host-ps2
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. **Build the project**
   ```bash
   make -j$(nproc)
   ```

5. **Generated files**
   - `stm32f411-usb-host-ps2.elf` - Executable file
   - `stm32f411-usb-host-ps2.hex` - Intel HEX format for programming
   - `stm32f411-usb-host-ps2.bin` - Binary format for programming
   - `stm32f411-usb-host-ps2.map` - Memory map file

### Build Options

- **Debug build**: `cmake .. -DCMAKE_BUILD_TYPE=Debug`
- **Release build**: `cmake .. -DCMAKE_BUILD_TYPE=Release`
- **Custom toolchain**: `cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/arm-none-eabi-gcc.cmake`

## Programming and Debugging

### Using ST-Link
```bash
# Flash the binary
st-flash write stm32f411-usb-host-ps2.bin 0x08000000

# Or use the make target
make flash
```

### Using OpenOCD
```bash
# Start OpenOCD server
make debug

# In another terminal, connect with GDB
arm-none-eabi-gdb stm32f411-usb-host-ps2.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) monitor reset halt
(gdb) continue
```

### Using STM32CubeProgrammer
1. Connect ST-Link to your STM32F411 board
2. Open STM32CubeProgrammer
3. Select ST-LINK connection
4. Load the `.hex` or `.bin` file
5. Program the device

## Usage

### Basic Operation

1. **Power on the STM32F411 board**
2. **Connect a USB keyboard** to the USB Type-A connector
3. **Connect the PS/2 output** to your target system
4. **Observe the status LED**:
   - Fast blink: Initialization in progress
   - Slow blink: Ready, waiting for keyboard
   - Solid on: Active conversion in progress
   - Very fast blink: Error state

### Status Indicators

The application provides several ways to monitor its status:

- **LED Indicators**: Visual feedback through status LED
- **Application States**: Internal state machine for robust operation
- **Error Handling**: Comprehensive error detection and recovery

### Key Translation

The converter supports:
- **Standard keys**: A-Z, 0-9, function keys, modifiers
- **Extended keys**: Arrow keys, Home, End, Page Up/Down, Insert, Delete
- **Modifier keys**: Shift, Ctrl, Alt (both left and right variants)
- **Special keys**: Enter, Backspace, Tab, Escape, Space

## Configuration

### System Clock
- **HSE**: 25 MHz external crystal
- **PLL**: Configured for 84 MHz system clock
- **APB1**: 42 MHz (for timers and USB)
- **APB2**: 84 MHz (for high-speed peripherals)

### USB Configuration
- **Speed**: Full Speed (12 Mbps)
- **Channels**: 8 host channels available
- **Power**: Bus-powered mode
- **Class**: HID (Human Interface Device)

### PS/2 Timing
- **Clock Frequency**: 12 kHz (within 10-16.7 kHz spec)
- **Data Format**: 11-bit frame (start, 8 data, parity, stop)
- **Parity**: Odd parity
- **Timing**: Hardware-accurate bit timing

## Extending the Project

### Adding Support for New Keys
1. Update the key mapping table in `scancode_translator.c`
2. Add USB HID key codes to `keyboard_handler.h`
3. Add PS/2 scan codes to `ps2_protocol.h`

### Supporting Different USB Devices
1. Extend the HID class driver in `usb_host_hid.c`
2. Add device-specific handling in `keyboard_handler.c`
3. Update the enumeration process in `usb_host_init.c`

### Protocol Extensions
1. Add new protocol support in the `ps2/` directory
2. Create protocol-specific translation tables
3. Update the main application loop accordingly

## Troubleshooting

### Common Issues

1. **USB device not detected**
   - Check USB connections and power
   - Verify USB host initialization
   - Check device enumeration process

2. **PS/2 output not working**
   - Verify PS/2 timing configuration
   - Check PS/2 line states with oscilloscope
   - Ensure proper pull-up resistors

3. **Build failures**
   - Verify ARM GCC toolchain installation
   - Check CMake version compatibility
   - Ensure all dependencies are installed

### Debug Tips

1. **Use status LED** for basic operational feedback
2. **Enable debug build** for additional information
3. **Use debugger** to step through initialization
4. **Check timing** with logic analyzer or oscilloscope

## Contributing

### Code Style
- Follow existing naming conventions
- Add comprehensive comments to new functions
- Update documentation for any changes
- Test thoroughly before submitting

### Testing
- Verify with multiple USB keyboards
- Test with different PS/2 target systems
- Check edge cases and error conditions
- Validate timing requirements

## License

This project is provided as a template for educational and development purposes. Please ensure compliance with any applicable licenses for the STM32 HAL library and other dependencies.

## References

- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xc-e-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [USB HID Specification](https://www.usb.org/sites/default/files/documents/hid1_11.pdf)
- [PS/2 Protocol Specification](https://www.computer-engineering.org/ps2protocol/)
- [STM32 USB Host Library](https://www.st.com/en/embedded-software/stsw-stm32065.html)

## Support

For questions, issues, or contributions, please refer to the project's issue tracker or documentation.