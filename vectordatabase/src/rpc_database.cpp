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
#include "include/types/database.h"

namespace vectordb {

int RpcClient::createDatabase(const std::string& dbName, CreateDatabaseResult* result, int timeout) {
    olama::DatabaseRequest request;
    request.set_database(dbName);
    request.set_dbtype(olama::DataType::BASE);
    olama::DatabaseResponse response;
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    grpc::Status status = stub_->createDatabase(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to create database: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to create database: " + response.msg();
        return -1;
    }
    Database database;
    database.name = dbName;
    result->success = true;
    result->database = database;
    result->affectedCount = response.affectedcount();
    return 0;
}

int RpcClient::listDatabases(ListDatabaseResult* result, int timeout) {
    olama::DatabaseRequest request;
    olama::DatabaseResponse response;
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    grpc::Status status = stub_->listDatabases(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to list databases: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to list database: " + response.msg();
        return -1;
    }
    std::vector<std::unique_ptr<Database>> databases;
    for (const auto& dbName : response.databases()) {
        std::unique_ptr<Database> db = std::make_unique<Database>();
        db->name = dbName;
        auto info_iter = response.info().find(dbName);
        if (info_iter != response.info().end()) {
            db->createTime = info_iter->second.create_time();
        } else {
            db->createTime = -1;
        }
        databases.push_back(std::move(db));
    }
    result->success = true;
    result->databases = std::move(databases);
    return 0;
}

int RpcClient::dropDatabase(const std::string& dbName, DropDatabaseResult* result, int timeout) {
    olama::DatabaseRequest request;
    request.set_database(dbName);
    request.set_dbtype(olama::DataType::BASE);
    olama::DatabaseResponse response;
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    grpc::Status status = stub_->dropDatabase(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to drop database: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to drop database: " + response.msg();
        return -1;
    }
    result->success = true;
    result->affectedCount = response.affectedcount();
    return 0;
}

}  // namespace vectordb
