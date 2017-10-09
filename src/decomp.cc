/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date May 2017
 *   \brief Decompostion functions
 *   \details File contains functions to decompose traces and/or parameters across the number of
 *   processors, providing the end user where each processor should start reading data and how
 *   much data should be read. This function can be used by end users when using the ExSeisPIOL
 *   and need to know Decomposition of I/O across processes. It is used internally by the
 *   ExSeisFlow workflow.
 *//*******************************************************************************************/
#include "share/decomp.hh"
#include "file/file.hh"
namespace PIOL {
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz / numRank;
    size_t r = sz % numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

std::pair<size_t, size_t> decompose(ExSeisPIOL * piol, File::ReadInterface * file)
{
    return decompose(file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}
}
