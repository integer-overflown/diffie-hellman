cmake_minimum_required(VERSION 3.20)
project(lab4_diffie_hellman)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

add_executable(lab4_diffie_hellman main.cpp)
target_link_libraries(lab4_diffie_hellman
        Qt::Core
)
