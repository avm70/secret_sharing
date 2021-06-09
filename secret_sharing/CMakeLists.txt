cmake_minimum_required(VERSION 2.8.12)

project(secret_sharing)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Sql)

set(SOURCES "tcpserver.cpp" "main.cpp" "tcpserver.h" "usermanager.h" "usermanager.cpp" "user.h" "user.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Network)
target_link_libraries(${PROJECT_NAME} Qt5::Sql)
