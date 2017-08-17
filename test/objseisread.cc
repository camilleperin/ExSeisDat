#include "objtest.hh"
TEST_F(ReadSeisObjSpecTest, SeisHORead1)
{
    make(smallNt, smallNs, 10e-6);
}

TEST_F(ReadSeisObjSpecTest, SeisHORead2)
{
    make(100, 0, 10e-6);
}

TEST_F(ReadSeisObjSpecTest, SeisHORead3)
{
    make(100, 100, 0);
}

TEST_F(ReadSeisObjSpecTest, SeisHORead4)
{
    make(100, 100, 10e-6);
}

TEST_F(ReadSeisObjSpecTest, SeisHORead5)
{
    make(0, 0, 0);
}

TEST_F(ReadSeisObjSpecTest, SeisReadSingle1)
{
    make(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 117);
    readTest<Block::DODF>(10U, 1U, 13, 117);
    readTest<Block::DO>(10U, 1U, 13, 117);
}

TEST_F(ReadSeisObjSpecTest, SeisReadSingle2)
{
    make(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 13);
    readTest<Block::DODF>(10U, 1U, 13, 13);
    readTest<Block::DO>(10U, 1U, 13, 13);
}

TEST_F(ReadSeisObjSpecTest, SeisReadZeroNt)
{
    make(20, 2000);
    readTest<Block::DOMD>(10U, 0U);
    readTest<Block::DODF>(10U, 0U);
    readTest<Block::DO>(10U, 0U);
}

TEST_F(ReadSeisObjSpecTest, SeisReadZeroNs)
{
    make(120, 0);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSeisObjSpecTest, SeisRead)
{
    make(120, 2000);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSeisObjSpecTest, FarmSeisBigRead)
{
    make(400000, 5000);
    readTest<Block::DOMD>(10U, 300000);
    readTest<Block::DODF>(10U, 300000);
    readTest<Block::DO>(10U, 300000);
}

//random read

TEST_F(ReadSeisObjSpecTest, SeisRandomReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 117);
    readRandomTest<Block::DODF>(vec, 117);
    readRandomTest<Block::DO>(vec, 117);
}

TEST_F(ReadSeisObjSpecTest, SeisRandomReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 13);
    readRandomTest<Block::DODF>(vec, 13);
    readRandomTest<Block::DO>(vec, 13);
}

TEST_F(ReadSeisObjSpecTest, SeisRandomReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    make(0, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSeisObjSpecTest, SeisRandomReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 0);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSeisObjSpecTest, SeisRandomRead)
{
    auto vec = getRandomVec(100U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSeisObjSpecTest, FarmSeisRandomBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10000, 5000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

