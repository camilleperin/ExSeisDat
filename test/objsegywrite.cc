#include "objsegytest.hh"
TEST_F(WriteObjSpecTest, SEGYHOWrite1)
{
    makeWriteSEGY(10000, 2000);
    writeHOPattern();
}

TEST_F(WriteObjSpecTest, SEGYHOWrite2)
{
    makeWriteSEGY(10000, 0);
    writeHOPattern();
}

TEST_F(WriteObjSpecTest, SEGYHOWrite3)
{
    makeWriteSEGY(0, 2000);
    writeHOPattern();
}

TEST_F(WriteObjSpecTest, SEGYHOWrite4)
{
    makeWriteSEGY(0, 0);
    writeHOPattern();
}

TEST_F(WriteObjSpecTest, SEGYWriteSingle1)
{
    makeWriteSEGY(11, 200);
    writeTest<Block::DOMD>(10, 1, 13, 13);
    writeTest<Block::DODF>(10, 1, 13, 13);
    writeTest<Block::DO>(10, 1, 13, 13);
}

TEST_F(WriteObjSpecTest, SEGYWriteSingle2)
{
    makeWriteSEGY(11, 200);
    writeTest<Block::DOMD>(10, 1, 13, 117);
    writeTest<Block::DODF>(10, 1, 13, 117);
    writeTest<Block::DO>(10, 1, 13, 117);
}

TEST_F(WriteObjSpecTest, SEGYWriteZeroNt)
{
    makeWriteSEGY(10, 2000);
    writeTest<Block::DOMD>(10U, 0U);
    writeTest<Block::DODF>(10U, 0U);
    writeTest<Block::DO>(10U, 0U);
}

TEST_F(WriteObjSpecTest, SEGYWriteZeroNs)
{
    makeWriteSEGY(110, 0);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteObjSpecTest, SEGYWrite)
{
    makeWriteSEGY(110, 2000);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteObjSpecTest, FarmSEGYBigWrite)
{
    makeWriteSEGY(300500, 5000);
    writeTest<Block::DOMD>(10U, 300000);
    writeTest<Block::DODF>(10U, 300000);
    writeTest<Block::DO>(10U, 300000);
}

//random read

TEST_F(WriteObjSpecTest, SEGYRandomWriteSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    makeWriteSEGY(*std::max_element(vec.begin(), vec.end()) + 1LU, 200);
    writeRandomTest<Block::DOMD>(vec, 13);
    writeRandomTest<Block::DODF>(vec, 13);
    writeRandomTest<Block::DO>(vec, 13);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    makeWriteSEGY(*std::max_element(vec.begin(), vec.end()) + 1LU, 200);
    writeRandomTest<Block::DOMD>(vec, 117);
    writeRandomTest<Block::DODF>(vec, 117);
    writeRandomTest<Block::DO>(vec, 117);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    makeWriteSEGY(0, 2000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteObjSpecTest, SEGYRandomWriteZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    makeWriteSEGY(*std::max_element(vec.begin(), vec.end()) + 1LU, 0);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteObjSpecTest, SEGYRandomWrite)
{
    auto vec = getRandomVec(100U, 1337);
    makeWriteSEGY(*std::max_element(vec.begin(), vec.end()) + 1LU, 2000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteObjSpecTest, FarmSEGYRandomBigWrite)
{
    auto vec = getRandomVec(300000U, 1337);
    makeWriteSEGY(*std::max_element(vec.begin(), vec.end()) + 1LU, 5000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

