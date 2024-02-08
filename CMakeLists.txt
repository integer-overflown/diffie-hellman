cmake_minimum_required(VERSION 3.23)
project(lab4_diffie_hellman)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(FindDeps)

add_executable(lab4_diffie_hellman main.cpp)
target_link_libraries(lab4_diffie_hellman
        Qt6::Core
)

add_subdirectory(src)

target_link_libraries(lab4_diffie_hellman diffie_hellman)
