# cli/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Define CLI source directories
set(CLI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CLI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include CLI headers
include_directories(${CLI_INCLUDE_DIR})

# Define source files for the CLI by finding them in subdirectories
file(GLOB_RECURSE COMMANDS_SOURCES "${CLI_SRC_DIR}/commands/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "${CLI_SRC_DIR}/utils/*.cpp")

# Define all CLI sources
set(BLAHAJPI_CLI_SOURCES
    ${CLI_SRC_DIR}/main.cpp
    ${CLI_SRC_DIR}/commands.cpp
    ${COMMANDS_SOURCES}
    ${UTILS_SOURCES}
)

# Create the CLI executable
add_executable(blahajpi_cli ${BLAHAJPI_CLI_SOURCES})

# Rename the output executable to just "blahajpi"
set_target_properties(blahajpi_cli PROPERTIES
    OUTPUT_NAME blahajpi
)

# Link to the library
target_link_libraries(blahajpi_cli PRIVATE blahajpi_lib)

# Set include directories
target_include_directories(blahajpi_cli
    PRIVATE
        ${CLI_SRC_DIR}
        ${CLI_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/lib/include
)

# Install the CLI executable
install(TARGETS blahajpi_cli
    RUNTIME DESTINATION bin
)

# Install CLI headers (if we want to make them accessible)
install(DIRECTORY ${CLI_INCLUDE_DIR}/bpicli
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# For Windows, copy required DLLs to the executable directory
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET blahajpi_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:blahajpi_lib>
            $<TARGET_FILE_DIR:blahajpi_cli>
        COMMENT "Copying shared library to CLI executable directory"
    )
endif()

# Add a symlink for the executable (Unix-like systems only)
if(UNIX)
    add_custom_command(TARGET blahajpi_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blahajpi
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blahaj-pi
        COMMENT "Creating symbolic link for backward compatibility"
    )
endif()

# Copy configuration files to the binary directory
file(GLOB CONFIG_FILES "${CMAKE_SOURCE_DIR}/configs/*.conf")
foreach(CONFIG_FILE ${CONFIG_FILES})
    get_filename_component(FILENAME ${CONFIG_FILE} NAME)
    add_custom_command(TARGET blahajpi_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CONFIG_FILE}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FILENAME}
        COMMENT "Copying configuration file: ${FILENAME}"
    )
endforeach()
