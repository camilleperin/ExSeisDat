/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "json.hpp"
#include "object/objseis.hh"
#include "data/datampiio.hh"
#include "share/seis.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////      Constructor & Destructor      ///////////////////////////////

std::vector<std::shared_ptr<Data::Interface>> makeMultiData(Piol piol, std::vector<std::string> & names, FileMode mode)
{
    std::vector<std::shared_ptr<Data::Interface>> data;
    for (auto name : names)
        data.push_back(std::make_shared<Data::MPIIO>(piol, name, mode));
    return data;
}

void ReadSeis::Init(const Opt * opt)
{
//TODO: This function will need to detect the file structure based on the header
    std::vector<uchar> dat(data->getFileSz());
    data->read(0LU, dat.size(), dat.data());

    desc = std::make_shared<SeisFileHeader>(dat);

    traceBlocks = makeMultiData(piol, desc->extents, FileMode::Read);
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, const Opt * opt, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    Init(opt);
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    Opt opt;
    Init(&opt);
}

///////////////////////////////////       Member functions      ///////////////////////////////////

SeisFileHeader::SeisFileHeader(const std::vector<uchar> & dat)
{
    nlohmann::json jf = nlohmann::json::parse(dat.begin(), dat.end());
    SeisF::set(&bytes, "bytes", jf);
    SeisF::set(&o1, "o1", jf);
    SeisF::set(&d1, "d1", jf);
    SeisF::set(&n1, "n1", jf);

    std::string end;
    SeisF::set(&end, "endianness", jf);
    endian = (end == "little" ? SeisF::Endian::Little : SeisF::Endian::Big);

    SeisF::set(&headerFile, "separateHeaders", jf);
    SeisF::set(extents, "extents", jf);
    SeisF::set(&packetSz, "packet", jf);
}

size_t ReadSeis::getFileSz(void) const
{
    return 0LU;
}

void WriteSeis::setFileSz(csize_t sz) const
{
}

std::shared_ptr<FileMetadata> ReadSeis::readHO(void) const
{
    return desc;
}

void WriteSeis::writeHO(const std::shared_ptr<FileMetadata> ho) const
{
}

void ReadSeis::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
}

void WriteSeis::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
}

void ReadSeis::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
}

void WriteSeis::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
}

void ReadSeis::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
}

void WriteSeis::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
}

void ReadSeis::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
}

void WriteSeis::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
}

void ReadSeis::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
}

void WriteSeis::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
}

void ReadSeis::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
}

void WriteSeis::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
}
}}
