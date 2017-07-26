#include "filetest.hh"

csize_t largens = 1000U;
csize_t largent = 2000000U;
csize_t bigtns = 32000U;
csize_t bigtnt = 40000U;
csize_t smallns = 261U;
csize_t smallnt = 400U;

//Read test of File::Seis -> Obj::Seis -> Data::MPIIO
TEST_F(FileSeisIntegRead, SeisReadHO)
{
    openFile<false>(smallSeisFile);

    piol->isErr();
    EXPECT_EQ(smallns, rfile->readNs());
    piol->isErr();
    EXPECT_EQ(smallnt, rfile->readNt());
    piol->isErr();
    if (sizeof(geom_t) == sizeof(double))
        EXPECT_DOUBLE_EQ(double(20e-6), rfile->readInc());
    else
        EXPECT_FLOAT_EQ(float(20e-6), rfile->readInc());
}

TEST_F(FileSeisIntegRead, FileReadTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    openFile<false>(smallSeisFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FileReadTraceWPrmSmall)
{
    nt = smallnt;
    ns = smallns;
    openFile<false>(smallSeisFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(smallSeisFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceWPrmSmall)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(smallSeisFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    openFile<true>(smallSeisFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FileReadTraceWPrmSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    openFile<true>(smallSeisFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(smallSeisFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceWPrmSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(smallSeisFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    openFile<false>(bigTraceSeisFile);
    readTraceTest<false, false>(10, nt);
}

TEST_F(FileSeisIntegRead, FileReadTraceWPrmBigNS)
{
    nt = 200;
    ns = bigtns;
    openFile<false>(bigTraceSeisFile);
    readTraceTest<true, false>(10, nt);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    size_t size = 2;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(bigTraceSeisFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadRandomTraceWPrmBigNS)
{
    nt = 200;
    ns = bigtns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(bigTraceSeisFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadTraceBigOffset)
{
    nt = 10;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<false, false>(1999990U, nt);
}

TEST_F(FileSeisIntegRead, FileReadTraceWPrmBigOffset)
{
    nt = 10;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<true, false>(1999990U, nt);
}

TEST_F(FileSeisIntegRead, FarmFileReadTraceBigNt)
{
    nt = largent;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FarmFileReadTraceWPrmBigNt)
{
    nt = largent;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegRead, FarmFileReadRandomTraceBigNt)
{
    nt = largent;
    ns = largens;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(largeSeisFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt = largent;
    ns = largens;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(largeSeisFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FileReadTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<false, false>(10, nt);
}

TEST_F(FileSeisIntegRead, FileReadTraceWPrmZeroNt)
{
    nt = 0U;
    ns = largens;
    openFile<false>(largeSeisFile);
    readTraceTest<true, false>(10, nt);
}

TEST_F(FileSeisIntegRead, FarmFileReadRandomTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(largeSeisFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegRead, FarmFileReadRandomTraceWPrmZeroNt)
{
    nt = 0U;
    ns = largens;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    openFile<false>(largeSeisFile);
    readRandomTraceTest<true, false>(size, offsets);
}

