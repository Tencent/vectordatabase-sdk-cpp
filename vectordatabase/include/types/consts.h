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

#include "include/rpc_client.h"

namespace vectordb {

const std::string EventualConsistency = "eventualConsistency";
const std::string StrongConsistency = "strongConsistency";

const std::string kUnit64 = "uint64";
const std::string kString = "string";
const std::string kArray = "array";
const std::string kVector = "vector";

const std::string kFLAT = "FLAT";
const std::string kHNSW = "HNSW";
const std::string kIVF_FLAT = "IVF_FLAT";
const std::string kIVF_PQ = "IVF_PQ";
const std::string kIVF_SQ4 = "IVF_PQ4";
const std::string kIVF_SQ8 = "IVF_PQ8";
const std::string kIVF_SQ16 = "IVF_PQ16";

const std::string L2 = "L2";
const std::string IP = "IP";
const std::string COSINE = "COSINE";

const std::string kPRIMARY = "primaryKey";
const std::string kFILTER = "filter";

}  // namespace vectordb
