cmake_minimum_required(VERSION 3.23)
project(fcg_trab_final)

set(CMAKE_CXX_STANDARD 17)

add_executable(fcg_trab_final main.cpp src/textrendering.cpp)
target_include_directories(fcg_trab_final PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(include/glfw)

add_subdirectory(include/glm)

target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw glm)
