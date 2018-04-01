project (rainbow510)

add_executable(rainbow510
    main.cpp
    LogitechLED.cpp
    KB_HID_access.cpp
    hidapi_mod.cpp
    Color.cpp)

include_directories(./)

target_link_libraries(rainbow510 PUBLIC libusb-1.0.so)
