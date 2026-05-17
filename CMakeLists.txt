cmake_minimum_required(VERSION 3.20)
project(juicy-game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/eliott-ecs)
add_subdirectory(extern/eliott-audio)
add_subdirectory(extern/eliott-math)
add_subdirectory(extern/eliott-renderer-3d)
add_subdirectory(extern/eliott-physics-3d)

add_executable(juicy-game
    src/main.cpp
)

target_include_directories(juicy-game PUBLIC
    include/
)

target_link_libraries(juicy-game PUBLIC
    eliott-ecs
    eliott-audio
    eliott-math
    eliott-renderer-3d
    eliott-physics-3d
)