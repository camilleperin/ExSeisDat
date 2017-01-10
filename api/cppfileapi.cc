/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *   \brief
 *   \details Functions etc for C++ API
 *//*******************************************************************************************/
#include "cppfileapi.hh"
#include "global.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"
namespace PIOL {
ExSeis::ExSeis(const Log::Verb maxLevel)
{
    piol = std::make_shared<ExSeisPIOL>(maxLevel);
}

ExSeis::ExSeis(bool initComm, const Log::Verb maxLevel)
{
    piol = std::make_shared<ExSeisPIOL>(initComm, maxLevel);
}

ExSeis::ExSeis(MPI_Comm comm, const Log::Verb maxLevel)
{
    Comm::MPI::Opt copt;
    copt.initMPI = false;
    copt.comm = comm;
    piol = std::make_shared<ExSeisPIOL>(copt, maxLevel);
}

void ExSeis::isErr(std::string msg) const
{
    piol->isErr(msg);
}

namespace File {
Direct::Direct(const Piol piol, const std::string name, FileMode mode, std::shared_ptr<Rule> rule_)
{
    const File::SEGY::Opt f;
    const Obj::SEGY::Opt o;
    const Data::MPIIO::Opt d;
    auto data = std::make_shared<Data::MPIIO>(piol, name, d, mode);
    auto obj = std::make_shared<Obj::SEGY>(piol, name, o, data, mode);
    file = std::make_shared<File::SEGY>(piol, name, f, obj, mode);
    rule = rule_;
}

const std::string & Direct::readText(void) const
{
    return file->readText();
}

size_t Direct::readNs(void) const
{
    return file->readNs();
}

size_t Direct::readNt(void)
{
    return file->readNt();
}

geom_t Direct::readInc(void) const
{
    return file->readInc();
}

void Direct::readParam(csize_t offset, csize_t sz, Param * prm) const
{
    file->readParam(offset, sz, prm);
}

void Direct::writeParam(csize_t offset, csize_t sz, const Param * prm)
{
    file->writeParam(offset, sz, prm);
}

void Direct::readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm) const
{
    file->readTrace(offset, sz, trace, prm);
}

void Direct::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm)
{
    file->writeTrace(offset, sz, trace, prm);
}

void Direct::readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    file->readTrace(sz, offset, trace, prm);
}

void Direct::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm)
{
    file->writeTrace(sz, offset, trace, prm);
}

void Direct::readParam(csize_t sz, csize_t * offset, Param * prm) const
{
    file->readParam(sz, offset, prm);
}

void Direct::writeParam(csize_t sz, csize_t * offset, const Param * prm)
{
    file->writeParam(sz, offset, prm);
}

void Direct::writeText(const std::string text_)
{
    file->writeText(text_);
}

void Direct::writeNs(const size_t ns_)
{
    file->writeNs(ns_);
}

void Direct::writeNt(const size_t nt_)
{
    file->writeNt(nt_);
}

void Direct::writeInc(const geom_t inc_)
{
    file->writeInc(inc_);
}
}}
