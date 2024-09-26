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

#include <gtest/gtest.h>

#include "include/types/filter.h"
namespace vectordb {

TEST(FilterTest, AndCondition) {
    Filter f("key1 = 'value1'");
    f.andCond("key2 = 'value2'");
    ASSERT_EQ(f.cond, "key1 = 'value1' and (key2 = 'value2')");
}

TEST(FilterTest, OrCondition) {
    Filter f("key1 = 'value1'");
    f.orCond("key2 = 'value2'");
    ASSERT_EQ(f.cond, "key1 = 'value1' or (key2 = 'value2')");
}

TEST(FilterTest, AndNotCondition) {
    Filter f("key1 = 'value1'");
    f.andNotCond("key2 = 'value2'");
    ASSERT_EQ(f.cond, "key1 = 'value1' and not (key2 = 'value2')");
}

TEST(FilterTest, OrNotCondition) {
    Filter f("key1 = 'value1'");
    f.orNotCond("key2 = 'value2'");
    ASSERT_EQ(f.cond, "key1 = 'value1' or not (key2 = 'value2')");
}

TEST(FilterTest, InFunction) {
    std::vector<std::string> values = {"value1", "value2", "value3"};
    std::string result = Filter::in("key1", values);
    ASSERT_EQ(result, "key1 in (\"value1\",\"value2\",\"value3\")");
}

TEST(FilterTest, NotInFunction) {
    std::vector<std::string> values = {"value1", "value2", "value3"};
    std::string result = Filter::notIn("key1", values);
    ASSERT_EQ(result, "key1 not in (\"value1\",\"value2\",\"value3\")");
}

TEST(FilterTest, IncludeFunction) {
    std::vector<std::string> values = {"value1", "value2"};
    std::string result = Filter::include("key1", values);
    ASSERT_EQ(result, "key1 include (\"value1\",\"value2\")");
}

TEST(FilterTest, ExcludeFunction) {
    std::vector<std::string> values = {"value1", "value2"};
    std::string result = Filter::exclude("key1", values);
    ASSERT_EQ(result, "key1 exclude (\"value1\",\"value2\")");
}

TEST(FilterTest, IncludeAllFunction) {
    std::vector<std::string> values = {"value1", "value2"};
    std::string result = Filter::includeAll("key1", values);
    ASSERT_EQ(result, "key1 include all (\"value1\",\"value2\")");
}

TEST(FilterTest, EmptyCondition) {
    Filter f;
    ASSERT_EQ(f.cond, "");
}

TEST(FilterTest, LockingMechanism) {
    Filter f("key1 = 'value1'");
    std::string initialCondition = f.cond;
    f.andCond("key2 = 'value2'");
    std::string updatedCondition = f.cond;
    ASSERT_EQ(initialCondition, "key1 = 'value1'");
    ASSERT_EQ(updatedCondition, "key1 = 'value1' and (key2 = 'value2')");
}

}  // namespace vectordb
