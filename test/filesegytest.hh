#include <iconv.h>
#include <string.h>
#include <memory>
#include <random>
#include <algorithm>
#include <typeinfo>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include "anc/mpi.hh"
#include "data/datampiio.hh"
#include "object/objsegy.hh"
#include "share/units.hh"
#include "share/segy.hh"
#include "share/datatype.hh"

#define private public
#define protected public
#include "cppfileapi.hh"
#include "file/filesegy.hh"
#include "file/fileseis.hh"
#include "segymdextra.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;
using PIOL::File::deScale;
using File::coord_t;
using File::grid_t;
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

class MockReadObj : public Obj::ReadInterface
{
    public :
    MockReadObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::ReadInterface(piol_, name_, data_) {}
    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD1(readHO, void(uchar *));
    MOCK_CONST_METHOD4(readDOMD, void(csize_t, csize_t, csize_t, uchar *));

    MOCK_CONST_METHOD4(readDODF, void(csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(readDO, void(csize_t, csize_t, csize_t, uchar *));

    MOCK_CONST_METHOD4(readDO, void(csize_t *, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(readDODF, void(csize_t *, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(readDOMD, void(csize_t *, csize_t, csize_t, uchar *));
};

class MockWriteObj : public Obj::WriteInterface
{
    public :
    MockWriteObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::WriteInterface(piol_, name_, data_) {
    }

    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
    MOCK_CONST_METHOD1(writeHO, void(const uchar *));
    MOCK_CONST_METHOD4(writeDOMD, void(csize_t, csize_t, csize_t, const uchar *));

    MOCK_CONST_METHOD4(writeDODF, void(csize_t, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDO, void(csize_t, csize_t, csize_t, const uchar *));

    MOCK_CONST_METHOD4(writeDO, void(csize_t *, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDODF, void(csize_t *, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDOMD, void(csize_t *, csize_t, csize_t, const uchar *));
};

template <class T>
struct FileReadTest : public Test
{
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPI::Opt opt;
    bool testEBCDIC;
    std::string testString = {"This is a string for testing EBCDIC conversion etc."};
    std::vector<uchar> tr;
    size_t nt = 40U;
    size_t ns = 200U;
    int inc = 10;
    csize_t format = 5;
    std::vector<uchar> ho;
    std::unique_ptr<T> rfile;
    std::shared_ptr<MockReadObj> rmock;

    FileReadTest()
    {
        testEBCDIC = false;
        rfile = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        ho.resize(SEGSz::getHOSz());
    }

    ~FileReadTest()
    {
        if (rmock.get())
            Mock::VerifyAndClearExpectations(&rmock);
    }

    template <bool OPTS = false>
    void makeFile(std::string name)
    {
        if (rfile.get())
            rfile.reset();
    #warning FILL
/*
        if (OPTS)
        {
            T::Opt fopt;
            Obj::ReadSEGY::Opt oopt;
            Data::MPIIO::Opt dopt;
            file = std::make_unique<T>(piol, name, fopt, oopt, dopt);
        }
        else*/
        rfile = std::make_unique<T>(piol, name);
        assert(rfile->obj);
        piol->isErr();
    }

    virtual void makeMock()
    {
        if (rfile.get())
            rfile.reset();
        if (rmock.get())
            rmock.reset();
        rmock = std::make_shared<MockReadObj>(piol, notFile, nullptr);
        piol->isErr();
        Mock::AllowLeak(rmock.get());

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
            for (size_t i = 0; i < testString.size(); i++)
                ho[i] = testString[i];
        if (testString.size())
            for (size_t i = testString.size(); i < SEGSz::getTextSz(); i++)
                ho[i] = ho[i % testString.size()];

        ho[NumSample] = ns >> 8 & 0xFF;
        ho[NumSample+1] = ns & 0xFF;
        ho[Increment] = inc >> 8 & 0xFF;
        ho[Increment+1] = inc & 0xFF;
        ho[Type+1] = format;

        EXPECT_CALL(*rmock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() +
                                                                       nt*SEGSz::getDOSz(ns)));
        EXPECT_CALL(*rmock, readHO(_)).Times(Exactly(1)).WillOnce(SetArrayArgument<0>(ho.begin(), ho.end()));

        rfile = std::make_unique<T>(piol, notFile, rmock);
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
        EXPECT_CALL(*rmock.get(), readDOMD(offset, ns, 1U, _))
                    .Times(Exactly(1))
                    .WillRepeatedly(SetArrayArgument<3>(iter, iter + SEGSz::getMDSz()));

        File::Param prm(1U);
        rfile->readParam(offset, 1U, &prm);
        ASSERT_EQ(ilNum(offset), File::getPrm<llint>(0U, Meta::il, &prm));
        ASSERT_EQ(xlNum(offset), File::getPrm<llint>(0U, Meta::xl, &prm));

        if (sizeof(geom_t) == sizeof(double))
        {
            ASSERT_DOUBLE_EQ(xNum(offset), File::getPrm<geom_t>(0U, Meta::xSrc, &prm));
            ASSERT_DOUBLE_EQ(yNum(offset), File::getPrm<geom_t>(0U, Meta::ySrc, &prm));
        }
        else
        {
            ASSERT_FLOAT_EQ(xNum(offset), File::getPrm<geom_t>(0U, Meta::xSrc, &prm));
            ASSERT_FLOAT_EQ(yNum(offset), File::getPrm<geom_t>(0U, Meta::ySrc, &prm));
        }
    }

    void initReadTrHdrsMock(size_t ns, size_t tn)
    {
        size_t zero = 0U;
        EXPECT_CALL(*rmock.get(), readDOMD(zero, ns, tn, _))
                    .Times(Exactly(1))
                    .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl, Meta::Copy, Meta::xSrc, Meta::ySrc});
        File::Param prm(rule, tn);
        rfile->readParam(0, tn, &prm);

        for (size_t i = 0; i < tn; i++)
        {
            ASSERT_EQ(ilNum(i), File::getPrm<llint>(i, Meta::il, &prm));
            ASSERT_EQ(xlNum(i), File::getPrm<llint>(i, Meta::xl, &prm));

            if (sizeof(geom_t) == sizeof(double))
            {
                ASSERT_DOUBLE_EQ(xNum(i), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                ASSERT_DOUBLE_EQ(yNum(i), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
            }
            else
            {
                ASSERT_FLOAT_EQ(xNum(i), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                ASSERT_FLOAT_EQ(yNum(i), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
            }
        }
        ASSERT_TRUE(tr.size());
        ASSERT_THAT(prm.c, ContainerEq(tr));
    }

    void initRandReadTrHdrsMock(size_t ns, size_t tn)
    {
        File::Param prm(tn);
        std::vector<size_t> offset(tn);
        std::iota(offset.begin(), offset.end(), 0);

        std::random_device rand;
        std::mt19937 mt(rand());
        std::shuffle(offset.begin(), offset.end(), mt);

        EXPECT_CALL(*rmock.get(), readDOMD(A<csize_t *>(), ns, tn, _))
                .Times(Exactly(1))
                .WillRepeatedly(SetArrayArgument<3>(tr.begin(), tr.end()));

        rfile->readTraceNonMono(tn, offset.data(), const_cast<trace_t *>(File::TRACE_NULL), &prm);

        for (size_t i = 0; i < tn; i++)
        {
            ASSERT_EQ(ilNum(offset[i]), File::getPrm<llint>(i, Meta::il, &prm));
            ASSERT_EQ(xlNum(offset[i]), File::getPrm<llint>(i, Meta::xl, &prm));

            if (sizeof(geom_t) == sizeof(double))
            {
                ASSERT_DOUBLE_EQ(xNum(offset[i]), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                ASSERT_DOUBLE_EQ(yNum(offset[i]), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
            }
            else
            {
                ASSERT_FLOAT_EQ(xNum(offset[i]), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                ASSERT_FLOAT_EQ(yNum(offset[i]), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
            }
        }
    }

    template <bool readPrm = false, bool MOCK = true, bool RmRule = false>
    void readTraceTest(csize_t offset, size_t tn)
    {
        size_t tnRead = (offset + tn > nt && nt > offset ? nt - offset : tn);
        std::vector<uchar> buf;
        if (MOCK)
        {
            if (rmock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            if (readPrm)
                buf.resize(tnRead * SEGSz::getDOSz(ns));
            else
                buf.resize(tnRead * SEGSz::getDFSz(ns));
            for (size_t i = 0U; i < tnRead; i++)
            {
                if (readPrm)
                    std::copy(tr.begin() + (offset+i) * SEGSz::getMDSz(), tr.begin() + (offset+i+1) * SEGSz::getMDSz(), buf.begin() + i*SEGSz::getDOSz(ns));
                for (size_t j = 0U; j < ns; j++)
                {
                    float val = offset + i + j;
                    size_t addr = readPrm ? (i*SEGSz::getDOSz(ns)+SEGSz::getMDSz()+j*sizeof(float)) : (i*ns+j)*sizeof(float);
                    getBigEndian(toint(val), &buf[addr]);
                }
            }
            if (readPrm)
                EXPECT_CALL(*rmock, readDO(offset, ns, tnRead, _))
                                .Times(Exactly(1)).WillOnce(SetArrayArgument<3>(buf.begin(), buf.end()));
            else
                EXPECT_CALL(*rmock, readDODF(offset, ns, tnRead, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<3>(buf.begin(), buf.end()));
        }

        std::vector<trace_t> bufnew(tn * ns);
        auto rule = std::make_shared<File::Rule>(true, true);
        if (RmRule)
        {
            rule->rmRule(Meta::xSrc);
            rule->addSEGYFloat(Meta::ShotNum, File::Tr::UpSrc, File::Tr::UpRcv);
            rule->addLong(Meta::Misc1, File::Tr::TORF);
            rule->addShort(Meta::Misc2, File::Tr::ShotNum);
            rule->addShort(Meta::Misc3, File::Tr::ShotScal);
            rule->rmRule(Meta::ShotNum);
            rule->rmRule(Meta::Misc1);
            rule->rmRule(Meta::Misc2);
            rule->rmRule(Meta::Misc3);
            rule->rmRule(Meta::ySrc);
            rule->addSEGYFloat(Meta::xSrc, File::Tr::xSrc, File::Tr::ScaleCoord);
            rule->addSEGYFloat(Meta::ySrc, File::Tr::ySrc, File::Tr::ScaleCoord);
        }
        File::Param prm(rule, tn);
        rfile->readTrace(offset, tn, bufnew.data(), (readPrm ? &prm : const_cast<File::Param *>(File::PARAM_NULL)));
        for (size_t i = 0U; i < tnRead; i++)
        {
            if (readPrm && tnRead && ns)
            {
                ASSERT_EQ(ilNum(i+offset), File::getPrm<llint>(i, Meta::il, &prm)) << "Trace Number " << i << " offset " << offset;
                ASSERT_EQ(xlNum(i+offset), File::getPrm<llint>(i, Meta::xl, &prm)) << "Trace Number " << i << " offset " << offset;

                if (sizeof(geom_t) == sizeof(double))
                {
                    ASSERT_DOUBLE_EQ(xNum(i+offset), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                    ASSERT_DOUBLE_EQ(yNum(i+offset), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
                }
                else
                {
                    ASSERT_FLOAT_EQ(xNum(i+offset), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                    ASSERT_FLOAT_EQ(yNum(i+offset), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
                }
            }
            for (size_t j = 0U; j < ns; j++)
                ASSERT_EQ(bufnew[i*ns + j], float(offset + i + j)) << "Trace Number: " << i << " " << j;
        }
    }

    void readTraceTest(bool readPrm, bool MOCK, csize_t offset, size_t tn)
    {
        if (readPrm && MOCK)
            readTraceTest<true, true>(offset, tn);
        else if (readPrm && !MOCK)
            readTraceTest<true, false>(offset, tn);
        else if (!readPrm && MOCK)
            readTraceTest<false, true>(offset, tn);
        else
            readTraceTest<false, false>(offset, tn);
    }

    template <bool readPrm = false, bool MOCK = true>
    void readRandomTraceTest(size_t tn, const std::vector<size_t> offset)
    {
        ASSERT_EQ(tn, offset.size());
        std::vector<uchar> buf;
        if (MOCK)
        {
            if (rmock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            if (readPrm)
                buf.resize(tn * SEGSz::getDOSz(ns));
            else
                buf.resize(tn * SEGSz::getDFSz(ns));
            for (size_t i = 0U; i < tn; i++)
            {
                if (readPrm && ns && tn)
                    std::copy(tr.begin() + offset[i] * SEGSz::getMDSz(), tr.begin() + (offset[i]+1) * SEGSz::getMDSz(), buf.begin() + i*SEGSz::getDOSz(ns));
                for (size_t j = 0U; j < ns; j++)
                {
                    size_t addr = readPrm ? (i*SEGSz::getDOSz(ns)+SEGSz::getMDSz()+j*sizeof(float)) : (i*ns+j)*sizeof(float);
                    float val = offset[i] + j;
                    getBigEndian(toint(val), &buf[addr]);
                }
            }
            if (readPrm)
                EXPECT_CALL(*rmock, readDO(offset.data(), ns, tn, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<3>(buf.begin(), buf.end()));
            else
                EXPECT_CALL(*rmock, readDODF(offset.data(), ns, tn, _))
                            .Times(Exactly(1)).WillOnce(SetArrayArgument<3>(buf.begin(), buf.end()));
        }

        std::vector<float> bufnew(tn * ns);
        File::Param prm(tn);
        if (readPrm)
            rfile->readTrace(tn, offset.data(), bufnew.data(), &prm);
        else
            rfile->readTrace(tn, offset.data(), bufnew.data());
        for (size_t i = 0U; i < tn; i++)
        {
            if (readPrm && tn && ns)
            {
                ASSERT_EQ(ilNum(offset[i]), File::getPrm<llint>(i, Meta::il, &prm)) << "Trace Number " << i << " offset " << offset[i];
                ASSERT_EQ(xlNum(offset[i]), File::getPrm<llint>(i, Meta::xl, &prm)) << "Trace Number " << i << " offset " << offset[i];

                if (sizeof(geom_t) == sizeof(double))
                {
                    ASSERT_DOUBLE_EQ(xNum(offset[i]), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                    ASSERT_DOUBLE_EQ(yNum(offset[i]), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
                }
                else
                {
                    ASSERT_FLOAT_EQ(xNum(offset[i]), File::getPrm<geom_t>(i, Meta::xSrc, &prm));
                    ASSERT_FLOAT_EQ(yNum(offset[i]), File::getPrm<geom_t>(i, Meta::ySrc, &prm));
                }
            }
            for (size_t j = 0U; j < ns; j++)
                ASSERT_EQ(bufnew[i*ns + j], float(offset[i] + j)) << "Trace Number: " << offset[i] << " " << j;
        }
    }

    void readRandomTraceTest(bool readPrm, bool MOCK, csize_t tn, const std::vector<size_t> offset)
    {
        if (readPrm && MOCK)
            readRandomTraceTest<true, true>(tn, offset);
        else if (readPrm && !MOCK)
            readRandomTraceTest<true, false>(tn, offset);
        else if (!readPrm && MOCK)
            readRandomTraceTest<false, true>(tn, offset);
        else
            readRandomTraceTest<false, false>(tn, offset);
    }
};

template <class T, class R>
struct FileWriteTest : public FileReadTest<R>
{
    std::unique_ptr<T> wfile;
    std::shared_ptr<MockWriteObj> wmock;

    FileWriteTest() : FileReadTest<R>()
    {
        wfile = nullptr;
    }

    ~FileWriteTest()
    {
        wfile.reset();
        if (wmock.get())
            Mock::VerifyAndClearExpectations(&wmock);
    }

    void makeFile(std::string name)
    {
        //name_ = name;
        if (wfile.get() != nullptr)
            wfile.reset();
        this->piol->isErr();
        Data::MPIIO::Opt d;
        auto data = std::make_shared<Data::MPIIO>(this->piol, name, d, FileMode::Test);

        {
            typename T::Obj::Opt o;
            typename T::Opt f;
            auto wobj = std::make_shared<T::Obj>(this->piol, name, o, data);
            wfile = std::make_unique<T>(this->piol, name, f, wobj);
        }

        {
            typename R::Obj::Opt ro;
            typename R::Opt rf;
            auto robj = std::make_shared<R::Obj>(this->piol, name, ro, data);
            this->rfile = std::make_unique<R>(this->piol, name, rf, robj);
        }

        writeHO<false>();
        this->rfile->nt = this->nt;
        this->rfile->ns = this->ns;
        this->rfile->inc = this->inc;
        this->rfile->text = this->testString;
    }

    template <bool callHO = true>
    void makeMock()
    {
        if (!wfile.get())
            wfile.reset();
        if (!wmock.get())
            wmock.reset();
        wmock = std::make_shared<MockWriteObj>(this->piol, notFile, nullptr);
        this->piol->isErr();
        Mock::AllowLeak(wmock.get());

        assert(wmock.get());
        wfile = std::make_unique<T>(this->piol, notFile, wmock);

        assert(wfile->obj);
        if (callHO)
        {
            this->piol->isErr();
            writeHO<true>();
        }
        else
        {
            wfile->nt = this->nt;
            wfile->writeNs(this->ns);
        }
    }

    template <bool MOCK = true>
    void writeHO()
    {
        if (MOCK)
        {
            size_t fsz = SEGSz::getHOSz() + this->nt*SEGSz::getDOSz(this->ns);
            EXPECT_CALL(*wmock, setFileSz(fsz)).Times(Exactly(1));
            std::vector<uchar> & ho = this->ho;
            for (size_t i = 0U; i < std::min(this->testString.size(), SEGSz::getTextSz()); i++)
                ho[i] = this->testString[i];

            ho[NumSample+1] = this->ns & 0xFF;
            ho[NumSample] = this->ns >> 8 & 0xFF;
            ho[Increment+1] = this->inc & 0xFF;
            ho[Increment] = this->inc >> 8 & 0xFF;
            ho[Type+1] = this->format;
            ho[3255U] = 1;
            ho[3500U] = 1;
            ho[3503U] = 1;
            ho[3505U] = 0;

            EXPECT_CALL(*wmock, writeHO(_)).Times(Exactly(1)).WillOnce(check0(ho.data(), SEGSz::getHOSz()));
        }

        wfile->writeNt(this->nt);
        this->piol->isErr();

        wfile->writeNs(this->ns);
        this->piol->isErr();

        wfile->writeInc(geom_t(this->inc*SI::Micro));
        this->piol->isErr();

        wfile->writeText(this->testString);
        this->piol->isErr();
    }

    void writeTrHdrGridTest(size_t offset)
    {
        std::vector<uchar> tr(SEGSz::getMDSz());
        getBigEndian(ilNum(offset), tr.data()+il);
        getBigEndian(xlNum(offset), tr.data()+xl);
        getBigEndian<int16_t>(1, &tr[ScaleCoord]);
        getBigEndian(int32_t(offset), &tr[SeqFNum]);

        EXPECT_CALL(*wmock, writeDOMD(offset, this->ns, 1U, _)).Times(Exactly(1))
                                                        .WillOnce(check3(tr.data(), SEGSz::getMDSz()));

        File::Param prm(1U);
        File::setPrm(0, Meta::il, ilNum(offset), &prm);
        File::setPrm(0, Meta::xl, xlNum(offset), &prm);
        File::setPrm(0, Meta::tn, offset, &prm);
        wfile->writeParam(offset, 1U, &prm);
    }

    void initWriteTrHdrCoord(std::pair<size_t, size_t> item, std::pair<int32_t, int32_t> val,
                             int16_t scal, size_t offset, std::vector<uchar> * tr)
    {
        getBigEndian(scal,              &tr->at(ScaleCoord));
        getBigEndian(val.first,         &tr->at(item.first));
        getBigEndian(val.second,        &tr->at(item.second));
        getBigEndian(int32_t(offset),   &tr->at(SeqFNum));
        EXPECT_CALL(*wmock, writeDOMD(offset, this->ns, 1U, _)).Times(Exactly(1))
                                                        .WillOnce(check3(tr->data(), SEGSz::getMDSz()));
    }

    void initWriteHeaders(size_t filePos, uchar * md) {
        coord_t src = coord_t(xNum(filePos), yNum(filePos));
        coord_t rcv = coord_t(xNum(filePos), yNum(filePos));
        coord_t cmp = coord_t(xNum(filePos), yNum(filePos));
        grid_t line = grid_t(ilNum(filePos), xlNum(filePos));

        int16_t scale = scalComp(1, calcScale(src));
        scale = scalComp(scale, calcScale(rcv));
        scale = scalComp(scale, calcScale(cmp));

        getBigEndian(scale, &md[ScaleCoord]);
        setCoord(File::Coord::Src, src, scale, md);
        setCoord(File::Coord::Rcv, rcv, scale, md);
        setCoord(File::Coord::CMP, cmp, scale, md);

        setGrid(File::Grid::Line, line, md);
        getBigEndian(int32_t(filePos), &md[SeqFNum]);
    }

    template <bool writePrm = false, bool MOCK = true>
    void writeTraceTest(csize_t offset, csize_t tn)
    {
        csize_t ns = this->ns;
        std::vector<uchar> buf;
        if (MOCK)
        {
            EXPECT_CALL(*wmock, writeHO(_)).Times(Exactly(1));
            EXPECT_CALL(*wmock, setFileSz(_)).Times(Exactly(1));
            if (wmock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            buf.resize(tn * (writePrm ? SEGSz::getDOSz(ns) : SEGSz::getDFSz(ns)));

            for (size_t i = 0U; i < tn; i++)
            {
                if(writePrm)
                    initWriteHeaders(offset+i, &buf[i*SEGSz::getDOSz(ns)]);
                for (size_t j = 0U; j < ns; j++)
                {
                    size_t addr = writePrm ? (i*SEGSz::getDOSz(ns)+SEGSz::getMDSz()+j*sizeof(float)) : (i*ns + j)*sizeof(float);
                    float val = offset + i + j;
                    getBigEndian(toint(val), &buf[addr]);
                }
            }
            if (writePrm)
                EXPECT_CALL(*wmock, writeDO(offset, ns, tn, _))
                                .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
            else
                EXPECT_CALL(*wmock, writeDODF(offset, ns, tn, _))
                                .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
        }
        std::vector<float> bufnew(tn * ns);
        if (writePrm)
        {
            File::Param prm(tn);
            for (size_t i = 0U; i < tn; i++)
            {
                File::setPrm(i, Meta::xSrc, xNum(offset+i), &prm);
                File::setPrm(i, Meta::xRcv, xNum(offset+i), &prm);
                File::setPrm(i, Meta::xCmp, xNum(offset+i), &prm);
                File::setPrm(i, Meta::ySrc, yNum(offset+i), &prm);
                File::setPrm(i, Meta::yRcv, yNum(offset+i), &prm);
                File::setPrm(i, Meta::yCmp, yNum(offset+i), &prm);
                File::setPrm(i, Meta::il, ilNum(offset+i), &prm);
                File::setPrm(i, Meta::xl, xlNum(offset+i), &prm);
                File::setPrm(i, Meta::tn, offset+i, &prm);
                for (size_t j = 0U; j < ns; j++)
                    bufnew[i*ns + j] = float(offset + i + j);
            }

            wfile->writeTrace(offset, tn, bufnew.data(), &prm);
        }
        else
        {
            for (size_t i = 0U; i < tn; i++)
                for (size_t j = 0U; j < ns; j++)
                    bufnew[i*ns + j] = float(offset + i + j);
            wfile->writeTrace(offset, tn, bufnew.data());
        }

        if (!MOCK)
        {
            this->rfile->nt = std::max(offset+tn, this->rfile->nt);
//            this->readTraceTest<writePrm, false>(offset, tn);
            this->readTraceTest(writePrm, false, offset, tn);
        }
    }

    template <bool writePrm = false, bool MOCK = true>
    void writeRandomTraceTest(size_t tn, const std::vector<size_t> offset)
    {
        size_t ns = this->ns;
        ASSERT_EQ(tn, offset.size());
        std::vector<uchar> buf;
        if (MOCK)
        {
            EXPECT_CALL(*wmock, writeHO(_)).Times(Exactly(1));
            EXPECT_CALL(*wmock, setFileSz(_)).Times(Exactly(1));
            if (wmock == nullptr)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }
            if (writePrm)
                buf.resize(tn * SEGSz::getDOSz(ns));
            else
                buf.resize(tn * SEGSz::getDFSz(ns));
            for (size_t i = 0U; i < tn; i++)
            {
                if (writePrm)
                    initWriteHeaders(offset[i], &buf[i*SEGSz::getDOSz(ns)]);
                for (size_t j = 0U; j < ns; j++)
                {
                    size_t addr = writePrm ? (i*SEGSz::getDOSz(ns)+SEGSz::getMDSz()+j*sizeof(float)) : (i*ns + j)*sizeof(float);
                    float val = offset[i] + j;
                    getBigEndian(toint(val), &buf[addr]);
                }
            }
            if (writePrm)
                EXPECT_CALL(*wmock, writeDO(offset.data(), ns, tn, _))
                                .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
            else
                EXPECT_CALL(*wmock, writeDODF(offset.data(), ns, tn, _))
                                .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
        }
        File::Param prm(tn);
        std::vector<float> bufnew(tn * ns);
        for (size_t i = 0U; i < tn; i++)
        {
            if (writePrm)
            {
                File::setPrm(i, Meta::xSrc, xNum(offset[i]), &prm);
                File::setPrm(i, Meta::xRcv, xNum(offset[i]), &prm);
                File::setPrm(i, Meta::xCmp, xNum(offset[i]), &prm);
                File::setPrm(i, Meta::ySrc, yNum(offset[i]), &prm);
                File::setPrm(i, Meta::yRcv, yNum(offset[i]), &prm);
                File::setPrm(i, Meta::yCmp, yNum(offset[i]), &prm);
                File::setPrm(i, Meta::il, ilNum(offset[i]), &prm);
                File::setPrm(i, Meta::xl, xlNum(offset[i]), &prm);
                File::setPrm(i, Meta::tn, offset[i], &prm);
            }
            for (size_t j = 0U; j < ns; j++)
                bufnew[i*ns + j] = float(offset[i] + j);
        }

        if (writePrm)
            wfile->writeTrace(tn, offset.data(), bufnew.data(), &prm);
        else
            wfile->writeTrace(tn, offset.data(), bufnew.data());

        if (!MOCK)
        {
            for (size_t i = 0U; i < tn; i++)
                this->rfile->nt = std::max(offset[i], this->rfile->nt);
            this->readRandomTraceTest(writePrm, false, tn, offset);
        }
    }

    template <bool Copy>
    void writeTraceHeaderTest(csize_t offset, csize_t tn)
    {
        const bool MOCK = true;
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
            EXPECT_CALL(*wmock.get(), writeDOMD(offset, this->ns, tn, _))
                        .Times(Exactly(1)).WillOnce(check3(buf.data(), buf.size()));
        }

        if (Copy)
        {
            auto rule = std::make_shared<File::Rule>(std::initializer_list<Meta>{Meta::Copy});
            File::Param prm(rule, tn);
            ASSERT_TRUE(prm.size());
            prm.c = buf;
            wfile->writeParam(offset, prm.size(), &prm);
        }
        else
        {
            File::Param prm(tn);
            for (size_t i = 0; i < tn; i++)
            {
                File::setPrm(i, Meta::xSrc, ilNum(i+1), &prm);
                File::setPrm(i, Meta::xRcv, ilNum(i+2), &prm);
                File::setPrm(i, Meta::xCmp, ilNum(i+3), &prm);
                File::setPrm(i, Meta::il, ilNum(i+4), &prm);
                File::setPrm(i, Meta::ySrc, xlNum(i+5), &prm);
                File::setPrm(i, Meta::yRcv, xlNum(i+6), &prm);
                File::setPrm(i, Meta::yCmp, xlNum(i+7), &prm);
                File::setPrm(i, Meta::xl, xlNum(i+8), &prm);
                File::setPrm(i, Meta::tn, offset + i, &prm);
            }
            wfile->writeParam(offset, prm.size(), &prm);
        }
    }
};

typedef FileReadTest<File::ReadSEGY> FileSEGYRead;
typedef FileReadTest<File::ReadSEGY> FileSEGYIntegRead;
typedef FileWriteTest<File::WriteSEGY, File::ReadSEGY> FileSEGYWrite;
typedef FileWriteTest<File::WriteSEGY, File::ReadSEGY> FileSEGYIntegWrite;

