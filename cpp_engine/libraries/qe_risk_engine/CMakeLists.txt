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

add_library(${PROJECT_NAME} SHARED ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

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