cmake_minimum_required(VERSION 3.22.0)
project(SRT VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp

    src/vec3.cpp
    src/vec3.hpp

    src/camera.hpp

    src/color.hpp

    src/hittable_list.hpp
    src/hittable.hpp

    src/ray.hpp

    src/sphere.hpp

    src/utils.hpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
