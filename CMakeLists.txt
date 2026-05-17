cmake_minimum_required(VERSION 4.3)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX OR APPLE)
    set(POSIX TRUE)
endif()

project(voxel_engine C)

file(GLOB_RECURSE SRC_FILES src/*.c)

if(POSIX)
list(APPEND SRC_FILES 
    src/threads/posix/mutex.c
    src/threads/posix/condition.c
)
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES})

add_subdirectory(lib/glfw)

add_library(glad lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

target_include_directories(voxel_engine PRIVATE
    lib/glad/include
    lib/stb/include
    lib/FastNoiseLite
    lib/cglm/include
)

target_link_libraries(voxel_engine PRIVATE
    glfw
    glad
)
