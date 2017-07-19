/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/file.hh"
#include "share/misc.hh"
#warning make generalised dependency
#include "file/dynsegymd.hh"
namespace PIOL { namespace File {
const Param * PARAM_NULL = (Param *)1;
const trace_t * TRACE_NULL = (trace_t *)1;

void ReadInterface::readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trc, Param * prm, csize_t skip) const
{
    //Sort the initial offset and make a new offset without duplicates
    auto idx = getSortIndex(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++)
        if (offset[idx[j-1]] != offset[idx[j]])
            nodups.push_back(offset[idx[j]]);

    File::Param sprm(prm->r, (prm != PARAM_NULL ? nodups.size() : 0LU));
    std::vector<trace_t> strc(ns * (trc != TRACE_NULL ? nodups.size() : 0LU));

    readTrace(nodups.size(), nodups.data(), (trc != TRACE_NULL ? strc.data() : trc),
                                            (prm != PARAM_NULL ? &sprm : prm), 0LU);

    if (prm != PARAM_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j)
        {
            n += (j && offset[idx[j-1]] != offset[idx[j]]);
            cpyPrm(n, &sprm, skip + idx[j], prm);
        }

    if (trc != TRACE_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j)
        {
            n += (j && offset[idx[j-1]] != offset[idx[j]]);
            for (size_t k = 0; k < ns; k++)
                trc[idx[j]*ns + k] = strc[n*ns + k];
        }
}

//TODO: Unit test
void ReadInterface::readParam(csize_t offset, csize_t sz, Param * prm, csize_t skip) const
{
    readTrace(offset, sz, const_cast<trace_t *>(TRACE_NULL), prm, skip);
}

void ReadInterface::readParam(csize_t sz, csize_t * offset, Param * prm, csize_t skip) const
{
    readTrace(sz, offset, const_cast<trace_t *>(TRACE_NULL), prm, skip);
}

const std::string & ReadInterface::readText(void) const
{
    return text;
}

size_t ReadInterface::readNs(void) const
{
    return ns;
}

geom_t ReadInterface::readInc(void) const
{
   return inc;
}
}}
