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

#include "include/rpc_client.h"
#include "include/helper.h"
#include "include/types/collection.h"

namespace vectordb {

int RpcClient::createCollection(const std::string& dbName, const std::string& collectionName,
    uint32_t shardNum, uint32_t replicaNum, const std::string& description, const Indexes& indexes,
    const CreateCollectionParams* params, CreateCollectionResult* result, int timeout) {
    olama::CreateCollectionRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    request.set_shardnum(shardNum);
    request.set_replicanum(replicaNum);
    request.set_description(description);

    for (const auto& v : indexes.vectorIndex) {
        olama::IndexColumn column;
        column.set_fieldname(v.fieldName);
        column.set_fieldtype(v.fieldType);
        column.set_indextype(v.indexType);
        column.set_dimension(v.dimension);
        column.set_metrictype(v.metricType);

        column.set_allocated_params(new olama::IndexParams());
        column.mutable_params()->set_m(v.params.m);
        column.mutable_params()->set_efconstruction(v.params.efConstruction);
        column.mutable_params()->set_nlist(v.params.nList);
        column.mutable_params()->set_nprobe(v.params.nProbe);
        (*request.mutable_indexes())[v.fieldName] = column;
    }

    for (const auto& v : indexes.filterIndex) {
        olama::IndexColumn column;
        column.set_fieldname(v.fieldName);
        column.set_fieldtype(v.fieldType);
        column.set_indextype(v.indexType);
        if (v.fieldType == kArray) {
            column.set_fieldelementtype(kString);
        }
        (*request.mutable_indexes())[v.fieldName] = column;
    }

    if (params != nullptr) {
        if (params->embedding != nullptr) {
            olama::EmbeddingParams* embeddingParams = new olama::EmbeddingParams();
            embeddingParams->set_field(params->embedding->field);
            embeddingParams->set_vector_field(params->embedding->vectorField);
            embeddingParams->set_model_name(params->embedding->model);
            request.set_allocated_embeddingparams(embeddingParams);
        }
    }

    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::CreateCollectionResponse response;
    grpc::Status status = stub_->createCollection(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to create collection: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to create collection: " + response.msg();
        return -1;
    }
    auto collection = std::make_unique<Collection>();
    collection->database = dbName;
    collection->collectionName = collectionName;
    collection->shardNum = shardNum;
    collection->replicaNum = replicaNum;
    collection->description = description;
    collection->indexes = indexes;
    result->success = true;
    result->message = response.msg();
    result->collection = std::move(collection);
    return 0;
}

int RpcClient::listCollections(const std::string& dbName, ListCollectionResult* result, int timeout) {
    olama::ListCollectionsRequest request;
    request.set_database(dbName);

    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::ListCollectionsResponse response;
    grpc::Status status = stub_->listCollections(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to list collections: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to list collection: " + response.msg();
        return -1;
    }
    std::vector<std::unique_ptr<Collection>> collections;
    for (const auto& collectionProto : response.collections()) {
        std::unique_ptr<Collection> collection = std::make_unique<Collection>();
        toCollection(collectionProto, collection.get());
        collections.push_back(std::move(collection));
    }
    result->success = true;
    result->message = response.msg();
    result->collections = std::move(collections);
    return 0;
}

int RpcClient::describeCollection(const std::string& dbName, const std::string& collectionName,
        DescribeCollectionResult* result, int timeout) {
    olama::DescribeCollectionRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::DescribeCollectionResponse response;
    grpc::Status status = stub_->describeCollection(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to describe collection: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to describe collection: " + response.msg();
        return -1;
    }
    if (!response.has_collection()) {
        result->success = false;
        result->message = "Fail to get collection.";
        return -1;
    }
    std::unique_ptr<Collection> collection = std::make_unique<Collection>();
    toCollection(response.collection(), collection.get());
    result->success = true;
    result->message = response.msg();
    result->collection = std::move(collection);
    return 0;
}

int RpcClient::truncateCollection(const std::string& dbName, const std::string& collectionName,
        TruncateCollectionResult* result, int timeout) {
    olama::TruncateCollectionRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::TruncateCollectionResponse response;
    grpc::Status status = stub_->truncateCollection(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to truncate collection: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to truncate collection: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->affectedCount = response.affectedcount();
    return 0;
}

int RpcClient::dropCollection(const std::string& dbName, const std::string& collectionName,
        DropCollectionResult* result, int timeout) {
    olama::DropCollectionRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);

    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::DropCollectionResponse response;
    grpc::Status status = stub_->dropCollection(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to drop collection: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to drop collection: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->affectedCount = response.affectedcount();
    return 0;
}

}  // namespace vectordb
