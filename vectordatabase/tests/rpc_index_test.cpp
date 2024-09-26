#include <gtest/gtest.h>

#include "include/rpc_client.h"
#include "tests/rpc_client_test_base.h"

namespace vectordb {

TEST_F(RpcClientTestBase, RebuildIndex) {
    RebuildIndexResult result;
    RebuildIndexParams* params = new RebuildIndexParams();
    params->dropBeforeRebuild = true;
    params->throttle = 1;

    int status = client.rebuildIndex("test_db6", "book-test-2", params, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    ASSERT_FALSE(result.taskIds.empty());
    EXPECT_EQ(result.taskIds.size(), 1);
}

// 边界条件测试 - 空参数重建索引
TEST_F(RpcClientTestBase, RebuildIndexEmptyParams) {
    RebuildIndexResult result;
    int status = client.rebuildIndex("test_db6", "book-test-2", nullptr, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, -1);
    ASSERT_GT(result.taskIds.size(), 0);
}

// 错误处理测试 - 重建不存在的索引
TEST_F(RpcClientTestBase, RebuildNonExistentIndex) {
    RebuildIndexResult result;
    int status = client.rebuildIndex("test_db6", "non_existent_collection", nullptr, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, -1);
}

// 错误处理测试 - 空数据库名或集合名
TEST_F(RpcClientTestBase, RebuildIndexEmptyDbOrCollectionName) {
    RebuildIndexParams* params = new RebuildIndexParams();
    RebuildIndexResult result;

    int status = client.rebuildIndex("", "test_collection", params, &result);
    std::cout << result.message << std::endl;
    EXPECT_EQ(status, 0);

    status = client.rebuildIndex("test_db6", "", params, &result);
    EXPECT_EQ(status, -1);
}

}  // namespace vectordb
