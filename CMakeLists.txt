cmake_minimum_required(VERSION 3.11)
include(cmake/warnings.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "/home/tibor/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(minecraft)

include(FetchContent)

FetchContent_Declare(FastNoise2 GIT_REPOSITORY https://github.com/lukasino1214/FastNoise2.git)
FetchContent_MakeAvailable(FastNoise2)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(daxa CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(minecraft "src/main.cpp" "src/camera.cpp" "src/chunk.cpp")
target_compile_features(minecraft PRIVATE cxx_std_20)
target_link_libraries(minecraft PRIVATE daxa::daxa glfw imgui::imgui glm::glm FastNoise2)
target_include_directories(minecraft PRIVATE ${Stb_INCLUDE_DIR})