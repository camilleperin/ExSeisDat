#include "objtest.hh"
TEST_F(WriteObjIntegTest, SEGYHOWrite1)
{
    makeReal(tempFile);
    writeHOPattern<false>();
}

TEST_F(WriteObjIntegTest, SEGYHOWrite2)
{
    makeReal(tempFile, 0);
    writeHOPattern<false>();
}

TEST_F(WriteObjIntegTest, SEGYWriteSingle1)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD, false>(10U, 1U, 0, 117);
    writeTest<Block::DODF, false>(10U, 1U, 0, 117);
    writeTest<Block::DO, false>(10U, 1U, 0, 117);
}

TEST_F(WriteObjIntegTest, SEGYWriteSingle2)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD, false>(10U, 1U, 0, 13);
    writeTest<Block::DODF, false>(10U, 1U, 0, 13);
    writeTest<Block::DO, false>(10U, 1U, 0, 13);
}

TEST_F(WriteObjIntegTest, SEGYWriteZeroNt)
{
    makeReal(tempFile, 200);
    writeTest<Block::DOMD, false>(10U, 2000);
    writeTest<Block::DODF, false>(10U, 200);
    writeTest<Block::DO, false>(10U, 200);
}

TEST_F(WriteObjIntegTest, SEGYWriteZeroNs)
{
    makeReal(tempFile, 0);
    writeTest<Block::DOMD, false>(10U, 100U);
    writeTest<Block::DODF, false>(10U, 100U);
    writeTest<Block::DO, false>(10U, 100U);
}

TEST_F(WriteObjIntegTest, SEGYWrite)
{
    makeReal(tempFile, 2000);
    writeTest<Block::DOMD, false>(10U, 100U);
    writeTest<Block::DODF, false>(10U, 100U);
    writeTest<Block::DO, false>(10U, 100U);
}

TEST_F(WriteObjIntegTest, FarmSEGYBigWrite)
{
    makeReal(tempFile, 5000);
    writeTest<Block::DOMD, false>(10U, 300000);
    writeTest<Block::DODF, false>(10U, 300000);
    writeTest<Block::DO, false>(10U, 300000);
}

//random write
TEST_F(WriteObjIntegTest, SEGYRandomWriteSingle1)
{
    makeReal(tempFile, 200);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(vec, 117);
    writeRandomTest<Block::DODF, false>(vec, 117);
    writeRandomTest<Block::DO, false>(vec, 117);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteSingle2)
{
    makeReal(tempFile, 200);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(vec, 13);
    writeRandomTest<Block::DODF, false>(vec, 13);
    writeRandomTest<Block::DO, false>(vec, 13);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteZeroNt)
{
    makeReal(tempFile, 2000);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD, false>(vec);
    writeRandomTest<Block::DODF, false>(vec);
    writeRandomTest<Block::DO, false>(vec);
}

TEST_F(WriteObjIntegTest, SEGYRandomWriteZeroNs)
{
    makeReal(tempFile, 0);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(vec);
    writeRandomTest<Block::DODF, false>(vec);
    writeRandomTest<Block::DO, false>(vec);
}

TEST_F(WriteObjIntegTest, SEGYRandomWrite)
{
    makeReal(tempFile, 2000);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(vec);
    writeRandomTest<Block::DODF, false>(vec);
    writeRandomTest<Block::DO, false>(vec);
}

TEST_F(WriteObjIntegTest, FarmSEGYRandomBigWrite)
{
    makeReal(tempFile, 5000);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD, false>(vec);
    writeRandomTest<Block::DODF, false>(vec);
    writeRandomTest<Block::DO, false>(vec);
}

