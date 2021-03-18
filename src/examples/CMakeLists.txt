cmake_minimum_required(VERSION 3.17)
project(swamp-snapshot C)

set(CMAKE_C_STANDARD 99)


set(deps ../../deps/)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(isDebug TRUE)
else()
    set(isDebug FALSE)
endif()

add_executable(swamp-snapshot
    main.c
)

target_include_directories(swamp-snapshot PRIVATE ${deps}clog/src/include)
target_include_directories(swamp-snapshot PRIVATE ${deps}tiny-libc/src/include)
target_include_directories(swamp-snapshot PRIVATE ${deps}runtime-c/src/include)
target_include_directories(swamp-snapshot PRIVATE ${deps}dump-c/src/include)
target_include_directories(swamp-snapshot PRIVATE ${deps}typeinfo-c/src/include)
target_include_directories(swamp-snapshot PRIVATE ${deps}flood-c/src/include)

target_link_libraries(swamp-snapshot swamp_snapshot_lib m)
