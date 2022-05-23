cmake_minimum_required(VERSION 3.13)
project(knx-virtual)

include(FetchContent)

FetchContent_Declare(
    knx-iot-stack
    GIT_REPOSITORY https://github.com/KNX-IOT/KNX-IOT-STACK.git
    GIT_TAG master
)

FetchContent_Declare(
    knx-iot-stack-gitlab
    GIT_REPOSITORY https://gitlab.knx.org/knxiot/knx-iot-point-api-open-stack.git
    GIT_TAG master
)


set(USE_GITLAB OFF CACHE BOOL "use gitlab as source for KNX IoT Stack")

if(USE_GITLAB)
    if(NOT knx-iot-stack_POPULATED)
        message(STATUS "Fetching KNX-IoT Source Code, please wait...")
        FetchContent_Populate(knx-iot-stack-gitlab)
        add_subdirectory(${knx-iot-stack-gitlab_SOURCE_DIR} ${knx-iot-stack-gitlab_BINARY_DIR})
    endif()
else()
    if(NOT knx-iot-stack_POPULATED)
        message(STATUS "Fetching KNX-IoT Source Code, please wait...")
        FetchContent_Populate(knx-iot-stack)
        add_subdirectory(${knx-iot-stack_SOURCE_DIR} ${knx-iot-stack_BINARY_DIR})
    endif()
endif()


add_executable(knx_iot_virtual_pb
    ${PROJECT_SOURCE_DIR}/knx_iot_virtual_pb.c
)
target_link_libraries(knx_iot_virtual_pb kisClientServer)


add_executable(knx_iot_virtual_sa
    ${PROJECT_SOURCE_DIR}/knx_iot_virtual_sa.c
)
target_link_libraries(knx_iot_virtual_sa kisClientServer)


add_executable(knx_iot_virtual_dimming_actuator
    ${PROJECT_SOURCE_DIR}/knx_iot_virtual_dimming_actuator.c
)
target_link_libraries(knx_iot_virtual_dimming_actuator kisClientServer)

if(WIN32)
    find_package(wxWidgets REQUIRED COMPONENTS net core base)
    include(${wxWidgets_USE_FILE})
    add_executable(knx_iot_virtual_gui_pb WIN32
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_gui_pb.cpp
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_pb.c)
    target_link_libraries(knx_iot_virtual_gui_pb ${wxWidgets_LIBRARIES} kisClientServer)
    target_compile_definitions(knx_iot_virtual_gui_pb PUBLIC KNX_GUI)
    set_target_properties(knx_iot_virtual_gui_pb PROPERTIES
        LINK_FLAGS /SUBSYSTEM:CONSOLE
    )

    add_executable(knx_iot_virtual_gui_sa WIN32
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_gui_sa.cpp
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_sa.c)
    target_link_libraries(knx_iot_virtual_gui_sa ${wxWidgets_LIBRARIES} kisClientServer)
    target_compile_definitions(knx_iot_virtual_gui_sa PUBLIC KNX_GUI)
    set_target_properties(knx_iot_virtual_gui_sa PROPERTIES
        LINK_FLAGS /SUBSYSTEM:CONSOLE
    )

endif()

if(UNIX)

    # Raspberry Pi demos, which embed Python in order to drive a Displayotron hat
    find_package(Python3 COMPONENTS Interpreter Development)

    add_custom_target(pi_hat_py)
    add_custom_command(
        TARGET pi_hat_py POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${PROJECT_SOURCE_DIR}/pi_hat.py
        ${PROJECT_BINARY_DIR}/pi_hat.py
    )

    # pi demo: switch actuator (e.g. receiving command)
    add_executable(knx_iot_sa_pi
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_sa.c
        ${PROJECT_SOURCE_DIR}/knx_iot_sa_pi.c
    )
    target_link_libraries(knx_iot_sa_pi
            kisClientServer
            Python3::Python
        )
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/knx_iot_virtual_sa_creds)
    add_dependencies(knx_iot_sa_pi pi_hat_py)
    target_compile_definitions(knx_iot_sa_pi PUBLIC NO_MAIN)

    # [o demo: push button (e.g. sending command)
    add_executable(knx_iot_pb_pi
        ${PROJECT_SOURCE_DIR}/knx_iot_virtual_pb.c
        ${PROJECT_SOURCE_DIR}/knx_iot_pb_pi.c
    )
    target_link_libraries(knx_iot_pb_pi
            kisClientServer
            Python3::Python
        )
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/knx_iot_virtual_pb_creds)
    add_dependencies(knx_iot_pb_pi pi_hat_py)
    target_compile_definitions(knx_iot_pb_pi PUBLIC NO_MAIN)
endif()
