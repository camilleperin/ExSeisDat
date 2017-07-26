#include <memory>
#include <string>
#include <iconv.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "global.hh"
#include "tglobal.hh"
#include "share/segy.hh"
#include "share/datatype.hh"
#include "anc/mpi.hh"
#include "data/datampiio.hh"
#include "file/iconv.hh"
#define private public
#define protected public
#include "object/object.hh"
#include "object/objsegy.hh"
#undef private
#undef protected
#include "segymdextra.hh"

using namespace testing;
using namespace PIOL;

class MockData : public Data::Interface
{
    public :
    MockData(const Piol piol_, const std::string name_) : Data::Interface(piol_, name_)
    {
    }

    MOCK_CONST_METHOD0(getFileSz, size_t(void));
    MOCK_CONST_METHOD3(read, void(csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD5(read, void(csize_t, csize_t, csize_t, csize_t, uchar *));
    MOCK_CONST_METHOD4(read, void(csize_t, csize_t, csize_t *, uchar *));
    MOCK_CONST_METHOD3(write, void(csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD5(write, void(csize_t, csize_t, csize_t, csize_t, const uchar *));
    MOCK_CONST_METHOD4(write, void(csize_t, csize_t, csize_t *, const uchar *));
    // TODO: This method is not tested
    MOCK_CONST_METHOD1(setFileSz, void(csize_t));
};

enum class Block
{
    DOMD,
    DODF,
    DO
};

class ObjTest : public Test
{
    protected :
    Piol piol;
    bool testEBCDIC;
    Comm::MPI::Opt opt;
    std::string testString = {"This is a string for testing EBCDIC conversion etc."};
    std::shared_ptr<MockData> mock;
    std::unique_ptr<Obj::ReadInterface> obj;
    std::vector<uchar> ho;
    size_t nt;
    size_t ns;
    int inc;
    int format;

    ObjTest()
    {
        testEBCDIC = false;
        mock = nullptr;
        obj = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
        ho.resize(SEGSz::getHOSz());
    }

    template <bool Check = true>
    void makeReadRealSEGY(std::string name, std::string text = "", size_t nt_ = 0, size_t ns_ = 0, int inc_ = 0, int format_ = 0)
    {
        auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
        piol->isErr();
        obj.reset(new Obj::ReadSEGY(piol, name, data));
        piol->isErr();
        auto desc = obj->readHO();
        ns = desc->ns;
        nt = desc->nt;
        geom_t ginc = desc->inc;
        testString = desc->text;
        if (Check)
        {
            EXPECT_EQ(ns, ns_);
            EXPECT_EQ(nt, nt_);
            EXPECT_DOUBLE_EQ(ginc, inc_ * geom_t(1e-6));
            EXPECT_EQ(testString, text);
        }
    }

    virtual void makeSEGY(size_t nt_, size_t ns_, int inc_ = 10, int format_ = 5)
    {
        nt = nt_;
        ns = ns_;
        inc = geom_t(inc_) * 1e-6;
        format = format_;

        mock = std::make_shared<MockData>(piol, notFile);

        piol->isErr();
        Mock::AllowLeak(mock.get());

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
        ho[Increment] = inc_ >> 8 & 0xFF;
        ho[Increment+1] = inc_ & 0xFF;
        ho[Type+1] = format;

        EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(SEGSz::getHOSz() +
                                                                       nt*SEGSz::getDOSz(ns)));
        EXPECT_CALL(*mock, read(0LU, SEGSz::getHOSz(), _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(ho.begin(), ho.end()));
        piol->isErr();
        obj.reset(new Obj::ReadSEGY(piol, notFile, mock));
        piol->isErr();

        auto desc = obj->readHO();
        EXPECT_EQ(ns, desc->ns);
        EXPECT_EQ(nt, desc->nt);
        EXPECT_DOUBLE_EQ(inc, desc->inc);
        EXPECT_EQ(testString, desc->text);
    }

    ~ObjTest()
    {
    }

    void SEGYFileSizeTest(size_t sz)
    {
        EXPECT_CALL(*mock, getFileSz()).WillOnce(Return(sz));
        piol->isErr();
        EXPECT_EQ(sz, obj->getFileSz());
    }

    template <Block Type, bool MOCK = true>
    void readTest(csize_t offset, csize_t tn, csize_t poff = 0, uchar magic = 0)
    {
        SCOPED_TRACE("readTest " + std::to_string(size_t(Type)));
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);
        size_t step = tn * bsz;
        std::vector<uchar> trnew(step + 2U*extra);

        std::vector<uchar> tr;
        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tn; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, read(locFunc(offset, ns), tn*bsz, _))
                        .WillOnce(SetArrayArgument<2>(tr.begin(), tr.end()));
            else
                EXPECT_CALL(*mock, read(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), tn, _))
                        .WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset, ns, tn, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset, ns, tn, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset, ns, tn, &trnew[extra]);
            break;
        }
        piol->isErr();

        size_t tcnt = 0;
        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < bsz; j++, tcnt++)
            {
                size_t pos = poff + locFunc(offset + i, ns) + j;
                ASSERT_EQ(trnew[extra+i*bsz+j], getPattern(pos % 0x100)) << i << " " << j;
            }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U)
        {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size()-extra+i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U*extra);
    }

    template <Block Type, bool MOCK = true>
    void readRandomTest(const std::vector<size_t> & offset, uchar magic = 0)
    {
        SCOPED_TRACE("readRandomTest " + std::to_string(size_t(Type)));
        size_t tn = offset.size();
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = tn * bsz;
        std::vector<uchar> trnew(step + 2U*extra);
        std::vector<uchar> tr;

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tn; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }

            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, read(bsz, tn, _, _))
                               .WillOnce(SetArrayArgument<3>(tr.begin(), tr.end()))
                               .RetiresOnSaturation();

        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset.data(), ns, tn, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset.data(), ns, tn, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset.data(), ns, tn, &trnew[extra]);
            break;
        }

        size_t tcnt = 0;
        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < bsz; j++, tcnt++)
            {
                size_t pos = locFunc(offset[i], ns) + j;
                ASSERT_EQ(trnew[extra+i*bsz+j], getPattern(pos % 0x100)) << i << " " << j;
            }
        for (size_t i = 0U; i < extra; i++, tcnt += 2U)
        {
            ASSERT_EQ(trnew[i], magic);
            ASSERT_EQ(trnew[trnew.size()-extra+i], magic);
        }
        ASSERT_EQ(tcnt, step + 2U*extra);
    }
};

typedef ObjTest ReadObjTest;

class WriteObjTest : public ReadObjTest
{
    protected :
    Piol piol;
    Comm::MPI::Opt opt;
    std::shared_ptr<MockData> mock;
    Obj::WriteInterface * wobj;

    WriteObjTest()
    {
        mock = nullptr;
        wobj = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }

    void makeRealSEGY(std::string name)
    {
        if (wobj != nullptr)
            delete wobj;

        auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Test);
        piol->isErr();
        wobj = new Obj::WriteSEGY(piol, name, data);
        piol->isErr();
        makeReadRealSEGY(name);
    }

    void makeSEGY(size_t nt_, size_t ns_, int inc_ = 10, int format_ = 5)
    {
        if (wobj != nullptr)
            delete wobj;

        mock = std::make_shared<MockData>(piol, notFile);
        piol->isErr();
        wobj = new Obj::WriteSEGY(piol, notFile, mock);
        piol->isErr();
    }

    ~WriteObjTest()
    {
        if (wobj != nullptr)
            delete wobj;
    }

    template <bool MOCK = true>
    void writeHOPattern(size_t off, uchar magic)
    {
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }

        if (MOCK)
        {
            size_t fsz = SEGSz::getHOSz() + this->nt*SEGSz::getDOSz(this->ns);
            EXPECT_CALL(*mock, setFileSz(fsz)).Times(Exactly(1));
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

            EXPECT_CALL(*mock, write(0LU, SEGSz::getHOSz(), _)).Times(Exactly(1)).WillOnce(check2(ho.data(), SEGSz::getHOSz()));
        }

#warning Needs to be generalised for Seis
        auto fo = std::make_shared<Obj::SEGYFileHeader>();
        fo->inc = this->inc;
        fo->incFactor = SI::Micro;
        fo->ns = this->ns;
        fo->nt = this->nt;
        fo->text = this->testString;
        wobj->writeHO(fo);

#warning FIX
//        if (!MOCK)
//            readHOPatternTest<MOCK>(off, magic);
    }

    template <Block Type, bool MOCK = true>
    void writeTest(csize_t offset, csize_t tn, csize_t ns, csize_t poff = 0, uchar magic = 0)
    {
        SCOPED_TRACE("writeTest " + std::to_string(size_t(Type)));

        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = tn * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tn; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, write(locFunc(offset, ns), tn*bsz, _))
                        .WillOnce(check2(tr, tr.size()));
            else
                EXPECT_CALL(*mock, write(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), tn, _))
                        .WillOnce(check4(tr, tr.size()));
        }
        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < bsz; j++)
            {
                size_t pos = poff + locFunc(offset + i, ns) + j;
                trnew[extra + i*bsz+j] = getPattern(pos % 0x100);
            }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                wobj->writeDODF(offset, ns, tn, &trnew[extra]);
            break;
            case Block::DOMD :
                wobj->writeDOMD(offset, ns, tn, &trnew[extra]);
            break;
            default :
            case Block::DO :
                wobj->writeDO(offset, ns, tn, &trnew[extra]);
            break;
        }

        if (!MOCK)
        {
            this->ns = ns;
            readTest<Type, MOCK>(offset, tn, poff, magic);
        }
    }

    template <Block Type, bool MOCK = true>
    void writeRandomTest(csize_t ns, const std::vector<size_t> & offset, uchar magic = 0)
    {
        SCOPED_TRACE("writeRandomTest " + std::to_string(size_t(Type)));
        size_t tn = offset.size();
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);
        size_t step = tn * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < tn; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, write(bsz, tn, _, _))
                               .WillOnce(check3(tr.data(), step))
                               .RetiresOnSaturation();
        }

        for (size_t i = 0U; i < tn; i++)
            for (size_t j = 0U; j < bsz; j++)
            {
                size_t pos = locFunc(offset[i], ns) + j;
                trnew[extra + i*bsz+j] = getPattern(pos % 0x100);
            }
        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[i+trnew.size()-extra] = magic;

        switch (Type)
        {
            case Block::DODF :
                wobj->writeDODF(offset.data(), ns, tn, &trnew[extra]);
            break;
            case Block::DOMD :
                wobj->writeDOMD(offset.data(), ns, tn, &trnew[extra]);
            break;
            default :
            case Block::DO :
                wobj->writeDO(offset.data(), ns, tn, &trnew[extra]);
            break;
        }
        if (!MOCK)
        {
            this->ns = ns;
            readRandomTest<Type, MOCK>(offset, magic);
        }
    }
};

typedef ReadObjTest ReadObjSpecTest;
typedef WriteObjTest WriteObjSpecTest;
typedef ReadObjTest ReadObjIntegTest;
typedef WriteObjTest WriteObjIntegTest;

