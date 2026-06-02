cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    else()
        set(CMAKE_TOOLCHAIN_FILE "C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
endif()


set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
project(juicy-game VERSION 1.0.0 LANGUAGES CXX)

find_package(SDL2 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/eliott-ecs)
add_subdirectory(extern/eliott-math)
add_subdirectory(extern/eliott-renderer-3d)

add_executable(juicy-game
    "src/Scenes/main.cpp"
    src/Engine.cpp
    src/Utils.cpp
    "src/Scenes/mainScene.cpp"
    "src/Assets/ModelAsset.cpp"
    "src/Assets/ModelManager.cpp"
    "src/UI/UIElem.cpp"
    "src/UI/UIWidgetBasic.cpp"
    "src/UI/ParamsScreen.cpp"
    "src/Scenes/MenuScene.cpp"
    "src/Particles/ParticleSystem.cpp"
    "src/Input/InputManager.cpp"
    "src/Input/ControllerSelectScreen.cpp")

target_include_directories(juicy-game PUBLIC
    include/
)

if(MSVC)
    target_link_options(juicy-game PRIVATE /FORCE:MULTIPLE)
endif()

add_custom_command(TARGET juicy-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:juicy-game>/assets"
)
add_custom_command(TARGET juicy-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/include/juicy-game/Shaders"
        "$<TARGET_FILE_DIR:juicy-game>/assets/Shaders"
)

target_link_libraries(juicy-game PUBLIC
    eliott-ecs
    eliott-math
    eliott-renderer-3d
    raylib
    glfw
    SDL2::SDL2
)