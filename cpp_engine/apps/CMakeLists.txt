project(apps)

set(includes ./ 
            ../libraries/ 
            ../libraries/qe_risk_engine/includes/
)

add_executable(demo main.cpp)
target_include_directories(demo PUBLIC ${includes})
target_link_libraries(demo qe_risk_engine)

install(TARGETS demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

