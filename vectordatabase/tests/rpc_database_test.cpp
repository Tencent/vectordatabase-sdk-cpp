#include <gtest/gtest.h>

#include "include/rpc_client.h"
#include "tests/rpc_client_test_base.h"

namespace vectordb {

TEST_F(RpcClientTestBase, CreateDatabase) {
    CreateDatabaseResult result;
    int status = client.createDatabase("test_db8", &result);
    std::cout << result.message << std::endl;
    EXPECT_EQ(status, 0);
    EXPECT_GT(result.affectedCount, 0);
}


TEST_F(RpcClientTestBase, ListDatabases) {
    ListDatabaseResult result;
    int status = client.listDatabases(&result);
    EXPECT_EQ(status, 0);
    ASSERT_FALSE(result.databases.empty());
    bool found = false;
    for (const auto& db : result.databases) {
        if (db->name == "test_db8") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
    std::cout << result.message << std::endl;
}

TEST_F(RpcClientTestBase, DropDatabase) {
    DropDatabaseResult dropResult;
    int status = client.dropDatabase("test_db8", &dropResult);
    EXPECT_EQ(status, 0);
    EXPECT_GT(dropResult.affectedCount, 0);
    ListDatabaseResult listResult;
    client.listDatabases(&listResult);
    bool found = false;
    for (const auto& db : listResult.databases) {
        if (db->name == "test_db8") {
            found = true;
            break;
        }
    }
    EXPECT_FALSE(found);
}
}  // namespace vectordb
