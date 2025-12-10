         cmake_minimum_required(VERSION 3.15)
project(websocket_client_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

# We removed cli.cpp as it was redundant
add_executable(websocket_client
    src/main.cpp
    src/websocket/websocket_client.cpp
    src/websocket/tls_context.cpp
)

target_include_directories(websocket_client PRIVATE src)

target_link_libraries(websocket_client
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32
)
