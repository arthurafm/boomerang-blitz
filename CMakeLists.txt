cmake_minimum_required(VERSION 3.23)
project(fcg_trab_final)

set(CMAKE_CXX_STANDARD 17)

add_executable(fcg_trab_final main.cpp src/tiny_obj_loader.cpp src/stb_image.cpp src/Camera.cpp src/SceneObject.cpp src/LoadedObj.cpp src/Window.cpp src/matrices.cpp src/Renderer.cpp include/Renderer.h src/Model.cpp include/Model.h src/collisions.cpp include/collisions.h)
target_include_directories(fcg_trab_final PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(include/glfw)

add_subdirectory(include/glm)

target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw glm)
