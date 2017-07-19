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
//#include "object/objsegy.hh"
//#include "data/datampiio.hh"
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
}}
