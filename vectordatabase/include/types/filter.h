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
#include <sstream>
#include <shared_mutex>
#include <type_traits>

namespace vectordb {

struct Filter {
    std::string cond;
    std::shared_mutex mutex;
    Filter() {}
    explicit Filter(const std::string& val) : cond(val) {}

    Filter& assemblyCond(const std::string& condition, std::string operation);
    Filter& andCond(const std::string& condition);
    Filter& orCond(const std::string& condition);
    Filter& andNotCond(const std::string& condition);
    Filter& orNotCond(const std::string& condition);

    template <typename T>
    static std::string assemblyFilterExpr(const std::string& key, const std::vector<T>& list,
        std::string operation);
    template <typename T>
    static std::string in(const std::string& key, const std::vector<T>& list);

    template <typename T>
    static std::string notIn(const std::string& key, const std::vector<T>& list);

    template <typename T>
    static std::string include(const std::string& key, const std::vector<T>& list);

    template <typename T>
    static std::string exclude(const std::string& key, const std::vector<T>& list);

    template <typename T>
    static std::string includeAll(const std::string& key, const std::vector<T>& list);
};


template <typename T>
std::string Filter::assemblyFilterExpr(const std::string& key, const std::vector<T>& list, std::string operation) {
    if (list.empty()) {
        return "";
    }

    std::string result = key + " " + operation + " (";
    result.reserve(key.size() + list.size() * 16);

    for (const auto& item : list) {
        if constexpr (std::is_same<T, std::string>::value) {
            result += "\"" + item + "\",";
        } else {
            result += std::to_string(item) + ",";
        }
    }
    if (result.size() != 0) {
        result.back() = ')';
    }
    return result;
}

template <typename T>
std::string Filter::in(const std::string& key, const std::vector<T>& list) {
    return assemblyFilterExpr(key, list, "in");
}

template <typename T>
std::string Filter::notIn(const std::string& key, const std::vector<T>& list) {
    return assemblyFilterExpr(key, list, "not in");
}

template <typename T>
std::string Filter::include(const std::string& key, const std::vector<T>& list) {
    return assemblyFilterExpr(key, list, "include");
}

template <typename T>
std::string Filter::exclude(const std::string& key, const std::vector<T>& list) {
    return assemblyFilterExpr(key, list, "exclude");
}

template <typename T>
std::string Filter::includeAll(const std::string& key, const std::vector<T>& list) {
    return assemblyFilterExpr(key, list, "include all");
}

}  // namespace vectordb
