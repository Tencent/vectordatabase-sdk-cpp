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

#include <memory>
#include <string>
#include <vector>

namespace vectordb {

struct IndexParams {
    uint32_t  m = 0;
    uint32_t efConstruction = 0;
    uint32_t nList = 0;
    uint32_t nProbe = 0;
};

struct IndexColumn {
    std::string fieldName;
    std::string fieldType;
    std::string indexType;
    uint32_t dimention = 0;
    std::string metricType;
    uint64_t indexCount;
    IndexParams params;
    std::string fieldElementType;
};

struct FilterIndex {
    std::string fieldName;
    std::string fieldType;
    std::string indexType;
    std::string elemType;
};

struct VectorIndex {
    std::string fieldName;
    std::string fieldType;
    std::string indexType;
    uint32_t dimension;
    std::string metricType;
    IndexParams params;
    uint64_t indexCount;
    std::string elemType;
};

struct Indexes {
    std::vector<VectorIndex> vectorIndex;
    std::vector<FilterIndex> filterIndex;
};

struct IndexStatus {
    std::string status;
    std::string progress;
    std::string startTime;
};

struct Embedding {
    std::string field;
    std::string vectorField;
    std::string model;
    bool enabled = false;
};

struct Collection {
    std::string database;
    std::string collectionName;
    int64_t documentCount = 0;
    std::vector<std::string> alias;
    uint32_t shardNum = 0;
    uint32_t replicaNum = 0;
    Indexes indexes;
    IndexStatus indexStatus;
    Embedding embedding;
    std::string description;
    uint64_t size = 0;
    std::string createTime;
};

struct CreateCollectionParams {
    std::unique_ptr<Embedding> embedding;
};

struct CreateCollectionResult {
    bool success;
    std::string message;
    std::unique_ptr<Collection> collection;
};

struct ListCollectionResult {
    bool success;
    std::string message;
    std::vector<std::unique_ptr<Collection>> collections;
};

struct DescribeCollectionResult {
    bool success;
    std::string message;
    std::unique_ptr<Collection> collection;
};

struct TruncateCollectionResult {
    bool success;
    std::string message;
    int affectedCount;
};

struct DropCollectionResult {
    bool success;
    std::string message;
    int affectedCount;
};

}  // namespace vectordb
