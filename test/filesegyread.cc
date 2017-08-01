#include "filetest.hh"

TEST_F(FileSEGYRead, FileConstructor)
{
    makeMock();
    EXPECT_EQ(piol, rfile->piol);
    EXPECT_EQ(notFile, rfile->name);
    EXPECT_EQ(rmock, rfile->obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSEGYRead, FileReadHO)
{
    makeMock();

    EXPECT_EQ(nt, rfile->readNt());
    piol->isErr();

    EXPECT_EQ(ns, rfile->readNs());
    piol->isErr();

    EXPECT_EQ(geom_t(inc)*SI::Micro, rfile->readInc());
    piol->isErr();

    std::string text = rfile->readText();
    ASSERT_TRUE(text.size() < SEGSz::getTextSz());
    EXPECT_EQ(text, testString);

//EBCDIC conversion check
}

TEST_F(FileSEGYRead, FileReadHOAPI)
{
    testEBCDIC = true;
    makeMock();

    std::string text = rfile->readText();
    ASSERT_TRUE(text.size() < SEGSz::getTextSz());
    EXPECT_EQ(text, testString);
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSEGYRead, FileReadTraceHeader)
{
    makeMock();
    initTrBlock();
    for (size_t i = 0; i < nt; i++)
        initReadTrMock(ns, i);
}

TEST_F(FileSEGYRead, FileReadTrHdrBigNs)
{
    makeMock();
    initTrBlock();

    const size_t bigns = 10000;
    rfile->ns = bigns;
    initReadTrMock(bigns, nt/2U);
}

TEST_F(FileSEGYRead, FileReadFileTrs)
{
    makeMock();
    initTrBlock();
    initReadTrHdrsMock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadFileTrsRandom)
{
    makeMock();
    initTrBlock();
    initRandReadTrHdrsMock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNS)
{
    nt = 100;
    ns = 10000;
    makeMock();
    readTraceTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
#warning //TODO: should be readTraceTest( 3728270, nt)?
    readTraceTest(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
#warning //TODO: should be readTraceTest( 3728270, nt)?
    initTrBlock();
    readTraceTest<true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadRandomTrace)
{
    nt = 3728270;
    ns = 300;
    size_t size = 100U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrm)
{
    nt = 3728270;
    ns = 300;
    size_t size = 100U;
    makeMock();
    initTrBlock();

    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSEGYRead, FarmFileReadTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock();
    readTraceTest(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock();
    readTraceTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock();
    readTraceTest(9, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 5U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 5U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSEGYRead, FileReadTraceBigNSRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    readTraceTest<false, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrmRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    initTrBlock();
    readTraceTest<true, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
    readTraceTest<false, true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
    initTrBlock();
    readTraceTest<true, true>(3728270, 3000);
}
