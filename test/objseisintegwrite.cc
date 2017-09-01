#include "objtest.hh"
TEST_F(WriteSeisObjIntegTest, SeisHOWrite1)
{
    makeReal(tempFile, 200, 10e-6);
    writeHOPattern();
}

TEST_F(WriteSeisObjIntegTest, SeisHOWrite2)
{
    makeReal(tempFile, 0, 10e-6);
    writeHOPattern();
}

TEST_F(WriteSeisObjIntegTest, SeisWriteSingle1)
{
    makeReal(tempFile, 200, 10e-6);
    writeTest<Block::DOMD>(10U, 1U, 0, 117);
    writeTest<Block::DODF>(10U, 1U, 0, 117);
    writeTest<Block::DO>(10U, 1U, 0, 117);
}

TEST_F(WriteSeisObjIntegTest, SeisWriteSingle2)
{
    makeReal(tempFile, 200, 10e-6);
    writeTest<Block::DOMD>(10U, 1U, 0, 13);
    writeTest<Block::DODF>(10U, 1U, 0, 13);
    writeTest<Block::DO>(10U, 1U, 0, 13);
}

TEST_F(WriteSeisObjIntegTest, SeisWriteZeroNt)
{
    makeReal(tempFile, 200, 10e-6);
    writeTest<Block::DOMD>(10U, 2000);
    writeTest<Block::DODF>(10U, 200);
    writeTest<Block::DO>(10U, 200);
}

TEST_F(WriteSeisObjIntegTest, SeisWriteZeroNs)
{
    makeReal(tempFile, 0, 10e-6);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSeisObjIntegTest, SeisWrite)
{
    makeReal(tempFile, 2000, 10e-6);
    writeTest<Block::DOMD>(10U, 100U);
    writeTest<Block::DODF>(10U, 100U);
    writeTest<Block::DO>(10U, 100U);
}

TEST_F(WriteSeisObjIntegTest, FarmSeisBigWrite)
{
    makeReal(tempFile, 5000, 10e-6);
    writeTest<Block::DOMD>(10U, 300000);
    writeTest<Block::DODF>(10U, 300000);
    writeTest<Block::DO>(10U, 300000);
}

//random write
TEST_F(WriteSeisObjIntegTest, SeisRandomWriteSingle1)
{
    makeReal(tempFile, 200, 10e-6);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(vec, 117);
    writeRandomTest<Block::DODF>(vec, 117);
    writeRandomTest<Block::DO>(vec, 117);
}

TEST_F(WriteSeisObjIntegTest, SeisRandomWriteSingle2)
{
    makeReal(tempFile, 200, 10e-6);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD>(vec, 13);
    writeRandomTest<Block::DODF>(vec, 13);
    writeRandomTest<Block::DO>(vec, 13);
}

TEST_F(WriteSeisObjIntegTest, SeisRandomWriteZeroNt)
{
    makeReal(tempFile, 2000, 10e-6);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSeisObjIntegTest, SeisRandomWriteZeroNs)
{
    makeReal(tempFile, 0, 10e-6);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSeisObjIntegTest, SeisRandomWrite)
{
    makeReal(tempFile, 2000, 10e-6);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

TEST_F(WriteSeisObjIntegTest, FarmSeisRandomBigWrite)
{
    makeReal(tempFile, 5000, 10e-6);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD>(vec);
    writeRandomTest<Block::DODF>(vec);
    writeRandomTest<Block::DO>(vec);
}

