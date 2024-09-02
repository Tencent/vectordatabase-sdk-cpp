
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

TEST_F(RpcClientTestBase, UpsertDocument) {
    UpsertDocumentResult result;
    std::vector<Document> documents = {
        {
            "0001",
            {0.2123f, 0.21f, 0.213f},
            {
                {"bookName", Field("西游记")},
                {"author", Field("吴承恩")},
                {"page", Field(static_cast<uint64_t>(21u))},
                {"segment", Field("富贵功名，前缘分定，为人切莫欺心。")},
                {"tag", Field(std::vector<std::string>{"孙悟空", "猪八戒", "唐僧"})}
            }
        },
        {
            "0002",
            {0.2123f, 0.22f, 0.213f},
            {
                {"bookName", Field("西游记")},
                {"author", Field("吴承恩")},
                {"page", Field(static_cast<uint64_t>(22u))},
                {"segment", Field("正大光明，忠良善果弥深。些些狂妄天加谴，眼前不遇待时临。")},
                {"tag", Field(std::vector<std::string>{"孙悟空", "猪八戒", "唐僧"})}
            }
        },
        {
            "0003",
            {0.2123f, 0.23f, 0.213f},
            {
                {"bookName", Field("三国演义")},
                {"author", Field("罗贯中")},
                {"page", Field(static_cast<uint64_t>(23u))},
                {"segment", Field("细作探知这个消息，飞报吕布。")},
                {"tag", Field(std::vector<std::string>{"曹操", "诸葛亮", "刘备"})}
            }
        },
        {
            "0004",
            {0.2123f, 0.24f, 0.213f},
            {
                {"bookName", Field("三国演义")},
                {"author", Field("罗贯中")},
                {"page", Field(static_cast<uint64_t>(24u))},
                {"segment", Field("布大惊，与陈宫商议。宫曰：“闻刘玄德新领徐州，可往投之。”布从其言，竟投徐州来。有人报知玄德。")},
                {"tag", Field(std::vector<std::string>{"曹操", "诸葛亮", "刘备"})}
            }
        },
        {
            "0005",
            {0.2123f, 0.25f, 0.213f},
            {
                {"bookName", Field("三国演义")},
                {"author", Field("罗贯中")},
                {"page", Field(static_cast<uint64_t>(25u))},
                {"segment", Field("玄德曰：“布乃当今英勇之士，可出迎之。”糜竺曰：“吕布乃虎狼之徒，不可收留；收则伤人矣。")},
                {"tag", Field(std::vector<std::string>{"曹操", "诸葛亮", "刘备"})}
            }
        }
    };
    UpsertDocumentParams* params = new UpsertDocumentParams();
    params->buildIndex = true;
    int status = client.upsert("test_db5", "test_collection2", documents, params, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);
    EXPECT_GT(result.affectedCount, 0);
}

TEST_F(RpcClientTestBase, QueryDocument) {
    QueryDocumentResult result;
    std::vector<std::string> documentIds;
    QueryDocumentParams* params = new QueryDocumentParams();
    params->limit = 100;
    int status = client.query("test_db5", "test_collection2", documentIds, params, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_GT(result.documents.size(), 0);
}

TEST_F(RpcClientTestBase, SearchDocument) {
    SearchDocumentResult result;
    SearchDocumentParams* params = new SearchDocumentParams();
    params->searchParams = std::make_unique<SearchParms>();
    params->searchParams->ef = 100;
    params->retrieveVector = false;
    params->limit = 10;
    std::vector<std::vector<float>> vectors = {
        {0.3123, 0.43, 0.213},
        {0.233, 0.12, 0.97}
    };
    int status = client.search("test_db5", "test_collection2", {}, vectors, {}, params, &result);
    std::cout << result.message << std::endl;
    EXPECT_EQ(status, 0);

    ASSERT_FALSE(result.documents.empty());
    EXPECT_GT(result.documents[0].size(), 1);
}


TEST_F(RpcClientTestBase, SearchDocumentById) {
    SearchDocumentResult result;
    SearchDocumentParams* params = new SearchDocumentParams();
    params->searchParams = std::make_unique<SearchParms>();
    params->searchParams->ef = 100;
    params->retrieveVector = false;
    params->limit = 2;
    params->filter = std::make_unique<Filter>("bookName=\"三国演义\"");
    std::vector<std::string> documentIds = {"0003"};
    int status = client.search("test_db5", "test_collection2", documentIds, {}, {}, params, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);
    ASSERT_GT(result.documents.size(), 0);
}

TEST_F(RpcClientTestBase, DeleteDocument) {
    DeleteDocumentResult result;
    DeleteDocumentParams* deleteParams = new DeleteDocumentParams();
    deleteParams->documentIds = {"0001", "0003"};
    deleteParams->filter = std::make_unique<Filter>("bookName=\"西游记\"");
    int status = client.dele("test_db5", "test_collection2", deleteParams, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_EQ(result.affectedCount, 1);
}

TEST_F(RpcClientTestBase, UpdateDocument) {
    UpdateDocumentResult result;
    UpdateDocumentParams* updateParams = new UpdateDocumentParams();
    updateParams->queryIds = {"0001", "0003"};
    updateParams->queryFilter = std::make_unique<Filter>("bookName=\"三国演义\"");
    updateParams->updateFields.insert({"page", Field(static_cast<uint64_t>(24))});
    int status = client.update("test_db5", "test_collection2", updateParams, &result);
    std::cout << result.message << std::endl;

    EXPECT_EQ(status, 0);

    EXPECT_EQ(result.affectedCount, 1);
}

}  // namespace vectordb
