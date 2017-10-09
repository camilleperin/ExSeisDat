/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date July 2016
 *   \brief Interface for object layer
 *   \details Functions to get file sizes
 *//*******************************************************************************************/
#include "object/object.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
size_t Interface::getFileSz(void) const
{
    return data->getFileSz();
}

void Interface::setFileSz(csize_t sz) const
{
    return data->setFileSz(sz);
}
}}
