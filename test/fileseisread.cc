#include "filetest.hh"

TEST_F(FileSeisRead, FileConstructor)
{
    makeMock();
    EXPECT_EQ(piol, rfile->piol);
    EXPECT_EQ(notFile, rfile->name);
    EXPECT_EQ(rmock, rfile->obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSeisRead, FileReadHO)
{
    makeMock();

    EXPECT_EQ(nt, rfile->readNt());
    piol->isErr();

    EXPECT_EQ(ns, rfile->readNs());
    piol->isErr();

    EXPECT_EQ(geom_t(inc*SI::Micro), rfile->readInc());
    piol->isErr();

    std::string text = rfile->readText();
    EXPECT_EQ(SEGSz::getTextSz(), text.size());

//EBCDIC conversion check
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

TEST_F(FileSeisRead, FileReadHOAPI)
{
    testEBCDIC = true;
    makeMock();

    std::string text = rfile->readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSeisRead, FileReadTraceHeader)
{
    makeMock();
    initTrBlock();
    for (size_t i = 0; i < nt; i++)
        initReadTrMock(ns, i);
}

TEST_F(FileSeisRead, FileReadTrHdrBigNs)
{
    makeMock();
    initTrBlock();

    const size_t bigns = 10000;
    rfile->ns = bigns;
    initReadTrMock(bigns, nt/2U);
}

TEST_F(FileSeisRead, FileReadFileTrs)
{
    makeMock();
    initTrBlock();
    initReadTrHdrsMock(ns, nt);
}

TEST_F(FileSeisRead, FileReadFileTrsRandom)
{
    makeMock();
    initTrBlock();
    initRandReadTrHdrsMock(ns, nt);
}

TEST_F(FileSeisRead, FileReadTraceBigNS)
{
    nt = 100;
    ns = 10000;
    makeMock();
    readTraceTest(10, nt);
}

TEST_F(FileSeisRead, FileReadTraceBigNSWPrm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSeisRead, FileReadTraceBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
#warning //TODO: should be readTraceTest( 3728270, nt)?
    readTraceTest(3728270, 3000);
}

TEST_F(FileSeisRead, FileReadTraceWPrmBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
#warning //TODO: should be readTraceTest( 3728270, nt)?
    initTrBlock();
    readTraceTest<true>(3728270, 3000);
}

TEST_F(FileSeisRead, FileReadRandomTrace)
{
    nt = 3728270;
    ns = 300;
    size_t size = 100U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSeisRead, FileReadRandomTraceWPrm)
{
    nt = 3728270;
    ns = 300;
    size_t size = 100U;
    makeMock();
    initTrBlock();

    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSeisRead, FarmFileReadTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock();
    readTraceTest(0, nt);
}

TEST_F(FileSeisRead, FarmFileReadTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSeisRead, FarmFileReadRandomTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSeisRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisRead, FileReadTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock();
    readTraceTest(10, nt);
}

TEST_F(FileSeisRead, FileReadTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSeisRead, FileReadRandomTraceZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSeisRead, FileReadRandomTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisRead, FileReadTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock();
    readTraceTest(9, nt);
}

TEST_F(FileSeisRead, FileReadTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSeisRead, FileReadRandomTraceZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 5U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSeisRead, FileReadRandomTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 5U;
    makeMock();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSeisRead, FileReadTraceBigNSRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    readTraceTest<false, true>(10, nt);
}

TEST_F(FileSeisRead, FileReadTraceBigNSWPrmRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMock();
    initTrBlock();
    readTraceTest<true, true>(10, nt);
}

TEST_F(FileSeisRead, FileReadTraceBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
    readTraceTest<false, true>(3728270, 3000);
}

TEST_F(FileSeisRead, FileReadTraceWPrmBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMock();
    initTrBlock();
    readTraceTest<true, true>(3728270, 3000);
}
