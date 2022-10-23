cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(cpp_wifidriver
    cpp_wifidriver.cpp
)
target_include_directories(cpp_wifidriver PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
)

target_link_libraries(cpp_wifidriver
        pico_stdlib              # for core functionality
        pico_cyw43_arch_lwip_threadsafe_background
)

# enable usb output, disable uart output
pico_enable_stdio_usb(cpp_wifidriver 1)
pico_enable_stdio_uart(cpp_wifidriver 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(cpp_wifidriver)
