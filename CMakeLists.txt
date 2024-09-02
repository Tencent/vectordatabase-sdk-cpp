cmake_minimum_required(VERSION 3.10)

project(VectorDatabaseSDK VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/vectordatabase)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/vectordatabase/src/*.cpp" "${CMAKE_SOURCE_DIR}/vectordatabase/proto/*.cc")
add_library(vectordb_sdk STATIC ${SOURCES})

add_subdirectory(vectordatabase/tests)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

if(NOT gRPC_FOUND)
    message(FATAL_ERROR "Could not find gRPC library.")
endif()

target_link_libraries(vectordb_sdk gRPC::grpc++ ${PROTOBUF_LIBRARY})