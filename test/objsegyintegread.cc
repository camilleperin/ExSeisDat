#include "objtest.hh"
TEST_F(ReadSEGYObjIntegTest, SEGYHORead1)
{
    makeReadReal(smallSEGYFile, 400, 261, 20e-6, 5);
}

TEST_F(ReadSEGYObjIntegTest, SEGYHORead2)
{
    makeReadReal(bigTraceSEGYFile, 40000, 32000, 20e-6, 5);
}

TEST_F(ReadSEGYObjIntegTest, SEGYHORead3)
{
    makeReadReal(largeSEGYFile, 2000000U, 1000U, 20e-6, 5);
}

TEST_F(ReadSEGYObjIntegTest, SEGYReadSingle1)
{
    makeReadReal<false>(plargeFile);
    ns = 200;
    readTest<Block::DOMD>(10U, 1U, 0, 13);
    readTest<Block::DODF>(10U, 1U, 0, 13);
    readTest<Block::DO>(10U, 1U, 0, 13);
}

TEST_F(ReadSEGYObjIntegTest, SEGYReadSingle2)
{
    makeReadReal<false>(plargeFile);
    ns = 200;
    readTest<Block::DOMD>(10U, 1U, 0, 117);
    readTest<Block::DODF>(10U, 1U, 0, 117);
    readTest<Block::DO>(10U, 1U, 0, 117);
}

TEST_F(ReadSEGYObjIntegTest, SEGYReadZeroNt)
{
    makeReadReal<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD>(10U, 0U);
    readTest<Block::DODF>(10U, 0U);
    readTest<Block::DO>(10U, 0U);
}

TEST_F(ReadSEGYObjIntegTest, SEGYReadZeroNs)
{
    makeReadReal<false>(plargeFile);
    ns = 0;
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSEGYObjIntegTest, SEGYRead)
{
    makeReadReal<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSEGYObjIntegTest, FarmSEGYBigRead)
{
    makeReadReal<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD>(10U, 100U);
    ns = 5000;
    readTest<Block::DOMD>(10U, 300000);
    readTest<Block::DODF>(10U, 300000);
    readTest<Block::DO>(10U, 300000);
}

//Random reads
TEST_F(ReadSEGYObjIntegTest, SEGYRandomReadSingle1)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    ns = 200;
    readRandomTest<Block::DOMD>(vec, 117);
    readRandomTest<Block::DODF>(vec, 117);
    readRandomTest<Block::DO>(vec, 117);
}

TEST_F(ReadSEGYObjIntegTest, SEGYRandomReadSingle2)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    ns = 200;
    readRandomTest<Block::DOMD>(vec, 13);
    readRandomTest<Block::DODF>(vec, 13);
    readRandomTest<Block::DO>(vec, 13);
}

TEST_F(ReadSEGYObjIntegTest, SEGYRandomReadZeroNt)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(0, 1337);
    ns = 2000;
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjIntegTest, SEGYRandomReadZeroNs)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    ns = 0;
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjIntegTest, SEGYRandomRead)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    ns = 2000;
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjIntegTest, FarmSEGYRandomBigRead)
{
    makeReadReal<false>(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    ns = 5000;
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

