#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "factory.h"

TEST(SampleTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(LogTest, InfoOutput) {
    EXPECT_EQ(1 + 1, 2);
}
