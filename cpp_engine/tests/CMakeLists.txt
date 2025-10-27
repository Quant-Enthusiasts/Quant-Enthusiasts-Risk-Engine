project(tests)

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

add_executable(test_marketdata_cache src/test_marketdata_cache.cpp)
target_include_directories(test_marketdata_cache PUBLIC ${includes})
target_link_libraries(test_marketdata_cache qe_risk_engine sqlite3)

install(TARGETS test_marketdata_cache DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(test_marketdata_integration src/test_marketdata_integration.cpp)
target_include_directories(test_marketdata_integration PUBLIC ${includes})
target_link_libraries(test_marketdata_integration qe_risk_engine)

install(TARGETS test_marketdata_integration DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)