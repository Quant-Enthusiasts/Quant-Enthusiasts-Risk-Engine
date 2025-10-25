project(python_interface)

set(includes ../qe_risk_engine/includes/)
set(sources src/pybind_wrapper.cpp)

message(STATUS "[===] Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "[===] Python3_LIBRARIES: ${Python3_LIBRARIES}")
message(STATUS "[===] Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

pybind11_add_module(quant_risk_engine ${sources})
target_include_directories(quant_risk_engine PUBLIC ${includes} ${Python3_INCLUDE_DIRS})
target_link_libraries(quant_risk_engine PUBLIC qe_risk_engine ${Python3_LIBRARIES})

# Install Python module alongside other libraries
install(TARGETS quant_risk_engine
    COMPONENT python
    LIBRARY DESTINATION lib
)