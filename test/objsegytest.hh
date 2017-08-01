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
    void makeReadRealSEGY(std::string name, size_t nt_ = 0, size_t ns_ = 0, int inc_ = 0, int format_ = 0)
    {
        auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
        piol->isErr();
        obj.reset(new Obj::ReadSEGY(piol, name, data));
        piol->isErr();
        ns = ns_;
        nt = nt_;
        inc = inc_;
        format = format_;
        if (Check)
            readHOPatternTest();
    }

    void readHOPatternTest(std::string text = "")
    {
        auto desc = obj->readHO();
        EXPECT_EQ(desc->ns, ns);
        EXPECT_EQ(desc->nt, nt);
        EXPECT_DOUBLE_EQ(desc->inc, geom_t(inc) * 1e-6);

        char len = strlen(desc->text.c_str());

        for (size_t i = 0; i < len; i++)
            EXPECT_EQ(text[i], desc->text[i]);

    }


    virtual void makeSEGY(size_t nt_, size_t ns_, int inc_ = 10, int format_ = 5)
    {
        nt = nt_;
        ns = ns_;
        inc = inc_;
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

        ho[NumSample+1] = ns & 0xFF;
        ho[NumSample] = ns >> 8 & 0xFF;
        ho[Increment+1] = inc_ & 0xFF;
        ho[Increment] = inc_ >> 8 & 0xFF;
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
        EXPECT_DOUBLE_EQ(geom_t(inc) * SI::Micro, desc->inc);
        for (size_t i = 0; i < SEGSz::getTextSz(); i++)
            EXPECT_EQ(testString[i % testString.size()], desc->text[i]);
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
    void readTest(csize_t offset, csize_t sz, csize_t poff = 0, uchar magic = 0)
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
        size_t step = sz * bsz;
        std::vector<uchar> trnew(step + 2U*extra);

        std::vector<uchar> tr;
        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < sz; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, read(locFunc(offset, ns), sz*bsz, _))
                        .WillOnce(SetArrayArgument<2>(tr.begin(), tr.end()));
            else
                EXPECT_CALL(*mock, read(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), sz, _))
                        .WillOnce(SetArrayArgument<4>(tr.begin(), tr.end()));
        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset, ns, sz, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset, ns, sz, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset, ns, sz, &trnew[extra]);
            break;
        }
        piol->isErr();

        size_t tcnt = 0;
        for (size_t i = 0U; i < sz; i++)
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
        size_t sz = offset.size();
        if (MOCK && mock == nullptr)
        {
            std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
            return;
        }
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = sz * bsz;
        std::vector<uchar> trnew(step + 2U*extra);
        std::vector<uchar> tr;

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < sz; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }

            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, read(bsz, sz, _, _))
                               .WillOnce(SetArrayArgument<3>(tr.begin(), tr.end()))
                               .RetiresOnSaturation();

        }

        for (size_t i = 0U; i < extra; i++)
            trnew[i] = trnew[trnew.size()-extra+i] = magic;

        switch (Type)
        {
            case Block::DODF :
                obj->readDODF(offset.data(), ns, sz, &trnew[extra]);
            break;
            case Block::DOMD :
                obj->readDOMD(offset.data(), ns, sz, &trnew[extra]);
            break;
            default :
            case Block::DO :
                obj->readDO(offset.data(), ns, sz, &trnew[extra]);
            break;
        }

        size_t tcnt = 0;
        for (size_t i = 0U; i < sz; i++)
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
    std::shared_ptr<Obj::WriteInterface> wobj;
    std::shared_ptr<Data::MPIIO> data;
    WriteObjTest()
    {
        mock = nullptr;
        opt.initMPI = false;
        piol = std::make_shared<ExSeisPIOL>(opt);
    }

    void makeRealSEGY(std::string name, size_t ns_ = 200, int inc_ = 10, int format_ = 5)
    {
        data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Test);
        piol->isErr();
        wobj.reset(new Obj::WriteSEGY(piol, name, data));
        piol->isErr();
        obj.reset(new Obj::ReadSEGY(piol, name, data));
        piol->isErr();

        nt = 0;
        ns = ns_;
        inc = inc_;
        format = format_;
    }

    void makeSEGY(size_t nt_, size_t ns_, int inc_ = 10, int format_ = 5)
    {
        nt = nt_;
        ns = ns_;
        inc = inc_;
        format = format_;
        mock = std::make_shared<MockData>(piol, notFile);
        piol->isErr();
        wobj.reset(new Obj::WriteSEGY(piol, notFile, mock));
        piol->isErr();
    }

    ~WriteObjTest()
    {
    }

    template <bool MOCK = true>
    void writeHOPattern(void)
    {
        if (MOCK)
        {
            if (!mock)
            {
                std::cerr << "Using Mock when not initialised: LOC: " << __LINE__ << std::endl;
                return;
            }

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
            EXPECT_CALL(*mock, write(0LU, SEGSz::getHOSz(), _)).Times(Exactly(1)).WillOnce(check2(ho.data(), SEGSz::getHOSz()));
        }

#warning Needs to be generalised for Seis
        auto fo = std::make_shared<Obj::SEGYFileHeader>();
        fo->incFactor = SI::Micro;
        fo->inc = geom_t(inc) * SI::Micro;
        fo->ns = ns;
        fo->nt = nt;
        fo->text = testString;
        wobj->writeHO(fo);

        if (!MOCK)
        {
            obj.reset(new Obj::ReadSEGY(piol, "", data));
            piol->isErr();

            readHOPatternTest(testString);
        }
    }

    template <Block Type, bool MOCK = true>
    void writeTest(csize_t offset, csize_t sz, csize_t poff = 0, uchar magic = 0)
    {
        SCOPED_TRACE("writeTest " + std::to_string(size_t(Type)));

        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);

        size_t step = sz * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < sz; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = poff + locFunc(offset + i, ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type == Block::DO)
                EXPECT_CALL(*mock, write(locFunc(offset, ns), sz*bsz, _))
                        .WillOnce(check2(tr, tr.size()));
            else
                EXPECT_CALL(*mock, write(locFunc(offset, ns), bsz, SEGSz::getDOSz(ns), sz, _))
                        .WillOnce(check4(tr, tr.size()));
        }
        for (size_t i = 0U; i < sz; i++)
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
                wobj->writeDODF(offset, ns, sz, &trnew[extra]);
            break;
            case Block::DOMD :
                wobj->writeDOMD(offset, ns, sz, &trnew[extra]);
            break;
            default :
            case Block::DO :
                wobj->writeDO(offset, ns, sz, &trnew[extra]);
            break;
        }

        if (!MOCK)
        {
            nt = offset + sz;
            obj.reset(new Obj::ReadSEGY(piol, "", data));
            piol->isErr();

            readTest<Type, MOCK>(offset, sz, poff, magic);
        }
    }

    template <Block Type, bool MOCK = true>
    void writeRandomTest(const std::vector<size_t> & offset, uchar magic = 0)
    {
        SCOPED_TRACE("writeRandomTest " + std::to_string(size_t(Type)));
        size_t sz = offset.size();
        const size_t extra = 20U;
        size_t bsz = (Type == Block::DOMD ? SEGSz::getMDSz() : (Type == Block::DODF ? SEGSz::getDFSz(ns) : SEGSz::getDOSz(ns)));
        auto locFunc = (Type != Block::DODF ? SEGSz::getDOLoc<float> : SEGSz::getDODFLoc<float>);
        size_t step = sz * bsz;
        std::vector<uchar> tr;
        std::vector<uchar> trnew(step + 2U*extra);

        if (MOCK)
        {
            tr.resize(step);
            for (size_t i = 0U; i < sz; i++)
                for (size_t j = 0U; j < bsz; j++)
                {
                    size_t pos = locFunc(offset[i], ns) + j;
                    tr[i*bsz+j] = getPattern(pos % 0x100);
                }
            if (Type != Block::DODF || bsz > 0)
                EXPECT_CALL(*mock, write(bsz, sz, _, _))
                               .WillOnce(check3(tr.data(), step))
                               .RetiresOnSaturation();
        }

        for (size_t i = 0U; i < sz; i++)
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
                wobj->writeDODF(offset.data(), ns, sz, &trnew[extra]);
            break;
            case Block::DOMD :
                wobj->writeDOMD(offset.data(), ns, sz, &trnew[extra]);
            break;
            default :
            case Block::DO :
                wobj->writeDO(offset.data(), ns, sz, &trnew[extra]);
            break;
        }
        if (!MOCK)
        {
            nt = (offset.size() ? *std::max_element(offset.begin(), offset.end()) + 1LU : 0LU);
            obj.reset(new Obj::ReadSEGY(piol, "", data));
            piol->isErr();

            readRandomTest<Type, MOCK>(offset, magic);
        }
    }
};

typedef ReadObjTest ReadObjSpecTest;
typedef WriteObjTest WriteObjSpecTest;
typedef ReadObjTest ReadObjIntegTest;
typedef WriteObjTest WriteObjIntegTest;

