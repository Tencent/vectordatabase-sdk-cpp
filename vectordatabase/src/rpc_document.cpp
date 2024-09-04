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
#include "include/types/document.h"

namespace vectordb {

int RpcClient::upsert(const std::string& dbName, const std::string& collectionName,
    const std::vector<Document>& documents, const UpsertDocumentParams* params,
    UpsertDocumentResult* result) {
    olama::UpsertRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    for (const auto& doc : documents) {
        olama::Document* d = request.add_documents();
        d->set_id(doc.id);
        for (const auto& [key, value] : doc.fields) {
            olama::Field protoField;
            convertField2Proto(value, &protoField);
            (*d->mutable_fields())[key] = protoField;
        }
        for (const auto& vec : doc.vector) {
            d->add_vector(vec);
        }
    }
    if (params != nullptr) {
        request.set_buildindex(params->buildIndex);
    } else {
        request.set_buildindex(true);
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(option_.timeout);
    context.set_deadline(deadline);
    olama::UpsertResponse response;
    grpc::Status status = stub_->upsert(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to upsert documents: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to upsert documents: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->affectedCount = response.affectedcount();
    return 0;
}

int RpcClient::query(const std::string& dbName, const std::string& collectionName,
    const std::vector<std::string>& documentIds,
    const QueryDocumentParams* params, QueryDocumentResult* result) {
    olama::QueryRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    olama::QueryCond* queryCond = new olama::QueryCond();
    for (const auto& docId : documentIds) {
        queryCond->add_documentids(docId);
    }
    request.set_allocated_query(queryCond);
    request.set_readconsistency(option_.readConsistency);
    if (params != nullptr) {
        if (params->filter) {
            request.mutable_query()->set_filter(params->filter->cond);
        }
        request.mutable_query()->set_retrievevector(params->retrieveVector);
        for (const auto& field : params->outputFields) {
            request.mutable_query()->add_outputfields(field);
        }
        request.mutable_query()->set_offset(params->offset);
        request.mutable_query()->set_limit(params->limit);
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(option_.timeout);
    context.set_deadline(deadline);
    olama::QueryResponse response;
    grpc::Status status = stub_->query(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to query documents: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to query documents: " + response.msg();
        return -1;
    }
    std::vector<Document> documents;
    for (const auto& doc : response.documents()) {
        Document d;
        d.id = doc.id();
        d.vector.assign(doc.vector().begin(), doc.vector().end());
        for (const auto& [key, value] : doc.fields()) {
            Field field;
            convertProto2Field(value, &field);
            d.fields[key] = field;
        }
        documents.push_back(d);
    }
    result->documents = documents;
    result->total = response.count();
    return 0;
}

int RpcClient::dele(const std::string& dbName, const std::string& collectionName,
        const DeleteDocumentParams* params, DeleteDocumentResult* result) {
    olama::DeleteRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);

    if (params != nullptr) {
        olama::QueryCond* queryCond = new olama::QueryCond();
        for (const auto& docId : params->documentIds) {
            queryCond->add_documentids(docId);
        }
        if (params->filter) {
            queryCond->set_filter(params->filter->cond);
        }
        request.set_allocated_query(queryCond);
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(option_.timeout);
    context.set_deadline(deadline);
    olama::DeleteResponse response;
    grpc::Status status = stub_->dele(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to dele documents: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to dele documents: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->affectedCount = response.affectedcount();
    return 0;
}

int RpcClient::update(const std::string& dbName, const std::string& collectionName,
        const UpdateDocumentParams* params, UpdateDocumentResult* result) {
    olama::UpdateRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    if (params) {
        olama::QueryCond* queryCond = new olama::QueryCond();
        for (const auto& docId : params->queryIds) {
            queryCond->add_documentids(docId);
        }
        if (params->queryFilter) {
            queryCond->set_filter(params->queryFilter->cond);
        }
        request.set_allocated_query(queryCond);
        auto* updateDoc = new olama::Document();
        for (const auto& value : params->updateVector) {
            updateDoc->add_vector(value);
        }
        for (const auto& [key, value] : params->updateFields) {
            olama::Field protoField;
            convertField2Proto(value, &protoField);
            (*updateDoc->mutable_fields())[key] = protoField;
        }
        request.set_allocated_update(updateDoc);
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(option_.timeout);
    context.set_deadline(deadline);
    olama::UpdateResponse response;
    grpc::Status status = stub_->update(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to update documents: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to update documents: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->affectedCount = static_cast<int>(response.affectedcount());
    return 0;
}

int RpcClient::search(const std::string& dbName, const std::string& collectionName,
    const std::vector<std::string>& documentIds, const std::vector<std::vector<float>>& vectors,
    const std::map<std::string, std::vector<std::string>>& text,
    const SearchDocumentParams* params, SearchDocumentResult* result) {
    olama::SearchRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    request.set_readconsistency(option_.readConsistency);
    olama::SearchCond* searchCond = new olama::SearchCond();
    for (const auto& docId : documentIds) {
        searchCond->add_documentids(docId);
    }
    for (const auto& vector : vectors) {
        auto* vectorArray = searchCond->add_vectors();
        for (const auto& vecValue : vector) {
            vectorArray->add_vector(vecValue);
        }
    }
    for (const auto& [key, value] : text) {
        for (const auto& str : value) {
            searchCond->add_embeddingitems(str);
        }
    }
    request.set_allocated_search(searchCond);
    if (params != nullptr) {
        if (params->filter) {
            request.mutable_search()->set_filter(params->filter->cond);
        }
        request.mutable_search()->set_retrievevector(params->retrieveVector);
        for (const auto& field : params->outputFields) {
            request.mutable_search()->add_outputfields(field);
        }
        request.mutable_search()->set_limit(params->limit);
        if (params->searchParams) {
            olama::SearchParams* protoSearchParams = new olama::SearchParams();
            protoSearchParams->set_nprobe(params->searchParams->nprobe);
            protoSearchParams->set_ef(params->searchParams->ef);
            protoSearchParams->set_radius(params->searchParams->radius);
            request.mutable_search()->set_allocated_params(protoSearchParams);
        }
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(option_.timeout);
    context.set_deadline(deadline);
    olama::SearchResponse response;
    grpc::Status status = stub_->search(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to search documents: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to search documents: " + response.msg();
        return -1;
    }
    result->warning = response.warning();
    for (const auto& resultSet : response.results()) {
        std::vector<Document> vecDocs;
        for (const auto& doc : resultSet.documents()) {
            Document d;
            d.id = doc.id();
            d.vector.assign(doc.vector().begin(), doc.vector().end());
            d.score = doc.score();
            for (const auto& [key, value] : doc.fields()) {
                Field field;
                convertProto2Field(value, &field);
                d.fields[key] = field;
            }
            vecDocs.push_back(std::move(d));
        }
        result->documents.push_back(vecDocs);
    }
    result->success = true;
    result->message = response.msg();
    return 0;
}

}  // namespace vectordb
