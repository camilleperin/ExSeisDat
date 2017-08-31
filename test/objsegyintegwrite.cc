#include "objtest.hh"
TEST_F(WriteSEGYObjIntegTest, SEGYHOWrite1)
{
    makeReal(tempFile);
    writeHOPattern();
}

TEST_F(WriteSEGYObjIntegTest, SEGYHOWrite2)
{
    makeReal(tempFile, 0);
    writeHOPattern();
}

TEST_F(WriteSEGYObjIntegTest, SEGYWriteSingle1)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD>(10U, 1U, 0, 117);
    writeTest<Block::DODF>(10U, 1U, 0, 117);
    writeTest<Block::DO>(10U, 1U, 0, 117);
}

TEST_F(WriteSEGYObjIntegTest, SEGYWriteSingle2)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD>(10U, 1U, 0, 13);
    writeTest<Block::DODF>(10U, 1U, 0, 13);
    writeTest<Block::DO>(10U, 1U, 0, 13);
}

TEST_F(WriteSEGYObjIntegTest, SEGYWriteZeroNt)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD>(10U, 2000);
    writeTest<Block::DODF>(10U, 200);
    writeTest<Block::DO>(10U, 200);
}

TEST_F(WriteSEGYObjIntegTest, SEGYWriteZeroNs)
{
    makeReal(tempFile, 0);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSEGYObjIntegTest, SEGYWrite)
{
    makeReal(tempFile, 2000);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSEGYObjIntegTest, FarmSEGYBigWrite)
{
    makeReal(tempFile, 5000);
    writeTest<Block::DOMD>(10U, 300000);
    writeTest<Block::DODF>(10U, 300000);
    writeTest<Block::DO>(10U, 300000);
}

//random write
TEST_F(WriteSEGYObjIntegTest, SEGYRandomWriteSingle1)
{
    makeReal(tempFile, 200);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(vec, 117);
    writeRandomTest<Block::DODF>(vec, 117);
    writeRandomTest<Block::DO>(vec, 117);
}

TEST_F(WriteSEGYObjIntegTest, SEGYRandomWriteSingle2)
{
    makeReal(tempFile, 200);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(vec, 13);
    writeRandomTest<Block::DODF>(vec, 13);
    writeRandomTest<Block::DO>(vec, 13);
}

TEST_F(WriteSEGYObjIntegTest, SEGYRandomWriteZeroNt)
{
    makeReal(tempFile, 2000);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjIntegTest, SEGYRandomWriteZeroNs)
{
    makeReal(tempFile, 0);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjIntegTest, SEGYRandomWrite)
{
    makeReal(tempFile, 2000);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSEGYObjIntegTest, FarmSEGYRandomBigWrite)
{
    makeReal(tempFile, 5000);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

