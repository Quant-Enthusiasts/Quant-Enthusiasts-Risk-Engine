cmake_minimum_required(VERSION 3.10)
project(QuantRiskEngine VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build test executables" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(UTILS_DIR ${SOURCE_DIR}/utils)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

if(NOT EXISTS ${SOURCE_DIR})
    message(FATAL_ERROR "Source directory not found: ${SOURCE_DIR}")
endif()

set(CORE_SOURCES
    ${SOURCE_DIR}/Instrument.cpp
    ${SOURCE_DIR}/MarketData.cpp
    ${SOURCE_DIR}/Portfolio.cpp
    ${SOURCE_DIR}/RiskEngine.cpp
)

set(UTILS_SOURCES
    ${UTILS_DIR}/BlackScholes.cpp
    ${UTILS_DIR}/BinomialTree.cpp
    ${UTILS_DIR}/JumpDiffusion.cpp
    ${UTILS_DIR}/ImpliedVolatilitySurface.cpp
)

foreach(src_file ${CORE_SOURCES} ${UTILS_SOURCES})
    if(NOT EXISTS ${src_file})
        message(WARNING "Source file not found: ${src_file}")
    endif()
endforeach()

set(ALL_SOURCES ${CORE_SOURCES} ${UTILS_SOURCES})

if(ENABLE_WARNINGS)
    if(MSVC)
        set(WARNING_FLAGS /W4 /WX-)
    else()
        set(WARNING_FLAGS -Wall -Wextra -pedantic -Wno-unused-parameter)
    endif()
endif()

if(WIN32)
    add_compile_definitions(NOMINMAX _USE_MATH_DEFINES)
    if(MSVC)
        set(PLATFORM_FLAGS /utf-8)
    endif()
else()
    set(PLATFORM_FLAGS -fPIC)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND PLATFORM_FLAGS -stdlib=libstdc++)
    endif()
endif()

add_executable(risk_engine ${SOURCE_DIR}/main.cpp ${ALL_SOURCES})

target_include_directories(risk_engine 
    PUBLIC 
        ${SOURCE_DIR}
        ${UTILS_DIR}
)

target_compile_options(risk_engine 
    PRIVATE 
        ${WARNING_FLAGS}
        ${PLATFORM_FLAGS}
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(risk_engine PRIVATE DEBUG_MODE)
    if(NOT MSVC)
        target_compile_options(risk_engine PRIVATE -g -O0)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(risk_engine PRIVATE NDEBUG)
    if(MSVC)
        target_compile_options(risk_engine PRIVATE /O2)
    else()
        target_compile_options(risk_engine PRIVATE -O3)
    endif()
endif()

if(BUILD_TESTS)
    enable_testing()
    
    if(NOT EXISTS ${TESTS_DIR})
        message(WARNING "Tests directory not found: ${TESTS_DIR}. Skipping test builds.")
        set(BUILD_TESTS OFF)
    endif()
    
    if(BUILD_TESTS)
        set(TEST_BLACKSCHOLES_SOURCES
            ${TESTS_DIR}/test_blackscholes.cpp
            ${UTILS_DIR}/BlackScholes.cpp
        )
        
        set(TEST_BINOMIAL_SOURCES
            ${TESTS_DIR}/test_binomial.cpp
            ${UTILS_DIR}/BinomialTree.cpp
            ${UTILS_DIR}/BlackScholes.cpp
        )
        
        set(TEST_PORTFOLIO_SOURCES
            ${TESTS_DIR}/test_portfolio.cpp
            ${SOURCE_DIR}/Portfolio.cpp
            ${SOURCE_DIR}/Instrument.cpp
            ${SOURCE_DIR}/MarketData.cpp
            ${UTILS_DIR}/BlackScholes.cpp
            ${UTILS_DIR}/BinomialTree.cpp
            ${UTILS_DIR}/JumpDiffusion.cpp
        )
        
        set(TEST_RISK_ENGINE_SOURCES
            ${TESTS_DIR}/test_risk_engine.cpp
            ${SOURCE_DIR}/RiskEngine.cpp
            ${SOURCE_DIR}/Portfolio.cpp
            ${SOURCE_DIR}/Instrument.cpp
            ${SOURCE_DIR}/MarketData.cpp
            ${UTILS_DIR}/BlackScholes.cpp
            ${UTILS_DIR}/BinomialTree.cpp
            ${UTILS_DIR}/JumpDiffusion.cpp
        )
        
        function(add_test_executable TEST_NAME TEST_SOURCES)
            set(test_missing FALSE)
            foreach(src_file ${TEST_SOURCES})
                if(NOT EXISTS ${src_file})
                    message(WARNING "Test source file not found: ${src_file}")
                    set(test_missing TRUE)
                endif()
            endforeach()
            
            if(NOT test_missing)
                add_executable(${TEST_NAME} ${TEST_SOURCES})
                target_include_directories(${TEST_NAME} 
                    PRIVATE 
                        ${SOURCE_DIR}
                        ${UTILS_DIR}
                )
                target_compile_options(${TEST_NAME} 
                    PRIVATE 
                        ${WARNING_FLAGS}
                        ${PLATFORM_FLAGS}
                )
                add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
                set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 60)
            else()
                message(WARNING "Skipping test ${TEST_NAME} due to missing source files")
            endif()
        endfunction()
        
        if(EXISTS ${TESTS_DIR}/test_blackscholes.cpp)
            add_test_executable(test_blackscholes "${TEST_BLACKSCHOLES_SOURCES}")
        endif()
        
        if(EXISTS ${TESTS_DIR}/test_binomial.cpp)
            add_test_executable(test_binomial "${TEST_BINOMIAL_SOURCES}")
        endif()
        
        if(EXISTS ${TESTS_DIR}/test_portfolio.cpp)
            add_test_executable(test_portfolio "${TEST_PORTFOLIO_SOURCES}")
        endif()
        
        if(EXISTS ${TESTS_DIR}/test_risk_engine.cpp)
            add_test_executable(test_risk_engine "${TEST_RISK_ENGINE_SOURCES}")
        endif()
        
        set(all_test_targets)
        foreach(test_name test_blackscholes test_binomial test_portfolio test_risk_engine)
            if(TARGET ${test_name})
                list(APPEND all_test_targets ${test_name})
            endif()
        endforeach()
        
        if(all_test_targets)
            add_custom_target(run_all_tests
                DEPENDS ${all_test_targets}
                COMMENT "Running all available tests..."
            )
            
            foreach(test_target ${all_test_targets})
                add_custom_command(TARGET run_all_tests POST_BUILD
                    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R ${test_target}
                    COMMENT "Running ${test_target}..."
                )
            endforeach()
        endif()
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(risk_engine PRIVATE pthread)
    if(BUILD_TESTS)
        foreach(test_target ${all_test_targets})
            if(TARGET ${test_target})
                target_link_libraries(${test_target} PRIVATE pthread)
            endif()
        endforeach()
    endif()
endif()

set_target_properties(risk_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME "risk-engine"
)

if(BUILD_TESTS AND all_test_targets)
    foreach(test_target ${all_test_targets})
        if(TARGET ${test_target})
            set_target_properties(${test_target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
            )
        endif()
    endforeach()
endif()

# --- START OF PYBIND11 ---

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

set(BINDINGS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../python_api/pybind_wrapper.cpp)

pybind11_add_module(quant_risk_engine
    ${BINDINGS_SOURCE}
    ${ALL_SOURCES}
)

target_link_libraries(quant_risk_engine PRIVATE
    ${Python_LIBRARIES}
)

target_include_directories(quant_risk_engine PRIVATE
    ${PYBIND11_INCLUDE_DIRS}
    ${SOURCE_DIR}
    ${UTILS_DIR}
)

target_compile_options(quant_risk_engine PRIVATE
    ${WARNING_FLAGS}
    ${PLATFORM_FLAGS}
)

set_target_properties(quant_risk_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(quant_risk_engine PRIVATE pthread)
endif()

# --- END OF PYBIND11 ---

message(STATUS "========================================")
message(STATUS "Quant Risk Engine Configuration")
message(STATUS "========================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:          ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System:            ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build tests:       ${BUILD_TESTS}")
message(STATUS "Warnings enabled:  ${ENABLE_WARNINGS}")
message(STATUS "Source directory:  ${SOURCE_DIR}")
message(STATUS "Binary directory:  ${CMAKE_BINARY_DIR}")
message(STATUS "========================================")

if(BUILD_TESTS AND all_test_targets)
    message(STATUS "Test executables:")
    foreach(test_target ${all_test_targets})
        message(STATUS "  - ${test_target}")
    endforeach()
    message(STATUS "========================================")
endif()