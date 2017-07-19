#include "objsegytest.hh"
TEST_F(WriteObjIntegTest, SEGYHOWrite1)
{
    makeRealSEGY(tempFile);
    writeHOPattern<false>(0, 107);
}

TEST_F(WriteObjIntegTest, SEGYHOWrite2)
{
    makeRealSEGY(tempFile);
    writeHOPattern<false>(0, 46);
}

TEST_F(WriteObjIntegTest, SEGYHOWrite3)
{
    makeRealSEGY(tempFile);
    writeHOPattern<false>(0, 0);
}

TEST_F(WriteObjIntegTest, SEGYWriteSingle1)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 1U, 200, 0, 117);
    writeTest<Block::DODF, false>(10U, 1U, 200, 0, 117);
    writeTest<Block::DO, false>(10U, 1U, 200, 0, 117);
}

TEST_F(WriteObjIntegTest, SEGYWriteSingle2)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 1U, 200, 0, 13);
    writeTest<Block::DODF, false>(10U, 1U, 200, 0, 13);
    writeTest<Block::DO, false>(10U, 1U, 200, 0, 13);
}

TEST_F(WriteObjIntegTest, SEGYWriteZeroNt)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 0U, 2000);
    writeTest<Block::DODF, false>(10U, 0U, 200);
    writeTest<Block::DO, false>(10U, 0U, 200);
}

TEST_F(WriteObjIntegTest, SEGYWriteZeroNs)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 100U, 0U);
    writeTest<Block::DODF, false>(10U, 100U, 0U);
    writeTest<Block::DO, false>(10U, 100U, 0U);
}

TEST_F(WriteObjIntegTest, SEGYWrite)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 100U, 2000);
    writeTest<Block::DODF, false>(10U, 100U, 2000);
    writeTest<Block::DO, false>(10U, 100U, 2000);
}

TEST_F(WriteObjIntegTest, FarmSEGYBigWrite)
{
    makeRealSEGY(tempFile);
    writeTest<Block::DOMD, false>(10U, 300000, 5000);
    writeTest<Block::DODF, false>(10U, 300000, 5000);
    writeTest<Block::DO, false>(10U, 300000, 5000);
}

//random write
TEST_F(WriteObjIntegTest, SEGYRandomWriteSingle1)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(200, vec, 117);
    writeRandomTest<Block::DODF, false>(200, vec, 117);
    writeRandomTest<Block::DO, false>(200, vec, 117);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteSingle2)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(200, vec, 13);
    writeRandomTest<Block::DODF, false>(200, vec, 13);
    writeRandomTest<Block::DO, false>(200, vec, 13);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteZeroNt)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD, false>(2000, vec);
    writeRandomTest<Block::DODF, false>(2000, vec);
    writeRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteZeroNs)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(0U, vec);
    writeRandomTest<Block::DODF, false>(0U, vec);
    writeRandomTest<Block::DO, false>(0U, vec);
}

TEST_F(WriteObjIntegTest, SEGYRandomWrite)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(2000, vec);
    writeRandomTest<Block::DODF, false>(2000, vec);
    writeRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(WriteObjIntegTest, FarmSEGYRandomBigWrite)
{
    makeRealSEGY(tempFile);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD, false>(5000U, vec);
    writeRandomTest<Block::DODF, false>(5000U, vec);
    writeRandomTest<Block::DO, false>(5000U, vec);
}

