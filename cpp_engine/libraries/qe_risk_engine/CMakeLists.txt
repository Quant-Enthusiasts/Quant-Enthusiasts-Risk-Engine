project(qe_risk_engine)

set(includes includes/)
set(sources src/BinomialTree.cpp
            src/BlackScholes.cpp
            src/ImpliedVolatilitySurface.cpp
            src/Instrument.cpp
            src/JumpDiffusion.cpp
            src/MarketData.cpp
            src/LocalMarketDB.cpp
            src/Portfolio.cpp
            src/RiskEngine.cpp
)

add_library(${PROJECT_NAME} SHARED ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

# Find and link SQLite3 for local market DB
find_package(SQLite3)
if (TARGET SQLite::SQLite3)
    target_link_libraries(${PROJECT_NAME} PRIVATE SQLite::SQLite3)
elseif (SQLite3_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SQLite3_LIBRARIES})
else()
    message(FATAL_ERROR "SQLite3 not found - LocalMarketDB requires sqlite3 library")
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