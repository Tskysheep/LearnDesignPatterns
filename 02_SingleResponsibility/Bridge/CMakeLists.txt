cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Bridge Bridge.cpp Bridge.h)
target_compile_features(Pattern_Bridge PRIVATE cxx_std_17)
