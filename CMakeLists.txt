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

set(BOOST_DIR /usr/local/boost_1_67_0)
INCLUDE_DIRECTORIES($ENV{HOME} ./OTExtensionBristol include src ${BOOST_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -maes -mpclmul -O3")

configure_file(Tools.hpp.in include/Tools.hpp)

link_directories(/usr/ssl/lib/ ${BOOST_DIR}/stage/lib/ ${PROJECT_SOURCE_DIR}/OTExtensionBristol/ ${PROJECT_SOURCE_DIR}/OTExtensionBristol/SimpleOT/)

FILE(GLOB CSources ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(corrOT ${PROJECT_SOURCE_DIR}/src/OT.cpp include/IntMod.hpp ${PROJECT_SOURCE_DIR}/src/Tools.cpp ${PROJECT_SOURCE_DIR}/src/PRG.cpp ${PROJECT_SOURCE_DIR}/src/Communication/Comm.cpp ${PROJECT_SOURCE_DIR}/src/OT/OTExtensionBristol.cpp
        )
add_executable(testOT test/test.cpp)

TARGET_LINK_LIBRARIES(corrOT
        libOTExtensionBristol.a libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)

TARGET_LINK_LIBRARIES(testOT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libcorrOT.a
        libOTExtensionBristol.a libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)

add_custom_target(
        Bristol_OT
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/OTExtensionBristol/
)

add_dependencies(testOT corrOT)
add_dependencies(corrOT Bristol_OT)