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
#include "share/units.hh"
#include "share/datatype.hh"

#define private public
#define protected public
#include "cppfileapi.hh"
#include "file/filesegy.hh"
#include "file/fileseis.hh"
#include "object/objsegy.hh"
#include "object/objseis.hh"
#include "share/segy.hh"
#include "segymdextra.hh"
#undef private
#undef protected

using namespace testing;
using namespace PIOL;
using PIOL::SEGY::deScale;
/*using File::coord_t;
using File::grid_t;
using File::calcScale;
using File::scalComp;
using File::setCoord;
using File::setGrid;
*/

class MockReadObj : public Obj::ReadInterface
{
    public :
    MockReadObj(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Data::Interface> data_)
               : Obj::ReadInterface(piol_, name_, data_) {}
    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD0(readHO, std::shared_ptr<FileMetadata>(void));
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
               : Obj::WriteInterface(piol_, name_, data_) {}

    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
    MOCK_CONST_METHOD1(writeHO, void(const std::shared_ptr<FileMetadata>));
    MOCK_CONST_METHOD4(writeDOMD, void(csize_t, csize_t, csize_t, const uchar *));

    MOCK_CONST_METHOD4(writeDODF, void(csize_t, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDO, void(csize_t, csize_t, csize_t, const uchar *));

    MOCK_CONST_METHOD4(writeDO, void(csize_t *, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDODF, void(csize_t *, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(writeDOMD, void(csize_t *, csize_t, csize_t, const uchar *));
};

template <class T, class R, bool isMock, bool isRead>
struct FileTest : public Test
{
    std::string name;
    std::shared_ptr<ExSeisPIOL> piol;
    Comm::MPI::Opt opt;
    bool testEBCDIC;
    std::string testString = {"This is a string for testing EBCDIC conversion etc."};
    std::vector<uchar> tr;
    std::vector<uchar> ho;
    size_t nt = 40U;
    size_t ns = 200U;
    int inc = 10;
    csize_t format = 5;

    typedef typename R::DObj RObj;
    typedef typename T::DObj TObj;
    typedef typename TObj::DataT TData;
    typedef typename RObj::DataT RData;

    typename TObj::DataT::Opt d;
    typename T::Opt wopt;
    typename R::Opt ropt;

    std::unique_ptr<T> wfile;
    std::unique_ptr<R> rfile;
    std::shared_ptr<MockReadObj> rmock;
    std::shared_ptr<MockWriteObj> wmock;

    std::shared_ptr<TData> dat;
    std::shared_ptr<Obj::SEGYFileHeader> fo;

    bool initTr;

    FileTest()
    {
        initTr = false;
        testEBCDIC = false;
        rfile = nullptr;
        wfile = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        wfile = nullptr;
        ho.resize(SEGSz::getHOSz());
    }

    ~FileTest()
    {
        if (rmock.get())
            Mock::VerifyAndClearExpectations(&rmock);
        if (wmock.get())
            Mock::VerifyAndClearExpectations(&wmock);
    }

    template <bool OPTS = false>
    void openFile(std::string name)
    {
        ASSERT_FALSE(isMock);
        ASSERT_TRUE(isRead);
/*        if (OPTS)
        {
            Data::MPIIO::Opt dopt;
            auto dat = std::make_shared<T::
            rfile = std::make_unique<T>(piol, name, ropt, ropt, dopt);
        }
        else*/
            rfile.reset(new R(piol, name));
        assert(rfile->obj);
        piol->isErr();
    }

    void createFile(std::string name_)
    {
        ASSERT_FALSE(isMock);
        ASSERT_FALSE(isRead);

        name = name_;
        if (wfile.get() != nullptr)
            wfile.reset();
        piol->isErr();

        dat = std::make_shared<TData>(piol, name, d, FileMode::Test);

        auto wobj = std::make_shared<TObj>(piol, name, &wopt, dat);
        wfile = std::make_unique<T>(piol, name, &wopt, wobj);

        auto robj = std::make_shared<RObj>(piol, name, &ropt, dat);
        rfile = std::make_unique<R>(piol, name, &ropt, robj);

        writeHO();
        rfile->nt = nt;
        rfile->ns = ns;
        rfile->inc = inc;
        rfile->text = testString;
    }

    template <bool callHO = true>
    void makeMock()
    {
        ASSERT_TRUE(isMock);
        if (isRead)
        {
            rmock.reset(new MockReadObj(piol, notFile, nullptr));
            piol->isErr();
            Mock::AllowLeak(rmock.get());

            //EXPECT_CALL(*rmock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() +
            //                                                               nt*SEGSz::getDOSz(ns)));

            auto desc = std::make_shared<FileMetadata>();
            desc->nt = nt;
            desc->ns = ns;
            desc->inc = inc * geom_t(SI::Micro);
            desc->text = testString;

            EXPECT_CALL(*rmock, readHO()).Times(Exactly(1)).WillOnce(Return(desc));

            rfile.reset(new R(piol, notFile, rmock));
        }
        else
        {
            wmock.reset( new MockWriteObj(piol, notFile, nullptr));
            piol->isErr();
            Mock::AllowLeak(wmock.get());

            assert(wmock.get());
            wfile.reset(new T(piol, notFile, wmock));

            assert(wfile->obj);
            if (callHO)
            {
                piol->isErr();
                writeHO();
            }
            else
            {
                wfile->nt = nt;
                wfile->writeNs(ns);
            }
        }
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
            int16_t scal1 = SEGY::deScale(xNum(i));
            int16_t scal2 = SEGY::deScale(yNum(i));

            if (scal1 > 1 || scal2 > 1)
                scale = std::max(scal1, scal2);
            else
                scale = std::min(scal1, scal2);

            getBigEndian(scale, &md[ScaleCoord]);
            getBigEndian(int32_t(std::lround(xNum(i)/scale)), &md[xSrc]);
            getBigEndian(int32_t(std::lround(yNum(i)/scale)), &md[ySrc]);
        }
        initTr = true;
    }

    void initReadTrMock(size_t ns, size_t offset)
    {
        if (!initTr)
            initTrBlock();
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
        if (!initTr)
            initTrBlock();

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
        if (!initTr)
            initTrBlock();

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

    template <bool readPrm = false, bool RmRule = false>
    void readTraceTest(csize_t offset, size_t tn)
    {
        if (!initTr)
            initTrBlock();

        size_t tnRead = (offset + tn > nt && nt > offset ? nt - offset : tn);
        std::vector<uchar> buf;
        if (isMock)
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

    template <bool readPrm = false>
    void readRandomTraceTest(size_t tn, const std::vector<size_t> offset)
    {
        if (!initTr)
            initTrBlock();

        ASSERT_EQ(tn, offset.size());
        std::vector<uchar> buf;
        if (isMock)
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

    void writeHO()
    {
        ASSERT_FALSE(isRead);
        if (isMock)
        {
            size_t fsz = SEGSz::getHOSz() + nt*SEGSz::getDOSz(ns);
            EXPECT_CALL(*wmock, setFileSz(fsz)).Times(Exactly(1));
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

            fo = std::make_shared<Obj::SEGYFileHeader>();
            fo->incFactor = SI::Micro;
            fo->inc = inc * geom_t(fo->incFactor);
            fo->ns = ns;
            fo->nt = nt;
            fo->text = testString;
            EXPECT_CALL(*wmock, writeHO(_)).WillOnce(checkfilemetadata(fo, 5));
        }

        wfile->writeNt(nt);
        piol->isErr();

        wfile->writeNs(ns);
        piol->isErr();

        wfile->writeInc(geom_t(inc)*SI::Micro);
        piol->isErr();

        wfile->writeText(testString);
        piol->isErr();
    }

    void writeTrHdrGridTest(size_t offset)
    {
        ASSERT_FALSE(isRead);
        std::vector<uchar> tr(SEGSz::getMDSz());
        getBigEndian(ilNum(offset), tr.data()+il);
        getBigEndian(xlNum(offset), tr.data()+xl);
        getBigEndian<int16_t>(1, &tr[ScaleCoord]);
        getBigEndian(int32_t(offset), &tr[SeqFNum]);

        EXPECT_CALL(*wmock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1))
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
        ASSERT_TRUE(isMock);
        getBigEndian(scal,              &tr->at(ScaleCoord));
        getBigEndian(val.first,         &tr->at(item.first));
        getBigEndian(val.second,        &tr->at(item.second));
        getBigEndian(int32_t(offset),   &tr->at(SeqFNum));
        EXPECT_CALL(*wmock, writeDOMD(offset, ns, 1U, _)).Times(Exactly(1))
                                                        .WillOnce(check3(tr->data(), SEGSz::getMDSz()));
    }

    void initWriteHeaders(size_t filePos, uchar * md)
    {
        ASSERT_TRUE(isMock);
        ASSERT_FALSE(isRead);
        coord_t src = coord_t(xNum(filePos), yNum(filePos));
        coord_t rcv = coord_t(xNum(filePos), yNum(filePos));
        coord_t cmp = coord_t(xNum(filePos), yNum(filePos));
        grid_t line = grid_t(ilNum(filePos), xlNum(filePos));

        int16_t scale = scalComp(1, calcScale(src));
        scale = scalComp(scale, calcScale(rcv));
        scale = scalComp(scale, calcScale(cmp));

        getBigEndian(scale, &md[ScaleCoord]);
        setCoord(Coord::Src, src, scale, md);
        setCoord(Coord::Rcv, rcv, scale, md);
        setCoord(Coord::CMP, cmp, scale, md);

        setGrid(Grid::Line, line, md);
        getBigEndian(int32_t(filePos), &md[SeqFNum]);
    }

    template <bool writePrm = false>
    void writeTraceTest(csize_t offset, csize_t tn)
    {
        ASSERT_FALSE(isRead);
        std::vector<uchar> buf;
        if (isMock)
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

        if (!isMock)
        {
            wfile->deinit();
            std::static_pointer_cast<RObj>(rfile->obj)->Init(&ropt);
            rfile->init(&ropt);
            readTraceTest<writePrm>(offset, tn);
        }
    }

    template <bool writePrm = false>
    void writeRandomTraceTest(size_t tn, const std::vector<size_t> offset)
    {
        ASSERT_FALSE(isRead);
        ASSERT_EQ(tn, offset.size());
        std::vector<uchar> buf;
        if (isMock)
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

        if (!isMock)
        {
            wfile->deinit();
            std::static_pointer_cast<RObj>(rfile->obj)->Init(&ropt);
            rfile->init(&ropt);

            readRandomTraceTest<writePrm>(tn, offset);
        }
    }

    template <bool Copy>
    void writeTraceHeaderTest(csize_t offset, csize_t tn)
    {
        ASSERT_FALSE(isRead);
        std::vector<uchar> buf;
        if (isMock)
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
                setCoord(Coord::Src, src, scale, md);
                setCoord(Coord::Rcv, rcv, scale, md);
                setCoord(Coord::CMP, cmp, scale, md);

                setGrid(Grid::Line, line, md);
                getBigEndian(int32_t(offset + i), &md[SeqFNum]);
            }
            EXPECT_CALL(*wmock.get(), writeDOMD(offset, ns, tn, _))
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

/*
template <class T, class R>
struct FileWriteTest : public FileReadTest<R>
{
    typedef typename R::DObj RObj;
    typedef typename T::DObj TObj;
    typedef typename TObj::Data TData;
    typedef typename RObj::Data RData;

    typename TObj::Data::Opt d;
    typename T::Opt wf;
    typename R::Opt rf;

    std::unique_ptr<T> wfile;
    std::shared_ptr<MockWriteObj> wmock;
    std::vector<uchar> ho;

    std::shared_ptr<TData> dat;
    std::string name;
    std::shared_ptr<Obj::SEGYFileHeader> fo;

    FileWriteTest() : FileReadTest<R>()
    {
        wfile = nullptr;
        ho.resize(SEGSz::getHOSz());
    }

    ~FileWriteTest()
    {
        wfile.reset();
        if (wmock.get())
            Mock::VerifyAndClearExpectations(&wmock);
    }
};*/

typedef FileTest<File::WriteSEGY, File::ReadSEGY, true, true> FileSEGYRead;
typedef FileTest<File::WriteSEGY, File::ReadSEGY, false, true> FileSEGYIntegRead;
typedef FileTest<File::WriteSEGY, File::ReadSEGY, true, false> FileSEGYWrite;
typedef FileTest<File::WriteSEGY, File::ReadSEGY, false, false> FileSEGYIntegWrite;

typedef FileTest<File::WriteSeis, File::ReadSeis, true, true> FileSeisRead;
typedef FileTest<File::WriteSeis, File::ReadSeis, false, true> FileSeisIntegRead;
typedef FileTest<File::WriteSeis, File::ReadSeis, true, false> FileSeisWrite;
typedef FileTest<File::WriteSeis, File::ReadSeis, false, false> FileSeisIntegWrite;

