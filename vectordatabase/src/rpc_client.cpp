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

#include <iostream>

#include <grpcpp/create_channel.h>
#include <grpcpp/impl/codegen/client_interceptor.h>
#include <grpcpp/impl/codegen/time.h>

#include "include/rpc_client.h"

namespace vectordb {

class AuthInterceptor : public grpc::experimental::Interceptor {
  public:
    AuthInterceptor(const std::string& username, const std::string& api_key)
        : auth_token_("Bearer account=" + username + "&api_key=" + api_key) {}

    void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
            auto* metadata = methods->GetSendInitialMetadata();
            metadata->insert({"authorization", auth_token_});
        }
        methods->Proceed();
    }

  private:
    std::string auth_token_;
};

class AuthInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface {
  public:
    AuthInterceptorFactory(const std::string& username, const std::string& api_key)
        : username_(username), api_key_(api_key) {}

    grpc::experimental::Interceptor* CreateClientInterceptor(
        grpc::experimental::ClientRpcInfo* info) override {
        return new AuthInterceptor(username_, api_key_);
    }

  private:
    std::string username_;
    std::string api_key_;
};

RpcClient::RpcClient(const std::string& url, const std::string& username, const std::string& key,
    const ClientOption* option) {
    if (option == nullptr) {
        option_ = ClientOption{};
    } else {
        option_ = *option;
    }

    std::string rpcTarget;
    if (url.find("http://") == 0) {
        rpcTarget = url.substr(7);
    } else {
        rpcTarget = url;
    }

    url_ = url;
    username_ = username;
    key_ = key;
    debug_ = false;

    std::vector<std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>> interceptors;
    interceptors.push_back(std::make_unique<AuthInterceptorFactory>(username_, key_));
    grpc::ChannelArguments channelArgs;
    channelArgs.SetMaxReceiveMessageSize(16 * 1024 * 1024);
    channelArgs.SetMaxSendMessageSize(16 * 1024 * 1024);
    channelArgs.SetInt(GRPC_ARG_INITIAL_RECONNECT_BACKOFF_MS, option_.timeout);

    grpcChannel_ = grpc::experimental::CreateCustomChannelWithInterceptors(
        rpcTarget,
        grpc::InsecureChannelCredentials(),
        channelArgs,
        {std::move(interceptors)});

    if (!grpcChannel_) {
        throw std::runtime_error("Failed to create gRPC channel");
    }

    const gpr_timespec deadline = gpr_time_add(
        gpr_now(GPR_CLOCK_REALTIME),
        gpr_time_from_seconds(option_.timeout / 1000, GPR_TIMESPAN));
    
    grpc_connectivity_state state = grpcChannel_->GetState(true);
    if (!grpcChannel_->WaitForConnected(deadline)) {
        throw std::runtime_error("Failed to establish connection within timeout");
    }

    stub_ = olama::SearchEngine::NewStub(grpcChannel_);
}

void RpcClient::setTimeout(int timeout) {
    option_.timeout = timeout;
}

void RpcClient::closeConnection() {
    grpcChannel_.reset();
}

}  // namespace vectordb
