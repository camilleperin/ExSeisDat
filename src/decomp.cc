/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "share/decomp.hh"
#include "file/file.hh"
namespace PIOL {
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz / numRank;
    size_t r = sz % numRank;

    //Calculate the starting position by multiplying rank by quotient and then
    //factoring in the remainder. The nth rank process is shifted nth elements
    //to the right assuming there are n remainders, otherwise just shifted by
    //the remainder.
    size_t start = q * rank + std::min(rank, r);

    //The size of the local decomposition is determined by the quotient with
    //an extra element if the rank is less than the remainder.
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

std::pair<size_t, size_t> decompose(ExSeisPIOL * piol, File::ReadInterface * file)
{
    return decompose(file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}
}
