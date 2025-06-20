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
#include "include/types/index.h"

namespace vectordb {

int RpcClient::rebuildIndex(const std::string& dbName, const std::string& collectionName,
    const RebuildIndexParams* params, RebuildIndexResult* result, int timeout) {
    olama::RebuildIndexRequest request;
    request.set_database(dbName);
    request.set_collection(collectionName);
    if (params != nullptr) {
        request.set_dropbeforerebuild(params->dropBeforeRebuild);
        request.set_throttle(static_cast<int32_t>(params->throttle));
    }
    grpc::ClientContext context;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        std::chrono::milliseconds(timeout);
    context.set_deadline(deadline);
    olama::RebuildIndexResponse response;
    grpc::Status status = stub_->rebuildIndex(&context, request, &response);
    if (!status.ok()) {
        result->success = false;
        result->message = "Fail to rebuild index: " + status.error_message();
        return -1;
    }
    if (response.code() != 0) {
        result->success = false;
        result->message = "Fail to rebuild index: " + response.msg();
        return -1;
    }
    result->success = true;
    result->message = response.msg();
    result->taskIds.assign(response.task_ids().begin(), response.task_ids().end());
    return 0;
}

}  // namespace vectordb
