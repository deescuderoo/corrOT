cmake_minimum_required(VERSION 3.10)
project(OT1)

INCLUDE_DIRECTORIES($ENV{HOME} $ENV{HOME}/libscapi/install/include/ $ENV{HOME}/libscapi/lib/OTExtensionBristol)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -maes -mpclmul -O3")


link_directories(/usr/ssl/lib/ $ENV{HOME}/libscapi/install/lib)

add_executable(OT1 main.cpp OT.cpp IntMod.tpp Tools.cpp)

TARGET_LINK_LIBRARIES(OT1 $ENV{HOME}/libscapi/libscapi.a
        OTExtensionBristol libsimpleot.a
        gmp gmpxx libboost_system.a pthread crypto dl ssl z)