# Parameters for compilation
set(N 999)
set(K 128)
set(K_ 1)
set(POWER 32)
set(INT_TYPE "unsigned int")

# CMake internals
cmake_minimum_required(VERSION 3.5)
project(correlated_OT)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

INCLUDE_DIRECTORIES($ENV{HOME} $ENV{HOME}/libscapi/install/include/ $ENV{HOME}/libscapi/lib/OTExtensionBristol include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -maes -mpclmul -O3")

configure_file(Tools.hpp.in include/Tools.hpp)

link_directories(/usr/ssl/lib/ $ENV{HOME}/libscapi/install/lib)

add_library(corrOT ${PROJECT_SOURCE_DIR}/src/OT.cpp include/IntMod.hpp ${PROJECT_SOURCE_DIR}/src/Tools.cpp ${PROJECT_SOURCE_DIR}/src/PRG.cpp)
add_executable(testOT test/test.cpp)

TARGET_LINK_LIBRARIES(corrOT $ENV{HOME}/libscapi/libscapi.a
        OTExtensionBristol libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)

TARGET_LINK_LIBRARIES(testOT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libcorrOT.a $ENV{HOME}/libscapi/libscapi.a
        OTExtensionBristol libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)

add_dependencies(testOT corrOT)

#message(${PROJECT_SOURCE_DIR})
#message(${PROJECT_BINARY_DIR})
