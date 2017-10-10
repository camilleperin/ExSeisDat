/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date July 2016
 *   \brief Bit Conversion function
 *   \details Converts data from EBCDIC to ASCII
 *//*******************************************************************************************/
#ifndef PIOLFILEICONV_INCLUDE_GUARD
#define PIOLFILEICONV_INCLUDE_GUARD
#include "global.hh"
#include <string>
namespace PIOL {
/*!\brief This function detects if the given unsigned character array contains ebcdic text. If so, it converts
 * the strong. Otherwise it is unmodified.
 * \param[in, out] piol The PIOL object
 * \param[in] file The name of the file (for error reporting)
 * \param[in] sz The length of the string.
 * \param[in, out] src The EBCDIC or ASCII string. After the function ends, the string is ASCII
 */
extern void getAscii(ExSeisPIOL * piol, const std::string file, csize_t sz, uchar * src);
}
#endif
