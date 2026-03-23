cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Command Command.cpp Command.h)
target_compile_features(Pattern_Command PRIVATE cxx_std_17)
