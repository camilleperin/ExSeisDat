/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details WriteSeis functions
 *//*******************************************************************************************/
#include <cmath>
#include <cstring>
#include <limits>
#include "global.hh"
#include "file/fileseis.hh"
#include "object/object.hh"
#include "share/misc.hh"
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
WriteSeis::Opt::Opt(void)
{
}

WriteSeis::WriteSeis(const Piol piol_, const std::string name_, const WriteSeis::Opt & opt, std::shared_ptr<Obj::WriteInterface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
}

WriteSeis::WriteSeis(const Piol piol_, const std::string name_, std::shared_ptr<Obj::WriteInterface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
}

WriteSeis::~WriteSeis(void)
{
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void WriteSeis::writeText(const std::string text_)
{
}

void WriteSeis::writeNs(csize_t ns_)
{

}

void WriteSeis::writeNt(csize_t nt_)
{
}

void WriteSeis::writeInc(const geom_t inc_)
{
}

void WriteSeis::writeTrace(csize_t offset, csize_t sz, trace_t * trc, const Param * prm, csize_t skip)
{
}

void WriteSeis::writeTrace(csize_t sz, csize_t * offset, trace_t * trc, const Param * prm, csize_t skip)
{
}
}}
