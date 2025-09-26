cmake_minimum_required(VERSION 3.16)

# Project configuration
project(stm32f411-usb-host-ps2 
    VERSION 1.0.0
    DESCRIPTION "STM32F411 USB Host to PS/2 Converter"
    LANGUAGES C ASM
)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set target processor and board
set(MCU_FAMILY STM32F4xx)
set(MCU_MODEL STM32F411xE)
set(MCU_BOARD STM32F411CEU6)

# ARM Cortex-M4F processor settings
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# Enable compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Toolchain configuration
include(cmake/toolchain/arm-none-eabi-gcc.cmake)

# STM32 HAL and CMSIS configuration
set(STM32_CMSIS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/CMSIS")
set(STM32_HAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/STM32F4xx_HAL_Driver")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/usb
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ps2
    ${STM32_CMSIS_PATH}/Include
    ${STM32_CMSIS_PATH}/Device/ST/STM32F4xx/Include
    ${STM32_HAL_PATH}/Inc
)

# Compiler definitions
add_definitions(
    -DSTM32F411xE
    -DUSE_HAL_DRIVER
    -DHSE_VALUE=25000000U
    -DHSI_VALUE=16000000U
    -DVDD_VALUE=3300U
    -DPREFETCH_ENABLE=1
    -DINSTRUCTION_CACHE_ENABLE=1
    -DDATA_CACHE_ENABLE=1
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_PARAMETERS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fno-builtin")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-Og -g3 -DDEBUG")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPU_PARAMETERS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/cmake/STM32F411CEUx_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

# Source files
set(SOURCES
    # Main application
    src/main.c
    src/system_init.c
    
    # HAL initialization
    src/hal/stm32f4xx_hal_msp.c
    src/hal/stm32f4xx_it.c
    src/hal/system_stm32f4xx.c
    
    # USB Host implementation
    src/usb/usb_host_init.c
    src/usb/usb_host_hid.c
    src/usb/keyboard_handler.c
    
    # PS/2 implementation
    src/ps2/ps2_init.c
    src/ps2/ps2_protocol.c
    src/ps2/scancode_translator.c
    
    # Startup file
    cmake/startup_stm32f411xe.s
)

# Create executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Link libraries (will be added when HAL is included)
# target_link_libraries(${PROJECT_NAME}.elf stm32f4xx_hal)

# Generate additional output formats
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMENT "Building ${PROJECT_NAME}.hex and ${PROJECT_NAME}.bin"
)

# Print size information
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Size information:"
)

# Optional: Create flash target for programming
add_custom_target(flash
    COMMAND st-flash write ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing ${PROJECT_NAME}.bin to STM32F411"
)

# Optional: Create debug target
add_custom_target(debug
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
    COMMENT "Starting OpenOCD for debugging"
)