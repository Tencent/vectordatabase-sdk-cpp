#include <gtest/gtest.h>

#include "include/rpc_client.h"

namespace vectordb {

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

}  // namespace vectordb
