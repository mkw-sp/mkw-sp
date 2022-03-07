project(Fuzzing)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR}/../payload)
include_directories(${PROJECT_SOURCE_DIR}/../vendor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Fuzzing
    # (Windows)
    ${PROJECT_SOURCE_DIR}/../payload/sp/Emulator/NetEmuWin.cpp
    # (Cross-platform)
    ${PROJECT_SOURCE_DIR}/../payload/sp/Emulator/OSEmu.cpp
    ${PROJECT_SOURCE_DIR}/../payload/sp/Host.c
    ${PROJECT_SOURCE_DIR}/../payload/sp/NetStorageClient.c
    ${PROJECT_SOURCE_DIR}/../payload/sp/Yaz.c
	tests.cpp
)

if (ASAN)
    add_compile_options("-fsanitize=address")
    set(ENV{ASAN_OPTIONS} "detect_leaks=1")
    add_compile_definitions(BUILD_ASAN)
endif()