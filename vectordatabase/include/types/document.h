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
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "include/types/filter.h"

namespace vectordb {

struct Field {
    using FieldValue = std::variant<
        std::string,
        uint64_t,
        double,
        std::vector<std::string>
    >;

    FieldValue oneofVal;

    Field() = default;
    explicit Field(const std::string& str) : oneofVal(str) {}
    explicit Field(uint64_t u64) : oneofVal(u64) {}
    explicit Field(double d) : oneofVal(d) {}
    explicit Field(const std::vector<std::string>& strArr) :
        oneofVal(strArr) {}

    bool hasValStr() const {
        return std::holds_alternative<std::string>(oneofVal);
    }

    bool hasValU64() const {
        return std::holds_alternative<uint64_t>(oneofVal);
    }

    bool hasValDouble() const {
        return std::holds_alternative<double>(oneofVal);
    }

    bool hasValStrArr() const {
        return std::holds_alternative<std::vector<std::string>>(oneofVal);
    }

    std::string getValStr() const {
        if (hasValStr()) {
            return std::get<std::string>(oneofVal);
        } else {
            return "";
        }
    }

    uint64_t getValU64() const {
        if (hasValU64()) {
            return std::get<uint64_t>(oneofVal);
        } else {
            return 0;
        }
    }

    double getValDouble() const {
        if (hasValDouble()) {
            return std::get<double>(oneofVal);
        } else {
            return 0;
        }
    }

    std::vector<std::string> getValStrArr() const {
        if (hasValStrArr()) {
            return std::get<std::vector<std::string>>(oneofVal);
        } else {
            return {};
        }
    }

    void setValStr(const std::string& val) {
        oneofVal = val;
    }

    void setValDouble(double val) {
        oneofVal = val;
    }

    void setValU64(uint64_t val) {
        oneofVal = val;
    }

    void setValStrArr(const std::vector<std::string> val) {
        oneofVal = val;
    }
};

struct Document {
    std::string id;
    std::vector<float> vector;
    std::unordered_map<std::string, Field> fields;
    float score = 0.0f;
};

struct UpsertDocumentParams {
    bool buildIndex;
};

struct UpsertDocumentResult {
    bool success;
    std::string message;
    int affectedCount = 0;
};

struct QueryDocumentParams {
    std::unique_ptr<Filter> filter;
    bool retrieveVector;
    std::vector<std::string> outputFields;
    int64_t offset;
    int64_t limit;
};

struct QueryDocumentResult {
    bool success;
    std::string message;
    std::vector<Document> documents;
    uint64_t total;
};

struct SearchParms {
    int nprobe = 0;
    int ef = 0;
    float radius = 0.0f;
};

struct SearchDocumentParams {
    std::unique_ptr<Filter> filter;
    std::unique_ptr<SearchParms> searchParams;
    bool retrieveVector;
    std::vector<std::string> outputFields;
    int64_t limit;
};

struct SearchDocumentResult {
    bool success;
    std::string message;
    std::string warning;
    std::vector<std::vector<Document>> documents;
};

struct UpdateDocumentParams {
    std::vector<std::string> queryIds;
    std::unique_ptr<Filter> queryFilter;
    std::vector<float> updateVector;
    std::unordered_map<std::string, Field> updateFields;
};

struct UpdateDocumentResult {
    bool success;
    std::string message;
    int affectedCount = 0;
};

struct DeleteDocumentParams {
    std::vector<std::string> documentIds;
    std::unique_ptr<Filter> filter;
    int64_t limit;
};

struct DeleteDocumentResult {
    bool success;
    std::string message;
    int affectedCount = 0;
};

struct CountResult {
    bool success;
    std::string message;
    uint64_t count;
};

}  // namespace vectordb
