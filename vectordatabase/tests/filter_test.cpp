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
