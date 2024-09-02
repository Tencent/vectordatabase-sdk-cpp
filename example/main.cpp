

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

#include <string>

#include "include/rpc_client.h"


namespace vectordb {

int main() {
    std::string database = "go-sdk-demo-db";
    std::string collectionName = "go-sdk-demo-col";
    std::string collectionAlias = "go-sdk-demo-alias";
    int status;

    // Create RpcClient
    // set ClientOption (optional)
    ClientOption* clientOption = new ClientOption();
    clientOption->timeout = 5000;
    clientOption->readConsistency = vectordb::EventualConsistency;
    // with ClientOption
    RpcClient cli = RpcClient("url", "username", "key", clientOption);
    // without ClientOption
    // RpcClient cli = RpcClient("url", "username", "key");

    // Create Database
    CreateDatabaseResult createDatabaseResult;
    std::string dbName = "test-db";
    status = cli.createDatabase(dbName, &createDatabaseResult);

    // List Databases
    ListDatabaseResult ListDatabaseResult;
    status = cli.listDatabases(&ListDatabaseResult);

    // Drop Database
    DropDatabaseResult dropDatabaseResult;
    status = cli.dropDatabase(dbName, &dropDatabaseResult);

    // Create Collection without Embedding

    // 第一步：设计索引（不是设计 Collection 的结构）
    // 1. 【重要的事】向量对应的文本字段不要建立索引，会浪费较大的内存，并且没有任何作用。
    // 2. 【必须的索引】：主键id、向量字段 vector 这两个字段目前是固定且必须的，参考下面的例子；
    // 3. 【其他索引】：检索时需作为条件查询的字段，比如要按书籍的作者进行过滤，这个时候 author 字段就需要建立索引，
    //     否则无法在查询的时候对 author 字段进行过滤，不需要过滤的字段无需加索引，会浪费内存；
    // 4.  向量数据库支持动态 Schema，写入数据时可以写入任何字段，无需提前定义，类似 MongoDB.
    // 5.  例子中创建一个书籍片段的索引，例如书籍片段的信息包括 {id, vector, segment, bookName, author, page},
    //     id 为主键需要全局唯一，segment 为文本片段, vector 字段需要建立向量索引，假如我们在查询的时候要查询指定书籍
    //     名称的内容，这个时候需要对 bookName 建立索引，其他字段没有条件查询的需要，无需建立索引。

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

    // 第二步：创建 Collection
    // 创建collection耗时较长，需要调整客户端的timeout
    // 这里以三可用区实例作为参考，具体实例不同的规格所支持的shard和replicas区间不同，需要参考官方文档
    cli.setTimeout(30);  // 单位：秒（s）
    status = cli.createCollection(dbName, collectionName, 3, 0, "test collection", indexes);

    // Create Collection without Embedding

    // 新建 Collection
    // 第一步，设计索引（不是设计表格的结构）
    // 1. 【重要的事】向量对应的文本字段不要建立索引，会浪费较大的内存，并且没有任何作用。
    // 2. 【必须的索引】：主键 id、向量字段 vector 这两个字段目前是固定且必须的，参考下面的例子；
    // 3. 【其他索引】：检索时需作为条件查询的字段，比如要按书籍的作者进行过滤，这个时候author字段就需要建立索引，
    //     否则无法在查询的时候对 author 字段进行过滤，不需要过滤的字段无需加索引，会浪费内存；
    // 4.  向量数据库支持动态 Schema，写入数据时可以写入任何字段，无需提前定义，类似 MongoDB.
    // 5.  例子中创建一个书籍片段的索引，例如书籍片段的信息包括 {id, vector, segment, bookName, page},
    //     id 为主键需要全局唯一，segment 为文本片段, vector 为 segment 的向量，vector 字段需要建立向量索引，假如我们在查询的时候要查询指定书籍
    //     名称的内容，这个时候需要对bookName建立索引，其他字段没有条件查询的需要，无需建立索引。
    // 6.  创建带 Embedding 的 collection 需要保证设置的 vector 索引的维度和 Embedding 所用模型生成向量维度一致，模型及维度关系：
    //     -----------------------------------------------------
    //             bge-base-zh                 ｜ 768
    //             bge-large-zh                ｜ 1024
    //             m3e-base                    ｜ 768
    //             text2vec-large-chinese      ｜ 1024
    //             e5-large-v2                 ｜ 1024
    //             multilingual-e5-base        ｜ 768
    //     -----------------------------------------------------
    CreateCollectionParams* createCollectionParams = new CreateCollectionParams();
    auto embedding = std::make_unique<Embedding>();
    embedding->field = "text";
    embedding->enabled = "vector";
    embedding->model = "bge-base-zh";
    createCollectionParams->embedding = std::move(embedding);
    cli.setTimeout(30);  // 单位：秒（s）
    status = cli.createCollection(dbName, collectionName, 3, 0, "test collection", indexes);

    // list Collection
    ListCollectionResult listCollectionResult;
    status = cli.listCollections(dbName, &listCollectionResult);

    // describe Collection
    DescribeCollectionResult describeCollectionResult;
    status = cli.describeCollection(dbName, collectionName, &describeCollectionResult);

    // drop Collection
    DropCollectionResult dropCollectionResult;
    status = cli.dropCollection(dbName, collectionName, &dropCollectionResult);

    // truncate Collection
    // 清空 Collection
    TruncateCollectionResult truncateCollectionResult;
    status = cli.truncateCollection(dbName, collectionName, &truncateCollectionResult);

    // upsert
    // 1. 支持动态 Schema，除了 id、vector 字段必须写入，可以写入其他任意字段；
    // 2. upsert 会执行覆盖写，若文档id已存在，则新数据会直接覆盖原有数据(删除原有数据，再插入新数据)
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
    UpsertDocumentParams* upsertDocumentParams = new UpsertDocumentParams();
    UpsertDocumentResult upsertDocumentResult;
    upsertDocumentParams->buildIndex = true;
    status = cli.upsert(dbName, collectionName, documents, upsertDocumentParams, &upsertDocumentResult);
    std::cout << upsertDocumentResult.message << std::endl;


    // query
    // 1. query 用于查询数据
    // 2. 可以通过传入主键 id 列表或 filter 实现过滤数据的目的
    // 3. 如果没有主键 id 列表和 filter 则必须传入 limit 和 offset，类似 scan 的数据扫描功能
    // 4. 如果仅需要部分 field 的数据，可以指定 output_fields 用于指定返回数据包含哪些 field，不指定默认全部返回
    std::vector<std::string> documentIds = {"0001", "0002", "0003", "0004", "0005"};
    std::unique_ptr<Filter> filter = std::make_unique<Filter>("bookName=\"三国演义\"");
    std::vector<std::string> outputField = {"id", "bookName"};

    QueryDocumentResult queryDocumentResult;
    std::vector<std::string> documentIds;
    QueryDocumentParams* queryDocumentParams = new QueryDocumentParams();
    queryDocumentParams->filter = std::move(filter);
    queryDocumentParams->retrieveVector = true;
    queryDocumentParams->outputFields = outputField;
    queryDocumentParams->offset = 1;
    queryDocumentParams->limit = 2;
    int status = cli.query(dbName, collectionName, documentIds, queryDocumentParams, &queryDocumentResult);
    std::cout << queryDocumentResult.message << std::endl;

    // search by vector
    // 批量相似性查询，根据指定的多个向量查找多个 Top K 个相似性结果

    // 输出相似性检索结果，检索结果为二维数组，每一位为一组返回结果，分别对应search时指定的多个向量
    SearchDocumentResult searchDocumentResultByVec;
    SearchDocumentParams* searchByVecParams = new SearchDocumentParams();
    searchByVecParams->searchParams = std::make_unique<SearchParms>();
    searchByVecParams->searchParams->ef = 100;  // 若使用HNSW索引，则需要指定参数ef，ef越大，召回率越高，但也会影响检索速度
    searchByVecParams->retrieveVector = false;  // 是否需要返回向量字段，False：不返回，True：返回
    searchByVecParams->limit = 10;  // 指定 Top K 的 K 值
    std::vector<std::vector<float>> vectors = {  // 指定检索向量，最多指定20个
        {0.3123, 0.43, 0.213},
        {0.233, 0.12, 0.97}
    };
    int status = cli.search(dbName, collectionName, {}, vectors, {}, searchByVecParams, &searchDocumentResultByVec);
    std::cout << searchDocumentResultByVec.message << std::endl;

    // search by id
    // 1. 支持通过 filter 过滤数据
    // 2. 如果仅需要部分 field 的数据，可以指定 output_fields 用于指定返回数据包含哪些 field，不指定默认全部返回
    // 3. limit 用于限制每个单元搜索条件的条数，如 vector 传入三组向量，limit 为 3，则 limit 限制的是每组向量返回 top 3 的相似度向量

    // 根据主键 id 查找 Top K 个相似性结果，向量数据库会根据ID 查找对应的向量，再根据向量进行TOP K 相似性检索
    SearchDocumentResult searchDocumentResultById;
    SearchDocumentParams* searchByIdParams = new SearchDocumentParams();
    searchByIdParams->searchParams = std::make_unique<SearchParms>();
    searchByIdParams->searchParams->ef = 100;
    searchByIdParams->retrieveVector = false;
    searchByIdParams->limit = 2;
    searchByIdParams->filter = std::make_unique<Filter>("bookName=\"三国演义\"");
    std::vector<std::string> documentIds = {"0003"};
    int status = cli.search(dbName, collectionName, documentIds, {}, {}, searchByIdParams, &searchDocumentResultById);
    std::cout << searchDocumentResultById.message << std::endl;

    // search by text
    // 通过 embedding 文本搜索
    // 1. searchByText 提供基于 embedding 文本的搜索能力，会先将 embedding 内容做 Embedding 然后进行按向量搜索
    // 其他选项类似 search 接口

    // searchByText 返回类型为 Dict，接口查询过程中 embedding 可能会出现截断
    // 如发生截断将会返回响应 warn 信息，如需确认是否截断可以
    // 使用 "warning" 作为 key 从 Dict 结果中获取警告信息，查询结果可以通过 "documents" 作为 key 从 Dict 结果中获取
    SearchDocumentResult searchDocumentResultByText;
    SearchDocumentParams* searchByTextParams = new SearchDocumentParams();
    searchByTextParams->searchParams = std::make_unique<SearchParms>();
    searchByTextParams->searchParams->ef = 100;
    searchByTextParams->limit = 2;
    std::map<std::string, std::vector<std::string>> text;
    text["text"] =  {"细作探知这个消息，飞报吕布。"};
    int status = cli.search(dbName, collectionName, {}, {}, text, searchByTextParams, &searchDocumentResultByText);
    std::cout << searchDocumentResultByText.message << std::endl;

    // update
    // 1. update 提供基于 [主键查询] 和 [Filter 过滤] 的部分字段更新或者非索引字段新增
    // filter 限制仅会更新 id = "0003"
    UpdateDocumentResult updateDocumentResult;
    UpdateDocumentParams* updateParams = new UpdateDocumentParams();
    updateParams->queryIds = {"0001", "0003"};
    updateParams->queryFilter = std::make_unique<Filter>("bookName=\"三国演义\"");
    updateParams->updateFields.insert({"page", Field(static_cast<uint64_t>(24))});
    int status = cli.update(dbName, collectionName, updateParams, &updateDocumentResult);
    std::cout << updateDocumentResult.message << std::endl;

    // delete
    // 1. delete 提供基于 [主键查询] 和 [Filter 过滤] 的数据删除能力
    // 2. 删除功能会受限于 collection 的索引类型，部分索引类型不支持删除操作

    // filter 限制只会删除 id="0001" 成功
    DeleteDocumentResult deleteDocumentResult;
    DeleteDocumentParams* deleteParams = new DeleteDocumentParams();
    deleteParams->documentIds = {"0001", "0003"};
    deleteParams->filter = std::make_unique<Filter>("bookName=\"西游记\"");
    int status = cli.dele(dbName, collectionName, deleteParams, &deleteDocumentResult);
    std::cout << deleteDocumentResult.message << std::endl;

    // rebuild_index
    // 索引重建，重建期间不支持写入
    RebuildIndexResult rebuildIndexResult;
    RebuildIndexParams* rebuildIndexParams = new RebuildIndexParams();
    rebuildIndexParams->dropBeforeRebuild = true;
    rebuildIndexParams->throttle = 1;
    status = cli.rebuildIndex(dbName, collectionName, rebuildIndexParams, &rebuildIndexResult);

}

}  // namespace vectordb
