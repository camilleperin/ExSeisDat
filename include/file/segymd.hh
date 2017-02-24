/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLFILESEGYMD_INCLUDE_GUARD
#define PIOLFILESEGYMD_INCLUDE_GUARD
#include <cstring>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
#include "share/units.hh"
#include "share/datatype.hh"

namespace PIOL { namespace File {
/*! The Datatype (or format in SEG-Y terminology)
 */
enum class Format : int16_t
{
    IBM  = 1,   //<! IBM format, big endian
    TC4  = 2,   //<! Two's complement, 4 byte
    TC2  = 3,   //<! Two's complement, 2 byte
    FPG  = 4,   //<! Fixed-point gain (obsolete)
    IEEE = 5,   //<! The IEEE format, big endian
    NA1  = 6,   //<! Unused
    NA2  = 7,   //<! Unused
    TC1  = 8    //<! Two's complement, 1 byte
};

/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment  = 3217U, //!< int16_t. The increment between traces in microseconds
    NumSample  = 3221U, //!< int16_t. The number of samples per trace
    Type       = 3225U, //!< int16_t. Trace data type. AKA format in SEGY terminology
    Sort       = 3229U, //!< int16_t. The sort order of the traces.
    Units      = 3255U, //!< int16_t. The unit system, i.e SI or imperial.
    SEGYFormat = 3501U, //!< int16_t. The SEG-Y Revision number
    FixedTrace = 3503U, //!< int16_t. Whether we are using fixed traces or not.
    Extensions = 3505U, //!< int16_t. If we use header extensions or not.
};

extern geom_t scaleConv(int16_t scale);
extern int16_t deScale(const geom_t val);

/*! \brief Get the header metadata value from the binary header.
 *  \param[in] item The header item of interest.
 *  \param[in] src The buffer of the header object.
 *  \return Return the header item value.
 */
template <class T = int16_t>
T getMd(const Hdr item, const uchar * src)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        return T(getHost<int16_t>(&src[size_t(item)-1U]));
        default :
        return T(0);
        break;
    }
}

/*! \brief Set the header metadata value corresponding to the item specified
 *  \param[in] item The header item of interest
 *  \param[in] src The metadata value to insert into the buffer.
 *  \param[in, out] dst The header as an array of uchar.
 */
template <typename T = int16_t>
void setMd(const Hdr item, const T src, uchar * dst)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        getBigEndian<int16_t>(src, &dst[static_cast<size_t>(item)-1U]);
        default :
        break;
    }
}

/*! \brief Extract parameters from an unsigned char array into the parameter structure
 *  \param[in] sz The number of sets of parameters
 *  \param[in] md The buffer in the SEG-Y trace header format
 *  \param[out] prm The parameter structure
 *  \param[in] stride The stride to use between adjacent blocks in the input buffer.
 *  \param[in] skip Skip the first "skip" entries when filling Param
 */
extern void extractParam(size_t sz, const uchar * md, Param * prm, size_t stride, size_t skip);

/*! \brief Extract parameters from an unsigned char array into the parameter structure
 *  \param[in] sz The number of sets of parameters
 *  \param[in] prm The parameter structure
 *  \param[out] md The buffer in the SEG-Y trace header format
 *  \param[in] stride The stride to use between adjacent blocks in the input buffer.
 *  \param[in] skip Skip the first "skip" entries when extracting entries from Param
 */
extern void insertParam(size_t sz, const Param * prm, uchar * md, size_t stride, size_t skip);
}}
#endif
