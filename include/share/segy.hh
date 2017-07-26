/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *//*******************************************************************************************/
#ifndef PIOLSHARESEGY_INCLUDE_GUARD
#define PIOLSHARESEGY_INCLUDE_GUARD
#include "global.hh"
#include "share/filemetadata.hh"
#include "file/iconv.hh"
#include "share/datatype.hh"

namespace PIOL { namespace SEGSz {
/*! \brief An enumeration containing important SEG-Y sizes
 */
enum class Size : size_t
{
    Text = 3200U,
    HO = 3600U,  //!< The size of the SEG-Y header (text extensions not used)
    DOMd = 240U  //!< The SEG-Y trace header size
};

/*! \brief Return the size of the text field
 *  \return  Returns the size of the text field in bytes
 */
constexpr size_t getTextSz()
{
    return static_cast<size_t>(Size::Text);
}

/*! \brief Return the size of the Header Object (assuming text extensions are not used)
 *  \return Returns the size of the HO in bytes.
 */
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

/*! \brief Return the size of the Data-Object Metadata object
 *  \return Returns the size of the DOMD in bytes
 */
constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::DOMd);
}

/*! \brief Return the size of the Data-Object Field object
 *  \param[in] ns The number of elements in the data-field.
 *  \return Returns the size of the data-field in bytes
 */
template <typename T = float>
constexpr size_t getDFSz(csize_t ns)
{
    return ns * sizeof(T);
}

/*! \brief Return the size of the Data-Object.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the DO size.
 */
template <typename T = float>
size_t getDOSz(csize_t ns)
{
    return getMDSz() + getDFSz<T>(ns);
}

/*! \brief Return the expected size of the file if there are nt data-objects and
 *         ns elements in a data-field.
 *  \param[in] nt The number of data objects.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the expected file size.
 */
template <typename T = float>
size_t getFileSz(csize_t nt, csize_t ns)
{
    return getHOSz() + nt*getDOSz<T>(ns);
}

/*! \brief Return the offset location of a specific data object.
 *  \param[in] i The location of the ith data object will be returned.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the location.
 */
template <typename T = float>
size_t getDOLoc(csize_t i, csize_t ns)
{
    return getFileSz<T>(i, ns);
}

/*! \brief Return the offset location of a specific data-field
 *  \param[in] i The location of the ith data-field will be returned.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the location.
 */
template <typename T = float>
size_t getDODFLoc(csize_t i, csize_t ns)
{
    return getFileSz<T>(i, ns) + getMDSz();
}

/*! \brief Return the number of traces in a file given a file size
 *  \param[in] fsz the size of a file or expected size in bytes
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the number of traces.
 */
template <typename T = float>
size_t getNt(csize_t fsz, csize_t ns)
{
    return (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz<T>(ns);
}
}
namespace SEGY {
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

/*! \brief Convert a SEG-Y scale integer to a floating point type
 *  \param[in] scale The int16_t scale taken from the SEG-Y file
 *  \return The scale convertered to floating point.
 */
extern geom_t scaleConv(int16_t scale);

/*! \fn int16_t PIOL::File::deScale(const geom_t val)
 * \brief Take a coordinate and extract a suitable scale factor to represent that number
 * in 6 byte fixed point format of the SEG-Y specification.
 * \param[in] val The coordinate of interest.
 * \return An appropriate scale factor for the coordinate.
 * \details Convert the number from float to a 6 byte SEGY fixed-point representation.
 * There are ten possible values for the scale factor. Shown are the possible values
 * and the form the input float should have to use that scale factor.
 * firstly, anything smaller than 4 decimal points is discarded since the approach
 * can not represent it.
*//*
 * Shown is the
 * position of the least significant digit:
 * -10000 - \d0000.0000
 * -1000  - \d000.0000
 * -100   - \d00.0000
 * -10    - \d0.0000
 * -1     - \d
 * 1      - \d
 * 10     - \d.\d
 * 100    - \d.\d\d
 * 1000   - \d.\d\d\d
 * 10000  - \d.\d\d\d\d
 * \todo Handle the annoying case of numbers at or around 2147483648 with a decimal somewhere.
 * \todo Add rounding before positive scale values
*/
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
}}
#endif

