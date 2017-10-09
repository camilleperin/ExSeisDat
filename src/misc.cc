/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date March 2017
 *   \brief Miscellanious functions
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
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}
}
