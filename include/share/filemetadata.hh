/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2017
 *   \brief
 *//*******************************************************************************************/
#ifndef PIOLSHARELAYOUT_INCLUDE_GUARD
#define PIOLSHARELAYOUT_INCLUDE_GUARD
#include "global.hh"

namespace PIOL {
struct FileMetadata
{
    size_t ns;          //!< The number of samples per trace.
    size_t nt;          //!< The number of traces.
    std::string text;   //!< Human readable text extracted from the file
    geom_t inc;         //!< The increment between samples in a trace

    bool operator==(FileMetadata & other)
    {
        return ns == other.ns && nt == other.nt && text == other.text && inc == other.inc;
    }
};
}
#endif
