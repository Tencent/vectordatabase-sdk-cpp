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
