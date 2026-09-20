cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm-main LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_FLAGS "-mthumb -mfloat-abi=hard -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -Wall -Wextra -std=gnu11 -O0 -g")
set(CMAKE_CXX_FLAGS "-mthumb -mfloat-abi=hard -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -fno-exceptions -fno-rtti -fno-threadsafe-statics -std=gnu++23 -O0 -g")

include_directories("${CMAKE_SOURCE_DIR}/../shared")

file(GLOB_RECURSE CPPSRCS "src/*.cpp")
file(GLOB_RECURSE CSRCS "src/*.c" "startup/*.c" "freertos/source/*.c" "freertos/ARM_CM4F/port.c")

add_executable(${PROJECT_NAME}.elf ${CSRCS} ${CPPSRCS})

target_link_options(${PROJECT_NAME}.elf PRIVATE
    "--specs=nano.specs"
    "--specs=nosys.specs"
    "-mcpu=cortex-m4"
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
    "-mthumb"
    "-lstdc++"
    "-nostartfiles" 
    "-T${CMAKE_SOURCE_DIR}/startup/linker-script.ld"
    "-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map"
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/source/ARM_CM4F
    $(CMAKE_CURRENT_SOURCE_DIR)/../shared
)

add_custom_target(load
    COMMAND openocd -f /usr/share/openocd/scripts/board/st_nucleo_f4.cfg
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Loading main.elf to board via OpenOCD..."    
)