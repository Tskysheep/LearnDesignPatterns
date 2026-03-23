cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Visitor Visitor.cpp Visitor.h)
target_compile_features(Pattern_Visitor PRIVATE cxx_std_17)
