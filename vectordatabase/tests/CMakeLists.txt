enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(runTests
    rpc_database_test.cpp
    rpc_collection_test.cpp
    rpc_index_test.cpp
    rpc_document_test.cpp
    filter_test.cpp
)

target_link_libraries(runTests vectordb_sdk GTest::GTest GTest::Main)