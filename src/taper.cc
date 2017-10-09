/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date April 2017
 *   \brief The taper operation
 *   \details Applies a one or two tailed taper to a set of traces; taper functions include a
 *   linear taper, cosine taper, and cosine squared taper as well as a custum taper.
 *//*******************************************************************************************/
#include <assert.h>
#include "global.hh"
#include "share/units.hh"
#include "share/api.hh"
namespace PIOL { namespace File {

/******************************************** Core *********************************************/
void taper(size_t sz, size_t ns, trace_t * trc, TaperFunc func, size_t nTailLft, size_t nTailRt)
{
    assert(ns > nTailLft && ns > nTailRt);
    for (size_t i = 0; i < sz; i++)
    {
        size_t jstart;
        for (jstart = 0; jstart < ns && trc[i*ns+jstart] == 0.0f; jstart++);

        for (size_t j = jstart; j < std::min(jstart+nTailLft, ns); j++)
            trc[i*ns+j] *= func(j-jstart+1, nTailLft);

        for (size_t j = ns - nTailRt; j < ns; j++)
            trc[i*ns+j] *= func(ns - j - 1LU, nTailRt);
    }
}

/******************************************** Non-Core *********************************************/
TaperFunc getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::Linear :
        return [](trace_t weight, trace_t ramp) { return 1.0f - std::abs((weight-ramp)/ramp); };
        break;
        case TaperType::Cos :
        return [](trace_t weight, trace_t ramp) { return 0.5f + 0.5f * cos(Math::pi*(weight-ramp)/ramp); };
        break;
        case TaperType::CosSqr :
        return [](trace_t weight, trace_t ramp) { return pow(0.5f + 0.5f*cos(Math::pi*(weight-ramp)/ramp), 2.0f); };
        break;
    }
}
}}
