/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLFILETESTSEGYMD_INCLUDE_GUARD
#define PIOLFILETESTSEGYMD_INCLUDE_GUARD

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

enum Hdr : size_t
{
    Increment  = 3216U,
    NumSample  = 3220U,
    Type       = 3224U,
    Sort       = 3228U,
    Units      = 3254U,
    SEGYFormat = 3500U,
    FixedTrace = 3502U,
    Extensions = 3504U,
};

enum TrHdr : size_t
{
    SeqNum      = 0U,
    SeqFNum     = 4U,
    ORF         = 8U,
    TORF        = 12U,
    RcvElv      = 40U,
    SurfElvSrc  = 44U,
    SrcDpthSurf = 48U,
    DtmElvRcv   = 52U,
    DtmElvSrc   = 56U,
    WtrDepSrc   = 60U,
    WtrDepRcv   = 64U,
    ScaleElev   = 68U,
    ScaleCoord  = 70U,
    xSrc        = 72U,
    ySrc        = 76U,
    xRcv        = 80U,
    yRcv        = 84U,
    xCMP        = 180U,
    yCMP        = 184U,
    il          = 188U,
    xl          = 192U
};

/*! Trace Header offsets to elevations
 */
enum class TrElev : size_t
{
    RcvElv      = 41U,  //!< int32_t. The Receiver group elevation
    SurfElvSrc  = 45U,  //!< int32_t. The surface elevation at the source.
    SrcDpthSurf = 49U,  //!< int32_t. The source depth below surface (opposite of above?).
    DtmElvRcv   = 53U,  //!< int32_t. The datum elevation for the receiver group.
    DtmElvSrc   = 57U,  //!< int32_t. The datum elevation for the source.
    WtrDepSrc   = 61U,  //!< int32_t. The water depth for the source.
    WtrDepRcv   = 65U   //!< int32_t. The water depth for the receive group.
};

/*! Trace Header offsets to scalers
 */
enum class TrScal : size_t
{
    ScaleElev   = 69U,  //!< int16_t. The scale coordinate for 41-68 (elevations + depths)
    ScaleCoord  = 71U   //!< int16_t. The scale coordinate for 73-88 + 181-188
};

/*! Trace Header offsets to coordinates
 */
enum class TrCrd : size_t
{
    xSrc = 73U,  //!< int32_t. The X coordinate for the source
    ySrc = 77U,  //!< int32_t. The Y coordinate for the source
    xRcv = 81U,  //!< int32_t. The X coordinate for the receive group
    yRcv = 85U,  //!< int32_t. The Y coordinate for the receive group
    xCMP = 181U, //!< int32_t  The X coordinate for the CMP
    yCMP = 185U  //!< int32_t. The Y coordinate for the CMP
};

/*! Trace Header offsets to grid components
 */
enum class TrGrd : size_t
{
    il        = 189U, //!< int32_t. The Inline grid point.
    xl        = 193U  //!< int32_t. The Crossline grid point.
};

struct coord_t
{
    geom_t x;   //!< The x coordinate
    geom_t y;   //!< The y coordinate

    /*! Constructor for initialising coordinates
     *  \param[in] x_ intialisation value for x
     *  \param[in] y_ intialisation value for x
     */
    coord_t(const geom_t x_, const geom_t y_) : x(x_), y(y_) { }
    /*! Default constructor (set both coordinates to 0)
     */
    coord_t() : x(geom_t(0)), y(geom_t(0)) { }
};

/*! A structure composed of two grid values to form a single grid point
 */
struct grid_t
{
    llint il;   //!< The inline value
    llint xl;   //!< The crossline value

    /*! Constructor for initialising a grid point
     *  \param[in] i_ intialisation value for il
     *  \param[in] x_ intialisation value for xl
     */
    grid_t(const geom_t i_, const geom_t x_) : il(i_), xl(x_) { }

    /*! Default constructor (set both grid values to 0)
     */
    grid_t() : il(llint(0)), xl(llint(0)) { }
};

/*! \brief Possible coordinate sets
 */
enum class Coord : size_t
{
    Src,    //!< Source Coordinates
    Rcv,    //!< Receiver Coordinates
    CMP     //!< Common Midpoint Coordinates
};

/*! \brief Possible Grids
 */
enum class Grid : size_t
{
    Line    //!< Inline/Crossline grid points
};

/*! \brief Return a pair of coordinate offsets
 *  \param[in] pair The coordinate point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
std::pair<TrCrd, TrCrd> getPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(TrCrd::xSrc, TrCrd::ySrc);
        case Coord::Rcv :
            return std::make_pair(TrCrd::xRcv, TrCrd::yRcv);
        default :
        case Coord::CMP :
            return std::make_pair(TrCrd::xCMP, TrCrd::yCMP);
    }
}

/*! \brief Return a pair of grid offsets
 *  \param[in] pair The grid point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
std::pair<TrGrd, TrGrd> getPair(Grid pair)
{
    switch (pair)
    {
//Note: When a new set of grid points are required:
//        case Grid::OFR :
//            return std::make_pair(TrHdr::ORF, TrHdr::TORF);
        default :
        case Grid::Line :
            return std::make_pair(TrGrd::il, TrGrd::xl);
    }
}

//extern geom_t scaleConv(int16_t scale);
extern geom_t getMd(const TrScal scal, const uchar * src);
extern geom_t getMd(const TrCrd item, const geom_t scale, const uchar * src);
extern int32_t getMd(const TrGrd item, const uchar * src);
extern void setCoord(const Coord item, const coord_t coord, const int16_t scale, uchar * buf);
extern coord_t getCoord(const Coord item, const geom_t scale, const uchar * buf);
extern grid_t getGrid(const Grid item, const uchar * buf);
extern void setGrid(const Grid item, const grid_t grid, uchar * buf);
//extern int16_t deScale(const geom_t val);
extern int16_t scalComp(int16_t scal1, int16_t scal2);
extern int16_t calcScale(const coord_t coord);
//extern void extractTraceParam(const uchar * md, TraceParam * prm);
//extern void insertTraceParam(const TraceParam * prm, uchar * md);
extern void setScale(const TrScal item, const int16_t scale, uchar * buf, size_t start = 0);
#endif
