#include "objsegytest.hh"
TEST_F(ReadObjSpecTest, TestBypassConstructor)
{
    makeSEGY();
    EXPECT_EQ(piol, obj->piol);
    EXPECT_EQ(notFile, obj->name);
    EXPECT_EQ(mock, obj->data);
}

TEST_F(ReadObjIntegTest, zeroSEGYFileSize)
{
    makeRealSEGY(zeroFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(0U, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjIntegTest, SmallSEGYFileSize)
{
    makeRealSEGY(smallFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(smallSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjIntegTest, BigSEGYFileSize)
{
    makeRealSEGY(largeFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data);
    EXPECT_EQ(largeSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ReadObjSpecTest, ZeroSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(0U);
}

TEST_F(ReadObjSpecTest, SmallSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(40U*prefix(2U));
}

TEST_F(ReadObjSpecTest, BigSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(8U*prefix(4U));
}

