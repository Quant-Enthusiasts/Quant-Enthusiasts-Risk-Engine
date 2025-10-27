project(qe_risk_engine)

set(includes includes/)
set(sources src/BinomialTree.cpp
            src/BlackScholes.cpp
            src/ImpliedVolatilitySurface.cpp
            src/Instrument.cpp
            src/JumpDiffusion.cpp
            src/MarketData.cpp
            src/Portfolio.cpp
            src/RiskEngine.cpp
)

# Add QuantLib wrapper if enabled
if(USE_QUANTLIB)
    list(APPEND sources src/QuantLibPricingEngine.cpp)
endif()

# On Windows, building as STATIC avoids needing explicit symbol exports for a DLL import library
add_library(${PROJECT_NAME} STATIC ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

# Link QuantLib if enabled
if(USE_QUANTLIB)
    target_link_libraries(${PROJECT_NAME} PUBLIC QuantLib::QuantLib)
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_QUANTLIB)
endif()

# Set proper install name for macOS
set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_NAME_DIR "@rpath"
)

install(TARGETS qe_risk_engine
    EXPORT qe_risk_engine-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY includes/
    DESTINATION include/qe_risk_engine
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)