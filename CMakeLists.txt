cmake_minimum_required(VERSION 3.0)
project(sig_auth)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    hash.cpp
    signature.cpp
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

add_executable(main ${SOURCES})
