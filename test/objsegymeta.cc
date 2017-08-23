#include "objtest.hh"
TEST_F(ReadSEGYObjSpecTest, TestBypassConstructor)
{
    make(10, 10);
    EXPECT_EQ(piol, obj->piol);
    EXPECT_EQ(notFile, obj->name);
    EXPECT_EQ(mock, obj->data);
}

TEST_F(ReadSEGYObjIntegTest, zeroSEGYFileSize)
{
    makeReadReal<false>(zeroFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(0U, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadSEGYObjIntegTest, SmallSEGYFileSize)
{
    makeReadReal<false>(smallFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(smallSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadSEGYObjIntegTest, BigSEGYFileSize)
{
    makeReadReal<false>(largeFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(largeSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadSEGYObjSpecTest, ZeroSEGYFileSize)
{
    make(10, 10);
    FileSizeTest(0U);
}

TEST_F(ReadSEGYObjSpecTest, SmallSEGYFileSize)
{
    make(10, 10);
    FileSizeTest(40U*prefix(2U));
}

TEST_F(ReadSEGYObjSpecTest, BigSEGYFileSize)
{
    make(10, 10);
    FileSizeTest(8U*prefix(4U));
}

