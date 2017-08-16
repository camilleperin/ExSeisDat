#include "objsegytest.hh"
TEST_F(ReadObjSpecTest, SEGYHORead1)
{
    makeSEGY(100, 100, 10e-6, 5);
}

TEST_F(ReadObjSpecTest, SEGYHORead2)
{
    makeSEGY(100, 0, 10e-6, 1);
}

TEST_F(ReadObjSpecTest, SEGYHORead3)
{
    makeSEGY(100, 100, 0, 1);
}

TEST_F(ReadObjSpecTest, SEGYHORead4)
{
    makeSEGY(100, 100, 10e-6, 0);
}

TEST_F(ReadObjSpecTest, SEGYHORead5)
{
    makeSEGY(0, 0, 0, 0);
}

TEST_F(ReadObjSpecTest, SEGYReadSingle1)
{
    makeSEGY(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 117);
    readTest<Block::DODF>(10U, 1U, 13, 117);
    readTest<Block::DO>(10U, 1U, 13, 117);
}

TEST_F(ReadObjSpecTest, SEGYReadSingle2)
{
    makeSEGY(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 13);
    readTest<Block::DODF>(10U, 1U, 13, 13);
    readTest<Block::DO>(10U, 1U, 13, 13);
}

TEST_F(ReadObjSpecTest, SEGYReadZeroNt)
{
    makeSEGY(20, 2000);
    readTest<Block::DOMD>(10U, 0U);
    readTest<Block::DODF>(10U, 0U);
    readTest<Block::DO>(10U, 0U);
}

TEST_F(ReadObjSpecTest, SEGYReadZeroNs)
{
    makeSEGY(120, 0);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadObjSpecTest, SEGYRead)
{
    makeSEGY(120, 2000);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadObjSpecTest, FarmSEGYBigRead)
{
    makeSEGY(400000, 5000);
    readTest<Block::DOMD>(10U, 300000);
    readTest<Block::DODF>(10U, 300000);
    readTest<Block::DO>(10U, 300000);
}

//random read

TEST_F(ReadObjSpecTest, SEGYRandomReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    makeSEGY(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 117);
    readRandomTest<Block::DODF>(vec, 117);
    readRandomTest<Block::DO>(vec, 117);
}

TEST_F(ReadObjSpecTest, SEGYRandomReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    makeSEGY(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 13);
    readRandomTest<Block::DODF>(vec, 13);
    readRandomTest<Block::DO>(vec, 13);
}

TEST_F(ReadObjSpecTest, SEGYRandomReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    makeSEGY(0, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadObjSpecTest, SEGYRandomReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    makeSEGY(*std::max_element(vec.begin(), vec.end())+10, 0);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadObjSpecTest, SEGYRandomRead)
{
    auto vec = getRandomVec(100U, 1337);
    makeSEGY(*std::max_element(vec.begin(), vec.end())+10, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadObjSpecTest, FarmSEGYRandomBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    makeSEGY(*std::max_element(vec.begin(), vec.end())+10000, 5000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

