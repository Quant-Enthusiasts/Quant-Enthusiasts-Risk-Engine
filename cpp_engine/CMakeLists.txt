cmake_minimum_required(VERSION 3.30)

project(QuantRiskEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure rpath for proper library linking in install directory
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set rpath for installed binaries to find libraries
# Binaries in bin/ need to find libraries in ../lib/
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../lib")
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")
endif()

# set(CUSTOM_LIBRARY_DIR_PYTHON ${CMAKE_SOURCE_DIR}/.venv/lib/python3.13/site-packages)

# message(STATUS "Python3_ROOT_DIR: ${Python3_ROOT_DIR}")
find_package(Python3 COMPONENTS Interpreter Development)

find_package(pybind11 REQUIRED)

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# if(WIN32)
#     message(STATUS "Windows detected")
#     set(dlloader_include_dir_platform ${CMAKE_SOURCE_DIR}/dl_loader/windows)
#     add_compile_definitions(WIN_EXPORT)
# endif()

# if(UNIX)
#     message(STATUS "Unix system detected")
#     set(dlloader_include_dir_platform ${CMAKE_SOURCE_DIR}/dl_loader/linux)
# endif()

# set(dlloader_include_dir ${CMAKE_SOURCE_DIR}/dl_loader)

MESSAGE(STATUS "the source directory is: ${CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "the binary directory is: ${CMAKE_BINARY_DIR}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libraries)
add_subdirectory(apps)
add_subdirectory(tests)

# Export configuration for the library
install(EXPORT qe_risk_engine-targets
    FILE qe_risk_engine-targets.cmake
    NAMESPACE QE::
    DESTINATION lib/cmake/qe_risk_engine
)