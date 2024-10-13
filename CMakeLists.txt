cmake_minimum_required(VERSION 3.20)

# Project name
project(Hucker)

# Set target platform for macOS
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_OSX_DEPLOYMENT_TARGET 12.7)
set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_OBJCXX_COMPILER /usr/local/opt/llvm/bin/clang++)
enable_language(OBJCXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -stdlib=libc++")
set(CMAKE_OBJCXX_FLAGS_RELEASE "-Os -fobjc-arc -stdlib=libc++")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp-extensions)

# C++ source files
file(GLOB CPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# mm source files
file(GLOB OBJCPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.mm
)

# C source file
set(C_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/rd_route.c)

# Define the library target
add_library(Hucker SHARED 
    ${OBJCPP_SOURCES}
    ${C_SOURCE}
    ${CPP_SOURCES}
)

# Link with Metal and other frameworks
target_link_libraries(Hucker 
    "-framework Metal"
    "-framework QuartzCore" 
    "-framework Foundation" 
    "-framework Cocoa" 
    "-framework Carbon" 
    "-framework GameController" 
    "-framework CoreGraphics" 
    "-framework MetalKit"
)