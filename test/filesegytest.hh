#include <iconv.h>
#include <string.h>
#include <memory>
#include <typeinfo>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/cmpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#include "share/units.hh"
#include "share/segy.hh"
#include "share/datatype.hh"

#define private public
#define protected public
#include "cppfileapi.hh"
#include "file/filesegy.hh"
#undef private
#undef protected

namespace PIOL { namespace File {
extern int16_t calcScale(const coord_t coord);
extern int16_t scalComp(int16_t scal1, int16_t scal2);
extern void setCoord(const File::Coord item, const coord_t coord, const int16_t scale, uchar * buf);
extern void setGrid(const File::Grid item, const grid_t grid, uchar * buf);
extern int16_t deScale(const geom_t val);
}}

using namespace testing;
using namespace PIOL;
using PIOL::File::deScale;
using PIOL::File::grid_t;
using PIOL::File::coord_t;
using PIOL::File::TraceParam;
using File::calcScale;
using File::scalComp;
using File::setCoord;
using File::setGrid;


enum Hdr : size_t
{
    Increment  = 3216U,
    NumSample  = 3220U,
    Type       = 3224U,
    Sort       = 3228U,
    Units      = 3254U,
    SEGYFormat = 3500U,
    FixedTrace = 3502U,
    Extensions = 3504U,
};

enum TrHdr : size_t
{
    SeqNum      = 0U,
    SeqFNum     = 4U,
    ORF         = 8U,
    TORF        = 12U,
    RcvElv      = 40U,
    SurfElvSrc  = 44U,
    SrcDpthSurf = 48U,
    DtmElvRcv   = 52U,
    DtmElvSrc   = 56U,
    WtrDepSrc   = 60U,
    WtrDepRcv   = 64U,
    ScaleElev   = 68U,
    ScaleCoord  = 70U,
    xSrc        = 72U,
    ySrc        = 76U,
    xRcv        = 80U,
    yRcv        = 84U,
    xCMP        = 180U,
    yCMP        = 184U,
    il          = 188U,
    xl          = 192U
};

class MockObj : public Obj::Interface
{
    public :
    MockObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::Interface(piol_, name_, data_) {}
    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD1(readHO, void(uchar *));
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
    MOCK_CONST_METHOD1(writeHO, void(const uchar *));
#warning TODO: Separate out groups of functions to separate files
#warning Not covered yet beyond sz=1
    MOCK_CONST_METHOD4(readDOMD, void(csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(writeDOMD, void(csize_t, csize_t, csize_t, const uchar *));

#warning Not covered yet.
    MOCK_CONST_METHOD4(readDODF, void(csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(writeDODF, void(csize_t, csize_t, csize_t, const uchar *));
};

struct FileSEGYTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    std::shared_ptr<MockObj> mock;
    Comm::MPI::Opt opt;
    bool testEBCDIC;
    std::string testString = {"This is a string for testing EBCDIC conversion etc."};
    std::unique_ptr<File::Interface> file;
    std::vector<uchar> tr;
    size_t nt = 40U;
    size_t ns = 200U;
    int inc = 10;
    csize_t format = 5;
    std::vector<uchar> ho;

    FileSEGYTest()
    {
        testEBCDIC = false;
        file = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        ho.resize(SEGSz::getHOSz());
    }

    ~FileSEGYTest()
    {
    }

    template <bool WRITE = true>
    void makeSEGY(std::string name)
    {
        if (file.get() != nullptr)
            file.reset();
        file = std::make_unique<File::Direct>(piol, name, (WRITE ? FileMode::Test : FileMode::Read));

        piol->isErr();

        if (WRITE)
            writeHO<false>();
    }

    template <bool WRITE = true, bool makeCall = true>
    void makeMockSEGY()
    {
        if (file.get() != nullptr)
            file.reset();
        if (mock != nullptr)
            mock.reset();
        mock = std::make_shared<MockObj>(piol, notFile, nullptr);
        piol->isErr();
        Mock::AllowLeak(mock.get());

        ho.resize(SEGSz::getHOSz());

        if (!WRITE)
        {
            EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns)));
            initReadHOMock(testEBCDIC);
        }
        else
            EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(0U));

        file = std::make_unique<File::SEGY>(piol, notFile, mock, (WRITE ? FileMode::Test : FileMode::Read));

        if (WRITE)
        {
            if (makeCall)
            {
                piol->isErr();
                writeHO<true>();
            }
            else
            {
                file->nt = nt;
                file->ns = ns;
            }
        }
    }

    void initReadHOMock(bool testEBCDIC)
    {
        if (testEBCDIC)
        {
            // Create an EBCDID string to convert back to ASCII in the test
            size_t tsz = testString.size();
            size_t tsz2 = tsz;
            char * t = &testString[0];
            char * newText = reinterpret_cast<char *>(ho.data());
            iconv_t toAsc = iconv_open("EBCDICUS//", "ASCII//");
            ::iconv(toAsc, &t, &tsz, &newText, &tsz2);
            iconv_close(toAsc);
        }
        else
        {
            for (size_t i = 0; i < testString.size(); i++)
                ho[i] = testString[i];
        }
        for (size_t i = testString.size(); i < SEGSz::getTextSz(); i++)
            ho[i] = ho[i % testString.size()];

        ho[NumSample] = ns >> 8 & 0xFF;
        ho[NumSample+1] = ns & 0xFF;
        ho[Increment] = inc >> 8 & 0xFF;
        ho[Increment+1] = inc & 0xFF;
        ho[Type+1] = format;
        EXPECT_CALL(*mock, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));
    }

    void initTrBlock()
    {
        tr.resize(nt * SEGSz::getMDSz());
        for (size_t i = 0; i < nt; i++)
        {
            uchar * md = &tr[i * SEGSz::getMDSz()];
            getBigEndian(ilNum(i), &md[il]);
            getBigEndian(xlNum(i), &md[xl]);

            int16_t scale;
            int16_t scal1 = deScale(xNum(i));
            int16_t scal2 = deScale(yNum(i));

            if (scal1 > 1 || scal2 > 1)
                scale = std::max(scal1, scal2);
            else
                scale = std::min(scal1, scal2);

            getBigEndian(scale, &md[ScaleCoord]);
            getBigEndian(int32_t(std::lround(xNum(i)/scale)), &md[xSrc]);
            getBigEndian(int32_t(std::lround(yNum(i)/scale)), &md[ySrc]);
        }
    }

    void initReadTrMock(size_t ns, size_t offset)
    {
        std::vector<uchar>::iterator iter = tr.begin() + offset*SEGSz::getMDSz();
        EXPECT_CALL(*mock.get(), readDOMD(offset, ns, 1U, _))
                    .Times(Exactly(2))
                    .WillRepeatedly(SetArrayArgument<3>(iter, iter + SEGSz::getMDSz()));

        grid_t line;
        file->readGridPoint(File::Grid::Line, offset, 1U, &line);
        ASSERT_EQ(ilNum(offset), line.il);
        ASSERT_EQ(xlNum(offset), line.xl);

        coord_t src;
        file->readCoordPoint(File::Coord::Src, offset, 1U, &src);
        ASSERT_DOUBLE_EQ(xNum(offset), src.x);
        ASSERT_DOUBLE_EQ(yNum(offset), src.y);
    }

    void initReadTrHdrsMock(size_t ns, size_t tn)
    {
        EXPECT_CALL(*mock.get(), readDOMD(0, ns, tn, _))
                    .Times(Exactly(2))
                    .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        std::vector<File::grid_t> line(tn);
        file->readGridPoint(File::Grid::Line, 0U, tn, line.data());

        std::vector<File::coord_t> src(tn);
        file->readCoordPoint(File::Coord::Src, 0U, tn, src.data());

        for (size_t i = 0; i < tn; i++)
        {
            ASSERT_EQ(ilNum(i), line[i].il);
            ASSERT_EQ(xlNum(i), line[i].xl);

            ASSERT_DOUBLE_EQ(xNum(i), src[i].x);
            ASSERT_DOUBLE_EQ(yNum(i), src[i].y);
        }
    }

    template <bool MOCK = true>
    void writeHO()
    {
        if (MOCK)
        {
            size_t fsz = SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns);
            EXPECT_CALL(*mock, setFileSz(fsz)).Times(Exactly(1));

            for (size_t i = 0U; i < std::min(testString.size(), SEGSz::getTextSz()); i++)
                ho[i] = testString[i];

            ho[NumSample+1] = ns & 0xFF;
            ho[NumSample] = ns >> 8 & 0xFF;
            ho[Increment+1] = inc & 0xFF;
            ho[Increment] = inc >> 8 & 0xFF;
            ho[Type+1] = format;
            ho[3255U] = 1;
            ho[3500U] = 1;
            ho[3503U] = 1;
            ho[3505U] = 0;

            EXPECT_CALL(*mock, writeHO(_)).Times(Exactly(1)).WillOnce(check0(ho.data(), SEGSz::getHOSz()));
        }

        file->writeNt(nt);
        piol->isErr();

        file->writeNs(ns);
        piol->isErr();

        file->writeInc(geom_t(inc*SI::Micro));
        piol->isErr();

        file->writeText(testString);
        piol->isErr();
    }

    void writeTrHdrGridTest(size_t offset)
    {
        std::vector<uchar> tr(SEGSz::getMDSz());
        getBigEndian(ilNum(offset), tr.data()+il);
        getBigEndian(xlNum(offset), tr.data()+xl);
        getBigEndian<int16_t>(1, &tr[ScaleCoord]);
        getBigEndian(int32_t(offset), &tr[SeqFNum]);

        EXPECT_CALL(*mock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1))
                                                        .WillOnce(check3(tr.data(), SEGSz::getMDSz()));

        TraceParam prm;
        prm.line = {ilNum(offset), xlNum(offset)};
        prm.tn = offset;
        file->writeTraceParam(offset, 1U, &prm);
    }

    void initWriteTrHdrCoord(std::pair<size_t, size_t> item, std::pair<int32_t, int32_t> val,
                             int16_t scal, size_t offset, std::vector<uchar> * tr)
    {
        getBigEndian(scal,              tr->data()+ScaleCoord);
        getBigEndian(val.first,         tr->data()+item.first);
        getBigEndian(val.second,        tr->data()+item.second);
        getBigEndian(int32_t(offset),   tr->data()+SeqFNum);
        EXPECT_CALL(*mock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1))
                                                        .WillOnce(check3(tr->data(), SEGSz::getMDSz()));
    }

    template <bool MOCK = true>
    void readTraceTest(csize_t offset, size_t tn)
    {
        std::vector<uchar> buf;
        if (MOCK)
        {
            if (mock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            if (tn * ns)
            {
                buf.resize(tn * SEGSz::getDFSz(ns));
                for (size_t i = 0U; i < tn; i++)
                    for (size_t j = 0U; j < ns; j++)
                    {
                        float val = offset + i + j;
                        getBigEndian(toint(val), &buf[(i*ns+j)*sizeof(float)]);
                    }
                EXPECT_CALL(*mock, readDODF(offset, ns, tn, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<3>(buf.begin(), buf.end()));
            }
        }

        std::vector<float> bufnew(tn * ns);
        file->readTrace(offset, tn, bufnew.data());
        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < ns; j++)
                ASSERT_EQ(bufnew[i*ns + j], float(offset + i + j)) << "Trace Number: " << i << " " << j;
    }

    template <bool MOCK = true>
    void writeTraceTest(csize_t offset, csize_t tn)
    {
        std::vector<uchar> buf;
        if (MOCK)
        {
            EXPECT_CALL(*mock, writeHO(_)).Times(Exactly(1));
            if (mock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            if (tn * ns)
            {
                buf.resize(tn * SEGSz::getDFSz(ns));
                for (size_t i = 0U; i < tn; i++)
                    for (size_t j = 0U; j < ns; j++)
                    {
                        float val = offset + i + j;
                        getBigEndian(toint(val), &buf[(i*ns + j)*sizeof(float)]);
                    }
                EXPECT_CALL(*mock, writeDODF(offset, ns, tn, _))
                                .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
            }
        }
        std::vector<float> bufnew(tn * ns);
        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < ns; j++)
                bufnew[i*ns + j] = float(offset + i + j);

        file->writeTrace(offset, tn, bufnew.data());

        if (MOCK == false)
            readTraceTest<MOCK>(offset, tn);
    }

    template <bool MOCK = true>
    void writeTraceHeaderTest(csize_t offset, csize_t tn)
    {
        std::vector<uchar> buf;
        if (MOCK)
        {
            buf.resize(tn * SEGSz::getMDSz());
            for (size_t i = 0; i < tn; i++)
            {
                coord_t src = coord_t(ilNum(i+1), xlNum(i+5));
                coord_t rcv = coord_t(ilNum(i+2), xlNum(i+6));
                coord_t cmp = coord_t(ilNum(i+3), xlNum(i+7));
                grid_t line = grid_t(ilNum(i+4), xlNum(i+8));

                int16_t scale = scalComp(1, calcScale(src));
                scale = scalComp(scale, calcScale(rcv));
                scale = scalComp(scale, calcScale(cmp));

                uchar * md = &buf[i*SEGSz::getMDSz()];
                getBigEndian(scale, &md[ScaleCoord]);
                setCoord(File::Coord::Src, src, scale, md);
                setCoord(File::Coord::Rcv, rcv, scale, md);
                setCoord(File::Coord::CMP, cmp, scale, md);

                setGrid(File::Grid::Line, line, md);
                getBigEndian(int32_t(offset + i), &md[SeqFNum]);
            }
            EXPECT_CALL(*mock.get(), writeDOMD(offset, ns, tn, _))
                        .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
        }

        std::vector<TraceParam> prm(tn);
        for (size_t i = 0; i < tn; i++)
        {
            prm[i].src = coord_t(ilNum(i+1), xlNum(i+5));
            prm[i].rcv = coord_t(ilNum(i+2), xlNum(i+6));
            prm[i].cmp = coord_t(ilNum(i+3), xlNum(i+7));
            prm[i].line = grid_t(ilNum(i+4), xlNum(i+8));
            prm[i].tn = offset + i;
        }
        file->writeTraceParam(offset, prm.size(), prm.data());
    }
};
typedef FileSEGYTest FileSEGYWrite;
typedef FileSEGYTest FileSEGYRead;
typedef FileSEGYTest FileSEGYIntegRead;
typedef FileSEGYTest FileSEGYIntegWrite;

