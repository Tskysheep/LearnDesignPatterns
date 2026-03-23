cmake_minimum_required(VERSION 3.16)
project(LearnDesignPatterns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC needs /utf-8 for source files with Chinese comments
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 组件协作
add_subdirectory(01_ComponentCollaboration)
# 单一职责
add_subdirectory(02_SingleResponsibility)
# 对象创建
add_subdirectory(03_ObjectCreation)
# 对象性能
add_subdirectory(04_ObjectPerformance)
# 接口隔离
add_subdirectory(05_InterfaceIsolation)
# 状态变化
add_subdirectory(06_StateChange)
# 数据结构
add_subdirectory(07_DataStructure)
# 行为变化
add_subdirectory(08_BehavioralChange)
# 领域问题
add_subdirectory(09_DomainProblem)
