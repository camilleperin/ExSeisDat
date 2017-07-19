#include "objsegytest.hh"
TEST_F(ReadObjIntegTest, SEGYHORead1)
{
    makeRealSEGY(plargeFile);
    readHOPatternTest<false>(0, 107);
}

TEST_F(ReadObjIntegTest, SEGYHORead2)
{
    makeRealSEGY(plargeFile);
    readHOPatternTest<false>(0, 46);
}

TEST_F(ReadObjIntegTest, SEGYHORead3)
{
    makeRealSEGY(plargeFile);
    readHOPatternTest<false>(0, 0);
}

TEST_F(ReadObjIntegTest, SEGYReadSingle1)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 1U, 200, 0, 13);
    readTest<Block::DODF, false>(10U, 1U, 200, 0, 13);
    readTest<Block::DO, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ReadObjIntegTest, SEGYReadSingle2)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 1U, 200, 0, 117);
    readTest<Block::DODF, false>(10U, 1U, 200, 0, 117);
    readTest<Block::DO, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ReadObjIntegTest, SEGYReadZeroNt)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 0U, 2000);
    readTest<Block::DODF, false>(10U, 0U, 2000);
    readTest<Block::DO, false>(10U, 0U, 2000);
}

TEST_F(ReadObjIntegTest, SEGYReadZeroNs)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 100U, 0U);
    readTest<Block::DODF, false>(10U, 100U, 0U);
    readTest<Block::DO, false>(10U, 100U, 0U);
}

TEST_F(ReadObjIntegTest, SEGYRead)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 100U, 2000);
    readTest<Block::DODF, false>(10U, 100U, 2000);
    readTest<Block::DO, false>(10U, 100U, 2000);
}

TEST_F(ReadObjIntegTest, FarmSEGYBigRead)
{
    makeRealSEGY(plargeFile);
    readTest<Block::DOMD, false>(10U, 300000, 5000);
    readTest<Block::DODF, false>(10U, 300000, 5000);
    readTest<Block::DO, false>(10U, 300000, 5000);
}

//Random reads
TEST_F(ReadObjIntegTest, SEGYRandomReadSingle1)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD, false>(200U, vec, 117);
    readRandomTest<Block::DODF, false>(200U, vec, 117);
    readRandomTest<Block::DO, false>(200U, vec, 117);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadSingle2)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD, false>(200U, vec, 13);
    readRandomTest<Block::DODF, false>(200U, vec, 13);
    readRandomTest<Block::DO, false>(200U, vec, 13);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadZeroNt)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(0, 1337);
    readRandomTest<Block::DOMD, false>(2000, vec);
    readRandomTest<Block::DODF, false>(2000, vec);
    readRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(ReadObjIntegTest, SEGYRandomReadZeroNs)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD, false>(0U, vec);
    readRandomTest<Block::DODF, false>(0U, vec);
    readRandomTest<Block::DO, false>(0U, vec);
}

TEST_F(ReadObjIntegTest, SEGYRandomRead)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD, false>(2000U, vec);
    readRandomTest<Block::DODF, false>(2000U, vec);
    readRandomTest<Block::DO, false>(2000U, vec);
}

TEST_F(ReadObjIntegTest, FarmSEGYRandomBigRead)
{
    makeRealSEGY(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    readRandomTest<Block::DOMD, false>(5000U, vec);
    readRandomTest<Block::DODF, false>(5000U, vec);
    readRandomTest<Block::DO, false>(5000U, vec);
}

