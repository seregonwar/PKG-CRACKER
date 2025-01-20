cmake_minimum_required(VERSION 3.10)
project(PKG-CRACKER)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent
include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Source files
set(SOURCES
    main.cpp
    src/core/bruteforce.cpp
    src/ui/menu.cpp
    src/utils/command_utils.cpp
    src/utils/file_utils.cpp
)

# Create executable
add_executable(PKG-CRACKER ${SOURCES})

# Include directories
target_include_directories(PKG-CRACKER PRIVATE ${CMAKE_SOURCE_DIR})

# Link libraries
target_link_libraries(PKG-CRACKER PRIVATE nlohmann_json::nlohmann_json)

# List of executables to copy
set(REQUIRED_EXECUTABLES
    orbis-pub-cmd.exe
    prospero-pub-cmd.exe
)

# Copy each required executable to the output directory
foreach(EXECUTABLE ${REQUIRED_EXECUTABLES})
    add_custom_command(TARGET PKG-CRACKER POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/${EXECUTABLE}"
            "$<TARGET_FILE_DIR:PKG-CRACKER>/${EXECUTABLE}"
        COMMENT "Copying ${EXECUTABLE} to output directory"
    )
endforeach()

# Optional: Install the executables
install(FILES ${REQUIRED_EXECUTABLES} DESTINATION bin)
