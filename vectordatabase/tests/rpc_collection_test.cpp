/*
 *Copyright (c) 2024, Tencent. All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of elasticfaiss nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gtest/gtest.h>

#include "include/rpc_client.h"
#include "tests/rpc_client_test_base.h"

namespace vectordb {

TEST_F(RpcClientTestBase, CreateCollection) {
    CreateCollectionResult result;
    Indexes indexes;

    // 初始化VectorIndex
    VectorIndex vecIndex;
    vecIndex.fieldName = "vector";
    vecIndex.fieldType = kVector;
    vecIndex.indexType = kHNSW;
    vecIndex.dimension = 3;
    vecIndex.metricType = COSINE;
    vecIndex.params.m = 16;
    vecIndex.params.efConstruction = 200;
    indexes.vectorIndex.push_back(vecIndex);

    indexes.filterIndex = std::vector<FilterIndex>{
        {"id", kString, kPRIMARY},
        {"bookName", kString, kFILTER},
        {"page", kUnit64, kFILTER},
        {"tag", kArray, kFILTER}
    };


    int status = client.createCollection("test_db5", "test_collection2", 1, 0, "Test Collection",
                                        indexes, nullptr, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);
}

TEST_F(RpcClientTestBase, ListCollections) {
    ListCollectionResult result;
    int status = client.listCollections("test_db5", &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);
    ASSERT_FALSE(result.collections.empty());

    bool found = false;
    for (const auto& collection : result.collections) {
        if (collection->collectionName == "test_collection2") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(RpcClientTestBase, DescribeCollection) {
    DescribeCollectionResult result;
    int status = client.describeCollection("test_db5", "test_collection2", &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_EQ(result.collection->collectionName, "test_collection2");
}

TEST_F(RpcClientTestBase, TruncateCollection) {
    TruncateCollectionResult result;
    int status = client.truncateCollection("test_db5", "test_collection2", &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_GT(result.affectedCount, 0);
}

TEST_F(RpcClientTestBase, DropCollection) {
    DropCollectionResult result;
    int status = client.dropCollection("test_db6", "test_collection2", &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_GT(result.affectedCount, 0);

    ListCollectionResult listResult;
    client.listCollections("test_db5", &listResult);
    bool found = false;
    for (const auto& collection : listResult.collections) {
        if (collection->collectionName == "test_collection2") {
            found = true;
            break;
        }
    }
    EXPECT_FALSE(found);
}

// 边界条件测试 - 删除不存在的集合
TEST_F(RpcClientTestBase, DropNonExistentCollection) {
    DropCollectionResult result;
    int status = client.dropCollection("test_db5", "non_existent_collection", &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);
}

}  // namespace vectordb
