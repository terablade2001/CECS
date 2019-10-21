cmake_minimum_required(VERSION 2.8.11)

project(ThreadTest)
# set(CECSDEBUG "-DCECSDEBUG")
set(CECSDEBUG "")

if (MINGW)
	set(CXX_FLAGS "-std=c++11 -Wall -D__ECSOBJ__=ECS")
	set(C_FLAGS "-Wall -D__ECSOBJ__=ECS")
endif(MINGW)
if (UNIX)
	set(CXX_FLAGS "-std=c++11 -Wall -Wno-unused-result -D__ECSOBJ__=ECS")
	set(C_FLAGS "-Wall -Wno-unused-result -D__ECSOBJ__=ECS")
endif(UNIX)
if (MSVC)
  set(CXX_FLAGS "-std=c++11 -W1 -D__ECSOBJ__=ECS ${CECSDEBUG}")
  set(C_FLAGS "-W1 -D__ECSOBJ__=ECS ${CECSDEBUG}")
endif (MSVC)

set(CMAKE_CONFIGURATION_TYPES "Release")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${C_FLAGS}")
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L. ")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../../src/include/
  ${CMAKE_CURRENT_LIST_DIR}/PThreadPool/include/
)
set (CXX_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS-c.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../main-test00.cpp
)
# set (CXX_SRCS
#   ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/../../src/CECS-c.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/main.cpp
#   ${CMAKE_CURRENT_LIST_DIR}/PThreadPool/PThreadPool.cpp
# )

# set (C_SRCS
    # ${CMAKE_CURRENT_LIST_DIR}/../../../C/src/CECS.c
# )

add_executable(ThreadTest ${CXX_SRCS} ${C_SRCS})
target_link_libraries(ThreadTest ${LINK_OPTIONS})
