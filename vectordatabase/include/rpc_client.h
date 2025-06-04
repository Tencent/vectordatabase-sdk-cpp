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

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <stdexcept>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/security/credentials.h>

#include "proto/olama.pb.h"
#include "proto/olama.grpc.pb.h"
#include "include/types/consts.h"
#include "include/types/collection.h"
#include "include/types/database.h"
#include "include/types/document.h"
#include "include/types/index.h"

namespace vectordb {

struct ClientOption {
    // Timeout: default 5s
    int timeout{5000};
    // ReadConsistency: default: EventualConsistency
    std::string readConsistency{EventualConsistency};
};

class RpcClient {
  public:
    // 构造函数
    // @param url: 服务器地址
    // @param username: 用户名
    // @param key: 认证密钥
    // @param option: 客户端配置选项,可选参数
    RpcClient(const std::string& url, const std::string& username, const std::string& key,
        const ClientOption* option = nullptr);

    // 设置超时时间
    // @param timeout: 超时时间(毫秒)
    void setTimeout(int timeout);

    // 关闭连接
    void closeConnection();

    // 创建数据库
    // @param dbName: 数据库名称
    // @param result: 创建结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int createDatabase(const std::string& dbName, CreateDatabaseResult* result, int timeout = 1000);

    // 列出所有数据库
    // @param result: 返回数据库列表
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int listDatabases(ListDatabaseResult* result, int timeout = 1000);

    // 删除数据库
    // @param dbName: 要删除的数据库名称
    // @param result: 删除结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int dropDatabase(const std::string& dbName, DropDatabaseResult* result, int timeout = 1000);

    // 创建集合
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param shardNum: 分片数量
    // @param replicaNum: 副本数量
    // @param description: 集合描述
    // @param indexes: 索引配置
    // @param params: 创建参数
    // @param result: 创建结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int createCollection(const std::string& dbName, const std::string& collectionName,
        uint32_t shardNum, uint32_t replicaNum, const std::string& description, const Indexes& indexes,
        const CreateCollectionParams* params = nullptr, CreateCollectionResult* result = nullptr, int timeout = 1000);

    // 列出数据库中的所有集合
    // @param dbName: 数据库名称
    // @param result: 返回集合列表
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int listCollections(const std::string& dbName, ListCollectionResult* result, int timeout = 1000);

    // 查看集合详情
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param result: 返回集合详细信息
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int describeCollection(const std::string& dbName, const std::string& collectionName,
        DescribeCollectionResult* result = nullptr, int timeout = 1000);

    // 清空集合
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param result: 清空结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int truncateCollection(const std::string& dbName, const std::string& collectionName,
        TruncateCollectionResult* result = nullptr, int timeout = 1000);

    // 删除集合
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param result: 删除结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int dropCollection(const std::string& dbName, const std::string& collectionName,
        DropCollectionResult* result = nullptr, int timeout = 1000);

    // 插入或更新文档
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param documents: 要插入或更新的文档列表
    // @param params: 插入参数
    // @param result: 插入结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int upsert(const std::string& dbName, const std::string& collectionName, const std::vector<Document>& documents,
        const UpsertDocumentParams* params = nullptr, UpsertDocumentResult* result = nullptr, int timeout = 1000);

    // 查询文档
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param documentIds: 要查询的文档ID列表
    // @param params: 查询参数
    // @param result: 查询结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int query(const std::string& dbName, const std::string& collectionName, const std::vector<std::string>& documentIds,
        const QueryDocumentParams* params = nullptr, QueryDocumentResult* result = nullptr, int timeout = 1000);

    // 向量搜索
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param documentIds: 文档ID列表
    // @param vectors: 向量列表
    // @param text: 文本搜索条件
    // @param params: 搜索参数
    // @param result: 搜索结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int search(const std::string& dbName, const std::string& collectionName,
        const std::vector<std::string>& documentIds, const std::vector<std::vector<float>>& vectors,
        const std::map<std::string, std::vector<std::string>>& text,
        const SearchDocumentParams* params = nullptr, SearchDocumentResult* result = nullptr, int timeout = 1000);
    
    // 删除文档
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param params: 删除参数,包含:
    //   - documentIds: 要删除的文档ID列表
    //   - filter: 过滤条件
    //   - limit: 限制删除数量,可选参数，为空时不限制删除数量
    // @param result: 返回结果
    // @param timeout: 超时时间,默认1000ms
    // @return: 0表示成功,非0表示失败
    int dele(const std::string& dbName, const std::string& collectionName,
        const DeleteDocumentParams* param, DeleteDocumentResult* result = nullptr, int timeout = 1000);
    int update(const std::string& dbName, const std::string& collectionName,
        const UpdateDocumentParams* param, UpdateDocumentResult* result = nullptr, int timeout = 1000);
    
    // 获取文档数量
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param filter: 过滤条件,可选参数,为空时返回集合总文档数
    // @param result: 返回结果
    // @param timeout: 超时时间,默认1000ms
    // @return: 0表示成功,非0表示失败
    int count(const std::string& dbName, const std::string& collectionName,
        const Filter* filter = nullptr, CountResult* result = nullptr, int timeout = 1000);

    // 重建索引
    // @param dbName: 数据库名称
    // @param collectionName: 集合名称
    // @param params: 重建索引参数,包含索引配置信息
    // @param result: 重建索引结果
    // @param timeout: 超时时间(毫秒),默认1000ms
    // @return: 0表示成功,非0表示失败
    int rebuildIndex(const std::string& dbName, const std::string& collectionName,
        const RebuildIndexParams* params, RebuildIndexResult* result = nullptr, int timeout = 1000);

  private:
    std::shared_ptr<grpc::Channel> grpcChannel_;
    std::unique_ptr<olama::SearchEngine::Stub> stub_;
    ClientOption option_;
    std::string url_;
    std::string username_;
    std::string key_;
    bool debug_;
};

}  // namespace vectordb
