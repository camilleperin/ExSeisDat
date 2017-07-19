/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2017
 *   \brief
 *//*******************************************************************************************/
#include "json.hpp"
#include "global.hh"
#ifndef PIOLSHARESEIS_INCLUDE_GUARD
#define PIOLSHARESEIS_INCLUDE_GUARD
namespace PIOL { namespace SeisF {
enum class Endian
{
    Little,
    Big,
    Middle
};

enum class Type
{
    i08,
    i16,
    i32,
    i64,
    flt,
    dbl
};

struct SeisTraceHeader
{
    std::string name;
    size_t sz;
    Type type;
};

struct SeisTopHeader
{
    size_t bytes;
    double o1;
    double d1;
    size_t n1;
    Endian endian;
    bool headerFile;
    std::vector<std::string> headers;
    std::vector<std::string> extents;
    size_t packetSz;
};

template <typename T>
void set(T * val, std::string name, nlohmann::json & j)
{
    auto p = j.find(name);
    if (p != j.end())
        *val = p.value();
}

template <typename T>
void set(std::vector<T> & val, std::string name, nlohmann::json & j)
{
    auto p = j.find(name);
    if (p != j.end())
        val = p->get<std::vector<T>>();
}
}}
#endif
