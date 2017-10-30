#ifndef UTIL_SGLOBALH_INCLUDE_GUARD
#define UTIL_SGLOBALH_INCLUDE_GUARD
#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cfileapi.h"

typedef void (* ModPrm)(size_t, size_t, CParam);    //!< Typedef for function which modifies parameters
typedef void (* ModTrc)(size_t, size_t, float *);   //!< Typedef for function which modifies traces

/*! Function which copies a string onto newly allocated memory. The caller is responsible
 *  for deallocating the memory with free. The newly allocated memory is equal to the length
 *  of the old string.
 *  \param[in] oldstr The old string to be copied to the new memory.
 *  \return The newly allocated string which contains the contents of oldstr
 */
extern char * copyString(const char * oldstr);
#endif

