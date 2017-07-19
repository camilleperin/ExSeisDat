/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details ReadSeis functions
 *//*******************************************************************************************/
#include "global.hh"
#include "file/fileseis.hh"
#include "object/object.hh"
namespace PIOL { namespace File {

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
ReadSeis::Opt::Opt(void)
{
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, const ReadSeis::Opt & opt, std::shared_ptr<Obj::ReadInterface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, std::shared_ptr<Obj::ReadInterface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
}
///////////////////////////////////       Member functions      ///////////////////////////////////

size_t ReadSeis::readNt(void)
{
    return nt;
}

void ReadSeis::readTrace(csize_t offset, csize_t sz, trace_t * trc, Param * prm, csize_t skip) const
{
}

void ReadSeis::readTrace(csize_t sz, csize_t * offset, trace_t * trc, Param * prm, csize_t skip) const
{
}
}}
