cmake_minimum_required(VERSION 3.14)
project(SpeedOfHumanDetector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- OpenCV ----
# You likely use OpenCV. Make sure it's installed on your system.
find_package(OpenCV REQUIRED)

# ---- Sources ----
# Adjust filenames to match your repo
set(SOURCES
    src/main.cpp
    src/SpeedDetector.cpp
    src/Tracker.cpp
)

add_executable(speed_detector ${SOURCES})

target_include_directories(speed_detector PRIVATE include)

target_link_libraries(speed_detector PRIVATE ${OpenCV_LIBS})
