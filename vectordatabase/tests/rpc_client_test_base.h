#include <gtest/gtest.h>

#include "include/rpc_client.h"

namespace vectordb {

class RpcClientTestBase : public ::testing::Test {
  protected:
    RpcClient client;

    RpcClientTestBase()
        : client("url", "username",
                 "key", nullptr) {
    }

    void SetUp() override {
        // pass
    }

    void TearDown() override {
        // pass
    }
};

}  // namespace vectordb
