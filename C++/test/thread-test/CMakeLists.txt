# cmake -G "CodeLite - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_BUILD_TYPE=Release ..
# set(CMAKE_C_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-gcc-5.1.0.exe")
# set(CMAKE_CXX_COMPILER "c:/TDM-GCC-64/bin/x86_64-w64-mingw32-g++.exe")
cmake_minimum_required(VERSION 2.8.11)

project(ThreadTest)
# set(CECSDEBUG "-DCECSDEBUG")
set(CECSDEBUG "")

set(CXX_FLAGS "-std=c++11 -Wall -Wno-unused-result -D__ECSOBJ__=ECS ${CECSDEBUG}")
set(C_FLAGS "-Wall -Wno-unused-result -D__ECSOBJ__=ECS ${CECSDEBUG}")

set(CMAKE_CONFIGURATION_TYPES "Release")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${C_FLAGS}")
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L. ")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set (CXX_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS-c.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/PThreadPool/PThreadPool.cpp
)
# ${CMAKE_CURRENT_LIST_DIR}/../main-test00.cpp
# set (C_SRCS
    # ${CMAKE_CURRENT_LIST_DIR}/../../../C/src/CECS.c
# )

add_executable(ThreadTest ${CXX_SRCS} ${C_SRCS})
target_link_libraries(ThreadTest ${LINK_OPTIONS})
