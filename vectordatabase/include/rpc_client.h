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
    RpcClient(const std::string& url, const std::string& username, const std::string& key,
        const ClientOption* option = nullptr);
    void setTimeout(int timeout);
    void closeConnection();

    // database
    int createDatabase(const std::string& dbName, CreateDatabaseResult* result);
    int listDatabases(ListDatabaseResult* result);
    int dropDatabase(const std::string& dbName, DropDatabaseResult* result);

    // collection
    int createCollection(const std::string& dbName, const std::string& collectionName,
        uint32_t shardNum, uint32_t replicaNum, const std::string& description, const Indexes& indexes,
        const CreateCollectionParams* params = nullptr, CreateCollectionResult* result = nullptr);
    int listCollections(const std::string& dbName, ListCollectionResult* result);
    int describeCollection(const std::string& dbName, const std::string& collectionName,
        DescribeCollectionResult* result = nullptr);
    int truncateCollection(const std::string& dbName, const std::string& collectionName,
        TruncateCollectionResult* result = nullptr);
    int dropCollection(const std::string& dbName, const std::string& collectionName,
        DropCollectionResult* result = nullptr);

    // document
    int upsert(const std::string& dbName, const std::string& collectionName, const std::vector<Document>& documents,
        const UpsertDocumentParams* params = nullptr, UpsertDocumentResult* result = nullptr);
    int query(const std::string& dbName, const std::string& collectionName, const std::vector<std::string>& documentIds,
        const QueryDocumentParams* params = nullptr, QueryDocumentResult* result = nullptr);
    int search(const std::string& dbName, const std::string& collectionName,
        const std::vector<std::string>& documentIds, const std::vector<std::vector<float>>& vectors,
        const std::map<std::string, std::vector<std::string>>& text,
        const SearchDocumentParams* params = nullptr, SearchDocumentResult* result = nullptr);
    int dele(const std::string& dbName, const std::string& collectionName,
        const DeleteDocumentParams* param, DeleteDocumentResult* result = nullptr);
    int update(const std::string& dbName, const std::string& collectionName,
        const UpdateDocumentParams* param, UpdateDocumentResult* result = nullptr);

    // index
    int rebuildIndex(const std::string& dbName, const std::string& collectionName,
        const RebuildIndexParams* params, RebuildIndexResult* result = nullptr);

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
