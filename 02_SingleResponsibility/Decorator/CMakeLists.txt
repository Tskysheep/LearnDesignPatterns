cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Decorator Decorator.cpp Decorator.h)
target_compile_features(Pattern_Decorator PRIVATE cxx_std_17)
