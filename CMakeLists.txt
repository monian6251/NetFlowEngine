cmake_minimum_required(VERSION 3.5)
project(NetFlowEngine)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Boost COMPONENTS log_setup log)

set(SOURCE_FILES
        NetflowController.cpp
        NetflowPacket.cpp
        NetflowLog.cpp
        Device.cpp
        NetflowTemplateV9.cpp)
add_executable(NetFlowEngine main.cpp ${SOURCE_FILES})

target_link_libraries(NetFlowEngine  ${Boost_LIBRARIES})