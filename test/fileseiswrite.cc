#include "filetest.hh"

//TODO:: Add tests were trace header writes cause a setFileSz call

TEST_F(FileSeisWrite, FileWriteHO)
{
    makeMock<true>();
}

TEST_F(FileSeisWrite, FileWriteHOLongString)
{
    //Extend the string beyond the text boundary
    //Extended text should be dropped in write call
    csize_t sz = testString.size();
    csize_t extendSz = 3400U - sz;
    testString.resize(sz + extendSz);
    for (size_t i = 3200U; i < sz+extendSz; i++)
        testString[i] = uchar(0x7F);

    makeMock<true>();
}

TEST_F(FileSeisWrite, FileWriteHOEmptyString)
{
    testString.resize(0);
    makeMock<true>();
}

TEST_F(FileSeisWrite, FileWriteTrHdrGrid)
{
    makeMock<true>();
    for (size_t i = 0; i < nt; i++)
        writeTrHdrGridTest(i);
}

TEST_F(FileSeisWrite, FileWriteTrHdrCoord1)
{
    makeMock<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xCMP, yCMP}, {160010, 240022}, -100, 10U, &tr);


    File::Param prm(1U);
    File::setPrm(0, Meta::xCmp, 1600.1, &prm);
    File::setPrm(0, Meta::yCmp, 2400.22, &prm);
    File::setPrm(0, Meta::tn, 10U, &prm);
    wfile->writeParam(10U, 1U, &prm);
}

TEST_F(FileSeisWrite, FileWriteTrHdrCoord2)
{
    makeMock<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1600100, 3400222}, -1000, 10U, &tr);

    File::Param prm(1U);
    File::setPrm(0, Meta::xSrc, 1600.1000, &prm);
    File::setPrm(0, Meta::ySrc, 3400.2220, &prm);
    File::setPrm(0, Meta::tn, 10U, &prm);

    wfile->writeParam(10U, 1U, &prm);
}

TEST_F(FileSeisWrite, FileWriteTrHdrCoord3)
{
    makeMock<true>();
    std::vector<uchar> tr(SEGSz::getMDSz());
    initWriteTrHdrCoord({xSrc, ySrc}, {1623001001,   34002220}, -10000, 10U, &tr);

    File::Param prm(1U);
    File::setPrm(0, Meta::xSrc, 162300.10009, &prm);
    File::setPrm(0, Meta::ySrc, 3400.22201, &prm);
    File::setPrm(0, Meta::tn, 10U, &prm);
    wfile->writeParam(10U, 1U, &prm);
}

TEST_F(FileSeisWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    makeMock<false>();
    writeTraceTest(0U, nt);
}

TEST_F(FileSeisWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    makeMock<false>();
    writeTraceTest<true>(0U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceNormal)
{
    nt = 100;
    ns = 300;
    size_t size = 1U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    size_t size = 1U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    makeMock<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceBigNs)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    makeMock<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceBigOffset)
{
    nt = 3000;
    ns = 3000;
    makeMock<false>();
    writeTraceTest(3728270U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceBigOffset)
{
    nt = 3000;
    ns = 3000;
    size_t size = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0] = 3001;
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 3000;
    ns = 3000;
    makeMock<false>();
    writeTraceTest<true>(3728270U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceWPrmBigOffset)
{
    nt = 3000;
    ns = 3000;
    size_t size = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0] = 3001;
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock<false>();
    writeTraceTest(0U, nt);
}

TEST_F(FileSeisWrite, FarmFileWriteRandomTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMock<false>();
    writeTraceTest<true>(0U, nt);
}

TEST_F(FileSeisWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeMock<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock<false>();
    writeTraceTest(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 10U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeMock<false>();
    writeTraceTest<true>(10U, nt);
}

TEST_F(FileSeisWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = 10U;
    auto offsets = getRandomVec(size, nt, 1337);
    makeMock<false>();
    writeRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSeisWrite, FileWriteParam)
{
    makeMock<true>();
    writeTraceHeaderTest<false>(0U, nt);
}

TEST_F(FileSeisWrite, FileWriteParamOne)
{
    nt = 400;
    makeMock<true>();
    writeTraceHeaderTest<false>(200, 1);
}

TEST_F(FileSeisWrite, FileWriteParamBigOffset)
{
    nt = 10000000U;
    makeMock<true>();
    writeTraceHeaderTest<false>(nt-1, 1);
}

TEST_F(FileSeisWrite, FileWriteParamCopy)
{
    makeMock<true>();
    writeTraceHeaderTest<true>(0U, nt);
}

TEST_F(FileSeisWrite, FileWriteParamOneCopy)
{
    nt = 400;
    makeMock<true>();
    writeTraceHeaderTest<true>(200, 1);
}

TEST_F(FileSeisWrite, FileWriteParamBigOffsetCopy)
{
    nt = 10000000U;
    makeMock<true>();
    writeTraceHeaderTest<true>(nt-1, 1);
}
//Akward to fit this into the current functions
/*TEST_F(FileSeisDeath, FileWriteTraceParamBigOffset)
{
    makeMock<true>();
    writeTraceHeaderTest<false, false>(NT_LIMITS+1);
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}*/
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEATH TESTS ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef FileSeisWrite FileSeisDeath;

TEST_F(FileSeisDeath, FileWriteAPIBadns)
{
    ns = 0x470000;
    makeMock<false>();
    wfile->writeNs(ns);
    wmock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

#ifdef NT_LIMITS
TEST_F(FileSeisDeath, FileWriteAPIBadnt)
{
    nt = NT_LIMITS + 1;
    makeMock<false>();
    wfile->writeNt(nt);

    wmock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}
#endif

TEST_F(FileSeisDeath, FileWriteAPIBadinc)
{
    geom_t ginc = geom_t(1)/geom_t(0);
    makeMock<false>();
    wfile->writeInc(ginc);

    wmock.reset();
    EXPECT_EXIT(piol->isErr(), ExitedWithCode(EXIT_FAILURE), ".*8 3 Fatal Error in PIOL. . Dumping Log 0");
}

