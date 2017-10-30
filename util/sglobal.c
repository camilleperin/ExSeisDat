#include "sglobal.h"
#include <assert.h>

char * copyString(const char * oldstr)
{
    size_t len = strlen(oldstr);
    //Note that small sizes can generate false positives with valgrind
    //due to copy optimisations.
    char * newstr = calloc(len+1, 1);
    assert(newstr);
    strncpy(newstr, oldstr, len);
    return newstr;
}
