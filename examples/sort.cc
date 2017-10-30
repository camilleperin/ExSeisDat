/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date February 2017
 *   \brief Sort a file in 9 lines
*//*******************************************************************************************/
#include "flow.hh"
using namespace PIOL;
int main(void)
{
    ExSeis piol;
    Set set(piol, "trace2, trace1", "temp");
    set.sort(SortType::OffLine);
    return 0;
}
