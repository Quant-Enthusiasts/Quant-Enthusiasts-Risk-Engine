project(tests)

# Enable CTest
enable_testing()

set(includes ./includes 
            ../libraries/ 
            ../libraries/qe_risk_engine/includes/
)

add_executable(test_blackscholes src/test_blackscholes.cpp)
target_include_directories(test_blackscholes PUBLIC ${includes})
target_link_libraries(test_blackscholes qe_risk_engine)

install(TARGETS test_blackscholes DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(test_portfolio src/test_portfolio.cpp)
target_include_directories(test_portfolio PUBLIC ${includes})
target_link_libraries(test_portfolio qe_risk_engine)

install(TARGETS test_portfolio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(test_risk_engine src/test_risk_engine.cpp)
target_include_directories(test_risk_engine PUBLIC ${includes})
target_link_libraries(test_risk_engine qe_risk_engine)

install(TARGETS test_risk_engine DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# QuantLib integration tests (only if QuantLib is enabled)
if(USE_QUANTLIB)
    add_executable(test_quantlib_integration src/test_quantlib_integration.cpp)
    target_include_directories(test_quantlib_integration PUBLIC ${includes})
    target_link_libraries(test_quantlib_integration qe_risk_engine)
    install(TARGETS test_quantlib_integration DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    
    # Register with CTest
    add_test(NAME QuantLibIntegration COMMAND test_quantlib_integration)
    set_tests_properties(QuantLibIntegration PROPERTIES
        TIMEOUT 120
        LABELS "integration;quantlib"
    )
endif()

# Register other tests with CTest
add_test(NAME BlackScholesTests COMMAND test_blackscholes)
set_tests_properties(BlackScholesTests PROPERTIES
    TIMEOUT 30
    LABELS "unit;pricing"
)

add_test(NAME PortfolioTests COMMAND test_portfolio)
set_tests_properties(PortfolioTests PROPERTIES
    TIMEOUT 30
    LABELS "unit;portfolio"
)

add_test(NAME RiskEngineTests COMMAND test_risk_engine)
set_tests_properties(RiskEngineTests PROPERTIES
    TIMEOUT 60
    LABELS "integration;risk"
)
