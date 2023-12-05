set(HAKO_CMAKE_VERSION ${HAKO_CMAKE_VERSION})

project(hakoniwa-px4sim
    LANGUAGES C CXX
)

set(OS_TYPE "posix")
if(WIN32)
    set(OS_TYPE "win")
endif(WIN32)
MESSAGE(STATUS "OS_TYPE=" ${OS_TYPE})

add_executable(
    hako-px4sim
    comm/udp_connector.cpp
    comm/tcp_connector.cpp
    mavlink/mavlink_dump.cpp
    mavlink/mavlink_decoder.cpp
    mavlink/mavlink_encoder.cpp
    mavlink/mavlink_capture.cpp
    mavlink/mavlink_capture_replay.cpp

    hako/pdu/hako_pdu_data.cpp
    ${HAKONIWA_SOURCE_DIR}/hako_capi.cpp
    hako/runner/hako_px4_master.cpp
    hako/runner/hako_px4_runner.cpp
    hako/runner/hako_sim_runner.cpp
    hako/runner/drone/drone_phys.cpp
    hako/runner/drone/drone_phys_sensor.cpp
    hako/runner/control/drone_control.cpp

    threads/px4sim_thread_receiver.cpp
    threads/px4sim_thread_sender.cpp
    threads/px4sim_thread_replay.cpp
    threads/px4sim_thread_capture.cpp

    assets/drone/physics/body_frame/drone_dynamics_body_frame.cpp
    assets/drone/physics/ground_frame/drone_dynamics_ground_frame.cpp

    utils/hako_params.cpp
    modules/hako_bypass.cpp
    modules/hako_phys.cpp

    px4sim_main.cpp
)

target_include_directories(
    hako-px4sim
    PRIVATE /usr/local/include
    PRIVATE /mingw64/include
    PRIVATE ${MAVLINK_SOURCE_DIR}/all
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${HAKONIWA_PDU_SOURCE_DIR}
    PRIVATE ${HAKONIWA_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/assets/drone/include
    PRIVATE ${GLM_SOURCE_DIR}
    PRIVATE ${HAKONIWA_CORE_SOURCE_DIR}/include
)

target_link_libraries(hako-px4sim hakoarun)
