cmake_minimum_required(VERSION 3.20)
project(Fast_Quantiles)

set(CMAKE_CXX_STANDARD 17)

include_directories(ddsketch)

add_executable(Fast_Quantiles
        main.cpp
        ddsketch/ddsketch.cpp
        ddsketch/ddsketch.h
        ddsketch/dense_store.cpp
        ddsketch/dense_store.h
        ddsketch/logarithmic_mapping.cpp
        ddsketch/logarithmic_mapping.h
        exact_quantiles/exact_quantiles.cpp
        exact_quantiles/exact_quantiles.h)

set(CMAKE_CXX_FLAGS  "-ltbb -pthread -O3 -mcpu=native -mtune=native -Wall -march=native")
#TODO if time allows check if -02 is even faster(against common knowledge)

target_link_libraries(Fast_Quantiles -ltbb)
target_include_directories(Fast_Quantiles PUBLIC "${PROJECT_BINARY_DIR}")
