/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*! Make the default object layer object.
 * \param[in] piol The piol shared object.
 * \param[in] name The name of the file.
 * \param[in] mode The filemode.
 * \return Return a shared_ptr to the obj layer object.
 * \todo TODO: This hack needs a further tidyup and out of file.hh.
 */
/*
//std::shared_ptr<Obj::Interface> makeDefaultObj(PIOL::Piol piol, std::string name, FileMode mode);
std::shared_ptr<Obj::WriteInterface> makeDefaultObj(Piol piol, std::string name, FileMode mode)
{
    auto data = std::make_shared<Data::MPIIO>(piol, name, mode);
    return std::make_shared<Obj::ReadSEGY>(piol, name, data, mode);
}*/

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
//pragma to ignore unusued-paramter warnings here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
WriteSEGY::WriteSEGY(Piol piol_, std::string name_, const WriteSEGY::Opt & opt_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
{
}
WriteSEGY::WriteSEGY(Piol piol_, std::string name_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
{
}
ReadSEGY::ReadSEGY(Piol piol_, std::string name_, const ReadSEGY::Opt & opt_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
}
ReadSEGY::ReadSEGY(Piol piol_, std::string name_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
}
#pragma GCC diagnostic pop

///////////////////////////////////       Member functions      ///////////////////////////////////
size_t ReadSEGY::getFileSz(void) const
{
    return data->getFileSz();
}

void ReadSEGY::readHO(uchar * ho) const
{
    data->read(0LU, SEGSz::getHOSz(), ho);
}

void ReadSEGY::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
    data->read(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void WriteSEGY::setFileSz(csize_t sz) const
{
    return data->setFileSz(sz);
}

void WriteSEGY::writeHO(const uchar * ho) const
{
    if (ho)
        data->write(0LU, SEGSz::getHOSz(), ho);
    else
        data->write(0LU, 0U, ho);
}

void WriteSEGY::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
    data->write(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void ReadSEGY::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
    data->read(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void WriteSEGY::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
    data->write(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void ReadSEGY::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
    data->read(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

void WriteSEGY::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
    data->write(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

//TODO: Add optional validation in this layer?
void ReadSEGY::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void WriteSEGY::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void ReadSEGY::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void WriteSEGY::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void ReadSEGY::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->read(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

void WriteSEGY::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->write(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}
}}
