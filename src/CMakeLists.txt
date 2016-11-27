include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(autopilot)
add_subdirectory(core)
add_subdirectory(test)
add_subdirectory(sensor)
add_subdirectory(settings)
add_subdirectory(uadc)
add_subdirectory(util)
add_subdirectory(vn200)
