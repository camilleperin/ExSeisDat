#include "objtest.hh"
TEST_F(ReadObjSpecTest, TestBypassConstructor)
{
    makeSEGY(10, 10);
    EXPECT_EQ(piol, obj->piol);
    EXPECT_EQ(notFile, obj->name);
    EXPECT_EQ(mock, obj->data);
}

TEST_F(ReadObjIntegTest, zeroSEGYFileSize)
{
    makeReadRealSEGY<false>(zeroFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(0U, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjIntegTest, SmallSEGYFileSize)
{
    makeReadRealSEGY<false>(smallFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(smallSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjIntegTest, BigSEGYFileSize)
{
    makeReadRealSEGY<false>(largeFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(largeSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjSpecTest, ZeroSEGYFileSize)
{
    makeSEGY(10, 10);
    FileSizeTest(0U);
}

TEST_F(ReadObjSpecTest, SmallSEGYFileSize)
{
    makeSEGY(10, 10);
    FileSizeTest(40U*prefix(2U));
}

TEST_F(ReadObjSpecTest, BigSEGYFileSize)
{
    makeSEGY(10, 10);
    FileSizeTest(8U*prefix(4U));
}

