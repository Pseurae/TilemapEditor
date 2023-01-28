cmake_minimum_required(VERSION 3.17)
project(TilemapEditor)

set(EXEC_NAME "Tilemap Editor")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    add_compile_definitions(OS_WINDOWS)
elseif(MSVC OR MSYS OR MINGW)
    add_compile_definitions(OS_WINDOWS)
    add_compile_definitions(OS_WINCROSS)
elseif(APPLE)
    add_compile_definitions(OS_MACOS)
elseif(UNIX)
    add_compile_definitions(OS_LINUX)
endif()

add_subdirectory(libs)

set(SRC_FILES
    src/main.cpp
    src/core/actions.cpp
    src/core/context.cpp
    src/core/tilemap.cpp
    src/core/tileset.cpp
    src/core/window.cpp
    src/helpers/events.cpp
    src/helpers/fs.cpp
    src/helpers/hotkeys.cpp
    src/helpers/registry.cpp
    src/helpers/texture.cpp
    src/panes/tilemap.cpp
    src/panes/tileset.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${RES_FILES})

if (APPLE)
    file(GLOB_RECURSE RES_FILES "res/*")
    set_source_files_properties(${RES_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/res)
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        OUTPUT_NAME ${EXEC_NAME}
        MACOSX_BUNDLE TRUE
    )
    target_sources(${PROJECT_NAME} PUBLIC ${RES_FILES})
else()
    file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} imgui plog stb_image nfd)
target_include_directories(${PROJECT_NAME} PUBLIC src)

