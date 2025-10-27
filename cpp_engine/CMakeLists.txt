cmake_minimum_required(VERSION 3.30)

project(QuantRiskEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    # Align runtime with static QuantLib build to avoid /MD vs /MT mismatch
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Optional QuantLib integration for pricing validation and exotic options
option(USE_QUANTLIB "Enable QuantLib for pricing validation and exotic instruments" OFF)

# Find QuantLib if enabled
if(USE_QUANTLIB)
    find_package(QuantLib CONFIG QUIET)
    if(QuantLib_FOUND)
        message(STATUS "QuantLib found: ${QuantLib_VERSION}")
        message(STATUS "QuantLib libraries: ${QuantLib_LIBRARIES}")
        message(STATUS "QuantLib include dirs: ${QuantLib_INCLUDE_DIRS}")
    else()
        message(WARNING "USE_QUANTLIB is ON but QuantLib was not found. Building without QuantLib.")
        message(WARNING "Make sure vcpkg toolchain is configured: CMAKE_TOOLCHAIN_FILE should point to vcpkg.cmake")
        set(USE_QUANTLIB OFF)
    endif()
endif()

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
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Try to find an existing pybind11; if not found, fetch it automatically
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    message(STATUS "pybind11 not found; fetching with FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

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

# Enable CTest
enable_testing()

add_subdirectory(libraries)
add_subdirectory(apps)
add_subdirectory(tests)

# Export configuration for the library
install(EXPORT qe_risk_engine-targets
    FILE qe_risk_engine-targets.cmake
    NAMESPACE QE::
    DESTINATION lib/cmake/qe_risk_engine
)