#include "filetest.hh"

//Write test of File::Seis -> Obj::Seis -> Data::MPIIO
TEST_F(FileSeisIntegWrite, SeisWriteReadHO)
{
    ns = 261U;
    nt = 400U;

    createFile(tempFile);
    EXPECT_EQ(ns, rfile->readNs());
    piol->isErr();

    EXPECT_EQ(nt, rfile->readNt());
    piol->isErr();

    std::string text = rfile->readText();
    piol->isErr();
    ASSERT_TRUE(testString.size() <= text.size());
    for (size_t i = 0; i < testString.size(); i++)
        ASSERT_EQ(testString[i], text[i]);
}

//Write test of File::Seis -> Obj::Seis -> Data::MPIIO
TEST_F(FileSeisIntegWrite, SeisWriteReadParam)
{
    ns = 261U;
    nt = 400U;
    coord_t coord(1600, 2000);
    grid_t grid(ilNum(201), xlNum(201));
    File::Param prm(1U), prm2(1U);

    createFile(tempFile);

    piol->isErr();
    wfile->writeNs(ns);
    wfile->writeNt(nt);
    piol->isErr();

    File::setPrm(0, Meta::il, grid.il, &prm);
    File::setPrm(0, Meta::xl, grid.xl, &prm);
    File::setPrm(0, Meta::xCmp, coord.x, &prm);
    File::setPrm(0, Meta::yCmp, coord.y, &prm);

    wfile->writeParam(201U, 1U, &prm);
    rfile->readParam(201U, 1U, &prm2);

    ASSERT_EQ(grid.il, File::getPrm<llint>(0U, Meta::il, &prm2));
    ASSERT_EQ(grid.xl, File::getPrm<llint>(0U, Meta::xl, &prm2));
    ASSERT_DOUBLE_EQ(coord.x, File::getPrm<geom_t>(0U, Meta::xCmp, &prm2));
    ASSERT_DOUBLE_EQ(coord.y, File::getPrm<geom_t>(0U, Meta::yCmp, &prm2));
}

TEST_F(FileSeisIntegWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceNormal)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceNormalOpt)
{
    nt = 100;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmNormalOpt)
{
    nt = 100;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceNormalOpt)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceWPrmNormalOpt)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    createFile(tempFile);
    writeTraceTest<false, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    createFile(tempFile);
    writeTraceTest<true, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceBigNs)
{
    nt = 100;
    ns = 10000;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceBigOffset)
{
    nt = 10;
    ns = 3000;
    createFile(tempFile);
    writeTraceTest<false, false>(3728270, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 10;
    ns = 3000;
    createFile(tempFile);
    writeTraceTest<true, false>(3728270, nt);
}

TEST_F(FileSeisIntegWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<false, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    createFile(tempFile);
    writeTraceTest<true, false>(0, nt);
}

TEST_F(FileSeisIntegWrite, FarmFileWriteRandomTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/100;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/100;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    createFile(tempFile);
    writeTraceTest<false, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    createFile(tempFile);
    writeTraceTest<true, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    createFile(tempFile);
    writeTraceTest<false, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    createFile(tempFile);
    writeTraceTest<true, false>(10, nt);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSeisIntegWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    createFile(tempFile);
    writeRandomTraceTest<true, false>(size, offsets);
}
