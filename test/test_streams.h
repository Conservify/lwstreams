#include <gtest/gtest.h>

#include <lwstreams/lwstreams.h>

class StreamsSuite : public ::testing::Test {
protected:
    StreamsSuite();
    virtual ~StreamsSuite();

    virtual void SetUp();
    virtual void TearDown();

};
