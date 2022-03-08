project(example-application)
cmake_minimum_required(VERSION 3.13)

include(FetchContent)

FetchContent_Declare(
    knx-iot-stack
    GIT_REPOSITORY https://github.com/KNX-IOT/KNX-IOT-STACK.git
    GIT_TAG master
)

if(NOT knx-iot-stack_POPULATED)
	FetchContent_Populate(knx-iot-stack)
	add_subdirectory(${knx-iot-stack_SOURCE_DIR} ${knx-iot-stack_BINARY_DIR})
endif()

add_executable(knx_iot_virtual_pb
	${PROJECT_SOURCE_DIR}/knx_iot_virtual_pb.c
)
target_link_libraries(knx_iot_virtual_pb kisClientServer)


add_executable(knx_iot_virtual_sa
	${PROJECT_SOURCE_DIR}/knx_iot_virtual_sa.c
)
target_link_libraries(knx_iot_virtual_sa kisClientServer)