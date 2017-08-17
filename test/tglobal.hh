#ifndef TGLOBAL_TEST_INCLUDE_GUARD
#define TGLOBAL_TEST_INCLUDE_GUARD

#include <string>
#include <cstdlib>
#include "global.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern const size_t magicNum1;
extern const size_t smallSize;
extern const size_t largeSize;
extern const size_t largeSEGYSize;
extern const size_t largeSeisSize;

extern const size_t smallNt;
extern const size_t smallNs;

extern const std::string notFile;
extern const std::string zeroFile;
extern const std::string smallFile;
extern const std::string largeFile;
extern const std::string plargeFile;    //Large file with a pattern
extern const std::string tempFile;      //File which is deleted after each creation
extern const std::string smallSEGYFile;
extern const std::string largeSEGYFile;
extern const std::string bigTraceSEGYFile;

extern const std::string smallSeisFile;
extern const std::string largeSeisFile;
extern const std::string bigTraceSeisFile;

extern int32_t ilNum(size_t);
extern int32_t xlNum(size_t);
extern geom_t xNum(size_t i);
extern geom_t yNum(size_t i);
extern uchar getPattern(size_t);
extern std::vector<size_t> getRandomVec(size_t nt, int seed);
extern std::vector<size_t> getRandomVec(size_t nt, size_t max, int seed);

constexpr size_t prefix(const size_t pow)
{
    return (pow ? 1024U*prefix(pow-1U) : 1U);
}
/*
MATCHER_P(checkfilemetadata, buf, "")
{
    *result_listener << arg->ns << " " << buf->ns << " " << arg->inc  << " " << buf->inc << " "
                     << arg->nt << " " << buf->nt << " " << buf->text << " "<< arg->text;
    return arg->ns == buf->ns && arg->nt == buf->nt &&
           arg->text == buf->text && arg->inc == buf->inc;
}
*/
ACTION_P2(checkfilemetadata, buf, sz)
{
    ASSERT_EQ(buf->ns, arg0->ns) << "Error with ns\n";
    ASSERT_EQ(buf->nt, arg0->nt) << "Error with nt\n";
    ASSERT_EQ(buf->inc, arg0->inc) << "Error with inc\n";

    size_t sz = strlen(buf->text.c_str());
    ASSERT_EQ(sz, strlen(arg0->text.c_str()));
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf->text[i], arg0->text[i]) << "Loop number " << i << std::endl;
}

ACTION_P2(check0, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg0[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check1, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg1[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check2, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg2[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check3, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg3[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check4, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg4[i]) << "Error with byte: " << i << "\n";
}
#endif
