# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/elton/Downloads/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(ProjectName C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ProjectName ProjectName.c)



pico_set_program_name(ProjectName "ProjectName")
pico_set_program_version(ProjectName "0.1")

pico_enable_stdio_uart(ProjectName 1)
pico_enable_stdio_usb(ProjectName 0)

# Add the standard library to the build
target_link_libraries(ProjectName
        pico_stdlib
        hardware_adc
        hardware_spi
        hardware_pwm
        hardware_timer
        )

# Add the standard include files to the build
target_include_directories(ProjectName PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/lib
)



# Add any user requested libraries
target_link_libraries(ProjectName 
        )

pico_add_extra_outputs(ProjectName)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
