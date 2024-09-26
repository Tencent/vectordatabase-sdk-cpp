
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
