#include "global.hh"
#include <algorithm>
#include <iterator>
#include <functional>
#include "ops/ops.hh"
#include "file/file.hh"

namespace PIOL { namespace File {

template <typename T>
using Func = std::function<geom_t(const T &)>;

template <typename T>
std::vector<CoordElem> getCoordMinMax(Piol piol, size_t offset, size_t sz, const T * coord,
                                      Func<T> elem1, Func<T> elem2)
{
    auto min = [elem1, elem2] (const T & a, const T & b) -> bool
    {
        if (elem1(a) < elem1(b))
            return true;
        if (elem1(a) == elem1(b) && elem2(a) < elem2(b))
            return true;
        return false;
    };
    auto p = std::minmax_element(coord, coord + sz, min);

    std::vector<geom_t> lminmax = { elem1(*p.first), elem1(*p.second) };
    std::vector<size_t> ltrace = {offset + std::distance(coord, p.first),
                                  offset + std::distance(coord, p.second)};
    auto tminmax = piol->comm->gather(lminmax);
    auto ttrace = piol->comm->gather(ltrace);
    //Global
    auto s = std::minmax_element(tminmax.begin(), tminmax.end());

    std::vector<CoordElem> minmax = {{*s.first, ttrace[std::distance(tminmax.begin(), s.first)]},
                                     {*s.second, ttrace[std::distance(tminmax.begin(), s.second)]}};
    return minmax;
}

template <typename T>
void getMinMax(Piol piol, size_t offset, size_t sz, const T * coord, Func<T> xlam, Func<T> ylam, CoordElem * minmax)
{
    //TODO: Create and use a subcommunicator to avoid the deadlock the next conditions will cause
    if (!sz || !coord)
    {
        piol->record("", Log::Layer::Ops, Log::Status::Warning,
                     "getCoordMinMax() was called but a process (" + std::to_string(piol->comm->getRank()) +") has no traces.",
                     Log::Verb::Extended);
        return;
    }
    if (!minmax)
    {
        piol->record("", Log::Layer::Ops, Log::Status::Warning,
                     "getCoordMinMax() was called but a process (" + std::to_string(piol->comm->getRank()) +") does not have minmax allocated.",
                     Log::Verb::Extended);
        return;
    }
    auto x = getCoordMinMax<T>(piol, offset, sz, coord, xlam, ylam);
    auto y = getCoordMinMax<T>(piol, offset, sz, coord, ylam, xlam);

    std::copy(x.begin(), x.end(), minmax);
    std::copy(y.begin(), y.end(), minmax + x.size());
}

void getMinMax(Piol piol, size_t offset, size_t sz, const coord_t * coord, CoordElem * minmax)
{
    auto xlam = [](const coord_t & a) -> geom_t { return a.x; };
    auto ylam = [](const coord_t & a) -> geom_t { return a.y; };
    getMinMax<coord_t>(piol, offset, sz, coord, xlam, ylam, minmax);
}

void getMinMax(Piol piol, size_t offset, size_t sz, File::Coord item, const TraceParam * prm, CoordElem * minmax)
{
    switch (item)
    {
        case File::Coord::Src :
        getMinMax<TraceParam>(piol, offset, sz, prm,
                          [](const TraceParam & a) -> geom_t { return a.src.x; },
                          [](const TraceParam & a) -> geom_t { return a.src.y; },
                          minmax);
        break;
        case File::Coord::Rcv :
        getMinMax<TraceParam>(piol, offset, sz, prm,
                          [](const TraceParam & a) -> geom_t { return a.rcv.x; },
                          [](const TraceParam & a) -> geom_t { return a.rcv.y; },
                          minmax);
        break;
        case File::Coord::CMP :
        getMinMax<TraceParam>(piol, offset, sz, prm,
                          [](const TraceParam & a) -> geom_t { return a.cmp.x; },
                          [](const TraceParam & a) -> geom_t { return a.cmp.y; },
                          minmax);
        break;
        default :
            piol->record("", Log::Layer::Ops, Log::Status::Warning,
                         "getCoordMinMax() was called for an unknown coordinate.",
                          Log::Verb::Extended);
        break;
    }
}
}}
