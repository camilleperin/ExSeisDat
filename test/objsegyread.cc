#include "objtest.hh"
TEST_F(ReadSEGYObjSpecTest, SEGYHORead1)
{
    make(100, 100, 10e-6, 5);
}

TEST_F(ReadSEGYObjSpecTest, SEGYHORead2)
{
    make(100, 0, 10e-6, 1);
}

TEST_F(ReadSEGYObjSpecTest, SEGYHORead3)
{
    make(100, 100, 0, 1);
}

TEST_F(ReadSEGYObjSpecTest, SEGYHORead4)
{
    make(100, 100, 10e-6, 0);
}

TEST_F(ReadSEGYObjSpecTest, SEGYHORead5)
{
    make(0, 0, 0, 0);
}

TEST_F(ReadSEGYObjSpecTest, SEGYReadSingle1)
{
    make(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 117);
    readTest<Block::DODF>(10U, 1U, 13, 117);
    readTest<Block::DO>(10U, 1U, 13, 117);
}

TEST_F(ReadSEGYObjSpecTest, SEGYReadSingle2)
{
    make(12, 200);
    readTest<Block::DOMD>(10U, 1U, 13, 13);
    readTest<Block::DODF>(10U, 1U, 13, 13);
    readTest<Block::DO>(10U, 1U, 13, 13);
}

TEST_F(ReadSEGYObjSpecTest, SEGYReadZeroNt)
{
    make(20, 2000);
    readTest<Block::DOMD>(10U, 0U);
    readTest<Block::DODF>(10U, 0U);
    readTest<Block::DO>(10U, 0U);
}

TEST_F(ReadSEGYObjSpecTest, SEGYReadZeroNs)
{
    make(120, 0);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSEGYObjSpecTest, SEGYRead)
{
    make(120, 2000);
    readTest<Block::DOMD>(10U, 100U);
    readTest<Block::DODF>(10U, 100U);
    readTest<Block::DO>(10U, 100U);
}

TEST_F(ReadSEGYObjSpecTest, FarmSEGYBigRead)
{
    make(400000, 5000);
    readTest<Block::DOMD>(10U, 300000);
    readTest<Block::DODF>(10U, 300000);
    readTest<Block::DO>(10U, 300000);
}

//random read

TEST_F(ReadSEGYObjSpecTest, SEGYRandomReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 117);
    readRandomTest<Block::DODF>(vec, 117);
    readRandomTest<Block::DO>(vec, 117);
}

TEST_F(ReadSEGYObjSpecTest, SEGYRandomReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 200);
    readRandomTest<Block::DOMD>(vec, 13);
    readRandomTest<Block::DODF>(vec, 13);
    readRandomTest<Block::DO>(vec, 13);
}

TEST_F(ReadSEGYObjSpecTest, SEGYRandomReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    make(0, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjSpecTest, SEGYRandomReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 0);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjSpecTest, SEGYRandomRead)
{
    auto vec = getRandomVec(100U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10, 2000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

TEST_F(ReadSEGYObjSpecTest, FarmSEGYRandomBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    make(*std::max_element(vec.begin(), vec.end())+10000, 5000);
    readRandomTest<Block::DOMD>(vec);
    readRandomTest<Block::DODF>(vec);
    readRandomTest<Block::DO>(vec);
}

