#include "objtest.hh"
TEST_F(ReadObjIntegTest, SEGYHORead1)
{
    makeReadRealSEGY(smallSEGYFile, 400, 261, 20e-6, 5);
}

TEST_F(ReadObjIntegTest, SEGYHORead2)
{
    makeReadRealSEGY(bigTraceSEGYFile, 40000, 32000, 20e-6, 5);
}

TEST_F(ReadObjIntegTest, SEGYHORead3)
{
    makeReadRealSEGY(largeSEGYFile, 2000000U, 1000U, 20e-6, 5);
}

TEST_F(ReadObjIntegTest, SEGYReadSingle1)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 200;
    readTest<Block::DOMD, false>(10U, 1U, 0, 13);
    readTest<Block::DODF, false>(10U, 1U, 0, 13);
    readTest<Block::DO, false>(10U, 1U, 0, 13);
}

TEST_F(ReadObjIntegTest, SEGYReadSingle2)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 200;
    readTest<Block::DOMD, false>(10U, 1U, 0, 117);
    readTest<Block::DODF, false>(10U, 1U, 0, 117);
    readTest<Block::DO, false>(10U, 1U, 0, 117);
}

TEST_F(ReadObjIntegTest, SEGYReadZeroNt)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD, false>(10U, 0U);
    readTest<Block::DODF, false>(10U, 0U);
    readTest<Block::DO, false>(10U, 0U);
}

TEST_F(ReadObjIntegTest, SEGYReadZeroNs)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 0;
    readTest<Block::DOMD, false>(10U, 100U);
    readTest<Block::DODF, false>(10U, 100U);
    readTest<Block::DO, false>(10U, 100U);
}

TEST_F(ReadObjIntegTest, SEGYRead)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD, false>(10U, 100U);
    readTest<Block::DODF, false>(10U, 100U);
    readTest<Block::DO, false>(10U, 100U);
}

TEST_F(ReadObjIntegTest, FarmSEGYBigRead)
{
    makeReadRealSEGY<false>(plargeFile);
    ns = 2000;
    readTest<Block::DOMD, false>(10U, 100U);
    ns = 5000;
    readTest<Block::DOMD, false>(10U, 300000);
    readTest<Block::DODF, false>(10U, 300000);
    readTest<Block::DO, false>(10U, 300000);
}

//Random reads
TEST_F(ReadObjIntegTest, SEGYRandomReadSingle1)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    ns = 200;
    readRandomTest<Block::DOMD, false>(vec, 117);
    readRandomTest<Block::DODF, false>(vec, 117);
    readRandomTest<Block::DO, false>(vec, 117);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadSingle2)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    ns = 200;
    readRandomTest<Block::DOMD, false>(vec, 13);
    readRandomTest<Block::DODF, false>(vec, 13);
    readRandomTest<Block::DO, false>(vec, 13);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadZeroNt)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(0, 1337);
    ns = 2000;
    readRandomTest<Block::DOMD, false>(vec);
    readRandomTest<Block::DODF, false>(vec);
    readRandomTest<Block::DO, false>(vec);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadZeroNs)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    ns = 0;
    readRandomTest<Block::DOMD, false>(vec);
    readRandomTest<Block::DODF, false>(vec);
    readRandomTest<Block::DO, false>(vec);
}

TEST_F(ReadObjIntegTest, SEGYRandomRead)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    ns = 2000;
    readRandomTest<Block::DOMD, false>(vec);
    readRandomTest<Block::DODF, false>(vec);
    readRandomTest<Block::DO, false>(vec);
}

TEST_F(ReadObjIntegTest, FarmSEGYRandomBigRead)
{
    makeReadRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    ns = 5000;
    readRandomTest<Block::DOMD, false>(vec);
    readRandomTest<Block::DODF, false>(vec);
    readRandomTest<Block::DO, false>(vec);
}

