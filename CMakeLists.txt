cmake_minimum_required(VERSION 2.8)

project(MyBot)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall -Wno-unused-function -pedantic")

include_directories(${CMAKE_SOURCE_DIR}/hlt)
include_directories(${CMAKE_SOURCE_DIR}/bot)
include_directories(${CMAKE_SOURCE_DIR}/bot/routing)
include_directories(${CMAKE_SOURCE_DIR}/bot/debug)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

foreach(dir ${dirs})
    file(GLOB_RECURSE SOURCE ${dir}/*.[ch]*)
    set(SOURCE_FILES "${SOURCE_FILES}" ${SOURCE})
endforeach()

include_directories(${CMAKE_SOURCE_DIR})
set(SOURCE_FILES "${SOURCE_FILES}" MyBot.cpp bot/mapping/force_map.h bot/mapping/connection.h bot/mapping/imap.h bot/routing/pathfinding.h bot/mapping/point.h bot/entities/entity.h bot/bot.cpp)

add_executable(MyBot ${SOURCE_FILES})
