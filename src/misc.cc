/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date March 2017
 *   \details For small functions outside the library that don't make sense anywhere else.
*//*******************************************************************************************/
#include "share/misc.hh"
#include <algorithm>
#include <numeric>
namespace PIOL
{
std::vector<size_t> getSortIndex(size_t sz, const size_t * list)
{
    std::vector<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    //This sorts one list with respect to another, so that we dont modify our initial list but now
    //have an index into our list as though it were sorted.
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}
}
