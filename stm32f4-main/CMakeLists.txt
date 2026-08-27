cmake_minimum_required(VERSION 3.20)

# POPRAWKA KLUCZOWA: Wymuszenie kompilatora ARM jako zmiennej CACHE (musi być FORCE)
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# Informujemy CMake, że budujemy na mikrokontroler (Generic/bare-metal), a nie na PC Linuxa!
set(CMAKE_SYSTEM_NAME Generic CACHE STRING "Target system" FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm-main LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_FLAGS "-mthumb -mfloat-abi=hard -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -Wall -Wextra -std=gnu11 -O0 -g")
set(CMAKE_CXX_FLAGS "-mthumb -mfloat-abi=hard -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -fno-exceptions -fno-rtti -fno-threadsafe-statics -std=gnu++23 -O0 -g")

file(GLOB_RECURSE CPPSRCS "src/*.cpp")
file(GLOB_RECURSE CSRCS "src/*.c" "startup/*.c")

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
)

add_custom_target(load
    COMMAND openocd -f /usr/share/openocd/scripts/board/st_nucleo_f4.cfg
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Loading main.elf to board via OpenOCD..."    
)

file(GLOB_RECURSE ALL_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_custom_target(clang-hicpp
    COMMAND clang-tidy 
        ${ALL_CPP_FILES}
        --checks=-*,cppcoreguidelines-*,hicpp-*
        --config=""
        --header-filter=inc/*.hpp
        --
        -std=c++23
        -I${CMAKE_CURRENT_SOURCE_DIR}/inc
        -I${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
        -I${CMAKE_CURRENT_SOURCE_DIR}/freertos/source/ARM_CM4F
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Running Clang-Tidy on C++ source files only..."
)

