#include "objsegytest.hh"
TEST_F(WriteObjSpecTest, SEGYHOWrite1)
{
    writeHOPattern(0, 107);
}

TEST_F(WriteObjSpecTest, SEGYHOWrite2)
{
    writeHOPattern(0, 46);
}

TEST_F(WriteObjSpecTest, SEGYHOWrite3)
{
    writeHOPattern(0, 0);
}

TEST_F(WriteObjSpecTest, SEGYWriteSingle1)
{
    writeTest<Block::DOMD>(10U, 1U, 200, 13, 13);
    writeTest<Block::DODF>(10U, 1U, 200, 13, 13);
    writeTest<Block::DO>(10U, 1U, 200, 13, 13);
}

TEST_F(WriteObjSpecTest, SEGYWriteSingle2)
{
    writeTest<Block::DOMD>(10U, 1U, 200, 13, 117);
    writeTest<Block::DODF>(10U, 1U, 200, 13, 117);
    writeTest<Block::DO>(10U, 1U, 200, 13, 117);
}

TEST_F(WriteObjSpecTest, SEGYWriteZeroNt)
{
    writeTest<Block::DOMD>(10U, 0U, 2000);
    writeTest<Block::DODF>(10U, 0U, 2000);
    writeTest<Block::DO>(10U, 0U, 2000);
}

TEST_F(WriteObjSpecTest, SEGYWriteZeroNs)
{
    writeTest<Block::DOMD>(10U, 100U, 0U);
    writeTest<Block::DODF>(10U, 100U, 0U);
    writeTest<Block::DO>(10U, 100U, 0U);
}

TEST_F(WriteObjSpecTest, SEGYWrite)
{
    writeTest<Block::DOMD>(10U, 100U, 2000);
    writeTest<Block::DODF>(10U, 100U, 2000);
    writeTest<Block::DO>(10U, 100U, 2000);
}

TEST_F(WriteObjSpecTest, FarmSEGYBigWrite)
{
    writeTest<Block::DOMD>(10U, 300000, 5000);
    writeTest<Block::DODF>(10U, 300000, 5000);
    writeTest<Block::DO>(10U, 300000, 5000);
}

//random read

TEST_F(WriteObjSpecTest, SEGYRandomWriteSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(200, vec, 13);
    writeRandomTest<Block::DODF>(200, vec, 13);
    writeRandomTest<Block::DO>(200, vec, 13);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(200, vec, 117);
    writeRandomTest<Block::DODF>(200, vec, 117);
    writeRandomTest<Block::DO>(200, vec, 117);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD>(2000, vec);
    writeRandomTest<Block::DODF>(2000, vec);
    writeRandomTest<Block::DO>(2000, vec);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(0U, vec);
    writeRandomTest<Block::DODF>(0U, vec);
    writeRandomTest<Block::DO>(0U, vec);
}

TEST_F(WriteObjSpecTest, SEGYRandomWrite)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(2000, vec);
    writeRandomTest<Block::DODF>(2000, vec);
    writeRandomTest<Block::DO>(2000, vec);
}

TEST_F(WriteObjSpecTest, FarmSEGYRandomBigWrite)
{
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD>(5000U, vec);
    writeRandomTest<Block::DODF>(5000U, vec);
    writeRandomTest<Block::DO>(5000U, vec);
}

