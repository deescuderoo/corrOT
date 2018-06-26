cmake_minimum_required(VERSION 3.10)
project(OT1)

INCLUDE_DIRECTORIES($ENV{HOME} $ENV{HOME}/libscapi/install/include/ $ENV{HOME}/libscapi/lib/OTExtensionBristol)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -maes -mpclmul -O3")


link_directories(/usr/ssl/lib/ $ENV{HOME}/libscapi/install/lib)


add_library(corrOT OT.cpp IntMod.tpp Tools.cpp PRG.cpp)
add_executable(cOT main.cpp)


TARGET_LINK_LIBRARIES(corrOT $ENV{HOME}/libscapi/libscapi.a
        OTExtensionBristol libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)

TARGET_LINK_LIBRARIES(cOT $ENV{HOME}/corrOT/cmake-build-debug/libcorrOT.a $ENV{HOME}/libscapi/libscapi.a
        OTExtensionBristol libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)