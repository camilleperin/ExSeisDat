#include "objtest.hh"
TEST_F(WriteSEGYObjSpecTest, SEGYHOWrite1)
{
    makeWrite(10000, 2000);
    writeHOPattern();
}

TEST_F(WriteSEGYObjSpecTest, SEGYHOWrite2)
{
    makeWrite(10000, 0);
    writeHOPattern();
}

TEST_F(WriteSEGYObjSpecTest, SEGYHOWrite3)
{
    makeWrite(0, 2000);
    writeHOPattern();
}

TEST_F(WriteSEGYObjSpecTest, SEGYHOWrite4)
{
    makeWrite(0, 0);
    writeHOPattern();
}

TEST_F(WriteSEGYObjSpecTest, SEGYWriteSingle1)
{
    makeWrite(11, 200);
    writeTest<Block::DOMD>(10, 1, 13, 13);
    writeTest<Block::DODF>(10, 1, 13, 13);
    writeTest<Block::DO>(10, 1, 13, 13);
}

TEST_F(WriteSEGYObjSpecTest, SEGYWriteSingle2)
{
    makeWrite(11, 200);
    writeTest<Block::DOMD>(10, 1, 13, 117);
    writeTest<Block::DODF>(10, 1, 13, 117);
    writeTest<Block::DO>(10, 1, 13, 117);
}

TEST_F(WriteSEGYObjSpecTest, SEGYWriteZeroNt)
{
    makeWrite(10, 2000);
    writeTest<Block::DOMD>(10U, 0U);
    writeTest<Block::DODF>(10U, 0U);
    writeTest<Block::DO>(10U, 0U);
}

TEST_F(WriteSEGYObjSpecTest, SEGYWriteZeroNs)
{
    makeWrite(110, 0);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSEGYObjSpecTest, SEGYWrite)
{
    makeWrite(110, 2000);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSEGYObjSpecTest, FarmSEGYBigWrite)
{
    makeWrite(300500, 5000);
    writeTest<Block::DOMD>(10U, 300000);
    writeTest<Block::DODF>(10U, 300000);
    writeTest<Block::DO>(10U, 300000);
}

//random read

TEST_F(WriteSEGYObjSpecTest, SEGYRandomWriteSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    makeWrite(*std::max_element(vec.begin(), vec.end()) + 1LU, 200);
    writeRandomTest<Block::DOMD>(vec, 13);
    writeRandomTest<Block::DODF>(vec, 13);
    writeRandomTest<Block::DO>(vec, 13);
}

TEST_F(WriteSEGYObjSpecTest, SEGYRandomWriteSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    makeWrite(*std::max_element(vec.begin(), vec.end()) + 1LU, 200);
    writeRandomTest<Block::DOMD>(vec, 117);
    writeRandomTest<Block::DODF>(vec, 117);
    writeRandomTest<Block::DO>(vec, 117);
}

TEST_F(WriteSEGYObjSpecTest, SEGYRandomWriteZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    makeWrite(0, 2000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjSpecTest, SEGYRandomWriteZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    makeWrite(*std::max_element(vec.begin(), vec.end()) + 1LU, 0);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjSpecTest, SEGYRandomWrite)
{
    auto vec = getRandomVec(100U, 1337);
    makeWrite(*std::max_element(vec.begin(), vec.end()) + 1LU, 2000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjSpecTest, FarmSEGYRandomBigWrite)
{
    auto vec = getRandomVec(300000U, 1337);
    makeWrite(*std::max_element(vec.begin(), vec.end()) + 1LU, 5000);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

